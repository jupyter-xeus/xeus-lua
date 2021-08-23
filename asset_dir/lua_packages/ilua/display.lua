local display = { _version = "0.1.0" }

local json = require('json')
local html = require('ilua.html')



function display.mimetype(mimetype, data, start)
    start = start or 0
    payload = {
        ["data"]={
            [mimetype] = data
        },
        ["source"] = "page",
        ["start"]  = start
    }
    payload_str = json.encode(payload)
    set_json_payload(payload_str)
end

function display.plain_text(str, start)
    display.mimetype("text/plain", str, start)
end

function display.html(html, start)
    display.mimetype("text/html", html, start)
end

function display.json(j, start)
    display.mimetype("application/json", j, start)
end

function display.latex(j, start)
    display.mimetype("text/latex", j, start)
end

function display.table(tbl, start)
    display.html(html.table_to_html(tbl), start)
end

return display