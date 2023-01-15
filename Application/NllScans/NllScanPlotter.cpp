//
// Created by Aleksei Lukianchuk on 15-Jan-23.
//

#include "NllScanPlotter.h"

#include "nlohmann/json.hpp"

#include <fstream>
#include <iostream>

using namespace std;
namespace fs =  std::filesystem;

namespace eft::stats::scans {

void NllScanPlotter::ReadFiles(std::filesystem::path& path) {
    EFT_PROFILE_FN();
    EFT_PROF_INFO("NllScanPlotter: read files from: {}", path.string());


    if(path.empty()) {
        EFT_PROF_CRITICAL("[NpRankingPlotter][ReadValues] no --input is set: directory to read values from");
        return;
    }
    EFT_PROF_INFO("Read result files from {}", path.string());

    for (const auto& entry: fs::directory_iterator{path}) {
        const auto filenameStr = entry.path().filename().string();
        if(entry.is_directory()) {
            EFT_PROF_INFO("{} is a directory, skip it", filenameStr);
        } else if(entry.is_regular_file()) {
            RegisterRes(PoiConfig::readFromJSON(entry));
        }
    }
}

void NllScanPlotter::RegisterRes(NllScanResult nllScanRes) {
    EFT_PROFILE_FN();
    if (nllScanRes.poi_configs.size() == 1) {
        RegisterRes1D(std::move(nllScanRes));
    }
    else {
        EFT_PROF_CRITICAL("NllScanPlotter::RegisterRes for {}D case is not supported yet",
                          nllScanRes.poi_configs.size());
        //throw std::runtime_error("");
    }
}

void NllScanPlotter::RegisterRes1D(NllScanResult nllScanRes) {
    EFT_PROFILE_FN();
    const string& poi = nllScanRes.poi_configs[0].Name();
    EFT_PROF_DEBUG("NllScanPlotter register 1D. Poi: [{}] at [{:4}] with nll: {}",
                   poi,
                   nllScanRes.poi_configs[0].Value(),
                   nllScanRes.nll_val);

    results1D_per_poi_[poi].insert(nllScanRes);
    results1D_.insert(std::move(nllScanRes));
}

void NllScanPlotter::PlotNll1D(const NllScanPlotter::Nll1Dresults& configs) {
    EFT_PROFILE_FN();
    EFT_PROF_INFO("Plot Nll 1D results for {} entries", configs.size());

    vector<double> nll_vals;
    nll_vals.reserve(configs.size());

    vector<double> mu_vals;
    mu_vals.reserve(configs.size());

    for (const auto& config : configs) {
        nll_vals.emplace_back(config.nll_val);
        mu_vals. emplace_back(config.poi_configs.at(0).Value());
    }

    EFT_PROF_DEBUG("{:3} | {:5} | {:10}", "idx", "mu", "nll");
    for (size_t idx {0}; idx < nll_vals.size(); ++idx) {
        EFT_PROF_DEBUG("{:3} | {:5} | {:10}", idx, mu_vals.at(idx), nll_vals.at(idx));
    }

    const auto min_nll = *std::min_element(nll_vals.begin(), nll_vals.end());
    const auto min_poi = *std::min_element(mu_vals.begin(), mu_vals.end());
    const auto max_poi = *std::max_element(mu_vals.begin(), mu_vals.end());

    EFT_PROF_DEBUG("{:3} | {:5} | {:10}", "idx", "mu", "2dnll");
    for (size_t idx {0}; idx < nll_vals.size(); ++idx) {
        EFT_PROF_DEBUG("{:3} | {:5} | {:10}", idx, mu_vals.at(idx), nll_vals.at(idx));
    }
    std::for_each(nll_vals.begin(), nll_vals.end(), [&min_nll](double& val) -> void
        {
            val -= min_nll;
            val *= 2;
        }
    );


}

//PoiConfig NllScanPlotter::ReadValuesOneFile(const std::filesystem::path& path)
//{
//    EFT_PROFILE_FN();
//    const string filename = path.string();
//    const string extension = path.extension().string();
//    if (extension != ".json") {
//        EFT_PROF_WARN("{} NOT [.json]", path.string());
//        return {};
//    }
//
//    ifstream ifs(filename);
//    if ( ! ifs.is_open() ) {
//        throw std::runtime_error("error opening: " + filename);
//    }
//
//    nlohmann::json j;
//    ifs >> j;
//
//    PoiConfig res;
//
//    try {
//        EFT_LOG_DURATION("Reading result from JSON");
//        res = j.get<PoiConfig>();
//    }
//    catch (nlohmann::json::type_error& e) {
//        EFT_PROF_WARN("NpRankingPlotter::ReadValuesOneFile{} error: {}.", path.string(), e.what()
//        );
//    }
//
//    return res;
//}

} // eft::stats::scans