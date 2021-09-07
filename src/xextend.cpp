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


    local function display_data(data,metadata,transient)
        display.detail._display_data(
            json.encode(data),
            json.encode(metadata),
            json.encode(transient)
        )
    end

    local function display.mimetype(mimetype, data)
        data = {
            [mimetype] = data
        }
        display_data(data, {}, {})
    end

    local function display.plain_text(str)
        display.mimetype("text/plain", str)
    end

    local function display.html(html)
        display.mimetype("text/html", html)
    end

    local function display.json(j)
        display.mimetype("application/json", j)
    end

    local function display.latex(j)
        display.mimetype("text/latex", j)
    end


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
    auto code_result = lua.script(script);
    if (!code_result.valid()) {
        sol::error err = code_result;
        std::cerr << "failed to load string-based script into the program" << err.what() << std::endl;
        throw std::runtime_error(err.what());
    }
}


void extend(sol::state_view & lua)
{

    add_ilua_module(lua);
}

}
