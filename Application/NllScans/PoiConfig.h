//
// Created by Aleksei Lukianchuk on 12-Jan-23.
//
// @ lukianchuk.aleksei@gmail.com


#ifndef EFTPROFILER_POICONFIG_H
#define EFTPROFILER_POICONFIG_H

#include <string>
#include <filesystem>
//#include "nlohmann/json.hpp"
#include "../../Vendors/nlohmann_json/include/nlohmann/json_fwd.hpp"
//#include "nlohmann/json_fwd.hpp"
#include "Grid.h"
#include "Profiler.h"

namespace eft::stats::scans {

class PoiConfig;
class PoiConfigBuilder;
void to_json(nlohmann::json& j, const PoiConfig& config);
void from_json(const nlohmann::json& j, PoiConfig& config);


class PoiConfig {
    friend class PoiConfigBuilder;
    friend std::ostream& operator << (std::ostream& os, const PoiConfig& config);
public:
    bool operator == (const PoiConfig& other) const noexcept { return name == this->name && val == this->val; }
    bool operator != (const PoiConfig& other) const noexcept { return ! (other == *this); }

    explicit PoiConfig(std::string name_) noexcept : name(std::move(name_)) {}
    PoiConfig() = default;
    static PoiConfig readFromString(const std::string& s);
    static PoiConfig readFromJSON(const std::filesystem::path& path);

    // todo: move build things to a builder class?
    //static void PrintHelp(std::ostream& os);
    inline PoiConfig& ToTestAt(float val_ = 0.) noexcept { val = val_; is_defined = true; return *this; }
    inline PoiConfig& WithName(std::string name_) noexcept { name = std::move(name_); return *this; }
    inline PoiConfig& WithCentralVal(float val_ = 0.) noexcept { central_val = val_; return *this; }
    inline PoiConfig& WithCentralErr(float err_ = 0.) noexcept { central_err = err_; return *this; }
    inline PoiConfig& WithVersionOfConfig(std::string ver = "v1") noexcept { version = std::move(ver); return *this; }
    inline PoiConfig& WithGridSize(size_t sz) noexcept { grid_size = sz; return *this; }
    inline PoiConfig& WithRangeLow(float r) noexcept { range_scan_low = r;   return *this; }
    inline PoiConfig& WithRangeHigh(float r) noexcept { range_scan_high = r;  return *this; }
    inline PoiConfig& WithRangeSigmas(float nb_sigma) noexcept;
    inline PoiConfig& WithRangeSigmasLow(float nb_sigma) noexcept { range_scan_sigmas_low = abs(nb_sigma); is_range_in_sigmas = true; return *this; }
    inline PoiConfig& WithRangeSigmasHigh(float nb_sigma) noexcept { range_scan_sigmas_high = abs(nb_sigma); is_range_in_sigmas = true; return *this; }


    [[nodiscard]] const std::string& Name()                 const noexcept { return name; }
    [[nodiscard]] const std::string& Version()              const noexcept { return version; }
    [[nodiscard]] float              Value()                const noexcept { return val; }
    [[nodiscard]] size_t             GridSize()             const noexcept { return grid_size; }
    [[nodiscard]] bool               IsGridPointKnown()     const noexcept { return is_defined; }
    [[nodiscard]] bool               IsRangeInSigmas()      const noexcept { return is_range_in_sigmas; }
    [[nodiscard]] inline float       ScanRangeLow()         const noexcept;
    [[nodiscard]] inline float       ScanRangeHigh()        const noexcept;
    [[nodiscard]] inline float       CentralValue()         const noexcept { return central_val; };
    [[nodiscard]] inline float       CentralError()         const noexcept { return central_err; };
private:
    inline void ComputeRangeFromSigmasIfNeeded() const noexcept;
private:
    std::string name;
    float val {0.}; // value to test at

    float central_val {0.};
    float central_err {0.};

    size_t grid_size  {1};

    mutable float range_scan_low {0.};
    mutable float range_scan_high {0.};
    float range_scan_sigmas_low {2.};
    float range_scan_sigmas_high {2.};

    std::string version {"v1"};
    bool is_defined {false};                    // if the value has to be identified from the grid
    bool is_range_in_sigmas {false};            // if the range is defined in terms of sigmas
    mutable bool is_range_computed_already {false}; // to cache computations
};

inline void PoiConfig::ComputeRangeFromSigmasIfNeeded() const noexcept {
    EFT_PROFILE_FN();
    if (not is_range_in_sigmas) {
        EFT_PROF_DEBUG("The range is not in sigmas...");
        return;
    }
    EFT_PROF_INFO("central value: {} +- {}", central_val, central_err);
    EFT_PROF_INFO("sigma_h: {}", range_scan_sigmas_high);
    if (range_scan_low == 0.f) {
        EFT_PROF_INFO("Need to recompute range scan low");
        EFT_PROF_INFO("sigma_l: {}", range_scan_sigmas_low);
        range_scan_low = central_val - range_scan_sigmas_low * central_err;
        EFT_PROF_INFO("Set range_scan_low is set to: {}", range_scan_low);
    }
    if (range_scan_high == 0.f) {
        EFT_PROF_INFO("Need to recompute range scan high");
        EFT_PROF_INFO("sigma_h: {}", range_scan_sigmas_high);
        range_scan_high = central_val + range_scan_sigmas_high * central_err;
        EFT_PROF_INFO("Set range_scan_high to: {}", range_scan_high);
    }

    is_range_computed_already = true;
}

[[nodiscard]] inline float PoiConfig::ScanRangeLow() const noexcept {
    if (is_range_computed_already) {
        return range_scan_low;
    }
    ComputeRangeFromSigmasIfNeeded();
    return range_scan_low;
}
[[nodiscard]] inline float PoiConfig::ScanRangeHigh() const noexcept {
    if (is_range_computed_already) {
        return range_scan_high;
    }
    ComputeRangeFromSigmasIfNeeded();
    return range_scan_high;
}

inline PoiConfig& PoiConfig::WithRangeSigmas(float nb_sigma) noexcept {
    WithRangeSigmasLow(nb_sigma);
    WithRangeSigmasHigh(nb_sigma);
    return *this;
}



} // eft::stats::scans


namespace std {
    template<>
    struct hash<eft::stats::scans::PoiConfig> {
        std::size_t operator()(const eft::stats::scans::PoiConfig& poi) const {
            using std::hash;
            using std::string;
            auto h1 = hash<string>()(poi.Name());
            auto h2 = hash<float>()(poi.Value()) << 1;
            return h1 ^ h2;
        }
    };
}

#endif //EFTPROFILER_POICONFIG_H
