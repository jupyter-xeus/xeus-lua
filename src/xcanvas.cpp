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

using coord_t = std::array<double, 2>;

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
        },
        [](xwidgets_type & self, coord_t coord, coord_t size){
            self.fill_rect(coord[0], coord[1], size[0], size[1]);
        }
    );
    canvas_lua_type["stroke_rect"] = sol::overload(
        [](xwidgets_type & self,  double x, double y, double width){
            self.stroke_rect(x, y, width);
        },
        [](xwidgets_type & self,  double x, double y, double width, double height){
            self.stroke_rect(x, y, width, height);
        },
        [](xwidgets_type & self, coord_t coord, coord_t size){
            self.stroke_rect(coord[0], coord[1], size[0], size[1]);
        }
    );
    canvas_lua_type["clear_rect"] = sol::overload(
        [](xwidgets_type & self,  double x, double y, double width){
            self.clear_rect(x, y, width);
        },
        [](xwidgets_type & self,  double x, double y, double width, double height){
            self.clear_rect(x, y, width, height);
        },
        [](xwidgets_type & self, coord_t coord, coord_t size){
            self.clear_rect(coord[0], coord[1], size[0], size[1]);
        }
    );

    // Arc methods
    canvas_lua_type["fill_arc"] = sol::overload(
        [](xwidgets_type & self, double x, double y, double radius, double start_angle, double end_angle, bool anticlockwise){
            self.fill_arc(x, y, radius, start_angle, end_angle, anticlockwise);
        },
        [](xwidgets_type & self, coord_t coord, double radius, double start_angle, double end_angle, bool anticlockwise){
            self.fill_arc(coord[0], coord[1], radius, start_angle, end_angle, anticlockwise);
        }
    );
    canvas_lua_type["fill_circle"] = sol::overload(
        [](xwidgets_type & self,  double x, double y, double r){
            self.fill_circle(x, y, r);
        },
        [](xwidgets_type & self, coord_t coord, double r){
            self.fill_circle(coord[0], coord[1], r);
        }
    );
    canvas_lua_type["stroke_arc"] = sol::overload(
        [](xwidgets_type & self, double x, double y, double radius, double start_angle, double end_angle, bool anticlockwise){
            self.stroke_arc(x, y, radius, start_angle, end_angle, anticlockwise);
        },
        [](xwidgets_type & self, coord_t coord, double radius, double start_angle, double end_angle, bool anticlockwise){
            self.stroke_arc(coord[0], coord[1], radius, start_angle, end_angle, anticlockwise);
        }
    );
    canvas_lua_type["stroke_circle"] = sol::overload(
        [](xwidgets_type & self,  double x, double y, double r){
            self.stroke_circle(x, y, r);
        },
        [](xwidgets_type & self, coord_t coord, double r){
            self.stroke_circle(coord[0], coord[1], r);
        }
    );
    // Line methods
    canvas_lua_type["stroke_line"] = sol::overload(
        [](xwidgets_type & self, double x1, double y1, double x2, double y2){
            self.stroke_line(x1, y1, x2, y2);
        },
        [](xwidgets_type & self, coord_t a, coord_t b){
            self.stroke_line(a[0], a[1], b[0], b[1]);
        }
    );

    // Path methods
    canvas_lua_type["begin_path"] = &xwidgets_type::begin_path;
    canvas_lua_type["close_path"] = &xwidgets_type::close_path;
    canvas_lua_type["stroke"] = &xwidgets_type::stroke;
    canvas_lua_type["fill"] = sol::overload(
        [](xwidgets_type & self, std::string rule){
            self.fill(rule);
        },
        [](xwidgets_type & self){
            self.fill();
        }
    );
    canvas_lua_type["move_to"] = sol::overload(
        [](xwidgets_type & self, double x, double y){
            self.move_to(x, y);
        },
        [](xwidgets_type & self, coord_t coord){
            self.move_to(coord[0], coord[1]);
        }
    );
    canvas_lua_type["line_to"] = sol::overload(
        [](xwidgets_type & self, double x, double y){
            self.line_to(x, y);
        },
        [](xwidgets_type & self, coord_t coord){
            self.line_to(coord[0], coord[1]);
        }
    );
    canvas_lua_type["rect"] = sol::overload(
        [](xwidgets_type & self, double x, double y, double width, double height){
            self.rect(x, y, width, height);
        },
        [](xwidgets_type & self, coord_t coord, coord_t size){
            self.rect(coord[0], coord[1], size[0], size[1]);
        }
    );
    canvas_lua_type["arc"] = sol::overload(
        [](xwidgets_type & self, double x, double y, double radius, double start_angle, double end_angle, bool anticlockwise){
            self.arc(x, y, radius, start_angle, end_angle, anticlockwise);
        },
        [](xwidgets_type & self, coord_t coord, double radius, double start_angle, double end_angle, bool anticlockwise){
            self.arc(coord[0], coord[1], radius, start_angle, end_angle, anticlockwise);
        }
    );
    canvas_lua_type["ellipse"] = sol::overload(
        [](xwidgets_type & self, double x, double y, double radius_x, double radius_y, double rotation, double start_angle, double end_angle, bool anticlockwise){
            self.ellipse(x, y, radius_x, radius_y, rotation, start_angle, end_angle, anticlockwise);
        },
        [](xwidgets_type & self, coord_t coord, coord_t radius, double rotation, double start_angle, double end_angle, bool anticlockwise){
            self.ellipse(coord[0], coord[1], radius[0], radius[1], rotation, start_angle, end_angle, anticlockwise);
        }
    );
    canvas_lua_type["arc_to"] = sol::overload(
        [](xwidgets_type & self, double x1, double y1, double x2, double y2, double radius){
            self.arc_to(x1, y1, x2, y2, radius);
        },
        [](xwidgets_type & self, coord_t a, coord_t b, double radius){
            self.arc_to(a[0], a[1], b[0], b[1], radius);
        }
    );
    canvas_lua_type["quadratic_curve_to"] = sol::overload(
        [](xwidgets_type & self, double cp1x, double cp1y, double x, double y){
            self.quadratic_curve_to(cp1x, cp1y, x, y);
        },
        [](xwidgets_type & self, coord_t a, coord_t b){
            self.quadratic_curve_to(a[0], a[1], b[0], b[1]);
        }
    );
    canvas_lua_type["bezier_curve_to"] = sol::overload(
        [](xwidgets_type & self, double cp1x, double cp1y, double cp2x, double cp2y, double x, double y){
            self.bezier_curve_to(cp1x, cp1y, cp2x, cp2y, x, y);
        },
        [](xwidgets_type & self, coord_t a, coord_t b, coord_t c){
            self.bezier_curve_to(a[0], a[1], b[0], b[1], c[0], c[1]);
        }
    );

    // Clip methods
    canvas_lua_type["clip"] = &xwidgets_type::clip;

    // Transform methods
    canvas_lua_type["save"] = &xwidgets_type::save;
    canvas_lua_type["restore"] = &xwidgets_type::restore;
    canvas_lua_type["translate"] = sol::overload(
        [](xwidgets_type & self, double x, double y){
            self.translate(x, y);
        },
        [](xwidgets_type & self, coord_t a){
            self.translate(a[0], a[1]);
        }
    );
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


    ilua.canvas.color["rgb"] = function(r,g,b)
        return "rgb(" .. tostring(r) .. ", " ..  tostring(g) .. ", " .. tostring(b) .. ")"
    end

    function ilua.canvas.detail.xcanvas:rand_coord(c)
        return {
            math.random(1, self.width),
            math.random(1, self.height)
        }
    end

    function ilua.canvas.detail.xcanvas:rand_color()
        return ilua.canvas.color.rgb(
            math.random(50, 200),
            math.random(50, 200),
            math.random(50, 200)
        )
    end

    function ilua.canvas.detail.xcanvas:with(options, callback)
        local options = options or {}
        local old_options = {}

        for kw,val in pairs(options) do
            if w[kw] ~= nil then
                old_options[kw] = self[kw]
                self[kw] = val
            else
                error(k.." has no attribute `"..kw.."`")
            end
        end

        callback()
        for kw,val in pairs(options) do
            if w[kw] ~= nil then
                self[kw] = old_options[kw]
            else
                error(k.." has no attribute `"..kw.."`")
            end
        end
    end

    function ilua.canvas.detail.xcanvas:with_save_and_restore(options, f)
        self:with(options, function()
            self:save()
            f()
            self:restore()
        end)
    end

    function ilua.canvas.detail.xcanvas:rotate_arround(point, angle)
        self:translate(point[1],point[2])
        self:rotate(angle)
        self:translate(-1.0 * point[1],-1.0 * point[2])
    end

    function ilua.canvas.detail.xcanvas:fill_bg(options)
        local options = options or {}
        self:with(options, function() 
            self:fill_rect({0,0}, {canvas.width, canvas.height})
        end)
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
