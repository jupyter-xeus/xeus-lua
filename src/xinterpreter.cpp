/***************************************************************************
* Copyright (c) 2018, Martin Renou, Johan Mabille, Sylvain Corlay, and     *
* Wolf Vollprecht                                                          *
* Copyright (c) 2018, QuantStack                                           *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/


// a lot is copied from https://raw.githubusercontent.com/tomstitt/lupyter/main/lupyter/lua_runtime/lua_runtime.c

#include <emscripten.h>
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

// #ifdef EMSCRIPTEN




// EM_ASYNC_JS(std::string, get_input, (), {
//   console.log("waiting for a fetch");
//   const response = await self.theFunc()
//   console.log("got the fetch response",response,self)
//   return response
// });


EM_JS(char *, get_input, (), {
  return Asyncify.handleAsync(function () {
    out("waiting for a fetch", self.theFunc);
    return self.theFunc("jojo").then(function (jsString) {
      out("got the fetch jsString",jsString);


      var lengthBytes = lengthBytesUTF8(jsString)+1;
      var stringOnWasmHeap = _malloc(lengthBytes);
      stringToUTF8(jsString, stringOnWasmHeap, lengthBytes);
      return stringOnWasmHeap;


      // (normally you would do something with the fetch here)
      //return jsString;
    });
  });
});


namespace nl = nlohmann;



extern "C" {

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

}

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

int get_metaindex(lua_State * L) {
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

int table_matches(lua_State * L, int table_index, const char * identifier, int identifier_length, nl::json & matches) {
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


int get_path(lua_State * L, const char * path, int path_length) {
  int offset = 0;
  char op = '.';
  lua_pushglobaltable(L);
  for (int i = 0; i < path_length; ++i) {
    if (path[i] == ':' || path[i] == '.') {
      if (lua_type(L, -1) != LUA_TTABLE && get_metaindex(L) == 0) {
        return 0;
      }
      lua_pushlstring(L, path+offset, i-offset);
      lua_gettable(L, -2);
      lua_replace(L, -2);
      op = path[i];
      offset = i+1;
    }
  }

  if (op == ':' && get_metaindex(L) == 0) {
    return 0;
  }

  if (lua_type(L, -1) != LUA_TTABLE) {
    lua_pop(L, 1);
    return 0;
  }

  return 1;
}

// TODO: this misses tables with __index metafields and indexes using []
int complete(lua_State * L, const char * code, int cursor_pos, nl::json & matches) {
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
    if (get_path(L, path, path_length) == 0) {
      return 0;
    }
    match_count = table_matches(L, -1, identifier, identifier_length, matches);
    // cursor_start for fields is just the start of the "basename"
    cursor_start = identifier-code;
  }

  else {
    int identifier_length = cursor_pos-cursor_start+1;
    const char * identifier = code+cursor_start;

    // check for global matches
    lua_pushglobaltable(L);
    match_count = table_matches(L, -1, identifier, identifier_length, matches);

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



namespace xlua
{





    interpreter::interpreter()
    {
      
        std::cout<<"register_interpreter\n";
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

        // the simpler variant of just calling luas "tostring"
        // did crazy stuff when called from co-routines
        lua.set_function("print", [this]( sol::variadic_args va) {
            std::stringstream ss;
            //ss<<"from thread"<<std::this_thread::get_id()<<" ";
            for (auto v : va) {
                switch(v.get_type())
                {
                    case sol::type::none:
                        ss<<"none ";
                        break;
                    case sol::type::lua_nil:
                        ss<<"nil ";
                        break;
                    case sol::type::string:
                        ss << v. template get<std::string>()<<" ";
                        break;
                    case sol::type::number:
                        ss << v. template get<double>()<<" ";
                        break;
                    case sol::type::thread:
                        ss <<"thread ";
                        break;
                    case sol::type::boolean:
                        ss << v. template get<bool>()<<" ";
                        break;
                    case sol::type::function:
                        ss<<"function ";
                        break;
                    case sol::type::userdata:
                        ss<<"userdata ";
                        break;
                    case sol::type::lightuserdata:
                        ss<<"lightuserdata ";
                        break;
                    case sol::type::table:
                        ss<<"table ";
                        break;

                    default:
                        ss<<"unknown ";
                }
            }
            ss<<"\n";
            std::cout<<"should print:"<<ss.str();
            this->publish_stream("stream", ss.str());
        });
     
        lua.set_function("myinput", [this]( ) {
            std::cout<<"run async\n";
            //eturn std::string("fubar");
            // .auto res =  xeus::blocking_input_request("prompt", false);
            // auto ems_this = dynamic_cast<xeus::ems_interpreter<interpreter>*>(this);
            // if(ems_this->m_async_input_func != nullptr)
            // {
            //    const std::string value = (*ems_this->m_async_input_func)().await();
                char* str = get_input();
                std::string as_string(str);
                free(str);
                std::cout<<"as_string "<<as_string<<"\n";
                return as_string;
            // }
            // else{
            //     return std::string("is nullptr");
            // }

        });
        // lua_pushlightuserdata(self->L, cbk);
        // lua_pushcclosure(self->L, this, 1);
        // lua_setglobal(self->L, "print_custom");

        L = lua;


    }

    interpreter::~interpreter()
    {
        //lua_close(p_state); 
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
        nl::json kernel_res;


        // Get payload
        kernel_res["payload"] = nl::json::object();
        kernel_res["status"] = "ok";

       
        // check if printing the code would yield an error
        std::stringstream test_code;
        test_code<<"print("<<code<<")";
        std::cout<<"test if printable\n";
        auto test_code_result = lua.script(test_code.str());
        std::cout<<"test if printable done\n";
        // only if just printing does not work we do evaluate the code itself
        if(!test_code_result.valid()){
            std::cout<<"run code: "<<code<<"\n";
            auto code_result= lua.script(code);

            if (code_result.valid())
            {
                std::cout<<"code ran fine\n";

                kernel_res["status"] = "ok";
                kernel_res["user_expressions"] = nl::json::object();
            }
            else
            {
                std::cout<<"code had an error\n";
                sol::error err = code_result;
                const auto error_str = err.what();

                if (!silent)
                {
                    std::cout<<"publish error "<<error_str<<"\n";
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

        int cursor_start =  complete(L, code.c_str(), cursor_pos, matches);

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
        return result;    }

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
       
    }


}
