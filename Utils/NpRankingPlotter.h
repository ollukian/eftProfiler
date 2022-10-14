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
#include "../Core/CommandLineArgs.h"

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

    void ReadSettingsFromCommandLine(CommandLineArgs* cmdLineArgs);
    void SetCallBack(EntriesSelector selector) noexcept  { callback_ = std::move(selector); }
    void ReadValues(const std::filesystem::path& path);
    void Plot(const std::unique_ptr<RankingPlotterSettings>& settings) noexcept;
private:
    NpRankingStudyRes ReadValuesOneFile(const std::filesystem::path& path);
    void              RegisterRes(const NpRankingStudyRes& res) noexcept;
    static NpInfoForPlot         ComputeInfoForPlot(const NpRankingStudyRes& res) noexcept;
    static std::shared_ptr<TH1D> MakeHisto1D(const std::string& name, size_t nb_bins) noexcept;

    static EntriesSelector CreateLambdaForIgnoringNpNames(const std::vector<std::string>& names_to_ignore) noexcept;
    static EntriesSelector CreateLambdaForMatchingNpNames(const std::vector<std::string>& names_to_ignore) noexcept;
private:
    EntriesSelector callback_ {[](const NpInfoForPlot&){return true;}};
    std::unordered_map<std::string, NpRankingStudyRes> np_study_res_;
    std::vector<NpInfoForPlot>                         res_for_plot_;
public:
    std::unique_ptr<RankingPlotterSettings> np_ranking_settings;
};

NpRankingPlotter::EntriesSelector
NpRankingPlotter::CreateLambdaForIgnoringNpNames(const std::vector<std::string>& names_to_ignore) noexcept {
    return [&](const NpInfoForPlot& info) -> bool {
        std::all_of(names_to_ignore.begin(), names_to_ignore.end(), [&](const auto& name) -> bool
        {
            return (info.name.find(name) == std::string::npos);
        });
    };
}

NpRankingPlotter::EntriesSelector
NpRankingPlotter::CreateLambdaForMatchingNpNames(const std::vector<std::string>& names_to_ignore) noexcept {
    return [&](const NpInfoForPlot& info) -> bool {
        std::all_of(names_to_ignore.begin(), names_to_ignore.end(), [&](const auto& name) -> bool
        {
            return (info.name.find(name) != std::string::npos);
        });
    };
}

}


#endif //EFTPROFILER_NPRANKINGPLOTTER_H
