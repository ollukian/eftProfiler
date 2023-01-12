//
// Created by Aleksei Lukianchuk on 12-Jan-23.
//

#include "PoiConfig.h"
#include "Core.h"

#include "nlohmann/json.hpp"

namespace eft::stats::scans {


void to_json(nlohmann::json& j, const PoiConfig& config) {
    j = nlohmann::json {
            {"config_version",      config.version},
            {"poi_name",            config.name},
            {"poi_val",             config.val},
            {"poi_central_val",     config.central_val},
            {"poi_central_err",     config.central_err},
            {"grid_size",           config.grid_size},
            {"range_scan_low",      config.range_scan_low},
            {"range_scan_high",     config.range_scan_high },
            {"range_scan_sigmas",   config.range_scan_sigmas },
    };
}

void from_json(const nlohmann::json& j, PoiConfig& config) {
    j.at("config_version").get_to(config.version);
    if (config.version == "v1") {
        j.at("poi_name").get_to(config.name);
        j.at("poi_val").get_to(config.val);
        j.at("poi_central_val").get_to(config.central_val);
        j.at("poi_central_err").get_to(config.central_err);
        j.at("grid_size").get_to(config.grid_size);
        j.at("range_scan_low").get_to(config.range_scan_low);
        j.at("range_scan_high").get_to(config.range_scan_high);
        j.at("range_scan_sigmas").get_to(config.range_scan_sigmas);
    }
    else {
        throw std::runtime_error(fmt::format("from_json[PoiConfig]: impossible to extract version {} of the file, {}",
                                             config.version,
                                             "only version v1 is supported"));
    }
}


} // eft::stats::scans