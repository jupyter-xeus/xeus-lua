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
#include "nlohmann/json.hpp"
namespace nl = nlohmann;



namespace xlua
{




void setup_display(
  sol::state_view & lua,
  interpreter & interp
)
{
    // get display table
    sol::table ilua_table = lua["ilua"];
    sol::table display_table = ilua_table["display"];
    sol::table detail_table = display_table["detail"];

    auto self = &interp;

    detail_table.set_function("_display_data", [self](
        const std::string & data_str,
        const std::string & metadata_str,
        const std::string & transient_str
    ){
        try
        {
            const auto data = nl::json::parse(data_str);
            const auto metadata = nl::json::parse(metadata_str);
            const auto transient = nl::json::parse(transient_str);
            self->display_data(data, metadata, transient);
        }
        catch (nl::json::parse_error& ex)
        {
            self->publish_execution_error("json::parse_error",ex.what(),std::vector<std::string>());
        }
    });

    display_table.set_function("clear_output", [self](
        const bool wait
    ){
        
            self->clear_output(wait);
    });

    detail_table.set_function("_update_display_data", [self](
        const std::string & data_str,
        const std::string & metadata_str,
        const std::string & transient_str
    ){
        try
        {
            const auto data = nl::json::parse(data_str);
            const auto metadata = nl::json::parse(metadata_str);
            const auto transient = nl::json::parse(transient_str);
            self->update_display_data(data, metadata, transient);
        }
        catch (nl::json::parse_error& ex)
        {
            self->publish_execution_error("json::parse_error",ex.what(),std::vector<std::string>());
        }
    });

    std::string script = R""""(

    local display = ilua.display


    function display.display(...)
        args = table.pack(...)
        for i=1,args.n do
            local arg = args[i]
            arg:display()
        end
    end


    local function display_mimetype(mimetype, encoded_str)
        local data_json_str = string.format('{"%s" : %s}', mimetype, encoded_str)
        return ilua.display.detail._display_data(data_json_str,"{}","{}")
    end
    function  ilua.display.plain_text(data)
        local encoded = ilua.json.detail.string_encoder(data)
        display_mimetype("text/plain", encoded)
    end
    function  ilua.display.latex(data)
        local encoded = ilua.json.detail.string_encoder(data)
        display_mimetype("text/latex", encoded)
    end
    function  ilua.display.html(data)
        local encoded = ilua.json.detail.string_encoder(data)
        display_mimetype("text/html", encoded)
    end
    function  ilua.display.json(jsondata)
        local data = {
            ["application/json"] = jsondata
        }
        local encoded = ilua.json.encode(data)
        return ilua.display.detail._display_data(encoded,"{}","{}")
    end

    
    )"""";
    sol::protected_function_result code_result  = lua.safe_script(script, &sol::script_pass_on_error);
    if (!code_result.valid()) {
        sol::error err = code_result;
        std::cerr << "failed to load string-based script into the program for xdisplay" << err.what() << std::endl;
        throw std::runtime_error(err.what());
    }

}


}
