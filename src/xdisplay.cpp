
#include <vector>
#include <string>

#include "sol/sol.hpp"
#include "xeus-lua/xinterpreter.hpp"
#include "nlohmann/json.hpp"
namespace nl = nlohmann;



namespace xlua
{




void setup_display(
  sol::state_view & lua,
  interpreter & interp
)
{
    // get display table
    sol::table ilua_table = lua["ilua"];
    sol::table display_table = ilua_table["display"];
    sol::table detail_table = display_table["detail"];

    auto self = &interp;

    detail_table.set_function("_display_data", [self](
        const std::string & data_str,
        const std::string & metadata_str,
        const std::string & transient_str
    ){
        try
        {
            const auto data = nl::json::parse(data_str);
            const auto metadata = nl::json::parse(metadata_str);
            const auto transient = nl::json::parse(transient_str);
            self->display_data(data, metadata, transient);
        }
        catch (nl::json::parse_error& ex)
        {
            self->publish_execution_error("json::parse_error",ex.what(),std::vector<std::string>());
        }
    });

    display_table.set_function("clear_output", [self](
        const bool wait
    ){
        
            self->clear_output(wait);
    });

    detail_table.set_function("_update_display_data", [self](
        const std::string & data_str,
        const std::string & metadata_str,
        const std::string & transient_str
    ){
        try
        {
            const auto data = nl::json::parse(data_str);
            const auto metadata = nl::json::parse(metadata_str);
            const auto transient = nl::json::parse(transient_str);
            self->update_display_data(data, metadata, transient);
        }
        catch (nl::json::parse_error& ex)
        {
            self->publish_execution_error("json::parse_error",ex.what(),std::vector<std::string>());
        }
    });
}


}
