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

    size_t grid_size  {1};

    float range_scan_low {0.};
    float range_scan_high {0.};
    float range_scan_sigmas {2.};

    std::string version {"v1"};

    explicit PoiConfig(std::string name_) noexcept : name(std::move(name_)) {}
    inline PoiConfig& ToTestAt(float val_ = 0.) noexcept { val = val_; return *this; }
    inline PoiConfig& WithCentralVal(float val_ = 0.) noexcept { central_val = val_; return *this; }
    inline PoiConfig& WithCentralErr(float err_ = 0.) noexcept { central_err = err_; return *this; }
    inline PoiConfig& WithVersionOfConfig(std::string ver = "v1") noexcept { version = std::move(ver); return *this; }
    inline PoiConfig& WithGridSize(size_t sz) noexcept { grid_size = sz; return *this; }
    inline PoiConfig& WithRangeLow(float r) noexcept { range_scan_low = r; return *this; }
    inline PoiConfig& WithRangeHigh(float r) noexcept { range_scan_high = r; return *this; }
    inline PoiConfig& WithRangeSigmas(float nb_sigma) noexcept { range_scan_sigmas = nb_sigma; return *this; }
};



} // eft::stats::scans

#endif //EFTPROFILER_POICONFIG_H
