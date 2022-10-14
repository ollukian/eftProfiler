//
// Created by Aleksei Lukianchuk on 15-Sep-22.
//

#ifdef _MSVC
#pragma once
#endif

#ifndef EFTPROFILER_NPRANKINGPLOTTER_H
#define EFTPROFILER_NPRANKINGPLOTTER_H

#include "../Application/NpRankingStudyRes.h"
#include "../Application/RankingPlotterSettings.h"

#include <vector>
#include <unordered_set>
#include <unordered_map>

//#include <nlohmann/json.hpp>

#include <filesystem>
#include <functional>

// fwd declaration
class TH1D;

namespace eft::plot {

using namespace eft::stats;

class NpRankingPlotter
{
public:
    using EntriesSelector = std::function<bool(NpInfoForPlot)>;

    void SetCallBack(EntriesSelector selector) noexcept  { callback_ = std::move(selector); }
    void ReadValues(const std::filesystem::path& path);
    void Plot(const std::shared_ptr<RankingPlotterSettings>& settings) noexcept;
private:
    NpRankingStudyRes ReadValuesOneFile(const std::filesystem::path& path);
    void RegisterRes(const NpRankingStudyRes& res) noexcept;
    static NpInfoForPlot ComputeInfoForPlot(const NpRankingStudyRes& res) noexcept;
    static std::shared_ptr<TH1D> MakeHisto1D(const std::string& name, size_t nb_bins) noexcept;
private:
    EntriesSelector callback_ {[](const NpInfoForPlot&){return true;}};
    std::unordered_map<std::string, NpRankingStudyRes> np_study_res_;
    std::vector<NpInfoForPlot>                         res_for_plot_;
};



}


#endif //EFTPROFILER_NPRANKINGPLOTTER_H
