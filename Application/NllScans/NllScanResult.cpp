//
// Created by Aleksei Lukianchuk on 12-Jan-23.
//

#include "NllScanResult.h"
#include "Core.h"

#include "nlohmann/json.hpp"

namespace eft::stats::scans {

using namespace std;

void to_json(nlohmann::json& j, const NllScanResult& res) {
    EFT_PROFILE_FN();
    j = nlohmann::json {
            {"config_version",      res.version},
            {"nll_val",             res.nll_val},
            {"fit_status",          res.fit_status},
            {"pois",                res.poi_configs},
    };
}

void from_json(const nlohmann::json& j, NllScanResult& res) {
    EFT_PROFILE_FN();
    string version;
    j.at("config_version").get_to(version);

    if (version == "v1") {
        res.version = std::move(version);
        j.at("nll_val").    get_to(res.nll_val);
        j.at("fit_status"). get_to(res.fit_status);
        j.at("pois").       get_to(res.poi_configs);
    }
    else {
        throw std::runtime_error(fmt::format("from_config[NllScanResult] only version v1 is implemented"));
    }
}

} // eft::stats::scans