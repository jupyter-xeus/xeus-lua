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
            std::cout<<"data string is "<<data_str<<std::endl;
            std::cout<<"metadata string is "<<metadata_str<<std::endl;
            std::cout<<"transient string is "<<transient_str<<std::endl;
            const auto data = nl::json::parse(data_str);
            const auto metadata = nl::json::parse(metadata_str);
            const auto transient = nl::json::parse(transient_str);
            std::cout<<"data is"<<data.dump(4)<<std::endl;
            std::cout<<"metadata is"<<metadata.dump(4)<<std::endl;
            std::cout<<"transient is"<<transient.dump(4)<<std::endl;
            return self->display_data(data, metadata, transient);
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


    display.mime_bundle_repr = function(data)
        -- if data has a mime_bundle_repr has
        -- a function mime_bundle_repr, use it to get the mimetype representation
        if type(data) == "table" and type(data.mime_bundle_repr) == "function" then
            local success, result = pcall(function() return data:mime_bundle_repr(mimetype) end)
            if success then
                -- if the result is a string, we assume it is a json string with mimetype as key and encoded data as value, and we pass it to the display function
                if type(result) == "string" then
                    return result
                else
                    -- assume json encodable 
                    local encoded = ilua.json.encode(result)    
                    return encoded
                end
                      

            else
                -- if the mime_bundle_repr function fails, we catch the error and print it to stderr, but we dont want to fail the whole display process, so we just return nil and let the caller handle it
                local err_msg = string.format("Error in mime_bundle_repr function for mimetype %s: %s", mimetype, result)
                ilua.detail.__io_write_to_stream("stderr", err_msg)
                return nil
            end
        else
            local str_data = pprint.pprint_str(data)
            local encoded = ilua.json.detail.string_encoder(str_data)
            return string.format('{"%s" : %s}', "text/plain", encoded)
        end
    end



    function display.display(...)
        args = table.pack(...)
        for i=1,args.n do
            local arg = args[i]
            -- if arg.display, is a function, we call it
            if type(arg) == "table" and type(arg.display) == "function" then
                arg:display()
            else
                local repr = display.mime_bundle_repr(arg)
                return ilua.display.detail._display_data(repr,"{}","{}")
            end
        end
    end

    function display.display_data(data, metadata, transient)
        local repr = display.mime_bundle_repr(data) 
        if metadata == nil then
            metadata = "{}"
        else
            metadata = ilua.json.encode(metadata)
        end
        if transient == nil then
            transient = "{}"
        else
            transient = ilua.json.encode(transient)
        end
        return ilua.display.detail._display_data(repr, metadata, transient)
    end

    function display.update_display_data(data, metadata, transient)
        local repr = display.mime_bundle_repr(data)
        if metadata == nil then
            metadata = "{}"
        else
            metadata = ilua.json.encode(metadata)
        end
        if transient == nil then
            transient = "{}"
        else
            transient = ilua.json.encode(transient)
        end
        return ilua.display.detail._update_display_data(repr, metadata, transient)
    end




    local function _display_mimetype(mimetype, data)
        return {
            mime_bundle_repr = function()
                bundle = {}
                bundle[mimetype] = data
                return ilua.json.encode(bundle)
            end
        }
    end

    function  ilua.display.plain_text(data)
        return _display_mimetype("text/plain", data)
    end
    function  ilua.display.latex(data)
        return _display_mimetype("text/latex", data)
    end
    function  ilua.display.html(data)
        return _display_mimetype("text/html", data)
    end
    function ilua.display.markdown(data)
        return _display_mimetype("text/markdown", data)
    end
    function  ilua.display.json(data)
        return _display_mimetype("application/json", data)
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
