//
// Created by Aleksei Lukianchuk on 14-Sep-22.
//

#ifndef EFTPROFILER_FITTER_H
#define EFTPROFILER_FITTER_H

#include "IFitter.h"

namespace eft::stats::fit {

class Fitter : public IFitter {
     RooAbsReal* CreatNll(RooAbsData* data, RooAbsPdf* pdf)      override;
     RooAbsReal* CreatNll(std::string&& data, std::string&& pdf) override;
     FitResPtr   Minimize(RooAbsReal* nll)                       override;
     FitResPtr   Fit(RooAbsData* data, RooAbsPdf* pdf)           override;
     FitResPtr   Fit(std::string&& data, std::string&& pdf)      override;
};

} // eft::stats::fit

#endif //EFTPROFILER_FITTER_H
