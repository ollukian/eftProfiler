//
// Created by Aleksei Lukianchuk on 20-Dec-22.
//

#ifndef EFTPROFILER_CORRELATIONSTUDYPROCESSOR_H
#define EFTPROFILER_CORRELATIONSTUDYPROCESSOR_H

#include "HesseStudyResult.h"
#include "../Application/NpRankingStudyRes.h"

class RooAbsData;
class RooAbsPdf;
class RooArgList;
class RooRealVar;
class RooArgSet;
class CommandLineArgs;

struct CorrelationStudyPlotSettings;


namespace eft::stats::ranking {

struct SelectorSettings;

class CorrelationStudyProcessor {
public:

    using NP = std::string;
    using Correlation = double;
    using NpCorrelations = std::vector<std::pair<NP, Correlation>>;

    CorrelationStudyProcessor() = default;
    explicit CorrelationStudyProcessor(CommandLineArgs* cmdLine);

    static NpCorrelations GetSortedCorrelationsFromFile(const std::string& path, const std::string& version = "v1");
    static NpCorrelations GetSortedCorrelationsFromFileV1(const std::string& path) ;

    static NpCorrelations FromVecNpInfo(const std::vector<NpInfoForPlot>& infos, const std::string& field = "+sigma");
    //static std::vector<NpInfoForPlot>  ToVecNpInfo(const NpCorrelations& infos,  const std::string& field = "+sigma");
    template<typename T>
    static int GetIdx(const std::vector<T>& elems, const T& elem) noexcept;

    static NpCorrelations
    ApplySelector(const NpCorrelations& corrs, const SelectorSettings& selector) noexcept;

    static void DrawCorrsComparison(const std::shared_ptr<CorrelationStudyPlotSettings>& settings);

    HesseStudyResult ComputeHesseNps();
    void             PlotCovariances(const HesseStudyResult& res) const;
    void             ExtractCorrelations(HesseStudyResult& res) const;

    void             PrintSuggestedNpsRanking(std::string path, const HesseStudyResult& res) const;
    static void      PrintSuggestedNpsRankingStream(std::ostream& os, const HesseStudyResult& res);
public:
    inline CorrelationStudyProcessor& SetPdf(RooAbsPdf* pdf) noexcept;
    inline CorrelationStudyProcessor& SetData(RooAbsData* data) noexcept;
    inline CorrelationStudyProcessor& SetNPs(RooArgSet* nps) noexcept;
    inline CorrelationStudyProcessor& SetGlobs(RooArgSet* globs) noexcept;
    inline CorrelationStudyProcessor& SetPOIs(RooArgSet* poi) noexcept;
    inline CorrelationStudyProcessor& SetPOI(RooRealVar* poi) noexcept;
    inline CorrelationStudyProcessor& SetPOIname(const std::string& poi) noexcept;
    inline CorrelationStudyProcessor& SetWS(IWorkspaceWrapper* ws) noexcept;
private:
    [[nodiscard]] bool VerifyConsistency() const; // checks that all params are set and the code can be run
private:
    RooAbsPdf*  pdf_        = nullptr;
    RooAbsData* data_       = nullptr;
    RooArgSet*  nps_        = nullptr;
    RooArgSet*  globs_      = nullptr;
    RooArgSet*  pois_        = nullptr;
    RooRealVar* poi_        = nullptr;
    IWorkspaceWrapper* ws_  = nullptr;
    std::string poi_name_;
    eft::stats::NpRankingStudySettings settings_;
};

struct SelectorSettings
{
    std::function<bool(NpInfoForPlot)> selector;
    std::string poi;
};

inline CorrelationStudyProcessor& CorrelationStudyProcessor::SetPdf(RooAbsPdf* pdf) noexcept {
    pdf_ = pdf;
    return *this;
};
inline CorrelationStudyProcessor& CorrelationStudyProcessor::SetData(RooAbsData* data) noexcept {
    data_ = data;
    return *this;
};
inline CorrelationStudyProcessor& CorrelationStudyProcessor::SetNPs(RooArgSet* nps) noexcept {
    nps_ = nps;
    return *this;
};
inline CorrelationStudyProcessor& CorrelationStudyProcessor::SetGlobs(RooArgSet* globs) noexcept {
    globs_ = globs;
    return *this;
};
inline CorrelationStudyProcessor& CorrelationStudyProcessor::SetPOIs(RooArgSet* pois) noexcept {
    pois_ = pois;
    return *this;
};
inline CorrelationStudyProcessor& CorrelationStudyProcessor::SetPOI(RooRealVar* poi) noexcept {
    poi_ = poi;
    return *this;
};
inline CorrelationStudyProcessor& CorrelationStudyProcessor::SetPOIname(const std::string& poi) noexcept {
    poi_name_ = poi;
    return *this;
};
inline CorrelationStudyProcessor& CorrelationStudyProcessor::SetWS(IWorkspaceWrapper* ws) noexcept {
    ws_ = ws;
    return *this;
}

template<typename T>
int CorrelationStudyProcessor::GetIdx(const std::vector<T>& elems, const T& elem) noexcept
{
    EFT_PROFILE_FN();
    if (elems.empty())
        return -1;

    int nb_elem = elems.size();

    for (int idx {0}; idx < nb_elem; ++idx) {
        if (elems.at(idx) == elem)
            return idx;
    }
    return -1;
}

} // eft::stats::ranking

#endif //EFTPROFILER_CORRELATIONSTUDYPROCESSOR_H
