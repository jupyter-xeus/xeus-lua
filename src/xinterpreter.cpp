/***************************************************************************
* Copyright (c) 2018, Martin Renou, Johan Mabille, Sylvain Corlay, and     *
* Wolf Vollprecht                                                          *
* Copyright (c) 2018, QuantStack                                           *
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
    const char * reserved_words[] = {
      "and", "break", "do", "else", "elseif", "end",
      "false", "for", "function", "if", "in",
      "local", "nil", "not", "or", "repeat", "return",
      "then", "true", "until", "while"
    #if LUA_VERSION_NUM >= 502
      , "goto"
    #endif
    };




    int is_identifier(char c){
      return isalpha(c) || std::isdigit(c) || c == '_';
    }

    auto string_func( sol::variadic_args va, bool spaces, bool new_line) {
        std::stringstream ss;
        for (auto v : va) {
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
            }
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
            sol::lib::io//,
            // sol::lib::ffi,
            //sol::lib::jit,
        );

        // replace / monkeypatch functions like
        // io.read / io.write / io.flush print 
        this->monkeypatch_io();

        // set package path
        this->set_package_path();

        // payload
        this->set_special_functions();
    }

    // TODO: this misses tables with __index metafields and indexes using []
    int interpreter::complete(const char * code, int cursor_pos, nl::json & matches) {
      int dot_loc = -1;
      int i;


      // cursor is one to the right of the starting char for completion
      cursor_pos -= 1;
      for (i = cursor_pos; i >= 0; --i) {
        if (code[i] == '.') {
          // check for string concat
          if (i > 0 && code[i-1] == '.') { break; }
          if (dot_loc == -1) { dot_loc = i; }
        }
        else if (code[i] == ':') {
          // check for '::', only in >= 5.2
            #if LUA_VERSION_NUM >= 502
            if (i > 0 && code[i-1] == ':') { break; }
            #endif
          if (dot_loc == -1) { dot_loc = i; }
          // invalid to have a colon after finding a dot/colon
          else { return 0; }
        }
        else if (!is_identifier(code[i])) {
          break;
        }
      }

      // break char is to the left of the start of the identifier
      int cursor_start = i+1;

      // don't try to match numbers
      if (std::isdigit(code[cursor_start])) { return 0; }

      int match_count = 0;
      if (dot_loc > 0) {
        const char * identifier = code+dot_loc+1;
        const char * path = code+cursor_start;
        int identifier_length = cursor_pos-dot_loc;
        int path_length = dot_loc-cursor_start+1;
        if (this->get_path(path, path_length) == 0) {
          return 0;
        }
        match_count = this->table_matches(-1, identifier, identifier_length, matches);
        // cursor_start for fields is just the start of the "basename"
        cursor_start = identifier-code;
      }

      else {
        int identifier_length = cursor_pos-cursor_start+1;
        const char * identifier = code+cursor_start;

        // check for global matches
        lua_pushglobaltable(L);
        match_count = this->table_matches(-1, identifier, identifier_length, matches);

        // check for reserved word match
        for (unsigned i = 0; i < sizeof(reserved_words)/sizeof(reserved_words[0]); ++i) {
          if (strncmp(identifier, reserved_words[i], identifier_length) == 0) {
            match_count++;
            matches.push_back(reserved_words[i]);
          }
        }
      }
      return cursor_start;
    }

    int interpreter::table_matches(int table_index, const char * identifier, int identifier_length, nl::json & matches) {
      int match_count = 0;
      lua_pushnil(L);
      // fix offset after pushing
      table_index = table_index < 0 ? table_index-1 : table_index;


      while (lua_next(L, table_index)) {
        if (lua_type(L, -2) == LUA_TSTRING) {
          const char * key = lua_tostring(L, -2);
          if (strncmp(identifier, key, identifier_length) == 0) {
            matches.push_back(key);
            match_count++;
          }
        }
        lua_pop(L, 1);
      }

      return match_count;
    }

    int interpreter::get_metaindex() {
      if (luaL_getmetafield(L, -1, "__index") == LUA_TNIL /* 0 in Lua51 */) {
        lua_pop(L, 1);
        return 0;
      }

      if (lua_type(L, -1) != LUA_TTABLE) {
        lua_pop(L, 2);
        return 0;
      }

      return 1;
    }


    int interpreter::get_path(const char * path, int path_length) {
      int offset = 0;
      char op = '.';
      lua_pushglobaltable(L);
      for (int i = 0; i < path_length; ++i) {
        if (path[i] == ':' || path[i] == '.') {
          if (lua_type(L, -1) != LUA_TTABLE && this->get_metaindex() == 0) {
            return 0;
          }
          lua_pushlstring(L, path+offset, i-offset);
          lua_gettable(L, -2);
          lua_replace(L, -2);
          op = path[i];
          offset = i+1;
        }
      }

      if (op == ':' && this->get_metaindex() == 0) {
        return 0;
      }

      if (lua_type(L, -1) != LUA_TTABLE) {
        lua_pop(L, 1);
        return 0;
      }

      return 1;
    }

    void interpreter::set_special_functions()
    {

        lua.set_function("_display_data", [this](
            const std::string & data_str,
            const std::string & metadata_str,
            const std::string & transient_str
        ){
            try
            {
                const auto data = nl::json::parse(data_str);
                const auto metadata = nl::json::parse(data_str);
                const auto transient = nl::json::parse(data_str);
                std::cout<<"display data\n";
                this->display_data(data, metadata, transient);
                std::cout<<"display data done\n";
            }
            catch (nl::json::parse_error& ex)
            {
                publish_execution_error("json::parse_error",ex.what(),std::vector<std::string>());
            }
        });


        lua.set_function("_update_display_data", [this](
            const std::string & data_str,
            const std::string & metadata_str,
            const std::string & transient_str
        ){
            try
            {
                const auto data = nl::json::parse(data_str);
                const auto metadata = nl::json::parse(data_str);
                const auto transient = nl::json::parse(data_str);
                std::cout<<"update display data\n";
                this->update_display_data(data, metadata, transient);
                std::cout<<"update display data done\n";
            }
            catch (nl::json::parse_error& ex)
            {
                publish_execution_error("json::parse_error",ex.what(),std::vector<std::string>());
            }
        });


        // lua.set_function("_fetch", [this](
        //     const std::string & url,
        //     const std::string & filename
        // ){
            


        //     emscripten_fetch_attr_t attr;
        //     emscripten_fetch_attr_init(&attr);
        //     strcpy(attr.requestMethod, "GET");
        //     attr.attributes = EMSCRIPTEN_FETCH_LOAD_TO_MEMORY | EMSCRIPTEN_FETCH_SYNCHRONOUS;
        //     emscripten_fetch_t *fetch = emscripten_fetch(&attr, "file.dat"); // Blocks here until the operation is complete.
        //     if (fetch->status == 200) {
        //     printf("Finished downloading %llu bytes from URL %s.\n", fetch->numBytes, fetch->url);
        //     // The data is now available at fetch->data[0] through fetch->data[fetch->numBytes-1];
        //     } else {
        //     printf("Downloading %s failed, HTTP failure status code: %d.\n", fetch->url, fetch->status);
        //     }
        //     emscripten_fetch_close(fetch);
            
        // });

        #ifdef XLUA_WITH_XWIDGETS

        register_xwidgets(lua);

        using xwidgtes_type = xw::slider<double>;
        const std::string widget_name = "xslider";
        
            
        #endif        
    }

    void interpreter::monkeypatch_io()
    {

        auto print_func = [this]( sol::variadic_args va) {
            const auto s = string_func(va, /*spaces*/true,/*newline*/true);
            this->publish_stream("stream", s);
        };

        auto write_func = [this]( sol::variadic_args va) {
            const auto s = string_func(va, /*spaces*/false,/*newline*/true);
            this->publish_stream("stream", s);
        };

        lua.set_function("print", print_func);
        lua.set_function("__io_write_custom", write_func);
        lua.set_function("__io_flush_custom", [](sol::variadic_args va){});
        lua.set_function("__io_read_custom", 
            [this]( ) {
                #ifdef __EMSCRIPTEN__
                    char* str = async_get_input_function("");
                    std::string as_string(str);
                    free(str);
                    return as_string;
                #else
                    return xeus::blocking_input_request("", false);
                #endif
            }
        );

        const std::string monkeypatch = R""""(
            require "io"
            __io_read = io.read
            __io_write = io.write
            __io_flush = io.flush
            function __io_read_dispatch(...)
                local args = table.pack(...)
                if io.input() == io.stdin and args.n == 0 then
                    return __io_read_custom(...)
                else
                    return __io_read(...)
                end
            end
            io.read = __io_read_dispatch

            function __io_write_dispatch(...)
                if io.output() == io.stdout then
                    return __io_write_custom(...)
                else
                    return __io_write(...)
                end
            end
            io.write = __io_write_dispatch
            function __io_flush_dispatch(...)
                if io.output() == io.stdout then
                    return __io_flush_custom(...)
                else
                    return __io_flush(...)
                end
            end
            io.flush = __io_flush_dispatch

        )"""";
        std::cout<<"monkeypatch\n";
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


        // reset  payload
        nl::json kernel_res;


        kernel_res["payload"] = nl::json::object();
        kernel_res["status"] = "ok";

       
        // check if printing the code would yield an error
        std::stringstream test_code;
        test_code<<"print("<<code<<")";
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

        int cursor_start =  this->complete(code.c_str(), cursor_pos, matches);

        nl::json result;
        result["status"] = "ok";
        result["matches"] = matches;
        result["cursor_start"] = cursor_start;
        //result["cursor_end"] = 6;

        return result;
    }

    nl::json interpreter::inspect_request_impl(const std::string& code,
                                               int cursor_pos,
                                               int detail_level)
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
        result["status"] = code;
        if (code.compare("incomplete") == 0)
        {
            result["indent"] = "   ";
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

    nl::json interpreter::internal_request_impl(const nl::json& content)
    {
       return nl::json::object();
    }


}
