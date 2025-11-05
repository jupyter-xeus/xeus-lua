/***************************************************************************
* Copyright (c) 2021, Thorsten Beier                                       *                                                       *
* Copyright (c) 2021, QuantStack                                           *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#ifndef XLUA_INTERPRETER_HPP
#define XLUA_INTERPRETER_HPP



#include <string>
#include <memory>

#include "nlohmann/json.hpp"

#include "xeus_lua_config.hpp"
#include "xeus/xinterpreter.hpp"
#include "xeus/xrequest_context.hpp"

extern "C" {
#ifndef XEUS_LUA_USE_LUAJIT
# include "lua.h"
# include "lauxlib.h"
# include "lualib.h"
#else
#include "luajit-2.1/lua.h"
#include "luajit-2.1/lauxlib.h"
#include "luajit-2.1/lualib.h"
#endif
}
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

        void execute_request_impl(send_reply_callback cb,
                                  int execution_counter,
                                  const std::string& code,
                                  xeus::execute_request_config config,
                                  nl::json user_expressions) override;

        nl::json complete_request_impl(const std::string& code, int cursor_pos) override;

        nl::json inspect_request_impl(const std::string& code,
                                      int cursor_pos,
                                      int detail_level) override;

        nl::json is_complete_request_impl(const std::string& code) override;

        nl::json kernel_info_request_impl() override;

        void shutdown_request_impl() override;

        void redirect_output();


    private:

        // sol::state lua;
        lua_State * L;

        // allow stdin for the current request
        bool m_allow_stdin;
    };
}


#endif
