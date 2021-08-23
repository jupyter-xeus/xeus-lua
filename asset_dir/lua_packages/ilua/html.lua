local html = { _version = "0.1.0" }




-- Flattens a table to string
local function toflat(x, tlevel)
  local s
  tlevel = tlevel + 1

  if type(x) == "table" then
    s = "{"
    local i, v = next(x)
    while i do
      if (tlevel < 15) then
        s = s .. i .. "=" .. toflat(v, tlevel) 
      else
        s = s .. i .. "={#}" 
      end

      i, v = next(x, i)
      if i then
        s = s .. ", " 
      end
    end
    return s .. "}\n"
  end
  if type(x) == "function" then
    return "FUNC"
  end
  if type(x) == "file" then
    return "FILE"
  end

  return tostring(x) 
end


-- Flattens a table to string
local function flat(x)  
  return toflat(x,1)
end

-- Flattens a table to html output
local function tohtml_table(x, table_level)
  local k, s,  tcolor
  local html_colors = {
    "#339900","#33CC00","#669900","#666600","#FF3300",
    "#FFCC00","#FFFF00","#CCFFCC","#CCCCFF","#CC66FF",
    "#339900","#33CC00","#669900","#666600","#FF3300",
    "#FFCC00","#FFFF00","#CCFFCC","#CCCCFF","#CC66FF"
  }
  local lineout = {}
  local tablefound = false
    if type(x) == "table" then
    s = ""
    k = 1
    local i, v = next(x)
    while i do
      if (type(v) == "table") then
        if (table_level<10) then
          lineout[k] =  "<b>" .. flat(i) .. "</b>".. tohtml_table(v, table_level + 1)   
        else
          lineout[k] = "<b>MAXIMUM LEVEL BREACHED</b>"
        end
        tablefound = true
      else
        lineout[k] = flat(i) .. "=" .. tohtml_table(v)
      end
      k = k + 1
      i, v = next(x, i)
    end

    for k,line in ipairs(lineout) do
      if (tablefound) then
        s = s .. "<tr><td>" .. line .. "</td></tr>\n"
      else
        s = s .. "<td>" .. line .. "</td>\n"
      end
    end
    if not (tablefound) then
      s = "<table border='1' bgcolor='#FFFFCC' cellpadding='5' cellspacing='0'>" ..
        "<tr>" .. s .. "</tr></table>\n"
    else
      tcolor = html_colors[table_level]
      s = "<table border='3' bgcolor='"..tcolor.."' cellpadding='10' cellspacing='0'>" ..
          s ..  "</table>\n"
    end

    return s 
  end
  if type(x) == "function" then
    return "FUNC"
  end
  if type(x) == "file" then
    return "FILE"
  end

  return tostring(x) 
end




-- Converts Lua table to HTML output in table.html file
function html.table_to_html(x)
  ret = tohtml_table(x,1)
  return ret
end

return html