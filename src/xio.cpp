/***************************************************************************
* Copyright (c) 2021, Thorsten Beier                                       *                                                       *
* Copyright (c) 2021, QuantStack                                           *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#include <iostream>
#include <sstream>

#include "sol/sol.hpp"
#include "xeus-lua/xinterpreter.hpp"
#include "nlohmann/json.hpp"
#include "xeus/xinput.hpp"

namespace xlua
{

void add_pprint_module(sol::state_view & lua){

    std::string script = R""""(
    -- Chen Tao - jagttt@gmail.com
    -- This is free and unencumbered software released into the public domain.
    -- Anyone is free to copy, modify, publish, use, compile, sell, or
    -- distribute this software, either in  code form or as a compiled
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
    )"""" 
    R""""(
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
    )"""" 
    R""""(
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
    )"""" 
    R""""(
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
    )"""" 
    R""""(
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
        )"""" 
        R""""(
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
            )"""" 
            R""""(
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
    )"""" 
    R""""(
    -- pprint all the arguments
    function pprint.pprint_impl(printer, ... )
        local args = {...}
        local ilua_printer = {data=""}
        local meta = {
            __call = function(self, ...)
                local args = {n = select('#', ...), ...}
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
            if ix < len then
                ilua_printer(' ')
            end
        end
        ilua_printer('\n')
        return printer(ilua_printer.data)
    end
    function pprint.pprint(...)
        return pprint.pprint_impl(io.write, ...)
    end
    function pprint.pprint_str(...)
        local result = ''
        local function printer(s)
            result = result .. s
        end
        pprint.pprint_impl(printer, ...)
        return result
    end
    setmetatable(pprint, {
        __call = function (_, ...)
            pprint.pprint(...)
        end
    })
    _G.pprint = pprint
    
    )"""";
    auto code_result = lua.script(script);
    if (!code_result.valid()) {
        sol::error err = code_result;
        std::cerr << "failed to load string-based script into the program" << err.what() << std::endl;
        throw std::runtime_error(err.what());
    }
}


void setup_io(
  sol::state_view  & lua,
  interpreter & interp
)
{
    sol::table ilua_table = lua["ilua"];
    sol::table detail_table = ilua_table["detail"];

    lua.script(R""""(

        ilua.detail.__original_print = print

        function print(...)
            if ilua.config.printer == "print" then
                ilua.detail.__original_print(...)
            elseif ilua.config.printer == "pprint" then
                pprint(...)
            else
                error(string.format("%s is an unknown printer", ilua.config.printer))
            end
        end
    )"""");


    detail_table.set_function("__io_read_custom", [](
       sol::variadic_args /*ingnored args*/
    ) {
        
        auto & self = static_cast<interpreter&>(xeus::get_interpreter());
        if(self.allow_stdin())
        {
            return xeus::blocking_input_request("", false);
        }
        else
        {
            std::string error_str = "stdin is not allowed";
            self.publish_execution_error(error_str,error_str, std::vector<std::string>());
            return std::string();
        }
    });

    detail_table.set_function("__io_write_to_stream", []( 
        std::string stream_name,
        sol::variadic_args to_write ) 
    {
        auto & interpreter = xeus::get_interpreter();
        std::stringstream ss;
        for (auto arg : to_write) {
            // call tostring on each argument to respect __tostring metamethods
            sol::state_view lua(arg.lua_state());
            sol::protected_function tostring = lua["tostring"];
            sol::protected_function_result tostring_result = tostring(arg);
            if (!tostring_result.valid()) {
                sol::error err = tostring_result;
                std::stringstream ss_err;
                ss_err << "Error in tostring call for io.write argument: " << err.what();
                interpreter.publish_stream("stderr", ss_err.str());
                continue; // skip this argument but continue with others
            }
            ss << tostring_result.get<std::string>();
        }
        interpreter.publish_stream(stream_name, ss.str());
    });

    detail_table.set_function("__unredirected_cout", []( 
        sol::variadic_args to_write ) 
    {
        auto & interpreter = xeus::get_interpreter();
        std::stringstream ss;
        for (auto arg : to_write) {
            // call tostring on each argument to respect __tostring metamethods
            sol::state_view lua(arg.lua_state());
            sol::protected_function tostring = lua["tostring"];
            sol::protected_function_result tostring_result = tostring(arg);
            if (!tostring_result.valid()) {
                sol::error err = tostring_result;
                std::stringstream ss_err;
                ss_err << "Error in tostring call for unredirected_cout argument: " << err.what();
                interpreter.publish_stream("stderr", ss_err.str());
                continue; // skip this argument but continue with others
            }
            ss << tostring_result.get<std::string>();
        }
        std::cout << ss.str()<< std::flush;
    });

    const std::string monkeypatch = R""""(
        require "io"

        ilua.detail.__original_stdout_file = io.stdout
        ilua.detail.__original_stderr_file = io.stderr

        ilua.detail.__original_io_output = io.output
        ilua.detail.__original_io_input = io.input
        ilua.detail.__original_print = print
        ilua.detail.__original_io_write = io.write
        ilua.detail.__original_io_read = io.read

        
        ilua.detail.__stdout_filename = os.tmpname()
        ilua.detail.__stderr_filename = os.tmpname() 
        ilua.detail.__stdin_filename = os.tmpname()


        

        ilua.detail.__stdout_file = io.open(ilua.detail.__stdout_filename, "w+")
        ilua.detail.__stderr_file = io.open(ilua.detail.__stderr_filename, "w+")
        ilua.detail.__stdin_file = io.open(ilua.detail.__stdin_filename, "w+")


        ilua.detail.__is_redirect_file = function(arg)
            return arg == ilua.detail.__stdout_file or arg == ilua.detail.__stderr_file or arg == ilua.detail.__stdin_file
        end


        ilua.detail.__redirect_output = function()


            local function redirect_output(file, stream_name)
                file:seek("set") 
                local file_content = file:read("*a")
                if file_content and #file_content > 0 then
                    ilua.detail.__io_write_to_stream(stream_name, file_content)
                end
            end
            redirect_output(ilua.detail.__stdout_file, "stdout")
            redirect_output(ilua.detail.__stderr_file, "stderr")

            local function flush_file(file, path)
                file:close()
                local new_file = io.open(path, "w+")  -- truncates file
                return new_file
            end
            
            local is_stdout_file = io.stdout == ilua.detail.__stdout_file
            local is_stderr_file = io.stderr == ilua.detail.__stderr_file

            ilua.detail.__stdout_file = flush_file(ilua.detail.__stdout_file, ilua.detail.__stdout_filename)
            ilua.detail.__stderr_file = flush_file(ilua.detail.__stderr_file, ilua.detail.__stderr_filename)

            if is_stdout_file then
                io.stdout = ilua.detail.__stdout_file
                io.output(ilua.detail.__stdout_file)
            end

            if is_stderr_file then
                io.stderr = ilua.detail.__stderr_file
            end

        end



        io.stdout = ilua.detail.__stdout_file
        io.stderr = ilua.detail.__stderr_file
        io.stdin = ilua.detail.__stdin_file
        io.output(ilua.detail.__stdout_file)



        -- even though we have the filebased redirection
        -- they have the drawback that they are only printed after the execute request is done
        -- therefore we still overwrite print / io.write to redirect output immediately,
        --  but still keep the file redirection as a backup for any output that doesn't go through those functions.

        -- custom print function
        ilua.detail.__print_dispatch =  function(...)
            if ilua.config.printer == "print" then
                local args = {...}
                local str_args = {}
                for i, arg in ipairs(args) do
                    table.insert(str_args, tostring(arg))   
                end
                ilua.detail.__io_write_to_stream("stdout", table.concat(str_args, "\t"), "\n")
            elseif ilua.config.printer == "pprint" then
                pprint(...)
            else
                error(string.format("%s is an unknown printer", ilua.config.printer))
            end
        end

    
        -- replace global print with print above
        _G.print = ilua.detail.__print_dispatch
        

        -- for write we need to be careful, only when io.output is set to our redirected file, we redirect it to kernel, otherwise we write to the io.output 
        io.write = function(...)
            if io.output() == ilua.detail.__stdout_file then    
                ilua.detail.__io_write_to_stream("stdout", ...)
            else
                return ilua.detail.__original_io_write(...)
            end    
        end

        io.read = function(...)
            if io.input() == ilua.detail.__stdin_file then
                return ilua.detail.__io_read_custom(...)
            else
                return ilua.detail.__original_io_read(...)
            end
        end



    )"""";
    
    add_pprint_module(lua);

    sol::protected_function_result code_result  = lua.safe_script(monkeypatch, &sol::script_pass_on_error);
    if (!code_result.valid()) {
        sol::error err = code_result;
        std::cerr << "failed to load string-based script into the program for xio" << err.what() << std::endl;
        std::cout << "monkeypatch was: " << monkeypatch << std::endl;        throw std::runtime_error(err.what());
    }



   
}

}
