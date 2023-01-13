//
// Created by Aleksei Lukianchuk on 12-Jan-23.
//

#include "PoiConfig.h"
#include "Core.h"

#include "StringUtils.h"

#include <set>
#include <string>

#include "nlohmann/json.hpp"

namespace eft::stats::scans {

using namespace std;

void to_json(nlohmann::json& j, const PoiConfig& config) {
    j = nlohmann::json {
            {"config_version",      config.Version()},
            {"poi_name",            config.Name()},
            //{"poi_val",             config.},
            //{"poi_central_val",     config.central_val},
            //{"poi_central_err",     config.central_err},
            //{"grid_size",           config.},
            {"range_scan_low",      config.ScanRangeLow()},
            {"range_scan_high",     config.ScanRangeHigh() },
            //{"range_scan_sigmas",   config.range_scan_sigmas },
    };
}

void from_json(const nlohmann::json& j, PoiConfig& config) {
    throw std::runtime_error("from_json[POIConfig] is not implemented");
//    j.at("config_version").get_to(config.version);
//    if (config.Version() == "v1") {
//        j.at("poi_name").get_to(config.name);
//        j.at("poi_val").get_to(config.val);
//        j.at("poi_central_val").get_to(config.central_val);
//        j.at("poi_central_err").get_to(config.central_err);
//        j.at("grid_size").get_to(config.grid_size);
//        j.at("range_scan_low").get_to(config.range_scan_low);
//        j.at("range_scan_high").get_to(config.range_scan_high);
//        j.at("range_scan_sigmas").get_to(config.range_scan_sigmas);
//    }
//    else {
//        throw std::runtime_error(fmt::format("from_json[PoiConfig]: impossible to extract version {} of the file, {}",
//                                             config.version,
//                                             "only version v1 is supported"));
//    }
}

PoiConfig PoiConfig::readFromString(const std::string& s) {
    EFT_PROFILE_FN();

    set<string> keys = {
        "val",
        "grid",
        "range",
        "at"
    };

    auto name_params = eft::StringUtils::Split(s, '(');
    const auto& name = name_params[0];
    auto params = name_params[1];

    auto tokens = eft::StringUtils::Split(params, ':');
    EFT_PROF_DEBUG("obtained {} tokens from: [{}]", tokens.size(), s);

    for (const auto& token : tokens) {
        stringstream ss {token};
        string token_name;
        string token_vals;
        ss >> token_name;

        auto vals = ss.str();
        EFT_PROF_DEBUG("token: [{}] has name: [{}] with vals: [{}]", token, token_name, vals);
    }

    return {};

    // reads POI configuration from a string
    // Configuration of the string:
    //      POI_NAME(setting_name value : setting_name2 value2 : ...)
    // * settings are to be separated by the ":" symbol
    /////////////////////////////////
    /// names of the settings ///////
    /////////////////////////////////
    // ceHRe33(
    //      val [value, error]
    //          value               => central value of the POI
    //          error               => error on the central value of the POI
    //      grid [nb {equidistant, hermite}]
    //          nb                  => number of points in the grid
    //          equidistant         => points are computed using euclidian equdistant measure
    //          hermite             => compute points from zeros of Hermite polynomials
    //      range [low, high]
    //          range [2]           => low = high == 2
    //          range [-1 2]        => low = -1, high = 2
    //          range [2s]          => low = high = 2 * sigma (sigma - error on the central value)
    //          range [1s 2s]       => low = 1 * sigma, high = 2 * sigma
    //      at [value]              ## to probe at a given value instead of computing the grid position ##
    //          value               => exact position at which to probe this POI, without using the grid
}


} // eft::stats::scans