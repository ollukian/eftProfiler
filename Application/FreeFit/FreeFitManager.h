//
// Created by Aleksei Lukianchuk on 23-Jan-23.
//

#ifndef EFTPROFILER_FREEFITMANAGER_H
#define EFTPROFILER_FREEFITMANAGER_H


class IWorkspaceWrapper;
class RooArgSet;
class RooAbsPdf;
class RooAbsData;
class CommandLineArgs;

#include "Core.h"
#include "../Fitter/FitSettings.h"
#include "../Fitter/ErrorsReEvaluation.h"
#include "../Application/NpRankingStudyRes.h"

#include <string>
#include <memory>

namespace eft::stats::freefit {
    using eft::stats::fit::Errors;
class FreeFitManager {
public:
    static FreeFitManager InitFromCommandLine(const std::shared_ptr<CommandLineArgs>& cmdLineArgs);
    void RunFit();
    //void SaveRes(const std::string& path) const;

    inline  FreeFitManager& SetPOIsToFloat(RooArgSet* pois)  noexcept { pois_to_float = pois; return *this; }
    inline  FreeFitManager& SetPOIsFullList(RooArgSet* pois) noexcept { all_pois = pois; return *this; }
    //inline  FreeFitManager& SetPOIsToEstimateErrorsFor(RooArgSet* pois) noexcept
    //    { fitSettings_.pois_to_estimate_errors = pois; return *this; }
            FreeFitManager& SetPOIsToFloat(const std::vector<std::string>& list);
            //FreeFitManager& SetPOIsFullList(const std::vector<std::string>& list);
    inline  FreeFitManager& SetWS(IWorkspaceWrapper* ws)     noexcept { ws_ = ws; return *this; }
    inline  FreeFitManager& SetGlobs(RooArgSet* globs)       noexcept { fitSettings_.globalObs = globs; return *this; }
    inline  FreeFitManager& SetNPs(RooArgSet* nps)           noexcept { fitSettings_.nps = nps; return *this; }
    inline  FreeFitManager& SetData(RooAbsData* data)        noexcept { fitSettings_.data = data; return *this; }
    inline  FreeFitManager& SetPDF(RooAbsPdf* pdf)           noexcept { fitSettings_.pdf = pdf; return *this; }
    inline  FreeFitManager& SetErorsEvaluation(Errors errors) noexcept { errors_type = errors; return *this; }
private:
    fit::FitSettings        fitSettings_;
    IWorkspaceWrapper* ws_  {nullptr};
    RooArgSet*              pois_to_float {nullptr};
    RooArgSet*              all_pois {nullptr};
    std::string             snapshot_;
    std::string             data_name;
    bool                    force_data {false};
    bool                    stat_only  {false};
    PrePostFit              prePostFit {PrePostFit::OBSERVED};


    Errors                  errors_type {Errors::DEFAULT};
};

} // freefit

#endif //EFTPROFILER_FREEFITMANAGER_H
