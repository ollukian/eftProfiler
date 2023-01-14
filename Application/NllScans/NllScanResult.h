//
// Created by Aleksei Lukianchuk on 12-Jan-23.
//

#ifndef EFTPROFILER_NLLSCANRESULT_H
#define EFTPROFILER_NLLSCANRESULT_H

#include <string>
#include <string>
#include "nlohmann/json_fwd.hpp"

#include "PoiConfig.h"

namespace eft::stats::scans {

struct NllScanResult;
void to_json(nlohmann::json& j, const NllScanResult& res);
void from_json(const nlohmann::json& j, NllScanResult& res);


struct NllScanResult {
    std::string             version         {"v1"};
    std::vector<PoiConfig>  poi_configs;
    double                  nll_val         {0.};
    size_t                  fit_status      {0};
};


} // eft::stats::scans

#endif //EFTPROFILER_NLLSCANRESULT_H
