/***************************************************************************
* Copyright (c) 2021, Thorsten Beier                                       *
* Wolf Vollprecht                                                          *
* Copyright (c) 2021, QuantStack                                           *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

// https://github.com/pakozm/IPyLua/blob/master/IPyLua/rlcompleter.lua
// a lot is copied from https://raw.githubusercontent.com/tomstitt/lupyter/main/lupyter/lua_runtime/lua_runtime.c


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
    
    // implemented in xextend.cpp
    void extend(sol::state_view & lua);
    // implemented in xcomplete.cpp
    int complete(sol::state_view & lua, const std::string & start,int cursor_pos, nl::json & matches);
    // implemented in xio.cpp
    void setup_io(sol::state_view & lua, interpreter & interp);
    // implemented in xdisplay.cpp
    void setup_display(sol::state_view & lua, interpreter & interp);

    #ifdef XLUA_WITH_XWIDGETS
    // implemented in xwidgets.cpp
    void register_xwidgets(sol::state_view & lua);
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

        // install pure lua modules
        extend(lua);

        // replace / monkeypatch functions like
        // io.read / io.write / io.flush print 
        setup_io(lua, *this);

        // add functions to display data
        setup_display(lua, *this);

        // add package path only do this for emscripten
        #ifdef __EMSCRIPTEN__
            const std::string script = R""""(package.path =  "/asset_dir/lua_packages/?.lua;/asset_dir/lua_packages/?/init.lua")"""";
            lua.script(script);  
        #endif

        // add widgets
        #ifdef XLUA_WITH_XWIDGETS
        register_xwidgets(lua);            
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


    bool is_simple_statement(const std::string& code)
    {
        // todo
        return code.find(' ') == std::string::npos &&
               code.find('\n') == std::string::npos &&
               code.find('\t') == std::string::npos &&
               code.find('.') == std::string::npos;
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

       
        //const auto is_simple = is_simple_statement(code);
        if(false)
        {
            // std::stringstream test_code;
            // test_code<<"print("<<code<<")";
            // auto test_code_result = lua.safe_script(test_code.str());
        }
        else{
            auto code_result= lua.safe_script(code);
            if (code_result.valid())
            {
                kernel_res["status"] = "ok";
                kernel_res["user_expressions"] = nl::json::object();
            }
            else
            {
                sol::error err = code_result;
                const auto error_str = err.what();
                if (!silent)
                {   
                    publish_execution_error(error_str,error_str,std::vector<std::string>(1,error_str));
                }

                kernel_res["status"] = "error";
                kernel_res["ename"] = "load file error";
                kernel_res["evalue"] = error_str;
                kernel_res["traceback"] = error_str;
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

        int cursor_start =  complete(lua, code.c_str(), cursor_pos, matches);

        nl::json result;
        result["status"] = "ok";
        result["matches"] = matches;
        result["cursor_start"] = cursor_start;
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
        return result;
    }

    void interpreter::shutdown_request_impl()
    {
    }



}
