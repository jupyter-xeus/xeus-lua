#include "xeus-lua/sol/sol.hpp"
#include "xeus-lua/xinterpreter.hpp"
#include "nlohmann/json.hpp"

#include "xeus/xinput.hpp"

namespace nl = nlohmann;



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


namespace xlua
{

int print_cb(lua_State * L, interpreter * interpr) {
    lua_getglobal(L, "tostring");
    for (int i = 1; i <= lua_gettop(L)-1; ++i) {
        lua_pushvalue(L, -1);
        lua_pushvalue(L, i);
        lua_call(L, 1, 1);
        const char * tostr = lua_tostring(L, -1);
        if (tostr == NULL) {
            return luaL_error(L, "'tostring' returned NULL value\n");
        }
        if (i > 1) {
            interpr->publish_stream("stream", " "); 
        }
        interpr->publish_stream("stream", tostr);
        lua_pop(L, 1);
    }
    interpr->publish_stream("stream", "\n");
    return 0;
}
int write_cb(lua_State * L, interpreter * interpr) {
    lua_getglobal(L, "tostring");
    for (int i = 1; i <= lua_gettop(L)-1; ++i) {
        lua_pushvalue(L, -1);
        lua_pushvalue(L, i);
        lua_call(L, 1, 1);
        const char * tostr = lua_tostring(L, -1);
        if (tostr == NULL) {
            return luaL_error(L, "'tostring' returned NULL value\n");
        }
        interpr->publish_stream("stream", tostr);
        lua_pop(L, 1);
    }
    return 0;
}

int my_print_lua_cb(lua_State * L) {
  interpreter * interpr = static_cast<interpreter *>(lua_touserdata(L, lua_upvalueindex(1)));
  return print_cb(L, interpr);
}
int my_write_lua_cb(lua_State * L) {
  interpreter * interpr = static_cast<interpreter *>(lua_touserdata(L, lua_upvalueindex(1)));
  return write_cb(L, interpr);
}



void setup_io(
  sol::state & lua,
  interpreter & interp
)
{
    // we use a non-sol / pure c-lua
    // solution to replace the print / io.write function
    // since a sol based function gave issues
    // when used from a co-routine


    lua_State * L = lua;
    lua_pushlightuserdata(L, &interp);
    lua_pushcclosure(L, my_write_lua_cb, 1);
    lua_setglobal(L, "__io_write_custom");


    lua_pushlightuserdata(L, &interp);
    lua_pushcclosure(L, my_print_lua_cb, 1);
    lua_setglobal(L, "__custom_print");

    lua.script(R""""(
        local __io_write_custom = _G["__io_write_custom"]
        ilua.detail.__io_write_custom = __io_write_custom
        _G["__io_write_custom"] = nil

        local __custom_print = _G["__custom_print"]
        ilua.detail.__custom_print = __custom_print
        _G["__custom_print"] = nil

        function print(...)
            p =  ilua.config.printer
            if p == "default" or p == "pretty" then
                pprint.pprint(...)
            else 
                ilua.detail.__custom_print(...)
            end
        end
    )"""");

    sol::table ilua_table = lua["ilua"];
    sol::table detail_table = ilua_table["detail"];
    auto self = &interp;
    detail_table.set_function("__io_read_custom", [self]( ) {
        if(self->allow_stdin())
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
            self->publish_execution_error(error_str,error_str,std::vector<std::string>());
            return std::string("");
        }
    });

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
                return ilua.detail.__io_write_custom('\n')
            else
                return ilua.detail.__io_flush(...)
            end
        end
        io.flush = ilua.detail.__io_flush_dispatch

    )"""";
    lua.script(monkeypatch);  

}

}
