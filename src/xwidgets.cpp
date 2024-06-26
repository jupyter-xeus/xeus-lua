#ifndef XLUA_XWIDGETS_HPP
#define XLUA_XWIDGETS_HPP

#include <vector>
#include <string>
#include <iostream>
#include <optional>

#include "sol/sol.hpp"
#include "xeus/xinterpreter.hpp"
#include "xeus/xguid.hpp"

#include "xproperty/xproperty.hpp"

#include "xwidgets/xslider.hpp"
#include "xwidgets/xbutton.hpp"
#include "xwidgets/xbox.hpp"
#include "xwidgets/xaccordion.hpp"
#include "xwidgets/xaudio.hpp"
#include "xwidgets/xcheckbox.hpp"
#include "xwidgets/xcolor_picker.hpp"
#include "xwidgets/xcontroller.hpp"
#include "xwidgets/xdropdown.hpp"
#include "xwidgets/xhtml.hpp"
#include "xwidgets/ximage.hpp"
#include "xwidgets/xlabel.hpp"
#include "xwidgets/xhtml.hpp"
#include "xwidgets/xpassword.hpp"
#include "xwidgets/xplay.hpp"
#include "xwidgets/xprogress.hpp"
#include "xwidgets/xradiobuttons.hpp"
#include "xwidgets/xselectionslider.hpp"
#include "xwidgets/xtab.hpp"
#include "xwidgets/xtextarea.hpp"
#include "xwidgets/xtext.hpp"
#include "xwidgets/xtogglebutton.hpp"
#include "xwidgets/xtogglebuttons.hpp"
#include "xwidgets/xvideo.hpp"
#include "xwidgets/xvalid.hpp"
#include "xwidgets/xlink.hpp"
#include "xwidgets/xoutput.hpp"


namespace sol {

        template<template <class> class B, class... P>
        struct is_container<xw::xmaterialize<B, P...>> : std::false_type {};

        template <>
        struct is_automagical<xw::xholder> : std::false_type {};


        // template <>
        // struct is_container<std::pair<>> : std::false_type {};

        template <>
        struct is_container<xw::xholder> : std::false_type {};

        template <>
        struct is_automagical<xeus::xguid> : std::false_type {};

        template <>
        struct is_container<xeus::xguid> : std::false_type {};

        template <>
        struct is_container<std::vector<xw::xholder> > : std::false_type {};

}

namespace xlua
{


#define XLUA_ADD_PROPERTY_WITH_EXPLICIT_NAME(CLS_OBJ,PROPERTY_TYPE,PROPERTY_NAME, MEMBER_NAME)\
    CLS_OBJ.set(#PROPERTY_NAME, sol::property(\
        [](xwidgets_type & widget){\
            return PROPERTY_TYPE(widget.MEMBER_NAME);\
        }, \
        [](xwidgets_type & widget, const PROPERTY_TYPE & val){\
            widget.MEMBER_NAME = val;\
        })\
    )

#define XLUA_ADD_PROPERTY(CLS_OBJ,PROPERTY_TYPE,PROPERTY_NAME)\
    CLS_OBJ.set(#PROPERTY_NAME, sol::property(\
        [](xwidgets_type & widget){\
            return PROPERTY_TYPE(widget.PROPERTY_NAME);\
        }, \
        [](xwidgets_type & widget, const PROPERTY_TYPE & val){\
            widget.PROPERTY_NAME = val;\
        })\
    )

#define XLUA_ADD_DATA_PROPERTY(CLS_OBJ,PROPERTY_TYPE,PROPERTY_NAME)\
    CLS_OBJ.set(#PROPERTY_NAME, sol::property(\
        [](xwidgets_type & widget){\
            return PROPERTY_TYPE(widget.PROPERTY_NAME);\
        }, \
        [](xwidgets_type & widget, const std::string & val){\
            std::vector<char> data(val.begin(), val.end());\
            widget.PROPERTY_NAME = data;\
        })\
    )

#define XLUA_ADD_INDEX_PROPERTY(CLS_OBJ,PROPERTY_TYPE,PROPERTY_NAME)\
    CLS_OBJ.set(#PROPERTY_NAME, sol::property(\
        [](xwidgets_type & widget){\
            return PROPERTY_TYPE(widget.PROPERTY_NAME) + 1;\
        }, \
        [](xwidgets_type & widget, const PROPERTY_TYPE & val){\
            widget.PROPERTY_NAME = val -1;\
        })\
    )


