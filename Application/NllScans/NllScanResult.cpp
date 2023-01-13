//
// Created by Aleksei Lukianchuk on 12-Jan-23.
//

#include "NllScanResult.h"

#include "nlohmann/json.hpp"

namespace eft::stats::scans {

void to_json(nlohmann::json& j, const NllScanResult& res) {
    j = nlohmann::json {
            {"config_version",      res.version},
            {"nll_val",             res.nll_val},
            {"fit_status",          res.fit_status},
            {"pois",                res.poi_configs},
    };
}

} // eft::stats::scans