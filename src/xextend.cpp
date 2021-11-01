/***************************************************************************
* Copyright (c) 2021, Thorsten Beier                                       *                                                       *
* Copyright (c) 2021, QuantStack                                           *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#include "sol/sol.hpp"
#include <iostream>

namespace xlua
{

void add_ilua_module(sol::state_view & lua){

    std::string script = R""""(
    ilua = {
        display = { _version = "0.1.0" ,
            detail = {}
        },
        widgets = {
            detail = {}
        },
        canvas = {
            color = {},
            detail = {}
        },
        config = {
            printer = "pprint",
            auto_print = true
        },
        detail = {},
        json = {
            detail = {}
        },
        time = {
            detail = {}
        }
    }
    )"""";
    sol::protected_function_result code_result  = lua.safe_script(script, &sol::script_pass_on_error);
    if (!code_result.valid()) {
        sol::error err = code_result;
        std::cerr << "failed to load string-based script into the program for xextend" << err.what() << std::endl;
        throw std::runtime_error(err.what());
    }
}



void extend(sol::state_view & lua)
{

    add_ilua_module(lua);
}

}
