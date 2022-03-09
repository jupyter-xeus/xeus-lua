/***************************************************************************
* Copyright (c) 2021, Thorsten Beier                                       *                                                       *
* Copyright (c) 2021, QuantStack                                           *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif


#include <algorithm>
#include <string>
#include <sstream>
#include <utility>
#include <vector>
#include <iostream>
#include <sstream>


#include "nlohmann/json.hpp"

#include "xeus/xinterpreter.hpp"
#include "xeus-lua/xinterpreter.hpp"
#include "sol/sol.hpp"


namespace nl = nlohmann;


namespace xlua
{   
    inline void my_panic(sol::optional<std::string> maybe_msg)
    {
        auto & interpreter = xeus::get_interpreter();
        std::stringstream ss;
        std::cerr << "Lua is in a panic state and will now abort() the application" << std::endl;
        ss << "Lua is in a panic state and will now abort() the application" << std::endl;
        if (maybe_msg) {
            const std::string& msg = maybe_msg.value();
            std::cerr << "\terror message: " << msg << std::endl;
            ss << "\terror message: " << msg << std::endl;
        }
        // When this function exits, Lua will exhibit default behavior and abort()
        const auto error_str = ss.str();
        interpreter.publish_execution_error(error_str,error_str,std::vector<std::string>(1,error_str));
    }

    int my_exception_handler (lua_State* L, sol::optional<const std::exception&> maybe_exception, sol::string_view description)
    {
        auto & interpreter = xeus::get_interpreter();
        std::stringstream ss;
        // L is the lua state, which you can wrap in a state_view if necessary
        // maybe_exception will contain exception, if it exists
        // description will either be the what() of the exception or a description saying that we hit the general-case catch(...)
        ss << "An exception occurred in a function, here's what it says ";
        if (maybe_exception) {
            ss << "(straight from the exception): ";
            const std::exception& ex = *maybe_exception;
            ss << ex.what() << std::endl;
        }
        else {
            ss << "(from the description parameter): ";
            ss.write(description.data(), static_cast<std::streamsize>(description.size()));
            ss << std::endl;
        }
        const auto error_str = ss.str();
        interpreter.publish_execution_error(error_str,error_str,std::vector<std::string>(1,error_str));

        // you must push 1 element onto the stack to be
        // transported through as the error object in Lua
        // note that Lua -- and 99.5% of all Lua users and libraries -- expects a string
        // so we push a single string (in our case, the description of the error)
        return sol::stack::push(L, description);
    }
    
    // implemented in xextend.cpp
    void extend(sol::state_view & lua);
    // implemented in xcomplete.cpp
    int complete(sol::state_view & lua, const std::string & start,int cursor_pos, nl::json & matches);
    // implemented in xio.cpp
    void setup_io(sol::state_view & lua, interpreter & interp);
    // implemented in xdisplay.cpp
    void setup_display(sol::state_view & lua, interpreter & interp);
    // implemented in xjson.cpp
    void setup_json(sol::state_view & lua, interpreter & interp);

    void setup_xtime(sol::state_view & lua);

    #ifdef XLUA_WITH_XWIDGETS
    // implemented in xwidgets.cpp
    void register_xwidgets(sol::state_view & lua);
    #endif

    #ifdef XLUA_WITH_XCANVAS
    // implemented in xwidgets.cpp
    void setup_xcanvas(sol::state_view & lua);
    #endif

    interpreter::interpreter()
    : m_allow_stdin(true)
    {
        //L = lua;
        L = luaL_newstate();
        sol::state_view lua(L);

        xeus::register_interpreter(this);

        //luaL_openlibs(L);
        lua.open_libraries(
            sol::lib::base,
            sol::lib::package,
            sol::lib::coroutine,
            sol::lib::string,
            sol::lib::os,
            sol::lib::math,
            sol::lib::table,
            sol::lib::debug,
            sol::lib::bit32,
            sol::lib::io
            #ifndef __EMSCRIPTEN__ 
            ,
            sol::lib::ffi,
            sol::lib::jit 
            #endif
        );

        lua.set_exception_handler(&my_exception_handler);
        lua.set_panic(sol::c_call<decltype(&my_panic), &my_panic>);

        // install pure lua modules
        extend(lua);

        // replace / monkeypatch functions like
        // io.read / io.write / io.flush print 
        setup_io(lua, *this);

        // json helper functions
        setup_json(lua, *this);

        // add functions to display data
        setup_display(lua, *this);

        setup_xtime(lua);

        // add package path only do this for emscripten
        #ifdef __EMSCRIPTEN__
            const std::string script = R""""(package.path =  "/asset_dir/lua_packages/?.lua;/asset_dir/lua_packages/?/init.lua")"""";
            lua.script(script);  
        #endif

        // add widgets
        #ifdef XLUA_WITH_XWIDGETS
        register_xwidgets(lua);            
        #endif    

        // add widgets
        #ifdef XLUA_WITH_XCANVAS
        setup_xcanvas(lua);            
        #endif    
    }

    bool interpreter::allow_stdin()const
    {
        return m_allow_stdin;
    }

    interpreter::~interpreter()
    {
        lua_close(L);
    }

    void interpreter::configure_impl()
    {
    }

    nl::json interpreter::execute_request_impl(int /*execution_count*/,
                                               const std::string& code,
                                               bool silent,
                                               bool /*store_history*/,
                                               nl::json /*user_expressions*/,
                                               bool allow_stdin)
    {
        sol::state_view lua(L);
        m_allow_stdin = allow_stdin;
        // reset  payload
        nl::json kernel_res;

        kernel_res["payload"] = nl::json::array();
        kernel_res["user_expressions"] = nl::json::object();
        kernel_res["status"] = "ok";

        sol::table ilua_table = lua["ilua"];
        sol::table config_table = ilua_table["config"];

        bool auto_print = config_table["auto_print"];
       
        bool need_eval = true;

        if(auto_print)
        {
            std::stringstream test_code;
            test_code << "print(" << code << ")";
            auto test_code_result = lua.safe_script(test_code.str(), &sol::script_pass_on_error);
            need_eval = !test_code_result.valid();
        }
            
        if (need_eval){
            sol::protected_function_result code_result = lua.safe_script(code,  &sol::script_pass_on_error);
            if (code_result.valid())
            {

                kernel_res["status"] = "ok";
                kernel_res["user_expressions"] = nl::json::object();
            }
            else
            {
                sol::error err = code_result;
                sol::call_status status = code_result.status();
                const auto error_str = err.what();
                if (!silent)
                {   
                    publish_execution_error(error_str,error_str,std::vector<std::string>(1,error_str));
                }

                kernel_res["status"] = "error";
                kernel_res["ename"] = "load file error";
                kernel_res["evalue"] = error_str;
                kernel_res["traceback"] = { error_str };
            }
        }
        return kernel_res;
    }

    nl::json interpreter::complete_request_impl(
        const std::string& code,
        int cursor_pos)
    {
        sol::state_view lua(L);
        nl::json matches = nl::json::array();

        int cursor_start = complete(lua, code.c_str(), cursor_pos, matches);

        nl::json result;
        result["status"] = "ok";
        result["matches"] = matches;
        result["cursor_start"] = cursor_start;
        result["metadata"] = nl::json::object();
	result["cursor_end"] = cursor_pos;  

        return result;
    }

    nl::json interpreter::inspect_request_impl(const std::string& /*code*/,
                                               int /*cursor_pos*/,
                                               int /*detail_level*/)
    {
        nl::json jresult;
        jresult["status"] = "ok";
        jresult["found"] = false;
        jresult["data"] = nl::json::object();
        jresult["metadata"] = nl::json::object();
        return jresult;
    }

    nl::json interpreter::is_complete_request_impl(const std::string& /*code*/)
    {
        nl::json jresult;
        jresult["status"] = "complete";
        return jresult;
    }

    nl::json interpreter::kernel_info_request_impl()
    {
        nl::json result;
        result["implementation"] = "xlua";
        result["implementation_version"] = "1.0.0";
        result["banner"] = "xlua";
        result["language_info"]["name"] = "lua";
        result["language_info"]["version"] = "14.0.0";
        result["language_info"]["mimetype"] = "text/x-luasrc";
        result["language_info"]["file_extension"] = ".lua";
        result["status"] = "ok";
        return result;
    }

    void interpreter::shutdown_request_impl()
    {
    }
}
