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


#define XLUA_ADD_PROPERTY(CLS_OBJ,PROPERTY_TYPE,PROPERTY_NAME)\
    CLS_OBJ.set(#PROPERTY_NAME, sol::property(\
        [](xwidgtes_type & widget){\
            return PROPERTY_TYPE(widget.PROPERTY_NAME);\
        }, \
        [](xwidgtes_type & widget, const PROPERTY_TYPE & val){\
            widget.PROPERTY_NAME = val;\
        })\
    )


namespace xlua
{




#define XLUA_ADD_PROPERTY(CLS_OBJ,PROPERTY_TYPE,PROPERTY_NAME)\
    CLS_OBJ.set(#PROPERTY_NAME, sol::property(\
        [](xwidgtes_type & widget){\
            return PROPERTY_TYPE(widget.PROPERTY_NAME);\
        }, \
        [](xwidgtes_type & widget, const PROPERTY_TYPE & val){\
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



    using xwidgtes_type = xc::canvas;
    const std::string widget_name  = "xcanvas";
    // make usertype metatable
    sol::usertype<xwidgtes_type> canvas_lua_type = detail_table.new_usertype<xwidgtes_type>(
        widget_name,
        // 1 constructors
        sol::constructors<xwidgtes_type()>()
    );

    // special functions
    canvas_lua_type[sol::meta_function::to_string] = [widget_name](xwidgtes_type & ){
        return widget_name;
    };


    // simpl
    canvas_lua_type["display"] = &xwidgtes_type::display;
    canvas_lua_type["flush"] = &xwidgtes_type::flush;
    canvas_lua_type["clear"] = &xwidgtes_type::clear;


    // some overloaded function
    canvas_lua_type["fill_rect"] = sol::overload( 
        [](xwidgtes_type & self,  double x, double y, double width){
            self.fill_rect(x,y,width);
        },
        [](xwidgtes_type & self,  double x, double y, double width, double height){
            self.fill_rect(x,y,width, height);
        }
    );

    // simple non-overloaded function
    canvas_lua_type["fill_circle"] = &xwidgtes_type::fill_circle;


    detail_table[widget_name] = canvas_lua_type;




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

    )"""";
    sol::protected_function_result code_result  = lua.safe_script(script, &sol::script_pass_on_error);
    if (!code_result.valid()) {
        sol::error err = code_result;
        std::cerr << "failed to load string-based script into the program for xcanvas" << err.what() << std::endl;
        throw std::runtime_error(err.what());
    }

}


}
