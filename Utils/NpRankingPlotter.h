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
    using EntriesSorter   = std::function<bool(NpInfoForPlot& l, NpInfoForPlot& r)>;

    void                            SetCallBack(EntriesSelector selector) noexcept  { callback_ = std::move(selector); }
    void                            SetSorter(EntriesSorter sorter) noexcept {sorter_ = std::move(sorter);}

    void                            ReadSettingsFromCommandLine(CommandLineArgs* cmdLineArgs);
    void  inline                    ReadSettingsFromCommandLine(const std::shared_ptr<CommandLineArgs>& cmdLineArgs) noexcept;
    void                            ReadValues(const std::filesystem::path& path);
    void                            Plot(const std::unique_ptr<RankingPlotterSettings>& settings) noexcept;
    [[nodiscard]]
    inline const std::vector<NpInfoForPlot>& GetResForPlot() const noexcept { return res_for_plot_; }
    [[nodiscard]]
    inline const std::vector<NpInfoForPlot>& GetResForPlot()       noexcept { return res_for_plot_; }

    [[nodiscard]]
    std::vector<NpInfoForPlot>          GetSelected(const EntriesSelector& selector) const noexcept;
    [[nodiscard]]
    inline std::vector<NpInfoForPlot>   GetSelected() const noexcept;

    inline void                         SortEntries(std::vector<NpInfoForPlot>& entries) const noexcept;
    inline static void                  SortEntries(std::vector<NpInfoForPlot>& entries, const EntriesSorter& sorter);

    [[nodiscard]]
    inline std::vector<NpInfoForPlot>   GetSelectedSorted(const EntriesSelector& selector) const noexcept;
    [[nodiscard]]
    inline std::vector<NpInfoForPlot>   GetSelectedSorted(const EntriesSorter& sorter)     const noexcept;
    [[nodiscard]]
    inline std::vector<NpInfoForPlot>   GetSelectedSorted()                                const noexcept;
    [[nodiscard]]
    inline std::vector<NpInfoForPlot>
            GetSelectedSorted(const EntriesSelector& selector,
                              const EntriesSorter& sorter) const noexcept;

private:
    NpRankingStudyRes               ReadValuesOneFile(const std::filesystem::path& path);
    void                            RegisterRes(const NpRankingStudyRes& res) noexcept;
    static NpInfoForPlot            ComputeInfoForPlot(const NpRankingStudyRes& res) noexcept;

    static inline EntriesSelector   CreateLambdaForIgnoringNpNames(const std::vector<std::string>& names_to_ignore) noexcept;
    static inline EntriesSelector   CreateLambdaForMatchingNpNames(const std::vector<std::string>& names_to_match) noexcept;
private:
    EntriesSelector callback_ {[](const NpInfoForPlot&){return true;}};
    EntriesSorter sorter_ { [&](const NpInfoForPlot&l, const NpInfoForPlot& r) -> bool {
        return ((l.impact_plus_sigma_var * l.impact_plus_sigma_var)
                +
                (l.impact_minus_sigma_var * l.impact_minus_sigma_var))
               >
               ((r.impact_plus_sigma_var * r.impact_plus_sigma_var)
                +
                (r.impact_minus_sigma_var * r.impact_minus_sigma_var));
    }  };
    std::unordered_map<std::string, NpRankingStudyRes> np_study_res_;
    std::vector<NpInfoForPlot>                         res_for_plot_;

    void ReadNpNamesFromFile(const std::string& path) const;
public:
    std::unique_ptr<RankingPlotterSettings> np_ranking_settings;
};

NpRankingPlotter::EntriesSelector
NpRankingPlotter::CreateLambdaForIgnoringNpNames(const std::vector<std::string>& names_to_ignore) noexcept {
    EFT_PROFILE_FN();
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
    EFT_PROFILE_FN();
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
//std::vector<NpInfoForPlot> NpRankingPlotter::GetSelectedSorted(const EntriesSelector& selector) const noexcept {
//    EFT_PROFILE_FN();
//    auto res = GetSelected(selector);
//    SortEntries(res);
//    return res;
//}
std::vector<stats::NpInfoForPlot> NpRankingPlotter::GetSelectedSorted() const noexcept {
    return GetSelectedSorted(callback_, sorter_);
}
std::vector<NpInfoForPlot> NpRankingPlotter::GetSelectedSorted(const EntriesSelector& selector) const noexcept {
    return GetSelectedSorted(selector, sorter_);
}
std::vector<NpInfoForPlot> NpRankingPlotter::GetSelectedSorted(const EntriesSorter& sorter) const noexcept {
    return GetSelectedSorted(callback_, sorter);
}

inline std::vector<NpInfoForPlot>
NpRankingPlotter::GetSelectedSorted(const EntriesSelector& selector,
                  const EntriesSorter& sorter) const noexcept
{
    EFT_PROFILE_FN();
    auto res = GetSelected(selector);
    SortEntries(res, sorter);
    return res;
}

void NpRankingPlotter::SortEntries(std::vector<NpInfoForPlot>& entries) const noexcept
{
    EFT_PROFILE_FN();
//    EFT_PROF_TRACE("Sort {} entries", entries.size());
//    std::sort(entries.begin(), entries.end(),
//              [ & ](const NpInfoForPlot& l, const NpInfoForPlot& r) {
//                  return ((l.impact_plus_sigma_var * l.impact_plus_sigma_var)
//                          +
//                          (l.impact_minus_sigma_var * l.impact_minus_sigma_var))
//                         >
//                         ((r.impact_plus_sigma_var * r.impact_plus_sigma_var)
//                          +
//                          (r.impact_minus_sigma_var * r.impact_minus_sigma_var));
//
//              }
//    );
    SortEntries(entries, sorter_);
}

std::vector<NpInfoForPlot> NpRankingPlotter::GetSelected() const noexcept {
    return GetSelected(callback_);
}

void
NpRankingPlotter::SortEntries(std::vector<NpInfoForPlot>& entries,
                              const NpRankingPlotter::EntriesSorter& sorter)
{
    EFT_PROFILE_FN();
    std::sort(entries.begin(), entries.end(), sorter);
}


}


#endif //EFTPROFILER_NPRANKINGPLOTTER_H
