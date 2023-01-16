//
// Created by Aleksei Lukianchuk on 12-Jan-23.
//
// @ lukianchuk.aleksei@gmail.com


#include "NllScanResult.h"
#include "Core.h"

#include "nlohmann/json.hpp"

namespace eft::stats::scans {

using namespace std;

void to_json(nlohmann::json& j, const NllScanResult& res) {
    EFT_PROFILE_FN();
    if        (res.version == "v1") {
        to_json_v1(j, res);
    } else if (res.version == "v2") {
        to_json_v2(j, res);
    } else {
        throw std::runtime_error(fmt::format("to_json[NllScanResult] only version v1/2 is implemented"));
    }
}

void from_json(const nlohmann::json& j, NllScanResult& res) {
    EFT_PROFILE_FN();
    string version;
    try {
        j.at("config_version").get_to(version);
        if        (version == "v1") {
            from_json_v1(j, res);
        } else if (version == "v2") {
            from_json_v2(j, res);
        } else {
            throw std::runtime_error(fmt::format("from_config[NllScanResult] only version v1/2 is implemented"));
        }
    } catch (nlohmann::json::exception& e) {
        EFT_PROF_CRITICAL("error parsing json: [{}]", e.what());
    } catch (std::exception& e) {
        EFT_PROF_CRITICAL("std::exception during parsing json: [{}]", e.what());
    } catch (...) {
        EFT_PROF_CRITICAL("Unknown exception during json parsing...");
    }
}

void from_json_v1(const nlohmann::json& j, NllScanResult& res) {
    EFT_PROFILE_FN();
    res.version = "v1";
    j.at("nll_val").get_to(res.nll_val);
    j.at("fit_status").get_to(res.fit_status);
    j.at("pois").get_to(res.poi_configs);
}
void to_json_v1(nlohmann::json& j, const NllScanResult& res) {
    EFT_PROFILE_FN();
    j = nlohmann::json {
            {"config_version",     "v1"},
            {"nll_val",             res.nll_val},
            {"fit_status",          res.fit_status},
            {"pois",                res.poi_configs},
    };
}

void to_json_v2(nlohmann::json& j, const NllScanResult& res) {
    EFT_PROFILE_FN();
    j = nlohmann::json{
            {"config_version", "v2"},
            {"nll_val",        res.nll_val},
            {"fit_status",     res.fit_status},
            {"pois",           res.poi_configs},
            {"statType",       res.statType},
            {"prePostFit",     res.prePostFit},
            {"studyType",      res.studyType}
    };
}
void from_json_v2(const nlohmann::json& j, NllScanResult& res) {
    EFT_PROFILE_FN();
    res.version = "v2";
    j.at("nll_val").get_to(res.nll_val);
    j.at("fit_status").get_to(res.fit_status);
    j.at("pois").get_to(res.poi_configs);
    j.at("statType").get_to(res.statType);
    j.at("prePostFit").get_to(res.prePostFit);
    j.at("studyType").get_to(res.studyType);
}

//    struct NllScanResult {
//        std::string             version     {"v2"};
//        std::vector<PoiConfig>  poi_configs;
//        double                  nll_val     {0.};
//        int                     fit_status  {0};
//        StatType                statType    {StatType::FULL};
//        PrePostFit              prePostFit  {PrePostFit::OBSERVED};
//        StudyType               studyType   {StudyType::OBSERVED};
//    };

} // eft::stats::scans