#define XLUA_ADD_CONTAINER_PROPERTY(CLS_OBJ,PROPERTY_TYPE,PROPERTY_NAME)\
    CLS_OBJ.set(#PROPERTY_NAME, sol::property(\
        [](xwidgets_type & widget){\
            return PROPERTY_TYPE(widget.PROPERTY_NAME);\
        }, \
        [](xwidgets_type & widget, sol::as_table_t<PROPERTY_TYPE> val){\
            widget.PROPERTY_NAME = val;\
        })\
    )




#define XLUA_REGISTER_OBSERVER(CLS_OBJ, PROPERTY_TYPE, PROPERTY_NAME) \
    CLS_OBJ["register_observer"] = [](xwidgets_type & widget, sol::protected_function function){ \
        auto callback = [function](const auto& s) { \
            auto res = function.call(PROPERTY_TYPE(s.PROPERTY_NAME)); \
            if (!res.valid()) \
            { \
                auto & interpreter =  xeus::get_interpreter(); \
                sol::error err = res; \
                const auto error_str = err.what(); \
                interpreter.publish_execution_error(error_str,error_str,std::vector<std::string>(1,error_str)); \
            } \
        }; \
        XOBSERVE(widget, value, callback); \
    }; 

#define XLUA_REGISTER_INDEX_OBSERVER(CLS_OBJ, PROPERTY_TYPE, PROPERTY_NAME) \
    CLS_OBJ["register_observer"] = [](xwidgets_type & widget, sol::protected_function function){ \
        auto callback = [function](const auto& s) { \
            auto res = function.call(PROPERTY_TYPE(s.PROPERTY_NAME)+1); \
            if (!res.valid()) \
            { \
                auto & interpreter =  xeus::get_interpreter(); \
                sol::error err = res; \
                const auto error_str = err.what(); \
                interpreter.publish_execution_error(error_str,error_str,std::vector<std::string>(1,error_str)); \
            } \
        }; \
        XOBSERVE(widget, PROPERTY_NAME, callback); \
    }; 

#define XLUA_REGISTER_OPTIONAL_INDEX_OBSERVER(CLS_OBJ, PROPERTY_TYPE, PROPERTY_NAME) \
    CLS_OBJ["register_observer"] = [](xwidgets_type & widget, sol::protected_function function){ \
        auto callback = [function](const auto& s) { \
            auto res = function.call(std::optional<int>(s.PROPERTY_NAME).value()+1); \
            if (!res.valid()) \
            { \
                auto & interpreter =  xeus::get_interpreter(); \
                sol::error err = res; \
                const auto error_str = err.what(); \
                interpreter.publish_execution_error(error_str,error_str,std::vector<std::string>(1,error_str)); \
            } \
        }; \
        XOBSERVE(widget, PROPERTY_NAME, callback); \
    }; 



template<class xwidgets_type, class extend_f>
void register_widget_impl(sol::state_view  & lua, const std::string widget_name, extend_f && extend)
{
    // get widgets table
    sol::table ilua_table = lua["ilua"];
    sol::table widgets_table = ilua_table["widgets"];
    sol::table detail_table = widgets_table["detail"];


    // make usertype metatable
    sol::usertype<xwidgets_type> xwidgets_lua_type = detail_table.new_usertype<xwidgets_type>(
        widget_name,
        // 1 constructors
        sol::constructors<xwidgets_type()>()
    );
    // typical member function that returns a variable
    xwidgets_lua_type["display"] = &xwidgets_type::display;
    xwidgets_lua_type["id"] = &xwidgets_type::id;

    xwidgets_lua_type[sol::meta_function::to_string] = [widget_name](xwidgets_type & ){
        return widget_name;
    };

    extend(xwidgets_lua_type);
    detail_table[widget_name] = xwidgets_lua_type;
}


std::string vector_cls_name(const std::string cls_name)
{
    std::stringstream ss;
    ss<<cls_name;
    ss<<"_vector";
    return ss.str();
}

