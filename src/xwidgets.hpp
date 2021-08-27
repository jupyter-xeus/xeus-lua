#ifndef XLUA_XWIDGETS_HPP
#define XLUA_XWIDGETS_HPP

#include "xeus-lua/sol/sol.hpp"
#include "xeus/xinterpreter.hpp"

#include "xproperty/xproperty.hpp"

#include "xwidgets/xslider.hpp"
#include "xwidgets/xbutton.hpp"

#include "xwidgets/xoutput.hpp"


namespace xlua
{


#define XLUA_ADD_PROPERTY(CLS_OBJ,PROPERTY_NAME, PROPERTY_TYPE)\
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

    extend(xwidgtes_lua_type);

}


void register_xwidgets(sol::state & lua)
{
    {
        using xwidgtes_type =  xw::button;
        register_widget_impl<xwidgtes_type>(lua, "xbutton",[](auto && xwidgtes_lua_type){

            XLUA_ADD_PROPERTY(xwidgtes_lua_type, description, std::string);
            XLUA_ADD_PROPERTY(xwidgtes_lua_type, tooltip, std::string);
            XLUA_ADD_PROPERTY(xwidgtes_lua_type, disabled, bool);
            XLUA_ADD_PROPERTY(xwidgtes_lua_type, icon, std::string);
            XLUA_ADD_PROPERTY(xwidgtes_lua_type, button_style, std::string);
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

            XLUA_ADD_PROPERTY(xwidgtes_lua_type, min, double);
            XLUA_ADD_PROPERTY(xwidgtes_lua_type, max, double);
            XLUA_ADD_PROPERTY(xwidgtes_lua_type, value, double);
            XLUA_ADD_PROPERTY(xwidgtes_lua_type, step, double);
            XLUA_ADD_PROPERTY(xwidgtes_lua_type, orientation, std::string);
            XLUA_ADD_PROPERTY(xwidgtes_lua_type, readout, bool);
            XLUA_ADD_PROPERTY(xwidgtes_lua_type, readout_format, std::string);
            XLUA_ADD_PROPERTY(xwidgtes_lua_type, continuous_update, bool);
            XLUA_ADD_PROPERTY(xwidgtes_lua_type, disabled, bool);

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
        using xwidgtes_type =  xw::button;
        register_widget_impl<xwidgtes_type>(lua, "xbutton",[](auto && xwidgtes_lua_type){

            XLUA_ADD_PROPERTY(xwidgtes_lua_type, description, std::string);
            XLUA_ADD_PROPERTY(xwidgtes_lua_type, tooltip, std::string);
            XLUA_ADD_PROPERTY(xwidgtes_lua_type, disabled, bool);
            XLUA_ADD_PROPERTY(xwidgtes_lua_type, icon, std::string);
            XLUA_ADD_PROPERTY(xwidgtes_lua_type, button_style, std::string);
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
        using xwidgtes_type = xw::output;
        const std::string widget_name = "xoutput";
        using cast_type = std::string;
        
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
}

}

#undef XLUA_ADD_PROPERTY
#endif
