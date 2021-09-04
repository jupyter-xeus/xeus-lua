#include "xeus-lua/sol/sol.hpp"

namespace xlua
{

void add_pprint_module(sol::state & lua){

    std::string script = R""""(
    -- Chen Tao - jagttt@gmail.com
    -- This is free and unencumbered software released into the public domain.
    -- Anyone is free to copy, modify, publish, use, compile, sell, or
    -- distribute this software, either in source code form or as a compiled
    -- binary, for any purpose, commercial or non-commercial, and by any
    -- means.
    -- In jurisdictions that recognize copyright laws, the author or authors
    -- of this software dedicate any and all copyright interest in the
    -- software to the public domain. We make this dedication for the benefit
    -- of the public at large and to the detriment of our heirs and
    -- successors. We intend this dedication to be an overt act of
    -- relinquishment in perpetuity of all present and future rights to this
    -- software under copyright law.
    -- THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
    -- EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
    -- MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
    -- IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
    -- OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
    -- ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
    -- OTHER DEALINGS IN THE SOFTWARE.
    -- For more information, please refer to <http://unlicense.org/>

    pprint = { VERSION = '0.1' }

    local depth = 1

    pprint.defaults = {
        -- If set to number N, then limit table recursion to N deep.
        depth_limit = false,
        -- type display trigger, hide not useful datatypes by default
        -- custom types are treated as table
        show_nil = true,
        show_boolean = true,
        show_number = true,
        show_string = true,
        show_table = true,
        show_function = true,
        show_thread = true,
        show_userdata = false,
        -- additional display trigger
        show_metatable = false,     -- show metatable
        show_all = false,           -- override other show settings and show everything
        use_tostring = true,       -- use __tostring to print table if available
        filter_function = nil,      -- called like callback(value[,key, parent]), return truty value to hide
        object_cache = false,     -- cache blob and table to give it a id, 'local' cache per print, 'global' cache
                                    -- per process, falsy value to disable (might cause infinite loop)
        -- format settings
        indent_size = 2,            -- indent for each nested table level
        level_width = 80,           -- max width per indent level
        wrap_string = true,         -- wrap string when it's longer than level_width
        wrap_array = false,         -- wrap every array elements
        sort_keys = true,           -- sort table keys
    }

    local TYPES = {
        ['nil'] = 1, ['boolean'] = 2, ['number'] = 3, ['string'] = 4, 
        ['table'] = 5, ['function'] = 6, ['thread'] = 7, ['userdata'] = 8
    }

    -- seems this is the only way to escape these, as lua don't know how to map char '\a' to 'a'
    local ESCAPE_MAP = {
    }
    local ESCAPE_MAP_ORG = {
        ['\a'] = '\\a', ['\b'] = '\\b', ['\f'] = '\\f', ['\n'] = '\\n', ['\r'] = '\\r',
        ['\t'] = '\\t', ['\v'] = '\\v', ['\\'] = '\\\\',
    }

    -- generic utilities
    local function escape(s)
        s = s:gsub('([%c\\])', ESCAPE_MAP)
        local dq = s:find('"') 
        local sq = s:find("'")
        if dq and sq then
            return s:gsub('"', '\\"'), '"'
        elseif sq then
            return s, '"'
        else
            return s, "'"
        end
    end

    local function is_plain_key(key)
        return type(key) == 'string' and key:match('^[%a_][%a%d_]*$')
    end

    local CACHE_TYPES = {
        ['table'] = true, ['function'] = true, ['thread'] = true, ['userdata'] = true
    }

    -- cache would be populated to be like:
    -- {
    --     function = { `fun1` = 1, _cnt = 1 }, -- object id
    --     table = { `table1` = 1, `table2` = 2, _cnt = 2 },
    --     visited_tables = { `table1` = 7, `table2` = 8  }, -- visit count
    -- }
    -- use weakrefs to avoid accidentall adding refcount
    local function cache_apperance(obj, cache, option)
        if not cache.visited_tables then
            cache.visited_tables = setmetatable({}, {__mode = 'k'})
        end
        local t = type(obj)

        -- TODO can't test filter_function here as we don't have the ix and key,
        -- might cause different results?
        -- respect show_xxx and filter_function to be consistent with print results
        if (not TYPES[t] and not option.show_table)
            or (TYPES[t] and not option['show_'..t]) then
            return
        end

        if CACHE_TYPES[t] or TYPES[t] == nil then
            if not cache[t] then
                cache[t] = setmetatable({}, {__mode = 'k'})
                cache[t]._cnt = 0
            end
            if not cache[t][obj] then
                cache[t]._cnt = cache[t]._cnt + 1
                cache[t][obj] = cache[t]._cnt
            end
        end
        if t == 'table' or TYPES[t] == nil then
            if cache.visited_tables[obj] == false then
                -- already printed, no need to mark this and its children anymore
                return
            elseif cache.visited_tables[obj] == nil then
                cache.visited_tables[obj] = 1
            else
                -- visited already, increment and continue
                cache.visited_tables[obj] = cache.visited_tables[obj] + 1
                return
            end
            for k, v in pairs(obj) do
                cache_apperance(k, cache, option)
                cache_apperance(v, cache, option)
            end
            local mt = getmetatable(obj)
            if mt and option.show_metatable then
                cache_apperance(mt, cache, option)
            end
        end
    end

    -- makes 'foo2' < 'foo100000'. string.sub makes substring anyway, no need to use index based method
    local function str_natural_cmp(lhs, rhs)
        while #lhs > 0 and #rhs > 0 do
            local lmid, lend = lhs:find('%d+')
            local rmid, rend = rhs:find('%d+')
            if not (lmid and rmid) then return lhs < rhs end

            local lsub = lhs:sub(1, lmid-1)
            local rsub = rhs:sub(1, rmid-1)
            if lsub ~= rsub then
                return lsub < rsub
            end
            
            local lnum = tonumber(lhs:sub(lmid, lend))
            local rnum = tonumber(rhs:sub(rmid, rend))
            if lnum ~= rnum then
                return lnum < rnum
            end

            lhs = lhs:sub(lend+1)
            rhs = rhs:sub(rend+1)
        end
        return lhs < rhs
    end

    local function cmp(lhs, rhs)
        local tleft = type(lhs)
        local tright = type(rhs)
        if tleft == 'number' and tright == 'number' then return lhs < rhs end
        if tleft == 'string' and tright == 'string' then return str_natural_cmp(lhs, rhs) end
        if tleft == tright then return str_natural_cmp(tostring(lhs), tostring(rhs)) end

        -- allow custom types
        local oleft = TYPES[tleft] or 9
        local oright = TYPES[tright] or 9
        return oleft < oright
    end

    -- setup option with default
    local function make_option(option)
        if option == nil then
            option = {}
        end
        for k, v in pairs(pprint.defaults) do
            if option[k] == nil then
                option[k] = v
            end
            if option.show_all then
                for t, _ in pairs(TYPES) do
                    option['show_'..t] = true
                end
                option.show_metatable = true
            end
        end
        return option
    end

    -- override defaults and take effects for all following calls
    function pprint.setup(option)
        pprint.defaults = make_option(option)
    end

    -- format lua object into a string
    function pprint.pformat(obj, option, printer)
        option = make_option(option)
        local buf = {}
        local function default_printer(s)
            table.insert(buf, s)
        end
        printer = printer or default_printer

        local cache
        if option.object_cache == 'global' then
            -- steal the cache into a local var so it's not visible from _G or anywhere
            -- still can't avoid user explicitly referentce pprint._cache but it shouldn't happen anyway
            cache = pprint._cache or {}
            pprint._cache = nil
        elseif option.object_cache == 'local' then
            cache = {}
        end

        local last = '' -- used for look back and remove trailing comma
        local status = {
            indent = '', -- current indent
            len = 0,     -- current line length
        }

        local wrapped_printer = function(s)
            printer(last)
            last = s
        end

        local function _indent(d)
            status.indent = string.rep(' ', d + #(status.indent))
        end

        local function _n(d)
            wrapped_printer('\n')
            wrapped_printer(status.indent)
            if d then
                _indent(d)
            end
            status.len = 0
            return true -- used to close bracket correctly
        end

        local function _p(s, nowrap)
            status.len = status.len + #s
            if not nowrap and status.len > option.level_width then
                _n()
                wrapped_printer(s)
                status.len = #s
            else
                wrapped_printer(s)
            end
        end

        local formatter = {}
        local function format(v)
            local f = formatter[type(v)]
            f = f or formatter.table -- allow patched type()
            if option.filter_function and option.filter_function(v, nil, nil) then
                return ''
            else
                return f(v)
            end
        end

        local function tostring_formatter(v)
            return tostring(v)
        end

        local function number_formatter(n)
            return n == math.huge and '[[math.huge]]' or tostring(n)
        end

        local function nop_formatter(v)
            return ''
        end

        local function make_fixed_formatter(t, has_cache)
            if has_cache then
                return function (v)
                    return string.format('[[%s %d]]', t, cache[t][v])
                end
            else
                return function (v)
                    return '[['..t..']]'
                end
            end
        end

        local function string_formatter(s, force_long_quote)
            local s, quote = escape(s)
            local quote_len = force_long_quote and 4 or 2
            if quote_len + #s + status.len > option.level_width then
                _n()
                -- only wrap string when is longer than level_width
                if option.wrap_string and #s + quote_len > option.level_width then
                    -- keep the quotes together
                    _p('[[')
                    while #s + status.len >= option.level_width do
                        local seg = option.level_width - status.len
                        _p(string.sub(s, 1, seg), true)
                        _n()
                        s = string.sub(s, seg+1)
                    end
                    _p(s) -- print the remaining parts
                    return ']]' 
                end
            end

            return force_long_quote and '[['..s..']]' or quote..s..quote
        end

        local function table_formatter(t)
            if option.use_tostring then
                local mt = getmetatable(t)
                if mt and mt.__tostring then
                    return string_formatter(tostring(t), true)
                end
            end

            local print_header_ix = nil
            local ttype = type(t)
            if option.object_cache then
                local cache_state = cache.visited_tables[t]
                local tix = cache[ttype][t]
                -- FIXME should really handle `cache_state == nil`
                -- as user might add things through filter_function
                if cache_state == false then
                    -- already printed, just print the the number
                    return string_formatter(string.format('%s %d', ttype, tix), true)
                elseif cache_state > 1 then
                    -- appeared more than once, print table header with number
                    print_header_ix = tix
                    cache.visited_tables[t] = false
                else
                    -- appeared exactly once, print like a normal table
                end
            end

            local limit = tonumber(option.depth_limit)
            if limit and depth > limit then
               if print_header_ix then
                  return string.format('[[%s %d]]...', ttype, print_header_ix)
               end
               return string_formatter(tostring(t), true)
            end

            local tlen = #t
            local wrapped = false
            _p('{')
            _indent(option.indent_size)
            _p(string.rep(' ', option.indent_size - 1))
            if print_header_ix then
                _p(string.format('--[[%s %d]] ', ttype, print_header_ix))
            end
            for ix = 1,tlen do
                local v = t[ix]
                if formatter[type(v)] == nop_formatter or 
                   (option.filter_function and option.filter_function(v, ix, t)) then
                   -- pass
                else
                    if option.wrap_array then
                        wrapped = _n()
                    end
                    depth = depth+1
                    _p(format(v)..', ')
                    depth = depth-1
                end
            end

            -- hashmap part of the table, in contrast to array part
            local function is_hash_key(k)
                if type(k) ~= 'number' then
                    return true
                end

                local numkey = math.floor(tonumber(k))
                if numkey ~= k or numkey > tlen or numkey <= 0 then
                    return true
                end
            end

            local function print_kv(k, v, t)
                -- can't use option.show_x as obj may contain custom type
                if formatter[type(v)] == nop_formatter or
                   formatter[type(k)] == nop_formatter or 
                   (option.filter_function and option.filter_function(v, k, t)) then
                    return
                end
                wrapped = _n()
                if is_plain_key(k) then
                    _p(k, true)
                else
                    _p('[')
                    -- [[]] type string in key is illegal, needs to add spaces inbetween
                    local k = format(k)
                    if string.match(k, '%[%[') then
                        _p(' '..k..' ', true)
                    else
                        _p(k, true)
                    end
                    _p(']')
                end
                _p(' = ', true)
                depth = depth+1
                _p(format(v), true)
                depth = depth-1
                _p(',', true)
            end

            if option.sort_keys then
                local keys = {}
                for k, _ in pairs(t) do
                    if is_hash_key(k) then
                        table.insert(keys, k)
                    end
                end
                table.sort(keys, cmp)
                for _, k in ipairs(keys) do
                    print_kv(k, t[k], t)
                end
            else
                for k, v in pairs(t) do
                    if is_hash_key(k) then
                        print_kv(k, v, t)
                    end
                end
            end

            if option.show_metatable then
                local mt = getmetatable(t)
                if mt then
                    print_kv('__metatable', mt, t)
                end
            end

            _indent(-option.indent_size)
            -- make { } into {}
            last = string.gsub(last, '^ +$', '')
            -- peek last to remove trailing comma
            last = string.gsub(last, ',%s*$', ' ')
            if wrapped then
                _n()
            end
            _p('}')

            return ''
        end

        -- set formatters
        formatter['nil'] = option.show_nil and tostring_formatter or nop_formatter
        formatter['boolean'] = option.show_boolean and tostring_formatter or nop_formatter
        formatter['number'] = option.show_number and number_formatter or nop_formatter -- need to handle math.huge
        formatter['function'] = option.show_function and make_fixed_formatter('function', option.object_cache) or nop_formatter
        formatter['thread'] = option.show_thread and make_fixed_formatter('thread', option.object_cache) or nop_formatter
        formatter['userdata'] = option.show_userdata and make_fixed_formatter('userdata', option.object_cache) or nop_formatter
        formatter['string'] = option.show_string and tostring_formatter or nop_formatter
        formatter['table'] = option.show_table and table_formatter or nop_formatter

        -- if option.object_cache then
        --     -- needs to visit the table before start printing
        --     cache_apperance(obj, cache, option)
        -- end

        _p(format(obj))
        printer(last) -- close the buffered one

        -- put cache back if global
        if option.object_cache == 'global' then
            pprint._cache = cache
        end

        return table.concat(buf)
    end



    -- pprint all the arguments
    function pprint.pprint( ... )
        local args = {...}

        local ilua_printer = {data=""}

        local meta = {
            __call = function(self, ...)
                local args = table.pack(...)
                for i=1,args.n do
                   self.data = self.data .. tostring(args[i])
                end

            end
        }

        setmetatable(ilua_printer, meta)


        -- select will get an accurate count of array len, counting trailing nils
        local len = select('#', ...)
        for ix = 1,len do
            pprint.pformat(args[ix], nil, ilua_printer)
            ilua_printer(' ')
        end
        ilua.detail.__io_write_custom_save(ilua_printer.data)
    end

    setmetatable(pprint, {
        __call = function (_, ...)
            pprint.pprint(...)
        end
    })

    --return pprint

    )"""";
    auto code_result = lua.script(script);
    if (!code_result.valid()) {
        sol::error err = code_result;
        std::cerr << "failed to load string-based script into the program" << err.what() << std::endl;
        throw std::runtime_error(err.what());
    }
}

void add_json_module(sol::state & lua){

    std::string script = R""""(
    --
    -- json.lua
    --
    -- Copyright (c) 2020 rxi
    --
    -- Permission is hereby granted, free of charge, to any person obtaining a copy of
    -- this software and associated documentation files (the "Software"), to deal in
    -- the Software without restriction, including without limitation the rights to
    -- use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
    -- of the Software, and to permit persons to whom the Software is furnished to do
    -- so, subject to the following conditions:
    --
    -- The above copyright notice and this permission notice shall be included in all
    -- copies or substantial portions of the Software.
    --
    -- THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    -- IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    -- FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    -- AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    -- LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
    -- OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
    -- SOFTWARE.
    --

    json = { _version = "0.1.2" }

    -------------------------------------------------------------------------------
    -- Encode
    -------------------------------------------------------------------------------

    local encode

    local escape_char_map = {
      [ "\\" ] = "\\",
      [ "\"" ] = "\"",
      [ "\b" ] = "b",
      [ "\f" ] = "f",
      [ "\n" ] = "n",
      [ "\r" ] = "r",
      [ "\t" ] = "t",
    }

    local escape_char_map_inv = { [ "/" ] = "/" }
    for k, v in pairs(escape_char_map) do
      escape_char_map_inv[v] = k
    end


    local function escape_char(c)
      return "\\" .. (escape_char_map[c] or string.format("u%04x", c:byte()))
    end


    local function encode_nil(val)
      return "null"
    end


    local function encode_table(val, stack)
      local res = {}
      stack = stack or {}

      -- Circular reference?
      if stack[val] then error("circular reference") end

      stack[val] = true

      if rawget(val, 1) ~= nil or next(val) == nil then
        -- Treat as array -- check keys are valid and it is not sparse
        local n = 0
        for k in pairs(val) do
          if type(k) ~= "number" then
            error("invalid table: mixed or invalid key types")
          end
          n = n + 1
        end
        if n ~= #val then
          error("invalid table: sparse array")
        end
        -- Encode
        for i, v in ipairs(val) do
          table.insert(res, encode(v, stack))
        end
        stack[val] = nil
        return "[" .. table.concat(res, ",") .. "]"

      else
        -- Treat as an object
        for k, v in pairs(val) do
          if type(k) ~= "string" then
            error("invalid table: mixed or invalid key types")
          end
          table.insert(res, encode(k, stack) .. ":" .. encode(v, stack))
        end
        stack[val] = nil
        return "{" .. table.concat(res, ",") .. "}"
      end
    end


    local function encode_string(val)
      return '"' .. val:gsub('[%z\1-\31\\"]', escape_char) .. '"'
    end


    local function encode_number(val)
      -- Check for NaN, -inf and inf
      if val ~= val or val <= -math.huge or val >= math.huge then
        error("unexpected number value '" .. tostring(val) .. "'")
      end
      return string.format("%.14g", val)
    end


    local type_func_map = {
      [ "nil"     ] = encode_nil,
      [ "table"   ] = encode_table,
      [ "string"  ] = encode_string,
      [ "number"  ] = encode_number,
      [ "boolean" ] = tostring,
    }


    encode = function(val, stack)
      local t = type(val)
      local f = type_func_map[t]
      if f then
        return f(val, stack)
      end
      error("unexpected type '" .. t .. "'")
    end


    function json.encode(val)
      return ( encode(val) )
    end


    -------------------------------------------------------------------------------
    -- Decode
    -------------------------------------------------------------------------------

    local parse

    local function create_set(...)
      local res = {}
      for i = 1, select("#", ...) do
        res[ select(i, ...) ] = true
      end
      return res
    end

    local space_chars   = create_set(" ", "\t", "\r", "\n")
    local delim_chars   = create_set(" ", "\t", "\r", "\n", "]", "}", ",")
    local escape_chars  = create_set("\\", "/", '"', "b", "f", "n", "r", "t", "u")
    local literals      = create_set("true", "false", "null")

    local literal_map = {
      [ "true"  ] = true,
      [ "false" ] = false,
      [ "null"  ] = nil,
    }


    local function next_char(str, idx, set, negate)
      for i = idx, #str do
        if set[str:sub(i, i)] ~= negate then
          return i
        end
      end
      return #str + 1
    end


    local function decode_error(str, idx, msg)
      local line_count = 1
      local col_count = 1
      for i = 1, idx - 1 do
        col_count = col_count + 1
        if str:sub(i, i) == "\n" then
          line_count = line_count + 1
          col_count = 1
        end
      end
      error( string.format("%s at line %d col %d", msg, line_count, col_count) )
    end


    local function codepoint_to_utf8(n)
      -- http://scripts.sil.org/cms/scripts/page.php?site_id=nrsi&id=iws-appendixa
      local f = math.floor
      if n <= 0x7f then
        return string.char(n)
      elseif n <= 0x7ff then
        return string.char(f(n / 64) + 192, n % 64 + 128)
      elseif n <= 0xffff then
        return string.char(f(n / 4096) + 224, f(n % 4096 / 64) + 128, n % 64 + 128)
      elseif n <= 0x10ffff then
        return string.char(f(n / 262144) + 240, f(n % 262144 / 4096) + 128,
                           f(n % 4096 / 64) + 128, n % 64 + 128)
      end
      error( string.format("invalid unicode codepoint '%x'", n) )
    end


    local function parse_unicode_escape(s)
      local n1 = tonumber( s:sub(1, 4),  16 )
      local n2 = tonumber( s:sub(7, 10), 16 )
       -- Surrogate pair?
      if n2 then
        return codepoint_to_utf8((n1 - 0xd800) * 0x400 + (n2 - 0xdc00) + 0x10000)
      else
        return codepoint_to_utf8(n1)
      end
    end


    local function parse_string(str, i)
      local res = ""
      local j = i + 1
      local k = j

      while j <= #str do
        local x = str:byte(j)

        if x < 32 then
          decode_error(str, j, "control character in string")

        elseif x == 92 then -- `\`: Escape
          res = res .. str:sub(k, j - 1)
          j = j + 1
          local c = str:sub(j, j)
          if c == "u" then
            local hex = str:match("^[dD][89aAbB]%x%x\\u%x%x%x%x", j + 1)
                     or str:match("^%x%x%x%x", j + 1)
                     or decode_error(str, j - 1, "invalid unicode escape in string")
            res = res .. parse_unicode_escape(hex)
            j = j + #hex
          else
            if not escape_chars[c] then
              decode_error(str, j - 1, "invalid escape char '" .. c .. "' in string")
            end
            res = res .. escape_char_map_inv[c]
          end
          k = j + 1

        elseif x == 34 then -- `"`: End of string
          res = res .. str:sub(k, j - 1)
          return res, j + 1
        end

        j = j + 1
      end

      decode_error(str, i, "expected closing quote for string")
    end


    local function parse_number(str, i)
      local x = next_char(str, i, delim_chars)
      local s = str:sub(i, x - 1)
      local n = tonumber(s)
      if not n then
        decode_error(str, i, "invalid number '" .. s .. "'")
      end
      return n, x
    end


    local function parse_literal(str, i)
      local x = next_char(str, i, delim_chars)
      local word = str:sub(i, x - 1)
      if not literals[word] then
        decode_error(str, i, "invalid literal '" .. word .. "'")
      end
      return literal_map[word], x
    end


    local function parse_array(str, i)
      local res = {}
      local n = 1
      i = i + 1
      while 1 do
        local x
        i = next_char(str, i, space_chars, true)
        -- Empty / end of array?
        if str:sub(i, i) == "]" then
          i = i + 1
          break
        end
        -- Read token
        x, i = parse(str, i)
        res[n] = x
        n = n + 1
        -- Next token
        i = next_char(str, i, space_chars, true)
        local chr = str:sub(i, i)
        i = i + 1
        if chr == "]" then break end
        if chr ~= "," then decode_error(str, i, "expected ']' or ','") end
      end
      return res, i
    end


    local function parse_object(str, i)
      local res = {}
      i = i + 1
      while 1 do
        local key, val
        i = next_char(str, i, space_chars, true)
        -- Empty / end of object?
        if str:sub(i, i) == "}" then
          i = i + 1
          break
        end
        -- Read key
        if str:sub(i, i) ~= '"' then
          decode_error(str, i, "expected string for key")
        end
        key, i = parse(str, i)
        -- Read ':' delimiter
        i = next_char(str, i, space_chars, true)
        if str:sub(i, i) ~= ":" then
          decode_error(str, i, "expected ':' after key")
        end
        i = next_char(str, i + 1, space_chars, true)
        -- Read value
        val, i = parse(str, i)
        -- Set
        res[key] = val
        -- Next token
        i = next_char(str, i, space_chars, true)
        local chr = str:sub(i, i)
        i = i + 1
        if chr == "}" then break end
        if chr ~= "," then decode_error(str, i, "expected '}' or ','") end
      end
      return res, i
    end


    local char_func_map = {
      [ '"' ] = parse_string,
      [ "0" ] = parse_number,
      [ "1" ] = parse_number,
      [ "2" ] = parse_number,
      [ "3" ] = parse_number,
      [ "4" ] = parse_number,
      [ "5" ] = parse_number,
      [ "6" ] = parse_number,
      [ "7" ] = parse_number,
      [ "8" ] = parse_number,
      [ "9" ] = parse_number,
      [ "-" ] = parse_number,
      [ "t" ] = parse_literal,
      [ "f" ] = parse_literal,
      [ "n" ] = parse_literal,
      [ "[" ] = parse_array,
      [ "{" ] = parse_object,
    }


    parse = function(str, idx)
      local chr = str:sub(idx, idx)
      local f = char_func_map[chr]
      if f then
        return f(str, idx)
      end
      decode_error(str, idx, "unexpected character '" .. chr .. "'")
    end


    function json.decode(str)
      if type(str) ~= "string" then
        error("expected argument of type string, got " .. type(str))
      end
      local res, idx = parse(str, next_char(str, 1, space_chars, true))
      idx = next_char(str, idx, space_chars, true)
      if idx <= #str then
        decode_error(str, idx, "trailing garbage")
      end
      return res
    end
    )"""";
    auto code_result = lua.script(script);
    if (!code_result.valid()) {
        sol::error err = code_result;
        std::cerr << "failed to load string-based script into the program" << err.what() << std::endl;
        throw std::runtime_error(err.what());
    }
}

void add_ilua_module(sol::state & lua){

    std::string script = R""""(


    ilua = {
        display = { _version = "0.1.0" ,
            detail = {}
        },
        widgets = {
            detail = {}
        },
        config = {
            printer = "default"
        },
        detail = {

        }
    }
    local display = ilua.display


    local function display_data(data,metadata,transient)
        display.detail._display_data(
            json.encode(data),
            json.encode(metadata),
            json.encode(transient)
        )
    end

    function display.mimetype(mimetype, data)
        data = {
            [mimetype] = data
        }
        display_data(data, {}, {})
    end

    function display.plain_text(str)
        display.mimetype("text/plain", str)
    end

    function display.html(html)
        display.mimetype("text/html", html)
    end

    function display.json(j)
        display.mimetype("application/json", j)
    end

    function display.latex(j)
        display.mimetype("text/latex", j)
    end


    function display.display(...)
        args = table.pack(...)
        for i=1,args.n do
            local arg = args[i]
            arg:display()
        end
    end

    function display.with_output(output_widget, f)
        output_widget:capture()
        f()
        output:release()
    end
    return display

    
    )"""";
    auto code_result = lua.script(script);
    if (!code_result.valid()) {
        sol::error err = code_result;
        std::cerr << "failed to load string-based script into the program" << err.what() << std::endl;
        throw std::runtime_error(err.what());
    }
}


void extend(sol::state & lua)
{
    add_pprint_module(lua);
    add_json_module(lua);
    add_ilua_module(lua);
}

}
