//
// Created by Aleksei Lukianchuk on 12-Jan-23.
//

#ifndef EFTPROFILER_POICONFIG_H
#define EFTPROFILER_POICONFIG_H

#include <string>
//#include "nlohmann/json.hpp"
#include "nlohmann/json_fwd.hpp"

namespace eft::stats::scans {

struct PoiConfig;

void to_json(nlohmann::json& j, const PoiConfig& config);
void from_json(const nlohmann::json& j, PoiConfig& config);


struct PoiConfig {
    std::string name;
    float val {0.};

    float central_val {0.};
    float central_err {0.};

    std::string version {"v1"};
};



} // eft::stats::scans

#endif //EFTPROFILER_POICONFIG_H
