//
// Created by Aleksei Lukianchuk on 14-Sep-22.
//

#ifndef EFTPROFILER_IFITTER_H
#define EFTPROFILER_IFITTER_H

#include "RooAbsReal.h"
#include "RooAbsData.h"
#include "RooAbsPdf.h"
#include "RooFitResult.h"
#include "IFitResult.h"
#include "FitSettings.h"

namespace eft::stats::fit {

class IFitter {
public:
    using FitResPtr = std::unique_ptr<RooFitResult>;
    //using FitResPtr = std::unique_ptr<IFitResult>;

    //using FitResPtr = IFitResult;
    virtual ~IFitter() noexcept = default;

    virtual RooAbsReal* CreatNll(const FitSettings& settings)      = 0;
    //virtual RooAbsReal* CreatNll(std::string&& data, std::string&& pdf, RooArgSet* globalObs) = 0;
    virtual FitResPtr   Minimize(const FitSettings& settings)       = 0;
    virtual FitResPtr   Fit(FitSettings& settings) = 0;
    virtual void SetGlobs(RooArgSet* globs) noexcept = 0;
    virtual void SetNps(RooArgSet* nps) noexcept = 0;
    //virtual FitResPtr   Fit(std::string&& data, std::string&& pdf)      = 0;
};

} // eft::stats::fit

#endif //EFTPROFILER_IFITTER_H
