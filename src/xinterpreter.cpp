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

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <algorithm>
#include <fstream>
#include <memory>
#include <string>
#include <sstream>
#include <utility>
#include <vector>
#include <iostream>
#include <sstream>
#include <thread>

#include "nlohmann/json.hpp"

#include "xeus/xinput.hpp"
#include "xeus/xinterpreter.hpp"

#include "xeus-lua/xinterpreter.hpp"

#include "xeus-lua/sol/sol.hpp"

extern "C" {

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

}

#include "xextend.hpp"
#include "xcomplete.hpp"

#ifdef XLUA_WITH_XWIDGETS
#include "xwidgets.hpp"
#endif


#ifdef __EMSCRIPTEN__

EM_JS(char *, async_get_input_function, (const char* str), {
  return Asyncify.handleAsync(function () {
    return self.async_get_input_function( UTF8ToString(str))
    .then(function (jsString) {
      var lengthBytes = lengthBytesUTF8(jsString)+1;
      var stringOnWasmHeap = _malloc(lengthBytes);
      stringToUTF8(jsString, stringOnWasmHeap, lengthBytes);
      return stringOnWasmHeap;
    });
  });
});

#endif

namespace nl = nlohmann;









namespace xlua
{
   

    template<class sol_obj>
    auto string_func(sol::state & lua, sol_obj & v, bool co_save)
    {
        if(co_save){
            std::stringstream ss;
            switch(v.get_type())
            {
                case sol::type::none:
                    ss<<"none";
                    break;
                case sol::type::lua_nil:
                    ss<<"nil";
                    break;
                case sol::type::string:
                    ss << v. template get<std::string>();
                    break;
                case sol::type::number:
                    ss << v. template get<double>();
                    break;
                case sol::type::thread:
                    ss <<"thread";
                    break;
                case sol::type::boolean:
                    ss << v. template get<bool>();
                    break;
                case sol::type::function:
                    ss<<"function";
                    break;
                case sol::type::userdata:
                    ss<<"userdata";
                    break;
                case sol::type::lightuserdata:
                    ss<<"lightuserdata";
                    break;
                case sol::type::table:
                    ss<<"table";
                    break;
                default:
                    ss<<"unknown";

                return ss.str();
            }
            return ss.str();
        } 
        else{
            sol::main_protected_function  to_string_func = lua["tostring"];

            //return "nope";
            auto code_result = to_string_func(v);
            if (!code_result.valid()) {
                sol::error err = code_result;
                throw std::runtime_error(err.what());
            }
            return std::string(code_result);
        }
    }

    auto variadic_string_func(sol::state & lua, sol::variadic_args va, bool spaces, bool new_line, bool co_save) {
        std::stringstream ss;
        for (auto v : va) {
            ss << string_func(lua, v, co_save);
            if(spaces)
            {
                ss<<" ";
            }
        }
        if(new_line)
        {
            ss<<"\n";
        }
        return ss.str();
    };



    interpreter::interpreter()
    : m_allow_stdin(true)
    {
        
        L = lua;

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

        extend(lua);

        // replace / monkeypatch functions like
        // io.read / io.write / io.flush print 
        this->monkeypatch_io();

        // set package path
        this->set_package_path();

        // payload
        this->set_special_functions();
    }


    void interpreter::set_special_functions()
    {

        // get display table
        sol::table ilua_table = lua["ilua"];
        sol::table display_table = ilua_table["display"];
        sol::table detail_table = display_table["detail"];

        detail_table.set_function("_display_data", [this](
            const std::string & data_str,
            const std::string & metadata_str,
            const std::string & transient_str
        ){
            try
            {
                const auto data = nl::json::parse(data_str);
                const auto metadata = nl::json::parse(metadata_str);
                const auto transient = nl::json::parse(transient_str);
                this->display_data(data, metadata, transient);
            }
            catch (nl::json::parse_error& ex)
            {
                publish_execution_error("json::parse_error",ex.what(),std::vector<std::string>());
            }
        });


        detail_table.set_function("_update_display_data", [this](
            const std::string & data_str,
            const std::string & metadata_str,
            const std::string & transient_str
        ){
            try
            {
                const auto data = nl::json::parse(data_str);
                const auto metadata = nl::json::parse(metadata_str);
                const auto transient = nl::json::parse(transient_str);
                this->update_display_data(data, metadata, transient);
            }
            catch (nl::json::parse_error& ex)
            {
                publish_execution_error("json::parse_error",ex.what(),std::vector<std::string>());
            }
        });


        #ifdef XLUA_WITH_XWIDGETS
        register_xwidgets(lua);            
        #endif        
    }

