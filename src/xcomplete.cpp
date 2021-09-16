// the completed implementation is basd on:
// https://github.com/tomstitt/lupyter
// https://github.com/tomstitt/lupyter/blob/main/lupyter/lua_runtime/lua_runtime.c
// MIT License

// Copyright (c) 2021 Tom Stitt

// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:

// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <string>

#include "sol/sol.hpp"
#include "nlohmann/json.hpp"

namespace nl = nlohmann;

namespace xlua
{



bool startswith(const std::string& str, const std::string& cmp)
{
  return str.compare(0, cmp.length(), cmp) == 0;
}

const char * reserved_words[] = {
  "and", "break", "do", "else", "elseif", "end",
  "false", "for", "function", "if", "in",
  "local", "nil", "not", "or", "repeat", "return",
  "then", "true", "until", "while"
#if LUA_VERSION_NUM >= 502
  , "goto"
#endif
};




int get_metaindex(lua_State * L) {
  if (luaL_getmetafield(L, -1, "__index") == LUA_TNIL /* 0 in Lua51 */) {
    lua_pop(L, 1);
    return 0;
  }

  if (lua_type(L, -1) != LUA_TTABLE) {
    lua_pop(L, 2);
    return 0;
  }

  return 1;
}



int get_path(lua_State * L, const char * path, int path_length) {
  int offset = 0;
  char op = '.';
  lua_pushglobaltable(L);
  for (int i = 0; i < path_length; ++i) {
    if (path[i] == ':' || path[i] == '.') {
      if (lua_type(L, -1) != LUA_TTABLE && get_metaindex(L) == 0) {
        return 0;
      }
      lua_pushlstring(L, path+offset, i-offset);
      lua_gettable(L, -2);
      lua_replace(L, -2);
      op = path[i];
      offset = i+1;
    }
  }

  if (op == ':' && get_metaindex(L) == 0) {
    return 0;
  }

  if (lua_type(L, -1) != LUA_TTABLE) {
    lua_pop(L, 1);
    return 0;
  }

  return 1;
}



int is_identifier(char c){
  return isalpha(c) || std::isdigit(c) || c == '_';
}

int table_matches(lua_State * L, int table_index, const char * identifier, int identifier_length, nl::json & matches) {
  int match_count = 0;
  lua_pushnil(L);
  // fix offset after pushing
  table_index = table_index < 0 ? table_index-1 : table_index;


  while (lua_next(L, table_index)) {
    if (lua_type(L, -2) == LUA_TSTRING) {
      const char * key = lua_tostring(L, -2);
      if (strncmp(identifier, key, identifier_length) == 0) {
        std::string str_key(key);
        if(!startswith(str_key, "sol."))
        {
            matches.push_back(key);
        }
        match_count++;
      }
    }
    lua_pop(L, 1);
  }

  return match_count;
}

// TODO: this misses tables with __index metafields and indexes using []
int complete_impl(lua_State * L, const char * code, int cursor_pos, nl::json & matches) {
  int dot_loc = -1;
  int i;


  // cursor is one to the right of the starting char for completion
  cursor_pos -= 1;
  for (i = cursor_pos; i >= 0; --i) {
    if (code[i] == '.') {
      // check for string concat
      if (i > 0 && code[i-1] == '.') { break; }
      if (dot_loc == -1) { dot_loc = i; }
    }
    else if (code[i] == ':') {
      // check for '::', only in >= 5.2
        #if LUA_VERSION_NUM >= 502
        if (i > 0 && code[i-1] == ':') { break; }
        #endif
      if (dot_loc == -1) { dot_loc = i; }
      // invalid to have a colon after finding a dot/colon
      else { return 0; }
    }
    else if (!is_identifier(code[i])) {
      break;
    }
  }

  // break char is to the left of the start of the identifier
  int cursor_start = i+1;

  // don't try to match numbers
  if (std::isdigit(code[cursor_start])) { return 0; }

  int match_count = 0;
  if (dot_loc > 0) {
    const char * identifier = code+dot_loc+1;
    const char * path = code+cursor_start;
    int identifier_length = cursor_pos-dot_loc;
    int path_length = dot_loc-cursor_start+1;
    if (get_path(L, path, path_length) == 0) {
      return 0;
    }
    match_count = table_matches(L, -1, identifier, identifier_length, matches);
    // cursor_start for fields is just the start of the "basename"
    cursor_start = identifier-code;
  }

  else {
    int identifier_length = cursor_pos-cursor_start+1;
    const char * identifier = code+cursor_start;

    // check for global matches
    lua_pushglobaltable(L);
    match_count = table_matches(L, -1, identifier, identifier_length, matches);

    // check for reserved word match
    for (unsigned i = 0; i < sizeof(reserved_words)/sizeof(reserved_words[0]); ++i) {
      if (strncmp(identifier, reserved_words[i], identifier_length) == 0) {
        match_count++;
        matches.push_back(reserved_words[i]);
      }
    }
  }
  return cursor_start;
}





int complete(
    sol::state_view & lua, 
    const std::string & start,
    int cursor_pos,
    nl::json & matches
)
{
    return complete_impl(lua, start.c_str(), cursor_pos, matches);
}


}
