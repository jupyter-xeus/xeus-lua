#ifndef XLUA_XWIDGETS_HPP
#define XLUA_XWIDGETS_HPP

#include <vector>
#include <string>

#include "xeus-lua/sol/sol.hpp"
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
#include "xwidgets/xnumeral.hpp"
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


#define XLUA_ADD_PROPERTY(CLS_OBJ,PROPERTY_TYPE,PROPERTY_NAME)\
    CLS_OBJ.set(#PROPERTY_NAME, sol::property(\
        [](xwidgtes_type & widget){\
            return PROPERTY_TYPE(widget.PROPERTY_NAME);\
        }, \
        [](xwidgtes_type & widget, const PROPERTY_TYPE & val){\
            widget.PROPERTY_NAME = val;\
        })\
    )

#define XLUA_ADD_INDEX_PROPERTY(CLS_OBJ,PROPERTY_TYPE,PROPERTY_NAME)\
    CLS_OBJ.set(#PROPERTY_NAME, sol::property(\
        [](xwidgtes_type & widget){\
            return PROPERTY_TYPE(widget.PROPERTY_NAME) + 1;\
        }, \
        [](xwidgtes_type & widget, const PROPERTY_TYPE & val){\
            widget.PROPERTY_NAME = val -1;\
        })\
    )


#define XLUA_ADD_CONTAINER_PROPERTY(CLS_OBJ,PROPERTY_TYPE,PROPERTY_NAME)\
    CLS_OBJ.set(#PROPERTY_NAME, sol::property(\
        [](xwidgtes_type & widget){\
            return PROPERTY_TYPE(widget.PROPERTY_NAME);\
        }, \
        [](xwidgtes_type & widget, sol::as_table_t<PROPERTY_TYPE> val){\
            widget.PROPERTY_NAME = val;\
        })\
    )




