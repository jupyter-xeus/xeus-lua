/***************************************************************************
* Copyright (c) 2018, Martin Renou, Johan Mabille, Sylvain Corlay, and     *
* Wolf Vollprecht                                                          *
* Copyright (c) 2018, QuantStack                                           *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#include <algorithm>
#include <fstream>
#include <memory>
#include <string>
#include <sstream>
#include <utility>
#include <vector>
#include <iostream>
#include <sstream>
#include "nlohmann/json.hpp"

#include "xeus/xinterpreter.hpp"

#include "xeus-lua/xinterpreter.hpp"

#include "xeus-lua/sol/sol.hpp"

namespace nl = nlohmann;




// static int l_my_print(lua_State* L) {
//     int nargs = lua_gettop(L);

//     for (int i=1; i <= nargs; i++) {
//         if (lua_isstring(L, i)) {
//             /* Pop the next arg using lua_tostring(L, i) and do your print */
//             std::cout<<"custom print "<< lua_tostring(L, i) <<"\n";
//         }
//         else {
//          Do something with non-strings if you like 
//         }
//     }

//     return 0;
// }

// static const struct luaL_Reg printlib [] = {
//   {"print", l_my_print},
//   {NULL, NULL} /* end of array */
// };

// int fubar(lua_State *L)
// {
//   lua_getglobal(L, "_G");
//   // luaL_register(L, NULL, printlib); // for Lua versions < 5.2
//   luaL_setfuncs(L, printlib, 0);  // for Lua versions 5.2 or greater
//   lua_pop(L, 1);
// }


namespace xlua
{





    interpreter::interpreter()
    {
        std::cout<<"register_interpreter\n";
        xeus::register_interpreter(this);


        // lua.set_function("print",
        //     sol::overload(
        //         [this] (std::string t) {
        //             std::cout<<"my print"<<t<<"\n"; 
        //             this->publish_stream("stream",t);
        //         },
        //         [this] (std::string t, float c) {
        //             std::cout<<"my print"<<t<<" c "<<c<<"\n"; 
        //             this->publish_stream("stream",t);
        //         }
        //     )
        // );

        lua.set_function("print", [this]( sol::variadic_args va) {
            std::stringstream ss;
            for (auto v : va) {
                if(v.is<std::string>())
                {   
                    ss<< v.as<std::string>() <<" ";
                }
                else if(v.is<int>())
                {   
                    ss<< v.as<int>() <<" ";
                }
            }
            this->publish_stream("stream", ss.str());
        });
     

    }

    interpreter::~interpreter()
    {
        //lua_close(p_state); 
    }

    void interpreter::configure_impl()
    {
    }

    nl::json interpreter::execute_request_impl(int /*execution_count*/,
                                               const std::string& code,
                                               bool silent,
                                               bool store_history,
                                               nl::json user_expressions,
                                               bool allow_stdin)
    {
        nl::json kernel_res;


        // Get payload
        kernel_res["payload"] = nl::json::object();



        auto code_result= lua.script(code);



        if (code_result.valid())
        {

                kernel_res["status"] = "ok";
                kernel_res["user_expressions"] = nl::json::object();
        
        }
        else
        {
            sol::error err = code_result;
            const auto error_str = err.what();

            if (!silent)
            {
                std::cout<<"publish error "<<error_str<<"\n";
                publish_execution_error(error_str,error_str,std::vector<std::string>(1,error_str));
            }

            kernel_res["status"] = "error";
            kernel_res["ename"] = "load file error";
            kernel_res["evalue"] = error_str;
            kernel_res["traceback"] = error_str;

        }

        return kernel_res;
    }

    nl::json interpreter::complete_request_impl(
        const std::string& code,
        int cursor_pos)
    {
        nl::json result;
        result["status"] = "ok";
        result["matches"] = {"a.test1", "a.test2"};
        result["cursor_start"] = 2;
        result["cursor_end"] = 6;

        return result;
    }

    nl::json interpreter::inspect_request_impl(const std::string& code,
                                               int cursor_pos,
                                               int detail_level)
    {
        nl::json result;
        result["status"] = "ok";
        result["found"] = true;
        result["data"] = {{"text/plain", ""}};
        result["metadata"] = {{"text/plain", ""}};
        return result;    }

    nl::json interpreter::is_complete_request_impl(const std::string& code)
    {
        nl::json result;
        result["status"] = code;
        if (code.compare("incomplete") == 0)
        {
            result["indent"] = "   ";
        }
        return result;
    }

    nl::json interpreter::kernel_info_request_impl()
    {
        nl::json result;
        result["implementation"] = "xlua";
        result["implementation_version"] = "1.0.0";
        result["banner"] = "xlua";
        result["language_info"]["name"] = "lua";
        result["language_info"]["version"] = "14.0.0";
        result["language_info"]["mimetype"] = "text/x-luasrc";
        result["language_info"]["file_extension"] = ".lua";
        return result;
    }

    void interpreter::shutdown_request_impl()
    {
    }

    nl::json interpreter::internal_request_impl(const nl::json& content)
    {
       
    }


}
