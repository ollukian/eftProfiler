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
    void                            SetCallBack(EntriesSelector selector) noexcept  { callback_ = std::move(selector); }
    void                            ReadValues(const std::filesystem::path& path);
    void                            Plot(const std::unique_ptr<RankingPlotterSettings>& settings) noexcept;
private:
    NpRankingStudyRes               ReadValuesOneFile(const std::filesystem::path& path);
    void                            RegisterRes(const NpRankingStudyRes& res) noexcept;
    static NpInfoForPlot            ComputeInfoForPlot(const NpRankingStudyRes& res) noexcept;
    static std::shared_ptr<TH1D>    MakeHisto1D(const std::string& name, size_t nb_bins) noexcept;

    static inline EntriesSelector   CreateLambdaForIgnoringNpNames(const std::vector<std::string>& names_to_ignore) noexcept;
    static inline EntriesSelector   CreateLambdaForMatchingNpNames(const std::vector<std::string>& names_to_match) noexcept;
private:
    EntriesSelector callback_ {[](const NpInfoForPlot&){return true;}};
    std::unordered_map<std::string, NpRankingStudyRes> np_study_res_;
    std::vector<NpInfoForPlot>                         res_for_plot_;

    using Replacement = std::pair<std::string, std::string>;

    static void        RemovePrefix(std::string& s,    const std::vector<std::string>& prefixes);
    static std::string RemovePrefixCopy(std::string s, const std::vector<std::string>& prefixes);

    static inline void        ReplaceStrings(std::string& s,    const std::vector<std::string>& replacements);
    static inline std::string ReplaceStringsCopy(std::string s, const std::vector<std::string>& replacements);

    static void               ReplaceStrings(std::string& s,    const std::vector<Replacement>& replacements);
    static inline std::string ReplaceStringsCopy(std::string s, const std::vector<Replacement>& replacements);

    static std::vector<Replacement> ParseReplacements(const std::vector<std::string>& replacements);
public:
    std::unique_ptr<RankingPlotterSettings> np_ranking_settings;
};

NpRankingPlotter::EntriesSelector
NpRankingPlotter::CreateLambdaForIgnoringNpNames(const std::vector<std::string>& names_to_ignore) noexcept {
    return [&](const NpInfoForPlot& info) -> bool {
//        bool res =  std::all_of(names_to_ignore.begin(), names_to_ignore.end(), [&](const auto& name) -> bool
//        {
//            return (info.name.find(name) == std::string::npos);
//        });
//        EFT_PROF_DEBUG("callback [name ignore] for POI: {:10}, np: {:20} result: {}", info.poi, info.name, res);
        return std::all_of(names_to_ignore.begin(), names_to_ignore.end(), [&](const auto& name) -> bool
        {
            return (info.name.find(name) == std::string::npos);
        });
    };
}

NpRankingPlotter::EntriesSelector
NpRankingPlotter::CreateLambdaForMatchingNpNames(const std::vector<std::string>& names_to_match) noexcept {
    return [&](const NpInfoForPlot& info) -> bool {
        return  std::all_of(names_to_match.begin(), names_to_match.end(), [&](const auto& name) -> bool
        {
            return (info.name.find(name) != std::string::npos);
        });
//        bool res = std::all_of(names_to_match.begin(), names_to_match.end(), [&](const auto& name) -> bool
//        {
//            return (info.name.find(name) != std::string::npos);
//        });
//        EFT_PROF_DEBUG("callback [name match] for POI: {:10}, np: {:20} result: {}", info.poi, info.name, res);
//        return res;
    };
}

inline void NpRankingPlotter::ReplaceStrings(std::string& s, const std::vector<std::string>& replacements)
{
    EFT_PROF_TRACE("Replace {} using {} replacements", s, replacements.size());
    ReplaceStrings(s, ParseReplacements(replacements));
}

inline std::string NpRankingPlotter::ReplaceStringsCopy(std::string s, const std::vector<std::string>& replacements) {
    ReplaceStrings(s, ParseReplacements(replacements));
    return s;
}

inline std::string NpRankingPlotter::ReplaceStringsCopy(std::string s, const std::vector<NpRankingPlotter::Replacement>& replacements)
{
    ReplaceStrings(s, replacements);
    return s;
}

}


#endif //EFTPROFILER_NPRANKINGPLOTTER_H
