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

void setup_json(
  sol::state_view & lua,
  interpreter & //interp
)
{
    // get display table
    sol::table ilua_table = lua["ilua"];
    sol::table json_table = ilua_table["json"];
    sol::table detail_table = json_table["detail"];

    //auto self = &interp;

    detail_table.set_function("string_encoder",[](
        const std::string & unencoded
    ){
        nl::json j = unencoded;
        std::string s = j.dump();
        return s;
    });

    detail_table.set_function("number_encoder",[](
        const double & unencoded
    ){
        nl::json j = unencoded;
        std::string s = j.dump();
        return s;
    });

    std::string script = R""""(
local is_array
local stringify_encoded_dict
local stringify_encoded_array
local key_to_string
local json_encoder_impl
local table_encoder

is_array = function(t)
  local i = 0
  for _ in pairs(t) do
      i = i + 1
      if t[i] == nil then return false end
  end
  return true
end

stringify_encoded_array = function(t)
    return "[" .. table.concat(t, ",") .. "]"
end

stringify_encoded_dict = function(t)
    return "{" .. table.concat(t, ",") .. "}"
end

key_to_string = function(key)
    if type(key) ~= "string" then
        error("json_encoder error: only string keys are allowed" .. type(key))
    else
        return key
    end
end


encode_kv_pair = function(key, value, cycle_detection)
    str_key = key_to_string(key)
    return json_encoder_impl(str_key, cycle_detection) .. ":" .. json_encoder_impl(value, cycle_detection)
end

table_encoder = function(t, cycle_detection)
    if cycle_detection[t] then 
        error("json_encoder error: detected cyclic reference") 
    end
    cycle_detection[t] = true
    local ret = {}
    if is_array(t) then
        for i, v in ipairs(t) do
          table.insert(ret, json_encoder_impl(v, cycle_detection))
        end
        cycle_detection[t] = false
        return stringify_encoded_array(ret)
    else
        for key,val in pairs(t) do           
            table.insert(ret, encode_kv_pair(key, val, cycle_detection))
        end
        cycle_detection[t] = false
        return stringify_encoded_dict(ret)
    end

end


local encoder_registry = {
  boolean = tostring,
  string = ilua.json.detail.string_encoder,
  number = ilua.json.detail.number_encoder,
  table = table_encoder
}

json_encoder_impl =  function(object, cycle_detection)
    cycle_detection = cycle_detection or {}
    local type_of_object = type(object)
    local encoder = encoder_registry[type_of_object]
    if type_of_object == "table" then
        return encoder(object, cycle_detection)
    elseif encoder ~=nil then
        return encoder(object)
    else
        error(string.format("cannot json_encode an object of type: %s",type_of_object))
    end
end

function ilua.json.encode(object)
    local cycle_detection = {}
    return json_encoder_impl(object, cycle_detection)
end

    )"""";
    sol::protected_function_result code_result  = lua.safe_script(script, &sol::script_pass_on_error);
    if (!code_result.valid()) {
        sol::error err = code_result;
        std::cerr << "failed to load string-based script into the program for xjson" << err.what() << std::endl;
        throw std::runtime_error(err.what());
    }

}

}
