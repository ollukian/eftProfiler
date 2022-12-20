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


namespace eft::stats::ranking {

class CorrelationStudyProcessor {
public:
    CorrelationStudyProcessor() = default;
    explicit CorrelationStudyProcessor(CommandLineArgs* cmdLine);

    HesseStudyResult ComputeHesseNps();
    void             PlotCovariances(const HesseStudyResult& res) const;
    void             ExtractCorrelations(HesseStudyResult& res) const;
    void             PrintSuggestedNpsRanking(std::string path, const HesseStudyResult& res) const;
    static void      PrintSuggestedNpsRankingStream(std::ostream& os, const HesseStudyResult& res) ;
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
};

} // eft::stats::ranking

#endif //EFTPROFILER_CORRELATIONSTUDYPROCESSOR_H
