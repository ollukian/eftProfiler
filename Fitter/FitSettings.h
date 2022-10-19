//
// Created by Aleksei Lukianchuk on 20-Oct-22.
//

#ifndef EFTPROFILER_FITSETTINGS_H
#define EFTPROFILER_FITSETTINGS_H

#include "ErrorsReEvaluation.h"

class RooAbsData;
class RooAbsPdf;
class RooArgSet;
class RooAbsReal;

namespace eft::stats::fit {

struct FitSettings {
    RooAbsData* data        = nullptr;
    RooAbsPdf*  pdf         = nullptr;
    RooArgSet*  globalObs   = nullptr;
    RooArgSet*  nps         = nullptr;
    RooAbsReal* nll         = nullptr;
    Errors      errors = Errors::DEFAULT;
};

} // eft::stats::fit

#endif //EFTPROFILER_FITSETTINGS_H
