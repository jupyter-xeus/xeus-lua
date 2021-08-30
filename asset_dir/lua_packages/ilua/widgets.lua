local widgets = { _version = "0.1.0" }



function setContains(set, key)
    return set[key] ~= nil
end
function widgets.slider(options)
    slider = xslider.new()
    options = options or {}
    if options["continuous_update"] ~= nil then
        slider.continuous_update = options["continuous_update"]
    end
    return slider
end


-- function xoutput:captured_call(func)
--     self:capture()
--     func()
--     self:release()
-- end

return widgets
