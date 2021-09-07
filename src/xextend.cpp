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
        config = {
            printer = "default"
        },
        detail = {

        }
    }
    local display = ilua.display


    function display.display(...)
        args = table.pack(...)
        for i=1,args.n do
            local arg = args[i]
            arg:display()
        end
    end

    function display.with_output(output_widget, f)
        output_widget:capture()
        f()
        output:release()
    end
    return display

    
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
