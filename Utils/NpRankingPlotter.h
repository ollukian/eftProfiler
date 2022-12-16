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

#include "StringUtils.h"

#include <vector>
#include <string>
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

    void                            ReadSettingsFromCommandLine(CommandLineArgs* cmdLineArgs);
    void  inline                    ReadSettingsFromCommandLine(const std::shared_ptr<CommandLineArgs>& cmdLineArgs) noexcept;
    void                            SetCallBack(EntriesSelector selector) noexcept  { callback_ = std::move(selector); }
    void                            ReadValues(const std::filesystem::path& path);
    void                            Plot(const std::unique_ptr<RankingPlotterSettings>& settings) noexcept;
    inline const std::vector<NpInfoForPlot>& GetResForPlot() const noexcept { return res_for_plot_; }
    inline const std::vector<NpInfoForPlot>& GetResForPlot()       noexcept { return res_for_plot_; }
private:
    NpRankingStudyRes               ReadValuesOneFile(const std::filesystem::path& path);
    void                            RegisterRes(const NpRankingStudyRes& res) noexcept;
    static NpInfoForPlot            ComputeInfoForPlot(const NpRankingStudyRes& res) noexcept;

    static inline EntriesSelector   CreateLambdaForIgnoringNpNames(const std::vector<std::string>& names_to_ignore) noexcept;
    static inline EntriesSelector   CreateLambdaForMatchingNpNames(const std::vector<std::string>& names_to_match) noexcept;
private:
    EntriesSelector callback_ {[](const NpInfoForPlot&){return true;}};
    std::unordered_map<std::string, NpRankingStudyRes> np_study_res_;
    std::vector<NpInfoForPlot>                         res_for_plot_;

    void ReadNpNamesFromFile(const std::string& path) const;
public:
    std::unique_ptr<RankingPlotterSettings> np_ranking_settings;
};

NpRankingPlotter::EntriesSelector
NpRankingPlotter::CreateLambdaForIgnoringNpNames(const std::vector<std::string>& names_to_ignore) noexcept {
    return [&](const NpInfoForPlot& info) -> bool {
        return std::all_of(names_to_ignore.begin(), names_to_ignore.end(), [&](const auto& name) -> bool
        {
            bool res = (info.name.find(name) == std::string::npos);
            EFT_PROF_DEBUG("callback [{:12}][{:10}] for POI: {:10}, np: {:20} result: {}",
                           "ignore name",
                           name,
                           info.poi,
                           info.name,
                           res);
            return res;
        });
    };
}

NpRankingPlotter::EntriesSelector
NpRankingPlotter::CreateLambdaForMatchingNpNames(const std::vector<std::string>& names_to_match) noexcept {
    return [&](const NpInfoForPlot& info) -> bool {
        return  std::all_of(names_to_match.begin(), names_to_match.end(), [&](const auto& name) -> bool
        {
            bool res = (info.name.find(name) != std::string::npos);
            EFT_PROF_DEBUG("callback [{:12}][{:10}] for POI: {:10}, np: {:20} result: {}",
                           "match name",
                           name,
                           info.poi,
                           info.name,
                           res);
            return res;
        });
    };
}

void NpRankingPlotter::ReadSettingsFromCommandLine(const std::shared_ptr<CommandLineArgs>& cmdLineArgs) noexcept {
    ReadSettingsFromCommandLine(cmdLineArgs.get());
}



}


#endif //EFTPROFILER_NPRANKINGPLOTTER_H
