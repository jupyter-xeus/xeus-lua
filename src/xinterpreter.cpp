/***************************************************************************
* Copyright (c) 2021, Thorsten Beier                                       *                                                       *
* Copyright (c) 2021, QuantStack                                           *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif


#include <algorithm>
#include <string>
#include <sstream>
#include <utility>
#include <vector>
#include <iostream>
#include <sstream>


#include "nlohmann/json.hpp"

#include "xeus/xhelper.hpp"
#include "xeus/xinterpreter.hpp"
#include "xeus-lua/xinterpreter.hpp"
#include "sol/sol.hpp"


namespace nl = nlohmann;

namespace xlua
{      

    std::vector<std::string> splitLines(const std::string& input) {
        std::vector<std::string> lines;
        std::istringstream stream(input);
        std::string line;

        while (std::getline(stream, line)) {
            lines.push_back(line);
        }

        return lines;
    }

    bool string_starts_with(const std::string& str, const std::string& prefix) {
        return str.size() >= prefix.size() && std::equal(prefix.begin(), prefix.end(), str.begin());
    }


    std::string trim_lines_at_end(const std::string& code) {
        auto lines = splitLines(code);
        std::vector<std::string> trimmed_lines;

        for (auto it = lines.rbegin(); it != lines.rend(); ++it) {
            auto& line = *it;

            auto first_non_whitespace = line.find_first_not_of(" \t\r\n");

            // check if line is empty or only contains whitespace
            if (first_non_whitespace == std::string::npos) {
                continue; // skip this line
            }
            // starts with a comment -- after trimming whitespace
            std::string trimmed_line = line;
            trimmed_line.erase(0, trimmed_line.find_first_not_of(" \t\r\n"));
            if (string_starts_with(trimmed_line, "--")) {
                continue; // skip this line 
            }
            trimmed_lines.push_back(line);
        }
        // reverse the order of the lines back to normal
        std::reverse(trimmed_lines.begin(), trimmed_lines.end());
        // join the lines back together
        std::ostringstream oss;
        for (size_t i = 0; i < trimmed_lines.size(); ++i) {
            oss << trimmed_lines[i];
            if (i != trimmed_lines.size() - 1) {
                oss << "\n";
            }
        }
        return oss.str();

    }


    bool has_valid_syntax(std::string code, sol::state_view & lua) {
        std::string wrapped_code = "function _xeus_lua_has_valid_syntax() \n " + code + " \n end";
        return lua.safe_script(wrapped_code, sol::script_pass_on_error).valid();
    }
    bool is_expression(std::string code, sol::state_view & lua) {
        std::string wrapped_code = "function _xeus_lua_is_expression() return \n" + code + " \n end";
        return lua.safe_script(wrapped_code, sol::script_pass_on_error).valid();
    }

    // has a semicolon at the end of the code? ignore all whitespace at the end
    bool ends_with_semicolon(const std::string & code) {
        auto it = std::find_if(code.rbegin(), code.rend(), [](unsigned char ch) {
            return !std::isspace(ch);
        });
        return it != code.rend() && *it == ';';
    }

    std::pair<std::string, std::string> split_lua_block(std::string code) {
        code.erase(code.find_last_not_of(" \n\r\t") + 1);
        size_t last_nl = code.find_last_of('\n');
        if (last_nl == std::string::npos) {
            return {"", code};
        }
        std::string head = code.substr(0, last_nl);
        std::string tail = code.substr(last_nl + 1);

        return {head, tail};
    }

    // helper function which calls 
    template<class CB, class T>
    auto handle_err(
        interpreter & interp,
        CB && cb,
        T & result, 
        const bool silent,
        std::string context
    ) {
        if (!result.valid()) {
            std::string error_msg = result.template get<sol::error>().what() + context;
            auto kernel_res = xeus::create_error_reply("Execution error in " + context, error_msg, std::vector<std::string>(1,error_msg));
            if (!silent)
            {
                interp.publish_execution_error(error_msg,error_msg,std::vector<std::string>(1,error_msg));
            }
            cb(kernel_res);
            return true;
        }
        return false;
    };


    // helper to print last value
    template<class T>
    void print_last_value(
        interpreter & interp, 
        sol::state_view & lua,
        interpreter::send_reply_callback cb,
        const T & value, 
        const bool silent,
        int execution_count
    ) {


        std::string to_print;
        if (value.get_type() == sol::type::none) {
            // print nothing for nil
            return;
        }
            
        if (value.get_type() == sol::type::string) {
            to_print = value. template get<std::string>();
            interp.publish_execution_result(execution_count, nl::json({{"text/plain", to_print}}), nl::json::object());
        }
        else if ( lua["ilua"]["detail"]["__is_redirect_file"](value).template get<bool>() ) {
            // if the value is a redirect file, print nothing, as it is not useful information for the user and clutters the output
            return;
        }
        else {
            // get mimetype representation of the value
            sol::protected_function_result lua_json_str = lua["ilua"]["display"]["mime_bundle_repr"](value);
            const std::string json_str = lua_json_str. template get<std::string>();           auto json_result = nl::json::parse(json_str);
            interp.publish_execution_result(execution_count, json_result, nl::json::object());
        }
        
    }

    inline void my_panic(sol::optional<std::string> maybe_msg)
    {
        auto & interpreter = xeus::get_interpreter();
        std::stringstream ss;
        std::cerr << "Lua is in a panic state and will now abort() the application" << std::endl;
        ss << "Lua is in a panic state and will now abort() the application" << std::endl;
        if (maybe_msg) {
            const std::string& msg = maybe_msg.value();
            std::cerr << "\terror message: " << msg << std::endl;
            ss << "\terror message: " << msg << std::endl;
        }
        // When this function exits, Lua will exhibit default behavior and abort()
        const auto error_str = ss.str();
        interpreter.publish_execution_error(error_str,error_str,std::vector<std::string>(1,error_str));
    }

    int my_exception_handler (lua_State* L, sol::optional<const std::exception&> maybe_exception, sol::string_view description)
    {
        auto & interpreter = xeus::get_interpreter();
        std::stringstream ss;
        // L is the lua state, which you can wrap in a state_view if necessary
        // maybe_exception will contain exception, if it exists
        // description will either be the what() of the exception or a description saying that we hit the general-case catch(...)
        ss << "An exception occurred in a function, here's what it says ";
        if (maybe_exception) {
            ss << "(straight from the exception): ";
            const std::exception& ex = *maybe_exception;
            ss << ex.what() << std::endl;
        }
        else {
            ss << "(from the description parameter): ";
            ss.write(description.data(), static_cast<std::streamsize>(description.size()));
            ss << std::endl;
        }
        const auto error_str = ss.str();
        interpreter.publish_execution_error(error_str,error_str,std::vector<std::string>(1,error_str));

        // you must push 1 element onto the stack to be
        // transported through as the error object in Lua
        // note that Lua -- and 99.5% of all Lua users and libraries -- expects a string
        // so we push a single string (in our case, the description of the error)
        return sol::stack::push(L, description);
    }

    // implemented in xextend.cpp
    void extend(sol::state_view & lua);
    // implemented in xcomplete.cpp
    int complete(sol::state_view & lua, const std::string & start,int cursor_pos, nl::json & matches);
    // implemented in xio.cpp
    void setup_io(sol::state_view & lua, interpreter & interp);
    // implemented in xdisplay.cpp
    void setup_display(sol::state_view & lua, interpreter & interp);
    // implemented in xjson.cpp
    void setup_json(sol::state_view & lua, interpreter & interp);

    void setup_xtime(sol::state_view & lua);

    #ifdef XLUA_WITH_XWIDGETS
    // implemented in xwidgets.cpp
    void register_xwidgets(sol::state_view & lua);
    #endif

    #ifdef XLUA_WITH_XCANVAS
    // implemented in xwidgets.cpp
    void setup_xcanvas(sol::state_view & lua);
    #endif

    interpreter::interpreter()
    : m_allow_stdin(true)
    {
        //L = lua;
        L = luaL_newstate();
        sol::state_view lua(L);

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
            sol::lib::io
            #ifndef __EMSCRIPTEN__ 
            ,
            sol::lib::ffi,
            sol::lib::jit 
            #endif
        );

        lua.set_exception_handler(&my_exception_handler);
        lua.set_panic(sol::c_call<decltype(&my_panic), &my_panic>);

        // install pure lua modules
        extend(lua);

        // replace / monkeypatch functions like
        // io.read / io.write / io.flush print 
        setup_io(lua, *this);

        // json helper functions
        setup_json(lua, *this);

        // add functions to display data
        setup_display(lua, *this);

        setup_xtime(lua);

        // add package path only do this for emscripten
        #ifdef __EMSCRIPTEN__
            const std::string script = R""""(package.path =  "/asset_dir/lua_packages/?.lua;/asset_dir/lua_packages/?/init.lua")"""";
            lua.script(script);  
        #endif

        // add widgets
        #ifdef XLUA_WITH_XWIDGETS
        register_xwidgets(lua);
        #endif

        // add widgets
        #ifdef XLUA_WITH_XCANVAS
        setup_xcanvas(lua);
        #endif
    }

    bool interpreter::allow_stdin()const
    {
        return m_allow_stdin;
    }

    interpreter::~interpreter()
    {
        lua_close(L);
    }

    void interpreter::configure_impl()
    {
    }

    void interpreter::execute_request_impl(send_reply_callback cb,
                                               int execution_count,
                                               const std::string& raw_code,
                                               xeus::execute_request_config config,
                                               nl::json user_expressions)
    {

        // some pre-processing on the code: trim empty lines and comments at the end, this allows users to put a comment at the end of the code without breaking the last value printing
        std::string code = trim_lines_at_end(raw_code);


        sol::state_view lua(L);
        m_allow_stdin = config.allow_stdin;
   
        // reset  payload
        nl::json kernel_res = xeus::create_successful_reply();

        sol::table ilua_table = lua["ilua"];
        sol::table config_table = ilua_table["config"];

        bool auto_print = config_table["auto_print"];


        // split the code into head and tail, where tail is the last line of the code and head is everything before it
        // this allows us to only auto-print the result of the last line
        auto [head, tail] = split_lua_block(code);
        bool everything_is_expression = is_expression(code, lua);
        bool head_alone_valid = true;
        bool tails_is_expression = true;
        if(everything_is_expression) {
            head_alone_valid = true;
            tails_is_expression = true;
            head = "";
            tail = code;
        }
        if (!everything_is_expression) {
            head_alone_valid = has_valid_syntax(head, lua);
            tails_is_expression = is_expression(tail, lua);
        }

        auto wrapped_cb = [this, cb = std::move(cb)](nl::json kernel_res) {
            sol::state_view lua = sol::state_view(this->L); 
            auto redirect_output = lua["ilua"]["detail"]["__redirect_output"];
            // check if function exists
            if (redirect_output.valid()) {
                redirect_output();
            }
            cb(kernel_res);
        };


        // if either head or tail alone is invalid, then we dont attempt to do any last value printing
        // => we just evalueate the whole block as is and print nothing
        if(!head_alone_valid || !tails_is_expression) {

            auto result = lua.safe_script(code, sol::script_pass_on_error);
            if(handle_err(*this, wrapped_cb, result, config.silent, "executing whole block" + code)) {
                return;
            }
            wrapped_cb(xeus::create_successful_reply());
        }
        else{
            // execute the head without printing anything
            if(!head.empty()) {
                 auto head_result = lua.safe_script(head, sol::script_pass_on_error);
                if(handle_err(*this, wrapped_cb, head_result, config.silent, "executing head block")) {
                    return;
                }
            }
            // wrap tail in a function
            std::string wrapped_tail = "function _xeus_lua_return_expression() return \n " + tail + "\n end";
            auto wrapped_tail_result = lua.safe_script(wrapped_tail, sol::script_pass_on_error);
            if(handle_err(*this, wrapped_cb, wrapped_tail_result, config.silent, "executing wrapped tail block")) {
                return;
            }
            // get the result value
            sol::protected_function  tail_func = lua["_xeus_lua_return_expression"];
            sol::protected_function_result tail_value = tail_func();


            if(!ends_with_semicolon(tail) && auto_print) {
                
                if(handle_err(*this, wrapped_cb, tail_value, config.silent, "tail function call")) {
                    return;
                }
                print_last_value(*this, lua, wrapped_cb, tail_value, config.silent, execution_count);
            }

            wrapped_cb(xeus::create_successful_reply());
        
        }

    }

    nl::json interpreter::complete_request_impl(
        const std::string& code,
        int cursor_pos)
    {
        sol::state_view lua(L);
        nl::json matches = nl::json::array();

        int cursor_start = complete(lua, code.c_str(), cursor_pos, matches);

        nl::json result = xeus::create_complete_reply(matches, cursor_start, cursor_pos);
        return result;
    }

    nl::json interpreter::inspect_request_impl(const std::string& /*code*/,
                                               int /*cursor_pos*/,
                                               int /*detail_level*/)
    {
        nl::json jresult = xeus::create_inspect_reply();
        return jresult;
    }

    nl::json interpreter::is_complete_request_impl(const std::string& code)
    {
        sol::state_view lua(L);
        bool is_complete = has_valid_syntax(code, lua);
        std::string status = is_complete ? "complete" : "incomplete";
        return xeus::create_is_complete_reply(status);
    }

    nl::json interpreter::kernel_info_request_impl()
    {
        nl::json result = xeus::create_info_reply(
            "xlua",
            "1.0.0",
            "lua",
            "14.0.0",
            "text/x-luasrc",
            ".lua"
        );
        result["banner"] = "xlua";
        return result;
    }

    nl::json interpreter::shutdown_request_impl(bool /*restart*/)
    {
        return xeus::create_shutdown_reply(false);
    }

    nl::json interpreter::interrupt_request_impl()
    {
        return xeus::create_interrupt_reply();
    }
}
