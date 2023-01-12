//
// Created by Aleksei Lukianchuk on 12-Jan-23.
//

#include "PoiConfig.h"

#include "nlohmann/json.hpp"

namespace eft::stats::scans {


void to_json(nlohmann::json& j, const PoiConfig& config) {
    j = nlohmann::json {
            {"config_version",  config.version},
            {"poi_name",        config.name},
            {"poi_val",         config.val},
            {"poi_central_val", config.central_val},
            {"poi_central_err", config.central_err}
    };
}


} // eft::stats::scans