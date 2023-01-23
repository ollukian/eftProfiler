//
// Created by Aleksei Lukianchuk on 21-Nov-22.
//

#include "PlotterUtils.h"
#include "StringUtils.h"

#include "TH1D.h"

#include "Profiler.h"
#include "Logger.h"

using namespace std;

namespace eft::utils {


string PlotterUtils::FormName(const PlotterUtils::PlotSettings& settings) noexcept {
    EFT_PROFILE_FN();
    string ignore_part;
    string select_part;

    if ( ! settings->ignore_name.empty() )
    {
        string ignore_in_one_string = fmt::format("Ignore_{}_patterns__", settings->ignore_name.size());
        for (const auto& patter : settings->ignore_name)
            ignore_in_one_string += patter + "__";

        ignore_part = "__" + ignore_in_one_string.substr(0, ignore_in_one_string.size() - 2);
    }

    if ( ! settings->match_names.empty() )
    {
        string matches_in_one_string = fmt::format("Select_{}_patterns__", settings->match_names.size());
        for (const auto& patter : settings->match_names)
            matches_in_one_string += patter + "__";

        select_part = "__" + matches_in_one_string.substr(0, matches_in_one_string.size() - 2);
    }

    std::string stem_name;
    if (settings->output.empty())
        stem_name = fmt::format("Impact_{}_{}_nps{}{}",
                                settings->poi,
                                settings->top,
                                select_part,
                                ignore_part);
    else
        stem_name = settings->output;
    return stem_name;
}

shared_ptr<TH1D> PlotterUtils::MakeHisto1D(const string& name, size_t nb_bins) noexcept {
    EFT_PROFILE_FN();
    return std::make_shared<TH1D>(name.c_str(), name.c_str(),
                                  nb_bins,
                                  0,
                                  nb_bins
    );
}

string PlotterUtils::GetLabel(const PlotterUtils::PlotSettings& settings,
                                   size_t idx,
                                   const string& nominal_name) noexcept
{
    EFT_PROFILE_FN();
    string bin_label;
    if (settings->np_names.empty()) {
        bin_label = nominal_name;
        if(!settings->replacements.empty())
            ReplaceStrings(bin_label, settings->replacements);
        if(!settings->remove_prefix.empty())
            RemovePrefix(bin_label, settings->remove_prefix);
        if(!settings->remove_suffix.empty())
            RemoveSuffix(bin_label, settings->remove_suffix);
    }
    else {
        bin_label = settings->np_names.at(idx);
    }
    return bin_label;
}

std::vector<PlotterUtils::Replacement>
PlotterUtils::ParseReplacements(const std::vector<std::string>& replacements)
{
    EFT_PROFILE_FN();
    // convert vector of "key:val" to vector of pairs: <key, val>
    EFT_PROF_TRACE("ParseReplacements for {} objects", replacements.size());
    vector<Replacement> res;
    res.reserve(replacements.size());
    for (const auto& raw : replacements)
    {
        EFT_PROF_DEBUG("Extract replacement from: {}", raw);
        auto pos_separator = raw.find(':');
        if (pos_separator == std::string::npos) {
            throw std::logic_error(fmt::format("Cannot parse replacement string {}, {}",
                                               raw,
                                               ". Must have format: 'key1:val1, key2:val2, ...'"));

        }

        std::string key = raw.substr(0, pos_separator);
        std::string val = raw.substr(pos_separator + 1, raw.length());
        res.emplace_back(std::move(key), std::move(val));
    }

    for (const auto& key_val : res) {
        EFT_PROF_DEBUG("Add replacing: {:10} ==> {:10}", key_val.first, key_val.second);
    }

    return res;
}

void PlotterUtils::ReplaceStrings(std::string& s, const std::vector<Replacement>& replacements)
{
    EFT_PROFILE_FN();
    for (const auto& replacement : replacements) {
        EFT_PROF_DEBUG("Replace {} using {:10} -> {:10} replacement", s, replacement.first, replacement.second);
        StringUtils::Replace(s, replacement.first, replacement.second);
    }
}

void PlotterUtils::RemovePrefix(string& s, const vector<string>& prefixes)
{
    EFT_PROFILE_FN();
    for (const auto& prefix : prefixes) {
        EFT_PROF_DEBUG("Remove prefix: {:10} from {:10}", prefix, s);
        StringUtils::RemovePrefix(s, prefix);
    }
}
void PlotterUtils::RemoveSuffix(string& s, const vector<string>& suffixes)
{
    EFT_PROFILE_FN();
    for (const auto& suffix : suffixes) {
        EFT_PROF_DEBUG("Remove suffix: {:10} from {:10}", suffix, s);
        StringUtils::RemoveSuffix(s, suffix);
    }
}


} // eft::utils