#define XLUA_REGISTER_OBSERVER(CLS_OBJ, PROPERTY_TYPE, PROPERTY_NAME) \
    CLS_OBJ["register_observer"] = [](xwidgtes_type & widget, sol::unsafe_function function){ \
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
    CLS_OBJ["register_observer"] = [](xwidgtes_type & widget, sol::unsafe_function function){ \
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
    CLS_OBJ["register_observer"] = [](xwidgtes_type & widget, sol::unsafe_function function){ \
        auto callback = [function](const auto& s) { \
            auto res = function.call(xtl::xoptional<int>(s.PROPERTY_NAME).value()+1); \
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



template<class xwidgtes_type, class extend_f>
void register_widget_impl(sol::state & lua, const std::string widget_name, extend_f && extend)
{
    // make usertype metatable
    sol::usertype<xwidgtes_type> xwidgtes_lua_type = lua.new_usertype<xwidgtes_type>(
        widget_name.c_str(),
        // 1 constructors
        sol::constructors<xwidgtes_type()>()
    );
    // typical member function that returns a variable
    xwidgtes_lua_type["display"] = &xwidgtes_type::display;
    xwidgtes_lua_type["id"] = &xwidgtes_type::id;

    extend(xwidgtes_lua_type);

}


std::string vector_cls_name(const std::string cls_name)
{
    std::stringstream ss;
    ss<<cls_name;
    ss<<"_vector";
    return ss.str();
}

void register_widget_related_types(sol::state & lua)
{
    // // xholder
    // {
    //     using binded_type = xw::xholder;
    //     std::string name = "xholder";
    //     sol::usertype<binded_type> xwidgtes_lua_type = lua.new_usertype<binded_type>(name,
    //         sol::constructors<binded_type()>()
    //     );
    // }
    // xguid
    {
        using binded_type = xeus::xguid;
        std::string name = "xguid";
        sol::usertype<binded_type> xwidgtes_lua_type = lua.new_usertype<binded_type>(name,
            sol::constructors<binded_type()>()
        );
    }

    // char vector
    {
        using binded_type = std::vector<char>;
        std::string name = vector_cls_name("char");
        sol::usertype<binded_type> xwidgtes_lua_type = lua.new_usertype<binded_type>(name,
            sol::constructors<binded_type()>()
        );
    }
    // string vector
    {
        using binded_type = std::vector<std::string>;
        std::string name = vector_cls_name("string");
        sol::usertype<binded_type> xwidgtes_lua_type = lua.new_usertype<binded_type>(name,
            sol::constructors<binded_type()>()
        );
    }

}


void register_xwidgets_impl(sol::state & lua)
{
    {
        using xwidgtes_type =  xw::button;
        register_widget_impl<xwidgtes_type>(lua, "xbutton",[](auto && xwidgtes_lua_type){

            XLUA_ADD_PROPERTY(xwidgtes_lua_type, std::string, description);
            XLUA_ADD_PROPERTY(xwidgtes_lua_type, std::string, tooltip);
            XLUA_ADD_PROPERTY(xwidgtes_lua_type, bool, disabled);
            XLUA_ADD_PROPERTY(xwidgtes_lua_type, std::string, icon);
            XLUA_ADD_PROPERTY(xwidgtes_lua_type, std::string, button_style);

            //XLUA_ADD_PROPERTY(xwidgtes_lua_type, button_style, std::string);
            xwidgtes_lua_type["on_click"] = [](xwidgtes_type & widget, sol::unsafe_function function){
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
                widget.on_click(typename xwidgtes_type::click_callback_type(callback));
            };
        });
    }

    
    {
        using xwidgtes_type =  xw::slider<double>;
        register_widget_impl<xwidgtes_type>(lua, "xslider",[](auto && xwidgtes_lua_type){

            XLUA_ADD_PROPERTY(xwidgtes_lua_type, double, min);
            XLUA_ADD_PROPERTY(xwidgtes_lua_type, double, max);
            XLUA_ADD_PROPERTY(xwidgtes_lua_type, double, value);
            XLUA_ADD_PROPERTY(xwidgtes_lua_type, double, step);
            XLUA_ADD_PROPERTY(xwidgtes_lua_type, std::string, orientation);
            XLUA_ADD_PROPERTY(xwidgtes_lua_type, bool, readout);
            XLUA_ADD_PROPERTY(xwidgtes_lua_type, std::string, readout_format);
            XLUA_ADD_PROPERTY(xwidgtes_lua_type, bool, continuous_update);
            XLUA_ADD_PROPERTY(xwidgtes_lua_type, bool, disabled);

            XLUA_REGISTER_OBSERVER(xwidgtes_lua_type, double, value);

        });
    }

    {
        using xwidgtes_type = xw::output;
        const std::string widget_name = "xoutput";
        
         // make usertype metatable
        sol::usertype<xwidgtes_type> xwidgtes_lua_type = lua.new_usertype<xwidgtes_type>(
            widget_name.c_str(),
            // 1 constructors
            sol::constructors<xwidgtes_type()>()
        );
        // typical member function that returns a variable
        xwidgtes_lua_type["display"] = &xwidgtes_type::display;
        xwidgtes_lua_type["capture"] = &xwidgtes_type::capture;
        xwidgtes_lua_type["release"] = &xwidgtes_type::release;
        xwidgtes_lua_type["captured"] = [](xwidgtes_type & widget, sol::unsafe_function function){
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
    }



    {
        using xwidgtes_type =  xw::hbox;
        register_widget_impl<xwidgtes_type>(lua, "xhbox",[](auto && xwidgtes_lua_type){
            xwidgtes_lua_type["add"] = [](xwidgtes_type & widget, xeus::xguid id){
                widget.add(id);
            };
        });
    }
    {
        using xwidgtes_type =  xw::hbox;
        register_widget_impl<xwidgtes_type>(lua, "vhbox",[](auto && xwidgtes_lua_type){

            xwidgtes_lua_type["add"] = [](xwidgtes_type & widget, xeus::xguid id){
                widget.add(id);
            };

        });
    }

    {
        using xwidgtes_type =  xw::accordion;
        register_widget_impl<xwidgtes_type>(lua, "xaccordion",[](auto && xwidgtes_lua_type){

            xwidgtes_lua_type["add"] = [](xwidgtes_type & widget, xeus::xguid id){
                widget.add(id);
            };

            xwidgtes_lua_type["set_title"] = [](xwidgtes_type & widget, std::size_t i, std::string title){
                widget.set_title(i-1, title);
            };

        });
    }
    {
        using xwidgtes_type =  xw::audio;
        register_widget_impl<xwidgtes_type>(lua, "xaudio",[](auto && xwidgtes_lua_type){

            XLUA_ADD_PROPERTY(xwidgtes_lua_type, std::vector<char>,  value);
            XLUA_ADD_PROPERTY(xwidgtes_lua_type, bool, autoplay);
            XLUA_ADD_PROPERTY(xwidgtes_lua_type, bool, loop);
            XLUA_ADD_PROPERTY(xwidgtes_lua_type, bool, controls);

        });
    }
    {
        using xwidgtes_type =  xw::checkbox;
        register_widget_impl<xwidgtes_type>(lua, "xcheckbox",[](auto && xwidgtes_lua_type){

            XLUA_ADD_PROPERTY(xwidgtes_lua_type, bool, value);
            XLUA_ADD_PROPERTY(xwidgtes_lua_type, bool, disabled);
            XLUA_ADD_PROPERTY(xwidgtes_lua_type, bool, indent);

            XLUA_REGISTER_OBSERVER(xwidgtes_lua_type, bool, value);

        });
    }
    {
        using xwidgtes_type =  xw::color_picker;
        register_widget_impl<xwidgtes_type>(lua, "xcolor_picker",[](auto && xwidgtes_lua_type){
       

            XLUA_ADD_PROPERTY(xwidgtes_lua_type, std::string, value);
            XLUA_ADD_PROPERTY(xwidgtes_lua_type, bool, disabled);
            XLUA_ADD_PROPERTY(xwidgtes_lua_type, bool, concise);

            XLUA_REGISTER_OBSERVER(xwidgtes_lua_type, std::string, value);

        });
    }
    {
        using xwidgtes_type =  xw::controller;
        register_widget_impl<xwidgtes_type>(lua, "xcontroller",[](auto && xwidgtes_lua_type){
       
            XLUA_ADD_PROPERTY(xwidgtes_lua_type, int, index);
            XLUA_ADD_PROPERTY(xwidgtes_lua_type, std::string, name);
            XLUA_ADD_PROPERTY(xwidgtes_lua_type, std::string, mapping);
            XLUA_ADD_PROPERTY(xwidgtes_lua_type, bool, connected);
            XLUA_ADD_PROPERTY(xwidgtes_lua_type, double, timestamp);
            //XLUA_ADD_PROPERTY(xwidgtes_lua_type, xcontroller_button_list_type, buttons);
            //XLUA_ADD_PROPERTY(xwidgtes_lua_type, xcontroller_axis_list_type, axes);

        });
    }
    {
        using xwidgtes_type =  xw::dropdown;
        register_widget_impl<xwidgtes_type>(lua, "xdropdown",[](auto && xwidgtes_lua_type){
       
            using options_type = typename xwidgtes_type::options_type;
            using value_type = typename xwidgtes_type::value_type;
            using index_type = typename xwidgtes_type::index_type;

            XLUA_ADD_INDEX_PROPERTY(xwidgtes_lua_type, index_type, index);
            XLUA_ADD_CONTAINER_PROPERTY(xwidgtes_lua_type, options_type, _options_labels);
            XLUA_ADD_PROPERTY(xwidgtes_lua_type, std::string, description);
            XLUA_ADD_PROPERTY(xwidgtes_lua_type, bool, disabled);
            XLUA_ADD_PROPERTY(xwidgtes_lua_type, value_type, value);

            XLUA_REGISTER_INDEX_OBSERVER(xwidgtes_lua_type, int, index);
        });
    }
    {
        using xwidgtes_type =  xw::html;
        register_widget_impl<xwidgtes_type>(lua, "xhtml",[](auto && xwidgtes_lua_type){
       
            XLUA_ADD_PROPERTY(xwidgtes_lua_type, std::string, description);
            XLUA_ADD_PROPERTY(xwidgtes_lua_type, std::string, value);
            XLUA_ADD_PROPERTY(xwidgtes_lua_type, std::string, placeholder);

            //XLUA_REGISTER_INDEX_OBSERVER(xwidgtes_lua_type, int, index);
        });
    }
    {
        using xwidgtes_type =  xw::image;
        register_widget_impl<xwidgtes_type>(lua, "ximage",[](auto && xwidgtes_lua_type){

            XLUA_ADD_PROPERTY(xwidgtes_lua_type, std::vector<char>,  value);
            XLUA_ADD_PROPERTY(xwidgtes_lua_type, std::string, format);
            XLUA_ADD_PROPERTY(xwidgtes_lua_type, std::string, width);
            XLUA_ADD_PROPERTY(xwidgtes_lua_type, std::string, height);

        });
    }
    {
        using xwidgtes_type =  xw::numeral<double>;
        register_widget_impl<xwidgtes_type>(lua, "xnumeral",[](auto && xwidgtes_lua_type){

            XLUA_ADD_PROPERTY(xwidgtes_lua_type, double, min);
            XLUA_ADD_PROPERTY(xwidgtes_lua_type, double, max);
            XLUA_ADD_PROPERTY(xwidgtes_lua_type, double, value);
            XLUA_ADD_PROPERTY(xwidgtes_lua_type, double, step);
            XLUA_ADD_PROPERTY(xwidgtes_lua_type, bool, continuous_update);
            XLUA_ADD_PROPERTY(xwidgtes_lua_type, bool, disabled);

            XLUA_REGISTER_OBSERVER(xwidgtes_lua_type, double, value);

        });
    }
    {
        using xwidgtes_type =  xw::password;
        register_widget_impl<xwidgtes_type>(lua, "xpassword",[](auto && xwidgtes_lua_type){
       
            XLUA_ADD_PROPERTY(xwidgtes_lua_type, std::string, description);
            XLUA_ADD_PROPERTY(xwidgtes_lua_type, std::string, value);
            XLUA_ADD_PROPERTY(xwidgtes_lua_type, std::string, placeholder);
            XLUA_ADD_PROPERTY(xwidgtes_lua_type, bool, disabled);
            
            XLUA_REGISTER_OBSERVER(xwidgtes_lua_type, std::string, value);
        });
    }
    {
        using xwidgtes_type =  xw::play;
        register_widget_impl<xwidgtes_type>(lua, "xplay",[](auto && xwidgtes_lua_type){

            XLUA_ADD_PROPERTY(xwidgtes_lua_type, double, min);
            XLUA_ADD_PROPERTY(xwidgtes_lua_type, double, max);
            XLUA_ADD_PROPERTY(xwidgtes_lua_type, double, value);

            XLUA_ADD_PROPERTY(xwidgtes_lua_type, double, interval);

            XLUA_ADD_PROPERTY(xwidgtes_lua_type, bool, disabled);
            XLUA_ADD_PROPERTY(xwidgtes_lua_type, double, step);

            XLUA_ADD_PROPERTY(xwidgtes_lua_type, bool, _playing);
            XLUA_ADD_PROPERTY(xwidgtes_lua_type, bool, _repeat);
            XLUA_ADD_PROPERTY(xwidgtes_lua_type, bool, show_repeat);

            XLUA_REGISTER_OBSERVER(xwidgtes_lua_type, double, value);

        });
    }
    {
        using xwidgtes_type =  xw::progress<double>;
        register_widget_impl<xwidgtes_type>(lua, "xprogress",[](auto && xwidgtes_lua_type){

            XLUA_ADD_PROPERTY(xwidgtes_lua_type, double, min);
            XLUA_ADD_PROPERTY(xwidgtes_lua_type, double, max);
            XLUA_ADD_PROPERTY(xwidgtes_lua_type, double, value);
            XLUA_ADD_PROPERTY(xwidgtes_lua_type, std::string, orientation);
            XLUA_ADD_PROPERTY(xwidgtes_lua_type, std::string, bar_style);

            XLUA_REGISTER_OBSERVER(xwidgtes_lua_type, double, value);

        });
    }
    {
        using xwidgtes_type =  xw::selectionslider;
        register_widget_impl<xwidgtes_type>(lua, "xselectionslider",[](auto && xwidgtes_lua_type){
       
            using options_type = typename xwidgtes_type::options_type;
            using value_type = typename xwidgtes_type::value_type;
            using index_type = typename xwidgtes_type::index_type;

            XLUA_ADD_INDEX_PROPERTY(xwidgtes_lua_type, index_type, index);
            XLUA_ADD_CONTAINER_PROPERTY(xwidgtes_lua_type, options_type, _options_labels);
            XLUA_ADD_PROPERTY(xwidgtes_lua_type, std::string, description);
            XLUA_ADD_PROPERTY(xwidgtes_lua_type, bool, disabled);
            XLUA_ADD_PROPERTY(xwidgtes_lua_type, value_type, value);

            XLUA_ADD_PROPERTY(xwidgtes_lua_type, std::string, orientation);
            XLUA_ADD_PROPERTY(xwidgtes_lua_type, bool, readout);
            XLUA_ADD_PROPERTY(xwidgtes_lua_type, bool, continuous_update);

            XLUA_REGISTER_INDEX_OBSERVER(xwidgtes_lua_type, int, index);
        });
    }
    {
        using xwidgtes_type =  xw::tab;
        register_widget_impl<xwidgtes_type>(lua, "xtab",[](auto && xwidgtes_lua_type){

            using titles_type = typename xwidgtes_type::titles_type;
            XLUA_ADD_CONTAINER_PROPERTY(xwidgtes_lua_type, titles_type, _titles);

            xwidgtes_lua_type["add"] = [](xwidgtes_type & widget, xeus::xguid id){
                widget.add(id);
            };

            XLUA_REGISTER_OPTIONAL_INDEX_OBSERVER(xwidgtes_lua_type, int, selected_index);
        });
    }
    {
        using xwidgtes_type =  xw::text;
        register_widget_impl<xwidgtes_type>(lua, "xtext",[](auto && xwidgtes_lua_type){

            using submit_callback_type = typename xwidgtes_type::submit_callback_type;

            XLUA_ADD_PROPERTY(xwidgtes_lua_type, std::string, description);
            XLUA_ADD_PROPERTY(xwidgtes_lua_type, std::string, value);
            XLUA_ADD_PROPERTY(xwidgtes_lua_type, std::string, placeholder);
            XLUA_ADD_PROPERTY(xwidgtes_lua_type, bool, disabled);
            XLUA_ADD_PROPERTY(xwidgtes_lua_type, bool, continuous_update);

            XLUA_REGISTER_OBSERVER(xwidgtes_lua_type, std::string, value);

            xwidgtes_lua_type["on_submit"] = [](xwidgtes_type & widget, sol::unsafe_function function){
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
        using xwidgtes_type =  xw::textarea;
        register_widget_impl<xwidgtes_type>(lua, "xtextarea",[](auto && xwidgtes_lua_type){

            XLUA_ADD_PROPERTY(xwidgtes_lua_type, std::string, description);
            XLUA_ADD_PROPERTY(xwidgtes_lua_type, std::string, value);
            XLUA_ADD_PROPERTY(xwidgtes_lua_type, std::string, placeholder);
            XLUA_ADD_PROPERTY(xwidgtes_lua_type, bool, disabled);
            XLUA_ADD_PROPERTY(xwidgtes_lua_type, bool, continuous_update);

            XLUA_REGISTER_OBSERVER(xwidgtes_lua_type, std::string, value);

        });
    }

    {
        using xwidgtes_type =  xw::togglebutton;
        register_widget_impl<xwidgtes_type>(lua, "xtogglebutton",[](auto && xwidgtes_lua_type){
            XLUA_ADD_PROPERTY(xwidgtes_lua_type, bool, value);
            XLUA_ADD_PROPERTY(xwidgtes_lua_type, bool, disabled);
            XLUA_ADD_PROPERTY(xwidgtes_lua_type, std::string, tooltip);
            XLUA_ADD_PROPERTY(xwidgtes_lua_type, std::string, icon);
            XLUA_ADD_PROPERTY(xwidgtes_lua_type, std::string, button_style);


            XLUA_REGISTER_OBSERVER(xwidgtes_lua_type, bool, value);

        });
    }
    {
        using xwidgtes_type =  xw::togglebuttons;
        register_widget_impl<xwidgtes_type>(lua, "xtogglebuttons",[](auto && xwidgtes_lua_type){

       
            using options_type = typename xwidgtes_type::options_type;
            using value_type = typename xwidgtes_type::value_type;
            using index_type = typename xwidgtes_type::index_type;

            XLUA_ADD_INDEX_PROPERTY(xwidgtes_lua_type, index_type, index);
            XLUA_ADD_CONTAINER_PROPERTY(xwidgtes_lua_type, options_type, _options_labels);

            XLUA_ADD_PROPERTY(xwidgtes_lua_type, std::string, description);
            XLUA_ADD_PROPERTY(xwidgtes_lua_type, bool, disabled);

            // XLUA_ADD_CONTAINER_PROPERTY(xwidgtes_lua_type, std::vector<std::string>, tooltips);
            // XLUA_ADD_CONTAINER_PROPERTY(xwidgtes_lua_type, std::vector<std::string>, icons);
            // XLUA_ADD_PROPERTY(xwidgtes_lua_type, std::string, button_style);

            XLUA_REGISTER_OBSERVER(xwidgtes_lua_type, std::string, value);

        });
    }
    {
        using xwidgtes_type =  xw::video;
        register_widget_impl<xwidgtes_type>(lua, "xvideo",[](auto && xwidgtes_lua_type){

            XLUA_ADD_PROPERTY(xwidgtes_lua_type, std::vector<char>,  value);
            XLUA_ADD_PROPERTY(xwidgtes_lua_type, std::string, format);
            XLUA_ADD_PROPERTY(xwidgtes_lua_type, std::string, width);
            XLUA_ADD_PROPERTY(xwidgtes_lua_type, std::string, height);
            XLUA_ADD_PROPERTY(xwidgtes_lua_type, bool, autoplay);
            XLUA_ADD_PROPERTY(xwidgtes_lua_type, bool, loop);
            XLUA_ADD_PROPERTY(xwidgtes_lua_type, bool, controls);

        });
    }
    {
        using xwidgtes_type =  xw::valid;
        register_widget_impl<xwidgtes_type>(lua, "xvalid",[](auto && xwidgtes_lua_type){
            XLUA_ADD_PROPERTY(xwidgtes_lua_type, std::string,  readout);
            XLUA_ADD_PROPERTY(xwidgtes_lua_type, bool, value);
            XLUA_REGISTER_OBSERVER(xwidgtes_lua_type, bool, value);
        });
    }
    {
        using xwidgtes_type =  xw::link;
        sol::usertype<xwidgtes_type> xwidgtes_lua_type = lua.new_usertype<xwidgtes_type>(
            "xlink",
            sol::factories([](
                const xeus::xguid id_source,
                std::string n_source,
                const xeus::xguid id_target,
                std::string n_target
            ){
                return xwidgtes_type(
                    xw::make_id_holder(id_source),
                    n_source,
                    xw::make_id_holder(id_target),
                    n_target
                );
            })
        );
        // typical member function that returns a variable
        xwidgtes_lua_type["display"] = &xwidgtes_type::display;
        xwidgtes_lua_type["id"] = &xwidgtes_type::id;
    }
    {
        using xwidgtes_type =  xw::directional_link;
        sol::usertype<xwidgtes_type> xwidgtes_lua_type = lua.new_usertype<xwidgtes_type>(
            "xdirectional_link",
            sol::factories([](
                const xeus::xguid id_source,
                std::string n_source,
                const xeus::xguid id_target,
                std::string n_target
            ){
                return xwidgtes_type(
                    xw::make_id_holder(id_source),
                    n_source,
                    xw::make_id_holder(id_target),
                    n_target
                );
            })
        );
        // typical member function that returns a variable
        xwidgtes_lua_type["display"] = &xwidgtes_type::display;
        xwidgtes_lua_type["id"] = &xwidgtes_type::id;
    }
}

void extend_xwidgets(sol::state & lua)
{
    const std::string extend = R""""(


new_ilua = {}

__atomic_widgets = {
    "xslider",
    "xbutton",
    "xaudio",
    "xcheckbox",
    "xcolor_picker",
    "xcontroller",
    "xdropdown",
    "xhtml",
    "ximage",
    "xlabel",
    "xhtml",
    "xnumeral",
    "xpassword",
    "xplay",
    "xprogress",
    "xradiobuttons",
    "xselectionslider",
    "xtextarea",
    "xtext",
    "xtogglebutton",
    "xtogglebuttons",
    "xvideo",
    "xvalid",
    "xoutput"
}

__link_widgets = {
    "xlink",
    "xdirectional_link"
}
__layout_widgets = {
    "xvbox",
    "xhbox",
    "xtab",
    "xaccordion"
}


function xoutput:captured_call(func)
    self:capture()
    func()
    self:release()
end

    )"""";
    lua.script(extend);  
}


void register_xwidgets(sol::state & lua)
{
    register_widget_related_types(lua);
    register_xwidgets_impl(lua);
    extend_xwidgets(lua);
}

}

#undef XLUA_REGISTER_OBSERVER
#undef XLUA_ADD_PROPERTY
#endif
