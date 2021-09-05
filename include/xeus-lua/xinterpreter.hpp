/***************************************************************************
* Copyright (c) 2018, Martin Renou, Johan Mabille, Sylvain Corlay, and     *
* Wolf Vollprecht                                                          *
* Copyright (c) 2018, QuantStack
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#ifndef XLUA_INTERPRETER_HPP
#define XLUA_INTERPRETER_HPP

#ifdef __GNUC__
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wattributes"
#endif

#include <string>
#include <memory>

#include "nlohmann/json.hpp"

#include "xeus_lua_config.hpp"
#include "xeus/xinterpreter.hpp"

#include "xeus-lua/sol/sol.hpp"


//#include "xeus/xems_interpreter.hpp"

namespace nl = nlohmann;

namespace xlua
{
    class XEUS_LUA_API interpreter : public xeus::xinterpreter
    {
    public:

        interpreter();
        virtual ~interpreter();
        bool allow_stdin()const;
    protected:

        void configure_impl() override;

        nl::json execute_request_impl(int execution_counter,
                                      const std::string& code,
                                      bool silent,
                                      bool store_history,
                                      nl::json user_expressions,
                                      bool allow_stdin) override;

        nl::json complete_request_impl(const std::string& code, int cursor_pos) override;

        nl::json inspect_request_impl(const std::string& code,
                                      int cursor_pos,
                                      int detail_level) override;

        nl::json is_complete_request_impl(const std::string& code) override;

        nl::json kernel_info_request_impl() override;

        void shutdown_request_impl() override;

        void redirect_output();


    private:

        sol::state lua;
        lua_State * L;

        // allow stdin for the current request
        bool m_allow_stdin;
    };
}

#ifdef __GNUC__
    #pragma GCC diagnostic pop
#endif

#endif
