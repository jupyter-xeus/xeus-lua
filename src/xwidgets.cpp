#ifndef XLUA_XWIDGETS_HPP
#define XLUA_XWIDGETS_HPP

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



#include "xwidgets/xoutput.hpp"


namespace sol {
        template <>
        struct is_automagical<xeus::xguid> : std::false_type {};

        template <>
        struct is_container<xeus::xguid> : std::false_type {};

}

namespace xlua
{


#define XLUA_ADD_PROPERTY(CLS_OBJ,PROPERTY_TYPE,PROPERTY_NAME)\
    CLS_OBJ.set(#PROPERTY_NAME, sol::property(\
        [](xwidgtes_type & widget){\
            return PROPERTY_TYPE(widget.PROPERTY_NAME);\
        }, \
        [](xwidgtes_type & widget, const PROPERTY_TYPE val){\
            widget.PROPERTY_NAME = val;\
        })\
    )

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
    // xholder
    {
        using binded_type = xw::xholder;
        std::string name = "xholder";
        sol::usertype<binded_type> xwidgtes_lua_type = lua.new_usertype<binded_type>(name,
            sol::constructors<binded_type()>()
        );
    }

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

}


void register_xwidgets(sol::state & lua)
{
    register_widget_related_types(lua);

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

            xwidgtes_lua_type["register_observer"] = [](xwidgtes_type & widget, sol::unsafe_function function){
                auto callback = [function](const auto& s) {
                    auto res = function.call(double(s.value));
                    if (!res.valid())
                    {
                        auto & interpreter =  xeus::get_interpreter();
                        sol::error err = res;
                        const auto error_str = err.what();
                        interpreter.publish_execution_error(error_str,error_str,std::vector<std::string>(1,error_str));
                    }
                };
                XOBSERVE(widget, value, callback);
            };
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
                widget.set_title(i, title);
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

            xwidgtes_lua_type["register_observer"] = [](xwidgtes_type & widget, sol::unsafe_function function){
                auto callback = [function](const auto& s) {
                    auto res = function.call(bool(s.value));
                    if (!res.valid())
                    {
                        auto & interpreter =  xeus::get_interpreter();
                        sol::error err = res;
                        const auto error_str = err.what();
                        interpreter.publish_execution_error(error_str,error_str,std::vector<std::string>(1,error_str));
                    }
                };
                XOBSERVE(widget, value, callback);
            };

        });
    }
    {
        using xwidgtes_type =  xw::color_picker;
        register_widget_impl<xwidgtes_type>(lua, "xcolor_picker",[](auto && xwidgtes_lua_type){
       

            XLUA_ADD_PROPERTY(xwidgtes_lua_type, std::string, value);
            XLUA_ADD_PROPERTY(xwidgtes_lua_type, bool, disabled);
            XLUA_ADD_PROPERTY(xwidgtes_lua_type, bool, concise);


            xwidgtes_lua_type["register_observer"] = [](xwidgtes_type & widget, sol::unsafe_function function){
                auto callback = [function](const auto& s) {
                    auto res = function.call(std::string(s.value));
                    if (!res.valid())
                    {
                        auto & interpreter =  xeus::get_interpreter();
                        sol::error err = res;
                        const auto error_str = err.what();
                        interpreter.publish_execution_error(error_str,error_str,std::vector<std::string>(1,error_str));
                    }
                };
                XOBSERVE(widget, value, callback);
            };

        });
    }
}

}

#undef XLUA_ADD_PROPERTY
#endif