void register_widget_related_types(sol::state_view  & lua)
{
    // get widgets table
    sol::table ilua_table = lua["ilua"];
    sol::table widgets_table = ilua_table["widgets"];
    sol::table detail_table = widgets_table["detail"];

    {
        using binded_type = xeus::xguid;
        std::string name = "xguid";
        sol::usertype<binded_type> lua_type = detail_table.new_usertype<binded_type>(name,
            sol::constructors<binded_type()>()
        );
        detail_table[name] = lua_type;
    }

    // // char vector
    // {
    //     using binded_type = std::vector<char>;
    //     std::string name = vector_cls_name("char");
    //     sol::usertype<binded_type> xwidgets_lua_type = detail_table.new_usertype<binded_type>(name,
    //         sol::constructors<binded_type()>()
    //     );
    // }
    // // string vector
    // {
    //     using binded_type = std::vector<std::string>;
    //     std::string name = vector_cls_name("string");
    //     sol::usertype<binded_type> xwidgets_lua_type = detail_table.new_usertype<binded_type>(name,
    //         sol::constructors<binded_type()>()
    //     );
    // }

}


void register_xwidgets_impl(sol::state_view  & lua)
{

    // get widgets table
    sol::table ilua_table = lua["ilua"];
    sol::table widgets_table = ilua_table["widgets"];
    sol::table detail_table = widgets_table["detail"];

    {
        using xwidgets_type =  xw::button;
        register_widget_impl<xwidgets_type>(lua, "xbutton",[](auto && xwidgets_lua_type){

            XLUA_ADD_PROPERTY(xwidgets_lua_type, std::string, description);
            XLUA_ADD_PROPERTY(xwidgets_lua_type, std::string, tooltip);
            XLUA_ADD_PROPERTY(xwidgets_lua_type, bool, disabled);
            XLUA_ADD_PROPERTY(xwidgets_lua_type, std::string, icon);
            XLUA_ADD_PROPERTY(xwidgets_lua_type, std::string, button_style);

            //XLUA_ADD_PROPERTY(xwidgets_lua_type, button_style, std::string);
            xwidgets_lua_type["on_click"] = [](xwidgets_type & widget, sol::protected_function function){
                auto callback = [function]() {
                    auto res = function.call();
                    if (!res.valid())
                    {
                        auto & interpreter =  xeus::get_interpreter();
                        sol::error err = res;
                        const auto error_str = err.what();
                        interpreter.publish_execution_error(error_str,error_str,std::vector<std::string>(1,error_str));
                    }
                };
                widget.on_click(typename xwidgets_type::click_callback_type(callback));
            };
        });
    }

    
    {
        using xwidgets_type =  xw::slider<double>;
        register_widget_impl<xwidgets_type>(lua, "xslider",[](auto && xwidgets_lua_type){

            XLUA_ADD_PROPERTY(xwidgets_lua_type, double, min);
            XLUA_ADD_PROPERTY(xwidgets_lua_type, double, max);
            XLUA_ADD_PROPERTY(xwidgets_lua_type, double, value);
            XLUA_ADD_PROPERTY(xwidgets_lua_type, double, step);
            XLUA_ADD_PROPERTY(xwidgets_lua_type, std::string, orientation);
            XLUA_ADD_PROPERTY(xwidgets_lua_type, bool, readout);
            XLUA_ADD_PROPERTY(xwidgets_lua_type, std::string, readout_format);
            XLUA_ADD_PROPERTY(xwidgets_lua_type, bool, continuous_update);
            XLUA_ADD_PROPERTY(xwidgets_lua_type, bool, disabled);

            XLUA_REGISTER_OBSERVER(xwidgets_lua_type, double, value);

        });
    }

    {
        using xwidgets_type = xw::output;
        const std::string widget_name = "xoutput";


         // make usertype metatable
        sol::usertype<xwidgets_type> xwidgets_lua_type = detail_table.new_usertype<xwidgets_type>(
            widget_name.c_str(),
            // 1 constructors
            sol::constructors<xwidgets_type()>()
        );
        // typical member function that returns a variable
        xwidgets_lua_type["display"] = &xwidgets_type::display;
        xwidgets_lua_type["capture"] = &xwidgets_type::capture;
        xwidgets_lua_type["release"] = &xwidgets_type::release;
        xwidgets_lua_type["captured"] = [](xwidgets_type & widget, sol::protected_function function){
            widget.capture();
            auto res = function.call();
            if (!res.valid())
            {
                auto & interpreter =  xeus::get_interpreter();
                sol::error err = res;
                const auto error_str = err.what();
                interpreter.publish_execution_error(error_str,error_str,std::vector<std::string>(1,error_str));
            }
            widget.release();
        };
        detail_table[widget_name] = xwidgets_lua_type;
    }



    {
        using xwidgets_type =  xw::hbox;
        register_widget_impl<xwidgets_type>(lua, "xhbox",[](auto && xwidgets_lua_type){
            xwidgets_lua_type["_add"] = [](xwidgets_type & widget, xeus::xguid id){
                widget.add(id);
            };
        });
    }
    {
        using xwidgets_type =  xw::vbox;
        register_widget_impl<xwidgets_type>(lua, "xvbox",[](auto && xwidgets_lua_type){

            xwidgets_lua_type["_add"] = [](xwidgets_type & widget, xeus::xguid id){
                widget.add(id);
            };

        });
    }

    {
        using xwidgets_type =  xw::accordion;
        register_widget_impl<xwidgets_type>(lua, "xaccordion",[](auto && xwidgets_lua_type){

            xwidgets_lua_type["_add"] = [](xwidgets_type & widget, xeus::xguid id){
                widget.add(id);
            };

            xwidgets_lua_type["set_title"] = [](xwidgets_type & widget, std::size_t i, std::string title){
                widget.set_title(i-1, title);
            };

        });
    }
    {
        using xwidgets_type =  xw::audio;
        register_widget_impl<xwidgets_type>(lua, "xaudio",[](auto && xwidgets_lua_type){

            XLUA_ADD_DATA_PROPERTY(xwidgets_lua_type, std::vector<char>,  value);
            XLUA_ADD_PROPERTY(xwidgets_lua_type, bool, autoplay);
            XLUA_ADD_PROPERTY(xwidgets_lua_type, bool, loop);
            XLUA_ADD_PROPERTY(xwidgets_lua_type, bool, controls);

        });
    }
    {
        using xwidgets_type =  xw::checkbox;
        register_widget_impl<xwidgets_type>(lua, "xcheckbox",[](auto && xwidgets_lua_type){

            XLUA_ADD_PROPERTY(xwidgets_lua_type, bool, value);
            XLUA_ADD_PROPERTY(xwidgets_lua_type, bool, disabled);
            XLUA_ADD_PROPERTY(xwidgets_lua_type, bool, indent);

            XLUA_REGISTER_OBSERVER(xwidgets_lua_type, bool, value);

        });
    }
    {
        using xwidgets_type =  xw::color_picker;
        register_widget_impl<xwidgets_type>(lua, "xcolor_picker",[](auto && xwidgets_lua_type){
       

            XLUA_ADD_PROPERTY(xwidgets_lua_type, std::string, value);
            XLUA_ADD_PROPERTY(xwidgets_lua_type, bool, disabled);
            XLUA_ADD_PROPERTY(xwidgets_lua_type, bool, concise);

            XLUA_REGISTER_OBSERVER(xwidgets_lua_type, std::string, value);

        });
    }
    {
        using xwidgets_type =  xw::controller;
        register_widget_impl<xwidgets_type>(lua, "xcontroller",[](auto && xwidgets_lua_type){
       
            XLUA_ADD_PROPERTY(xwidgets_lua_type, int, index);
            XLUA_ADD_PROPERTY(xwidgets_lua_type, std::string, name);
            XLUA_ADD_PROPERTY(xwidgets_lua_type, std::string, mapping);
            XLUA_ADD_PROPERTY(xwidgets_lua_type, bool, connected);
            XLUA_ADD_PROPERTY(xwidgets_lua_type, double, timestamp);
            //XLUA_ADD_PROPERTY(xwidgets_lua_type, xcontroller_button_list_type, buttons);
            //XLUA_ADD_PROPERTY(xwidgets_lua_type, xcontroller_axis_list_type, axes);

        });
    }
    {
        using xwidgets_type =  xw::dropdown;
        register_widget_impl<xwidgets_type>(lua, "xdropdown",[](auto && xwidgets_lua_type){
       
            using options_type = typename xwidgets_type::options_type;
            using value_type = typename xwidgets_type::value_type;
            using index_type = typename xwidgets_type::index_type;

            XLUA_ADD_INDEX_PROPERTY(xwidgets_lua_type, index_type, index);
            XLUA_ADD_CONTAINER_PROPERTY(xwidgets_lua_type, options_type, _options_labels);
            XLUA_ADD_PROPERTY(xwidgets_lua_type, std::string, description);
            XLUA_ADD_PROPERTY(xwidgets_lua_type, bool, disabled);
            XLUA_ADD_PROPERTY(xwidgets_lua_type, value_type, value);

            XLUA_REGISTER_INDEX_OBSERVER(xwidgets_lua_type, int, index);
        });
    }
    {
        using xwidgets_type =  xw::html;
        register_widget_impl<xwidgets_type>(lua, "xhtml",[](auto && xwidgets_lua_type){
       
            XLUA_ADD_PROPERTY(xwidgets_lua_type, std::string, description);
            XLUA_ADD_PROPERTY(xwidgets_lua_type, std::string, value);
            XLUA_ADD_PROPERTY(xwidgets_lua_type, std::string, placeholder);

            //XLUA_REGISTER_INDEX_OBSERVER(xwidgets_lua_type, int, index);
        });
    }
    {
        using xwidgets_type =  xw::image;
        register_widget_impl<xwidgets_type>(lua, "ximage",[](auto && xwidgets_lua_type){

            XLUA_ADD_DATA_PROPERTY(xwidgets_lua_type, std::vector<char>,  value);
            XLUA_ADD_PROPERTY(xwidgets_lua_type, std::string, format);
            XLUA_ADD_PROPERTY(xwidgets_lua_type, std::string, width);
            XLUA_ADD_PROPERTY(xwidgets_lua_type, std::string, height);

        });
    }
    {
        using xwidgets_type =  xw::label;
        register_widget_impl<xwidgets_type>(lua, "xlabel",[](auto && xwidgets_lua_type){
       
            XLUA_ADD_PROPERTY(xwidgets_lua_type, std::string, description);
            XLUA_ADD_PROPERTY(xwidgets_lua_type, std::string, value);
            XLUA_ADD_PROPERTY(xwidgets_lua_type, std::string, placeholder);

            //XLUA_REGISTER_INDEX_OBSERVER(xwidgets_lua_type, int, index);
        });
    }
    // {
    //     using xwidgets_type =  xw::numeral<double>;
    //     register_widget_impl<xwidgets_type>(lua, "xnumeral",[](auto && xwidgets_lua_type){

    //         XLUA_ADD_PROPERTY(xwidgets_lua_type, double, min);
    //         XLUA_ADD_PROPERTY(xwidgets_lua_type, double, max);
    //         XLUA_ADD_PROPERTY(xwidgets_lua_type, double, value);
    //         XLUA_ADD_PROPERTY(xwidgets_lua_type, double, step);
    //         XLUA_ADD_PROPERTY(xwidgets_lua_type, bool, continuous_update);
    //         XLUA_ADD_PROPERTY(xwidgets_lua_type, bool, disabled);

    //         XLUA_REGISTER_OBSERVER(xwidgets_lua_type, double, value);

    //     });
    // }
    {
        using xwidgets_type =  xw::password;
        register_widget_impl<xwidgets_type>(lua, "xpassword",[](auto && xwidgets_lua_type){
       
            XLUA_ADD_PROPERTY(xwidgets_lua_type, std::string, description);
            XLUA_ADD_PROPERTY(xwidgets_lua_type, std::string, value);
            XLUA_ADD_PROPERTY(xwidgets_lua_type, std::string, placeholder);
            XLUA_ADD_PROPERTY(xwidgets_lua_type, bool, disabled);
            
            XLUA_REGISTER_OBSERVER(xwidgets_lua_type, std::string, value);
        });
    }
    {
        using xwidgets_type =  xw::play;
        register_widget_impl<xwidgets_type>(lua, "xplay",[](auto && xwidgets_lua_type){

            XLUA_ADD_PROPERTY(xwidgets_lua_type, double, min);
            XLUA_ADD_PROPERTY(xwidgets_lua_type, double, max);
            XLUA_ADD_PROPERTY(xwidgets_lua_type, double, value);

            XLUA_ADD_PROPERTY(xwidgets_lua_type, double, interval);

            XLUA_ADD_PROPERTY(xwidgets_lua_type, bool, disabled);
            XLUA_ADD_PROPERTY(xwidgets_lua_type, double, step);
            
            #if XWIDGETS_VERSION_MAJOR==0 && XWIDGETS_VERSION_MINOR < 28
                XLUA_ADD_PROPERTY_WITH_EXPLICIT_NAME(xwidgets_lua_type, bool, playing, _playing);
                XLUA_ADD_PROPERTY_WITH_EXPLICIT_NAME(xwidgets_lua_type, bool, repeat, _repeat);
            #else
                XLUA_ADD_PROPERTY(xwidgets_lua_type, bool, playing);
                XLUA_ADD_PROPERTY(xwidgets_lua_type, bool, repeat);
            #endif
            XLUA_ADD_PROPERTY(xwidgets_lua_type, bool, show_repeat);

            XLUA_REGISTER_OBSERVER(xwidgets_lua_type, double, value);

        });
    }
    {
        using xwidgets_type =  xw::progress<double>;
        register_widget_impl<xwidgets_type>(lua, "xprogress",[](auto && xwidgets_lua_type){

            XLUA_ADD_PROPERTY(xwidgets_lua_type, double, min);
            XLUA_ADD_PROPERTY(xwidgets_lua_type, double, max);
            XLUA_ADD_PROPERTY(xwidgets_lua_type, double, value);
            XLUA_ADD_PROPERTY(xwidgets_lua_type, std::string, orientation);
            XLUA_ADD_PROPERTY(xwidgets_lua_type, std::string, bar_style);

            XLUA_REGISTER_OBSERVER(xwidgets_lua_type, double, value);

        });
    }
    {
        using xwidgets_type =  xw::selectionslider;
        register_widget_impl<xwidgets_type>(lua, "xselectionslider",[](auto && xwidgets_lua_type){
       
            using options_type = typename xwidgets_type::options_type;
            using value_type = typename xwidgets_type::value_type;
            using index_type = typename xwidgets_type::index_type;

            XLUA_ADD_INDEX_PROPERTY(xwidgets_lua_type, index_type, index);
            XLUA_ADD_CONTAINER_PROPERTY(xwidgets_lua_type, options_type, _options_labels);
            XLUA_ADD_PROPERTY(xwidgets_lua_type, std::string, description);
            XLUA_ADD_PROPERTY(xwidgets_lua_type, bool, disabled);
            XLUA_ADD_PROPERTY(xwidgets_lua_type, value_type, value);

            XLUA_ADD_PROPERTY(xwidgets_lua_type, std::string, orientation);
            XLUA_ADD_PROPERTY(xwidgets_lua_type, bool, readout);
            XLUA_ADD_PROPERTY(xwidgets_lua_type, bool, continuous_update);

            XLUA_REGISTER_INDEX_OBSERVER(xwidgets_lua_type, int, index);
        });
    }
    {
        using xwidgets_type =  xw::tab;
        register_widget_impl<xwidgets_type>(lua, "xtab",[](auto && xwidgets_lua_type){

            using titles_type = typename xwidgets_type::titles_type;
            #if XWIDGETS_VERSION_MAJOR==0 && XWIDGETS_VERSION_MINOR < 28
                XLUA_ADD_PROPERTY_WITH_EXPLICIT_NAME(xwidgets_lua_type, titles_type, titles, _titles);
            #else
                XLUA_ADD_CONTAINER_PROPERTY(xwidgets_lua_type, titles_type, titles);
            #endif

            xwidgets_lua_type["_add"] = [](xwidgets_type & widget, xeus::xguid id){
                widget.add(id);
            };

            XLUA_REGISTER_OPTIONAL_INDEX_OBSERVER(xwidgets_lua_type, int, selected_index);
        });
    }
    {
        using xwidgets_type =  xw::text;
        register_widget_impl<xwidgets_type>(lua, "xtext",[](auto && xwidgets_lua_type){

            using submit_callback_type = typename xwidgets_type::submit_callback_type;

            XLUA_ADD_PROPERTY(xwidgets_lua_type, std::string, description);
            XLUA_ADD_PROPERTY(xwidgets_lua_type, std::string, value);
            XLUA_ADD_PROPERTY(xwidgets_lua_type, std::string, placeholder);
            XLUA_ADD_PROPERTY(xwidgets_lua_type, bool, disabled);
            XLUA_ADD_PROPERTY(xwidgets_lua_type, bool, continuous_update);

            XLUA_REGISTER_OBSERVER(xwidgets_lua_type, std::string, value);

            xwidgets_lua_type["on_submit"] = [](xwidgets_type & widget, sol::protected_function function){
                auto callback = [function]() {
                    auto res = function.call();
                    if (!res.valid())
                    {
                        auto & interpreter =  xeus::get_interpreter();
                        sol::error err = res;
                        const auto error_str = err.what();
                        interpreter.publish_execution_error(error_str,error_str,std::vector<std::string>(1,error_str));
                    }
                };
                widget.on_submit(submit_callback_type(callback));
            };

        });
    }
    {
        using xwidgets_type =  xw::textarea;
        register_widget_impl<xwidgets_type>(lua, "xtextarea",[](auto && xwidgets_lua_type){

            XLUA_ADD_PROPERTY(xwidgets_lua_type, std::string, description);
            XLUA_ADD_PROPERTY(xwidgets_lua_type, std::string, value);
            XLUA_ADD_PROPERTY(xwidgets_lua_type, std::string, placeholder);
            XLUA_ADD_PROPERTY(xwidgets_lua_type, bool, disabled);
            XLUA_ADD_PROPERTY(xwidgets_lua_type, bool, continuous_update);

            XLUA_REGISTER_OBSERVER(xwidgets_lua_type, std::string, value);

        });
    }

    {
        using xwidgets_type =  xw::togglebutton;
        register_widget_impl<xwidgets_type>(lua, "xtogglebutton",[](auto && xwidgets_lua_type){
            XLUA_ADD_PROPERTY(xwidgets_lua_type, bool, value);
            XLUA_ADD_PROPERTY(xwidgets_lua_type, bool, disabled);
            XLUA_ADD_PROPERTY(xwidgets_lua_type, std::string, tooltip);
            XLUA_ADD_PROPERTY(xwidgets_lua_type, std::string, icon);
            XLUA_ADD_PROPERTY(xwidgets_lua_type, std::string, button_style);


            XLUA_REGISTER_OBSERVER(xwidgets_lua_type, bool, value);

        });
    }
    {
        using xwidgets_type =  xw::togglebuttons;
        register_widget_impl<xwidgets_type>(lua, "xtogglebuttons",[](auto && xwidgets_lua_type){

       
            using options_type = typename xwidgets_type::options_type;
            using value_type = typename xwidgets_type::value_type;
            using index_type = typename xwidgets_type::index_type;

            XLUA_ADD_INDEX_PROPERTY(xwidgets_lua_type, index_type, index);
            XLUA_ADD_CONTAINER_PROPERTY(xwidgets_lua_type, options_type, _options_labels);

            XLUA_ADD_PROPERTY(xwidgets_lua_type, std::string, description);
            XLUA_ADD_PROPERTY(xwidgets_lua_type, bool, disabled);

            // XLUA_ADD_CONTAINER_PROPERTY(xwidgets_lua_type, std::vector<std::string>, tooltips);
            // XLUA_ADD_CONTAINER_PROPERTY(xwidgets_lua_type, std::vector<std::string>, icons);
            // XLUA_ADD_PROPERTY(xwidgets_lua_type, std::string, button_style);

            XLUA_REGISTER_OBSERVER(xwidgets_lua_type, std::string, value);

        });
    }
    {
        using xwidgets_type =  xw::video;
        register_widget_impl<xwidgets_type>(lua, "xvideo",[](auto && xwidgets_lua_type){

            XLUA_ADD_DATA_PROPERTY(xwidgets_lua_type, std::vector<char>,  value);
            XLUA_ADD_PROPERTY(xwidgets_lua_type, std::string, format);
            XLUA_ADD_PROPERTY(xwidgets_lua_type, std::string, width);
            XLUA_ADD_PROPERTY(xwidgets_lua_type, std::string, height);
            XLUA_ADD_PROPERTY(xwidgets_lua_type, bool, autoplay);
            XLUA_ADD_PROPERTY(xwidgets_lua_type, bool, loop);
            XLUA_ADD_PROPERTY(xwidgets_lua_type, bool, controls);

        });
    }
    {
        using xwidgets_type =  xw::valid;
        register_widget_impl<xwidgets_type>(lua, "xvalid",[](auto && xwidgets_lua_type){
            XLUA_ADD_PROPERTY(xwidgets_lua_type, std::string,  readout);
            XLUA_ADD_PROPERTY(xwidgets_lua_type, bool, value);
            XLUA_REGISTER_OBSERVER(xwidgets_lua_type, bool, value);
        });
    }
    {
        using xwidgets_type =  xw::link;
        sol::usertype<xwidgets_type> xwidgets_lua_type = detail_table.new_usertype<xwidgets_type>(
            "xlink",
            sol::factories([](
                const xeus::xguid id_source,
                std::string n_source,
                const xeus::xguid id_target,
                std::string n_target
            ){
                return xwidgets_type(
                    xw::make_id_holder(id_source),
                    n_source,
                    xw::make_id_holder(id_target),
                    n_target
                );
            })
        );
        // typical member function that returns a variable
        xwidgets_lua_type["display"] = &xwidgets_type::display;
        xwidgets_lua_type["id"] = &xwidgets_type::id;
        detail_table["xlink"] = xwidgets_lua_type;
    }
    {
        using xwidgets_type =  xw::directional_link;
        sol::usertype<xwidgets_type> xwidgets_lua_type = detail_table.new_usertype<xwidgets_type>(
            "xdirectional_link",
            sol::factories([](
                const xeus::xguid id_source,
                std::string n_source,
                const xeus::xguid id_target,
                std::string n_target
            ){
                return xwidgets_type(
                    xw::make_id_holder(id_source),
                    n_source,
                    xw::make_id_holder(id_target),
                    n_target
                );
            })
        );
        // typical member function that returns a variable
        xwidgets_lua_type["display"] = &xwidgets_type::display;
        xwidgets_lua_type["id"] = &xwidgets_type::id;
        detail_table["xdirectional_link"] = xwidgets_lua_type;        
    }
}

