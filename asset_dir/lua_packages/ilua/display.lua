local display = { _version = "0.1.0" }

local json = require('json')
local html = require('ilua.html')



local function display_data(data,metadata,transient, update)
    update = update or false
    -- print("update",update)
    if update then
        _update_display_data(
            json.encode(data),
            json.encode(metadata),
            json.encode(transient)
        )
    else
        _display_data(
            json.encode(data),
            json.encode(metadata),
            json.encode(transient)
        )
    end
end

function display.mimetype(mimetype, data, update)
    data = {
        [mimetype] = data
    }
    display_data(data, {}, {}, update)
end

function display.plain_text(str, update)
    display.mimetype("text/plain", str, update)
end

function display.html(html, update)
    display.mimetype("text/html", html, update)
end

function display.json(j, update)
    display.mimetype("application/json", j, update)
end

function display.latex(j, update)
    display.mimetype("text/latex", j, update)
end

function display.table(tbl, update)
    display.html(html.table_to_html(tbl), update)
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