//
// Created by Aleksei Lukianchuk on 15-Jan-23.
//

#ifndef EFTPROFILER_NLLSCANPLOTTER_H
#define EFTPROFILER_NLLSCANPLOTTER_H

#include "Core.h"

#include "PoiConfig.h"
#include "NllScanResult.h"

#include <unordered_map>
#include <unordered_set>

#include <filesystem>

namespace eft::stats::scans {


class NllScanPlotter {
    //using PoiConfigs = std::unordered_set<PoiConfig>;
    using Nll1Dresults = std::unordered_set<NllScanResult>;
    using Nll1DresultsPerPOI = std::unordered_map<std::string, Nll1Dresults>;
public:

    [[nodiscard]]
    const Nll1Dresults& GetResults1D() const noexcept { return results1D_; }
    [[nodiscard]]
    const Nll1Dresults& GetResults1DPoi(const std::string& poi) const { return results1D_per_poi_.at(poi); }

    void ReadFiles(std::filesystem::path& path);
    void inline ReadFiles(const std::string& path_str);
    void PlotNll1D(const Nll1Dresults& configs);
    void RegisterRes(NllScanResult nllScanRes);

    void RegisterRes1D(NllScanResult nllScanRes);
    static NllScanResult ReadValuesOneFile(const std::filesystem::path& path);
    static inline NllScanResult ReadValuesOneFile(const std::string& path);
    //static PoiConfig ReadValuesOneFile(const std::filesystem::path& path);
private:
    Nll1Dresults results1D_;
    Nll1DresultsPerPOI results1D_per_poi_;
    //PoiConfigs configs_;
    //std::unordered_map<std::string, PoiConfigs> configs_per_poi_;
    //std::unordered_map<>;
};

NllScanResult NllScanPlotter::ReadValuesOneFile(const std::string& path) {
    std::filesystem::path path1 {path};
    return ReadValuesOneFile(path1);
}

void inline NllScanPlotter::ReadFiles(const std::string& path_str) {
    std::filesystem::path path {path_str};
    ReadFiles(path);
}


} // eft::stats::scans

#endif //EFTPROFILER_NLLSCANPLOTTER_H
