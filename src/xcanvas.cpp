/***************************************************************************
* Copyright (c) 2021, Thorsten Beier                                       *                                                       *
* Copyright (c) 2021, QuantStack                                           *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#include <vector>
#include <string>
#include <iostream>

#include "sol/sol.hpp"
#include "xeus-lua/xinterpreter.hpp"
#include "xcanvas/xcanvas.hpp"
#include "nlohmann/json.hpp"
namespace nl = nlohmann;





namespace sol {

        template<template <class> class B, class... P>
        struct is_container<xw::xmaterialize<B, P...>> : std::false_type {};



}


namespace xlua
{

#define XLUA_ADD_PROPERTY(CLS_OBJ,PROPERTY_TYPE,PROPERTY_NAME)\
    CLS_OBJ.set(#PROPERTY_NAME, sol::property(\
        [](xwidgets_type & widget){\
            return PROPERTY_TYPE(widget.PROPERTY_NAME);\
        }, \
        [](xwidgets_type & widget, const PROPERTY_TYPE & val){\
            widget.PROPERTY_NAME = val;\
        })\
    )


void setup_xcanvas(
  sol::state_view & lua
)
{
    // get display table
    sol::table ilua_table = lua["ilua"];
    sol::table canvas_table = ilua_table["canvas"];
    sol::table detail_table = canvas_table["detail"];

    using xwidgets_type = xc::canvas;
    const std::string widget_name  = "xcanvas";
    // make usertype metatable
    sol::usertype<xwidgets_type> canvas_lua_type = detail_table.new_usertype<xwidgets_type>(
        widget_name,
        // 1 constructors
        sol::constructors<xwidgets_type()>()
    );

    // special functions
    canvas_lua_type[sol::meta_function::to_string] = [widget_name](xwidgets_type & ){
        return widget_name;
    };

    // Simple methods
    canvas_lua_type["display"] = &xwidgets_type::display;

    // Rect methods
    canvas_lua_type["fill_rect"] = sol::overload(
        [](xwidgets_type & self,  double x, double y, double width){
            self.fill_rect(x, y, width);
        },
        [](xwidgets_type & self,  double x, double y, double width, double height){
            self.fill_rect(x, y, width, height);
        }
    );
    canvas_lua_type["stroke_rect"] = sol::overload(
        [](xwidgets_type & self,  double x, double y, double width){
            self.stroke_rect(x, y, width);
        },
        [](xwidgets_type & self,  double x, double y, double width, double height){
            self.stroke_rect(x, y, width, height);
        }
    );
    canvas_lua_type["clear_rect"] = sol::overload(
        [](xwidgets_type & self,  double x, double y, double width){
            self.clear_rect(x, y, width);
        },
        [](xwidgets_type & self,  double x, double y, double width, double height){
            self.clear_rect(x, y, width, height);
        }
    );

    // Arc methods
    canvas_lua_type["fill_arc"] = &xwidgets_type::fill_arc;
    canvas_lua_type["fill_circle"] = &xwidgets_type::fill_circle;
    canvas_lua_type["stroke_arc"] = &xwidgets_type::stroke_arc;
    canvas_lua_type["stroke_circle"] = &xwidgets_type::stroke_circle;

    // Line methods
    canvas_lua_type["stroke_line"] = &xwidgets_type::stroke_line;

    // Path methods
    canvas_lua_type["begin_path"] = &xwidgets_type::begin_path;
    canvas_lua_type["close_path"] = &xwidgets_type::close_path;
    canvas_lua_type["stroke"] = &xwidgets_type::stroke;
    canvas_lua_type["fill"] = &xwidgets_type::fill;
    canvas_lua_type["move_to"] = &xwidgets_type::move_to;
    canvas_lua_type["line_to"] = &xwidgets_type::line_to;
    canvas_lua_type["rect"] = &xwidgets_type::rect;
    canvas_lua_type["arc"] = &xwidgets_type::arc;
    canvas_lua_type["ellipse"] = &xwidgets_type::ellipse;
    canvas_lua_type["arc_to"] = &xwidgets_type::arc_to;
    canvas_lua_type["quadratic_curve_to"] = &xwidgets_type::quadratic_curve_to;
    canvas_lua_type["bezier_curve_to"] = &xwidgets_type::bezier_curve_to;

    // Clip methods
    canvas_lua_type["clip"] = &xwidgets_type::clip;

    // Transform methods
    canvas_lua_type["save"] = &xwidgets_type::save;
    canvas_lua_type["restore"] = &xwidgets_type::restore;
    canvas_lua_type["translate"] = &xwidgets_type::translate;
    canvas_lua_type["rotate"] = &xwidgets_type::rotate;

    // Extras
    canvas_lua_type["cache"] = &xwidgets_type::cache;
    canvas_lua_type["flush"] = &xwidgets_type::flush;
    canvas_lua_type["clear"] = &xwidgets_type::clear;

    detail_table[widget_name] = canvas_lua_type;

    // add properties
    XLUA_ADD_PROPERTY(canvas_lua_type, int, width);
    XLUA_ADD_PROPERTY(canvas_lua_type, int, height);
    XLUA_ADD_PROPERTY(canvas_lua_type, bool, sync_image_data);
    XLUA_ADD_PROPERTY(canvas_lua_type, double, global_alpha);
    XLUA_ADD_PROPERTY(canvas_lua_type, std::string, font);

    XLUA_ADD_PROPERTY(canvas_lua_type, std::string, text_align);
    XLUA_ADD_PROPERTY(canvas_lua_type, std::string, text_baseline);
    XLUA_ADD_PROPERTY(canvas_lua_type, std::string, direction);
    XLUA_ADD_PROPERTY(canvas_lua_type, std::string, global_composite_operation);

    XLUA_ADD_PROPERTY(canvas_lua_type, double, shadow_offset_x);
    XLUA_ADD_PROPERTY(canvas_lua_type, double, shadow_offset_y);
    XLUA_ADD_PROPERTY(canvas_lua_type, double, shadow_blur);
    XLUA_ADD_PROPERTY(canvas_lua_type, std::string, shadow_color);

    XLUA_ADD_PROPERTY(canvas_lua_type, double, line_width);
    XLUA_ADD_PROPERTY(canvas_lua_type, std::string, line_cap);
    XLUA_ADD_PROPERTY(canvas_lua_type, std::string, line_join);
    XLUA_ADD_PROPERTY(canvas_lua_type, double, miter_limit);
    XLUA_ADD_PROPERTY(canvas_lua_type, double, line_dash_offset);
    XLUA_ADD_PROPERTY(canvas_lua_type, std::string, fill_style);
    XLUA_ADD_PROPERTY(canvas_lua_type, std::string, stroke_style);

    // patch the print function with pure lua code
    std::string script = R""""(

    local atomic_widgets = {
        xcanvas = ilua.canvas.detail.xcanvas
    }

    for k,widget_cls in pairs(  atomic_widgets) do
        mc = getmetatable(widget_cls)
        function mc.__tostring(...)
            return k
        end
    end

    for k,widget_cls in pairs(  atomic_widgets) do
    ilua.canvas[k:sub(2)] = function(options)
            options = options or {}
            w = widget_cls.new()
            for kw,val in pairs(options) do
                if w[kw] ~= nil then
                    w[kw] = val
                else
                    error(k.." has no attribute `"..kw.."`")
                end
            end
            return w
        end
    end


    ilua.canvas["rgb"] = function(r,g,b)
        return "rgb(" .. tostring(r) .. ", " ..  tostring(g) .. ", " .. tostring(b) .. ")"
    end


    )"""";
    sol::protected_function_result code_result  = lua.safe_script(script, &sol::script_pass_on_error);
    if (!code_result.valid()) {
        sol::error err = code_result;
        std::cerr << "failed to load string-based script into the program for xcanvas" << err.what() << std::endl;
        throw std::runtime_error(err.what());
    }

}


}