void extend_xwidgets(sol::state_view  & lua)
{
    const std::string extend = R""""(
local atomic_widgets = {
    xslider =  ilua.widgets.detail.xslider,
    xbutton =  ilua.widgets.detail.xbutton,
    xaudio =  ilua.widgets.detail.xaudio,
    xcheckbox =  ilua.widgets.detail.xcheckbox,
    xcolor_picker =  ilua.widgets.detail.xcolor_picker,
    xcontroller =  ilua.widgets.detail.xcontroller,
    xdropdown =  ilua.widgets.detail.xdropdown,
    xhtml =  ilua.widgets.detail.xhtml,
    ximage =  ilua.widgets.detail.ximage,
    xlabel =  ilua.widgets.detail.xlabel,
    xhtml =  ilua.widgets.detail.xhtml,
    xnumeral =  ilua.widgets.detail.xnumeral,
    xpassword =  ilua.widgets.detail.xpassword,
    xplay =  ilua.widgets.detail.xplay,
    xprogress =  ilua.widgets.detail.xprogress,
    xradiobuttons =  ilua.widgets.detail.xradiobuttons,
    xselectionslider =  ilua.widgets.detail.xselectionslider,
    xtextarea =  ilua.widgets.detail.xtextarea,
    xtext =  ilua.widgets.detail.xtext,
    xtogglebutton =  ilua.widgets.detail.xtogglebutton,
    xtogglebuttons =  ilua.widgets.detail.xtogglebuttons,
    xvideo =  ilua.widgets.detail.xvideo,
    xvalid =  ilua.widgets.detail.xvalid,
    xoutput =  ilua.widgets.detail.xoutput
}

local layout_widgets = {
    xvbox = ilua.widgets.detail.xvbox,
    xhbox = ilua.widgets.detail.xhbox,
    xtab = ilua.widgets.detail.xtab,
    xaccordion = ilua.widgets.detail.xaccordion
}
local link_widgets = {
    xlink = ilua.widgets.detail.xlink,
    xdirectional_link = ilua.widgets.detail.xdirectional_link
}


local concat_tables = function(...)
    local ret = {}
    -- tables = table.pack(...)
    tables = {n = select('#', ...), ...}
    for i=1,tables.n do
        t = tables[i]
        for kw,val in pairs(t) do
            ret[kw] = val
        end
    end
    
    return ret
end



-- add 
for k,widget_cls in pairs(layout_widgets) do
    function widget_cls:add(...)
        args = {n = select('#', ...), ...}
        for i=1,args.n do
            arg = args[i]
            self:_add(arg:id())
        end
    end
end

-- add factory function
for k,widget_cls in pairs( concat_tables( layout_widgets, atomic_widgets)) do
    ilua.widgets[k:sub(2)] = function(options) 
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

-- add factory function
for k,widget_cls in pairs( link_widgets) do
    ilua.widgets[k:sub(2)] = function(widget_a, property_a, widget_b, property_b) 
        w = widget_cls.new(widget_a:id(), property_a, widget_b:id(), property_b)
        return w
    end
end

for k,widget_cls in pairs( concat_tables( link_widgets, layout_widgets, atomic_widgets)) do
    mc = getmetatable(widget_cls)
    function mc.__tostring(...)
        return k
    end
end
    )"""";
    auto code_result = lua.script(extend);
    if (!code_result.valid()) {
        sol::error err = code_result;
        std::cerr << "failed to load string-based script into the program for xwidgets" << err.what() << std::endl;
        throw std::runtime_error(err.what());
    }
}


void register_xwidgets(sol::state_view  & lua)
{
    register_widget_related_types(lua);
    register_xwidgets_impl(lua);
    extend_xwidgets(lua);
}

}

#undef XLUA_REGISTER_OBSERVER
#undef XLUA_ADD_PROPERTY
#endif
