/***************************************************************************
* Copyright (c) 2021, Thorsten Beier                                       *                                                       *
* Copyright (c) 2021, QuantStack                                           *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/


#ifdef XEUS_LUA_EMSCRIPTEN_WASM_BUILD
#include "emscripten.h"
#else
#include <thread>
#endif

#include <chrono>
#include <string>
#include <iostream>

#include "sol/sol.hpp"
#include "xeus-lua/xinterpreter.hpp"
#include "nlohmann/json.hpp"

namespace nl = nlohmann;

namespace xlua
{

using time_point_t = std::chrono::time_point<std::chrono::system_clock>;

void setup_xtime(sol::state_view & lua)
{
    // get display table
    sol::table ilua_table = lua["ilua"];
    sol::table time_table = ilua_table["time"];
    sol::table detail_table = time_table["detail"];

    // make usertype metatable
    sol::usertype<time_point_t> canvas_lua_type = detail_table.new_usertype<time_point_t>(
        "time_point",
        // 1 constructors
        sol::constructors<time_point_t()>()
    );

    time_table["now"] = [](){
        return std::chrono::system_clock::now();
    }; 

    time_table["time_delta_ms"] = [](const time_point_t & t_start, const time_point_t & t_end){
        return std::chrono::duration<double, std::milli>(t_end-t_start).count();
    };

    time_table["sleep_ms"] = sol::overload(
      [](const double x) {
#ifdef XEUS_LUA_EMSCRIPTEN_WASM_BUILD
          emscripten_sleep(x);
#else
          std::this_thread::sleep_for(std::chrono::milliseconds(int(x+0.5)));
#endif
      }, [](const int x) {
#ifdef XEUS_LUA_EMSCRIPTEN_WASM_BUILD
          emscripten_sleep(x);
#else
          std::this_thread::sleep_for(std::chrono::milliseconds(x));
#endif
    });

    // patch the print function with pure lua code
    std::string script = R""""(
        ilua.time["scheduled_ms"] = function(n, delta_ms, f)
            time_begin_outer = ilua.time.now()
            local _f = function(iter)
                time_begin = ilua.time.now()

                -- delta since begin
                local d_begin = ilua.time.time_delta_ms(time_begin_outer, time_begin)

                f(iter, d_begin)
                time_end = ilua.time.now()
                delta = ilua.time.time_delta_ms(time_begin, time_end)
                dd = delta_ms - delta
                if dd > 0 then
                    ilua.time.sleep_ms(dd)
                end
            end
            if n <= 0 then
                local iter = 1
                while true do
                    _f(iter)
                    iter = iter + 1
                end
            else 
                for iter=1,n do
                    _f(iter)
                end
            end
        end

        ilua.time["scheduled_fps"] = function(n, fps, f)
            local t_ms = 1000.0 / fps
            ilua.time.scheduled_ms(n, t_ms, f)
        end
        ilua.time["duration_ms"] = function( f)
            local t_begin = ilua.time.now()
            f()
            local t_end = ilua.time.now()
            return ilua.time.time_delta_ms(t_begin, t_end)
        end
    )"""";
    sol::protected_function_result code_result  = lua.safe_script(script, &sol::script_pass_on_error);
    if (!code_result.valid()) {
        sol::error err = code_result;
        std::cerr << "failed to load string-based script into the program for xtime" << err.what() << std::endl;
        throw std::runtime_error(err.what());
    }
}

}
