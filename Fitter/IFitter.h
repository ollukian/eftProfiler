//
// Created by Aleksei Lukianchuk on 14-Sep-22.
//

#ifndef EFTPROFILER_IFITTER_H
#define EFTPROFILER_IFITTER_H

#include "RooAbsReal.h"
#include "RooAbsData.h"
#include "RooAbsPdf.h"
#include "RooFitResult.h"

namespace eft::stats::fit {

class IFitter {
public:
    using FitResPtr = std::unique_ptr<RooFitResult>;
    virtual ~IFitter() = default;

    virtual RooAbsReal* CreatNll(RooAbsData* data, RooAbsPdf* pdf)      = 0;
    virtual RooAbsReal* CreatNll(std::string&& data, std::string&& pdf) = 0;
    virtual FitResPtr   Minimize(RooAbsReal* nll)                       = 0;
    virtual FitResPtr   Fit(RooAbsData* data, RooAbsPdf* pdf)           = 0;
    virtual FitResPtr   Fit(std::string&& data, std::string&& pdf)      = 0;
};

} // eft::stats::fit

#endif //EFTPROFILER_IFITTER_H