    void interpreter::monkeypatch_io()
    {
        // get display table
        sol::table ilua_table = lua["ilua"];
        sol::table detail_table = ilua_table["detail"];

        auto print_func_save = [this]( sol::variadic_args va) {
            const auto s = variadic_string_func(this->lua, va, /*spaces*/true,/*newline*/true, /*cosave*/true);
            this->publish_stream("stream", s);
        };
        auto print_func_unsave = [this]( sol::variadic_args va) {
            const auto s = variadic_string_func(this->lua, va, /*spaces*/true,/*newline*/true, /*cosave*/false);
            this->publish_stream("stream", s);
        };

        auto write_func_unsave = [this]( sol::variadic_args va) {
            const auto s = variadic_string_func(this->lua, va, /*spaces*/false,/*newline*/true, /*cosave*/false);
            this->publish_stream("stream", s);
        };
        auto write_func_save = [this]( sol::variadic_args va) {
            const auto s = variadic_string_func(this->lua, va, /*spaces*/false,/*newline*/true, /*cosave*/true);
            this->publish_stream("stream", s);
        };

        detail_table.set_function("__print_save", print_func_save);
        detail_table.set_function("__print_unsave", print_func_unsave);
        lua.script(R""""(
            function print(...)
                p = ilua.config.printer
                if p == "pprint" or p == "default" then
                    pprint.pprint(...)
                elseif p == "lua" then
                    ilua.detail.__print_unsave(...)
                elseif p == "save" then
                    ilua.detail.__print_save(...)
                end
            end
        )"""");
        detail_table.set_function("__io_write_custom_save", write_func_save);
        detail_table.set_function("__io_write_custom_unsave", write_func_unsave);
        detail_table.set_function("__io_write_custom", write_func_save);
        detail_table.set_function("__io_flush_custom", [](sol::variadic_args ){});
        detail_table.set_function("__io_read_custom", 
            [this]( ) {
                if(this->m_allow_stdin)
                {
                    #ifdef __EMSCRIPTEN__
                        char* str = async_get_input_function("");
                        std::string as_string(str);
                        free(str);
                        return as_string;
                    #else
                        return xeus::blocking_input_request("", false);
                    #endif
                }
                else
                {
                    std::string error_str = "stdin is not allowed";
                    this->publish_execution_error(error_str,error_str,std::vector<std::string>());
                }
            }
        );

        const std::string monkeypatch = R""""(
            require "io"
            ilua.detail.__io_read = io.read
            ilua.detail.__io_write = io.write
            ilua.detail.__io_flush = io.flush
            function ilua.detail.__io_read_dispatch(...)
                local args = table.pack(...)
                if io.input() == io.stdin and args.n == 0 then
                    return ilua.detail.__io_read_custom(...)
                else
                    return ilua.detail.__io_read(...)
                end
            end
            io.read = ilua.detail.__io_read_dispatch

            function ilua.detail.__io_write_dispatch(...)
                if io.output() == io.stdout then
                    return ilua.detail.__io_write_custom(...)
                else
                    return ilua.detail.__io_write(...)
                end
            end
            io.write = ilua.detail.__io_write_dispatch
            function ilua.detail.__io_flush_dispatch(...)
                if io.output() == io.stdout then
                    return ilua.detail.__io_flush_custom(...)
                else
                    return ilua.detail.__io_flush(...)
                end
            end
            io.flush = ilua.detail.__io_flush_dispatch

        )"""";
        lua.script(monkeypatch);  
    }

    void interpreter::set_package_path()
    {
        #ifdef __EMSCRIPTEN__
            const std::string script = R""""(package.path =  "/asset_dir/lua_packages/?.lua;/asset_dir/lua_packages/?/init.lua")"""";
            lua.script(script);  
        #endif
    }

    interpreter::~interpreter()
    {
    }

    void interpreter::configure_impl()
    {
    }

    nl::json interpreter::execute_request_impl(int /*execution_count*/,
                                               const std::string& code,
                                               bool silent,
                                               bool store_history,
                                               nl::json user_expressions,
                                               bool allow_stdin)
    {
        m_allow_stdin = allow_stdin;
        // reset  payload
        nl::json kernel_res;


        kernel_res["payload"] = nl::json::object();
        kernel_res["status"] = "ok";

       
        // check if printing the code would yield an error
        std::stringstream test_code;
        test_code<<"pprint.pprint("<<code<<")";
        auto test_code_result = lua.script(test_code.str());
        



        if(!test_code_result.valid()){
            auto code_result= lua.script(code);

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
        nl::json matches = nl::json::array();

        int cursor_start =  complete(lua, code.c_str(), cursor_pos, matches);

        nl::json result;
        result["status"] = "ok";
        result["matches"] = matches;
        result["cursor_start"] = cursor_start;
        //result["cursor_end"] = 6;

        return result;
    }

    nl::json interpreter::inspect_request_impl(const std::string& ,
                                               int ,
                                               int )
    {
        nl::json result;
        result["status"] = "ok";
        result["found"] = true;
        result["data"] = {{"text/plain", ""}};
        result["metadata"] = {{"text/plain", ""}};
        return result;    
    }

    nl::json interpreter::is_complete_request_impl(const std::string& code)
    {
        nl::json result;
        sol::load_result code_result = lua.load(code);//, "[string]", sol::load_mode::text);
        if (code_result.valid()) {
            std::cout<<code<<" is complete\n";
            result["status"] = "complete";
        }
        else
        {
            std::cout<<code<<" is NOT complete\n";
            result["indent"] = "    ";
            result["status"] = "incomplete";
        }
        return result;
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
