/***************************************************************************
* Copyright (c) 2021, Thorsten Beier                                       *                                                       *
* Copyright (c) 2021, QuantStack                                           *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#include <vector>
#include <string>
#include <iostream>

#include "sol/sol.hpp"
#include "xeus-lua/xinterpreter.hpp"
#include "xcanvas/xcanvas.hpp"
#include "nlohmann/json.hpp"
namespace nl = nlohmann;



namespace xlua
{





void setup_xcanvas(
  sol::state_view & lua
)
{
    // get display table
    sol::table ilua_table = lua["ilua"];
    sol::table display_table = ilua_table["display"];
    sol::table detail_table = display_table["detail"];



    using canvas_type = xc::canvas;
    const std::string widget_name  = "xcanvas";
    // make usertype metatable
    sol::usertype<canvas_type> canvas_lua_type = detail_table.new_usertype<canvas_type>(
        widget_name,
        // 1 constructors
        sol::constructors<canvas_type()>()
    );
    // typical member function that returns a variable
    //canvas_lua_type["display"] = &canvas_type::display;
    //canvas_lua_type["id"] = &canvas_type::id;

    canvas_lua_type[sol::meta_function::to_string] = [widget_name](canvas_type & ){
        return widget_name;
    };

    detail_table[widget_name] = canvas_lua_type;





    std::string script = R""""(

    local display = ilua.display
    
    )"""";
    sol::protected_function_result code_result  = lua.safe_script(script, &sol::script_pass_on_error);
    if (!code_result.valid()) {
        sol::error err = code_result;
        std::cerr << "failed to load string-based script into the program for xcanvas" << err.what() << std::endl;
        throw std::runtime_error(err.what());
    }

}


}
