/***************************************************************************
* Copyright (c) 2021, Thorsten Beier                                       *                                                       *
* Copyright (c) 2021, QuantStack                                           *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

// the complete method is inspired by
// https://github.com/tomstitt/lupyter
// https://github.com/tomstitt/lupyter/blob/main/lupyter/lua_runtime/lua_runtime.c

#include <iostream>
#include <sstream>
#include <string>
#include <array>

#include "sol/sol.hpp"
#include "nlohmann/json.hpp"

namespace nl = nlohmann;

namespace xlua
{

inline bool startswith(const std::string& str, const std::string& cmp)
{
    return str.compare(0, cmp.length(), cmp) == 0;
}

std::vector<std::string> split_string(const std::string & str, const char split_on)
{
    std::istringstream iss(str);
    std::vector<std::string> tokens;
    std::string token;
    while (std::getline(iss, token, split_on)) {
        if (!token.empty())
        {
            tokens.push_back(token);
        }
    }
    return tokens;
}

std::vector<std::string> split_string_dot(const std::string & inputString)
{
    std::vector<std::string> result_tokens;
    for(auto tokens : split_string(inputString,':'))
    {
        for(const auto & inner_token : split_string(tokens,'.'))
        {
            result_tokens.push_back(inner_token);
        }
    }
    return result_tokens;
}

class Constants
{
public:
    static inline std::array<std::string,22> const keywords =  
    {
        "and",    "break",  "do",
        "else",   "elseif", "end",
        "false",  "for",    "function", 
        "if",     "in",     "local", 
        "nil",    "not",    "or", 
        "repeat", "return", "then", 
        "true",   "until",  "while", 
        "goto"
    };
};

std::pair<bool, sol::table> get_table_from_path(sol::state_view & lua, const std::vector<std::string> & path)
{
    std::size_t i=0;
    sol::table current_table = lua["_G"];
    while(i < path.size())
    {
        sol::object as_obj = current_table[path[i]];
        sol::type t = as_obj.get_type();
        if (t != sol::type::table)
        {
            return std::make_pair(false, current_table);   
        }

        current_table = current_table[path[i]];
        if(current_table == sol::lua_nil)
        {
            return std::make_pair(false, current_table);
        }
        ++i;
    }
    return std::make_pair(true, current_table);;
}


template<class table_like>
void match_from_table_like(table_like & table, const std::string & to_match, nl::json & matches)
{
    for (auto& [k, v] : table)
    { 
        auto stringkey = k.template as<std::string>();
        if(startswith(stringkey, to_match))
        {
          matches.push_back(stringkey);
        }
    }
}

int complete(sol::state_view & lua, const std::string & code, int cursor_pos, nl::json & matches)
{
    auto is_dot = [&](char c)
    {
       return c == '.' || c == ':';
    };

    auto is_identifier = [](char c){
      return std::isalpha(c) || std::isdigit(c) || c == '_';
    };

    int cursor_start;
    int dotpos = -1;
    bool found_dot = false;    
    char split_on = ' ';
    --cursor_pos;

    // some extra scope to not have the "i" in the scope above
    {
        int i;
        for (i = cursor_pos; i >= 0; --i) 
        {
            if (is_dot(code[i]))
            {
                if (i > 0 && is_dot(code[i-1]))
                { 
                    break; 
                }
                else if (!found_dot)
                { 
                    dotpos = i; 
                    split_on = code[i];
                    found_dot = true;
                }
                else if(code[i] == ':')
                {
                    return 0; 
                }
            }
            else if(!is_identifier(code[i]))
            {
                break;
            }
        }

        cursor_start = i+1;
    }


    if (std::isdigit(code[cursor_start]))
    { 
        return 0; 
    }

    if (found_dot && dotpos > 0)
    {
        const std::string to_match = code.substr(dotpos + 1, cursor_pos-dotpos);
        const std::string path = code.substr(cursor_start, dotpos-cursor_start+1);
        auto path_tokens = split_string_dot(path);
        auto [has_path, table] = get_table_from_path(lua, path_tokens);
        if(has_path)
        {
            match_from_table_like(table, to_match, matches);
            cursor_start = dotpos + 1;
        }
        else // path / table at path does not exist
        {
            return 0;
        }
    }

    else 
    {
        const std::string to_match = code.substr(cursor_start, cursor_pos-cursor_start+1);

        // check for global matches
        match_from_table_like(lua, to_match, matches);

        // check for kw matches
        for(auto kw : Constants::keywords)
        {
            if(startswith(kw, to_match))
            {
                matches.push_back(kw);
            }
        }
    }
    return cursor_start;
}

}
