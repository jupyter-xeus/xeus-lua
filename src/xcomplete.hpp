#ifndef XLUA_XCOMPLETE_HPP
#define XLUA_XCOMPLETE_HPP

#include "xeus-lua/sol/sol.hpp"
#include "nlohmann/json.hpp"
namespace nl = nlohmann;

namespace xlua
{

int complete(
    sol::state & lua, 
    const std::string & start,
    int cursor_pos,
    nl::json & matches
);


}

#endif
