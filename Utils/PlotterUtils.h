//
// Created by Aleksei Lukianchuk on 21-Nov-22.
//

#ifndef EFTPROFILER_PLOTTERUTILS_H
#define EFTPROFILER_PLOTTERUTILS_H

#include "../Core/Logger.h"
#include <string>
#include <memory>

class TH1D;

//#include "NpRankingPlotter.h"
#include "../Application/RankingPlotterSettings.h"

namespace eft::utils {

class PlotterUtils {
    using PlotSettings = std::unique_ptr<eft::plot::RankingPlotterSettings>;
public:
    static std::string        FormName(const PlotSettings& settings) noexcept;
    static std::string        GetLabel(const PlotSettings& settings,
                                       size_t idx,
                                       const std::string& nominal_name = "") noexcept;
    static std::shared_ptr<TH1D>    MakeHisto1D(const std::string& name, size_t nb_bins) noexcept;

    using Replacement = std::pair<std::string, std::string>;
    static std::vector<Replacement> ParseReplacements(const std::vector<std::string>& replacements);

    static void               RemovePrefix(std::string& s,    const std::vector<std::string>& prefixes);
    static inline std::string RemovePrefixCopy(std::string s, const std::vector<std::string>& prefixes);
    static void               RemoveSuffix(std::string& s,    const std::vector<std::string>& suffix);
    static inline std::string RemoveSuffixCopy(std::string s, const std::vector<std::string>& suffix);

    static inline void        ReplaceStrings(std::string& s,    const std::vector<std::string>& replacements);
    static inline std::string ReplaceStringsCopy(std::string s, const std::vector<std::string>& replacements);

    static void               ReplaceStrings(std::string& s,    const std::vector<Replacement>& replacements);
    static inline std::string ReplaceStringsCopy(std::string s, const std::vector<Replacement>& replacements);
};

inline void PlotterUtils::ReplaceStrings(std::string& s, const std::vector<std::string>& replacements)
{
    EFT_PROF_TRACE("Replace {} using {} replacements", s, replacements.size());
    ReplaceStrings(s, ParseReplacements(replacements));
}

inline std::string PlotterUtils::ReplaceStringsCopy(std::string s, const std::vector<std::string>& replacements) {
    ReplaceStrings(s, ParseReplacements(replacements));
    return s;
}

inline std::string PlotterUtils::ReplaceStringsCopy(std::string s, const std::vector<PlotterUtils::Replacement>& replacements)
{
    ReplaceStrings(s, replacements);
    return s;
}
inline std::string PlotterUtils::RemovePrefixCopy(std::string s, const std::vector<std::string>& prefixes)
{
    RemovePrefix(s, prefixes);
    return s;
}
inline std::string PlotterUtils::RemoveSuffixCopy(std::string s, const std::vector<std::string>& suffixes)
{
    RemoveSuffix(s, suffixes);
    return s;
}

} // eft::utils

#endif //EFTPROFILER_PLOTTERUTILS_H
