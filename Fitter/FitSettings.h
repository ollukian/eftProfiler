//
// Created by Aleksei Lukianchuk on 20-Oct-22.
//

#ifndef EFTPROFILER_FITSETTINGS_H
#define EFTPROFILER_FITSETTINGS_H

#include "ErrorsReEvaluation.h"

//#include "RooArgSet.h"
//#include "RooAbsReal.h"

class RooAbsData;
class RooAbsPdf;
class RooArgSet;
class RooAbsReal;

namespace eft::stats::fit {

struct  FitSettings {
    RooAbsData* data        {nullptr};
    RooAbsPdf*  pdf         {nullptr};//{nullptr};
    RooArgSet*  globalObs   {nullptr};//{new RooArgSet{}};//{nullptr};
    RooArgSet*  nps         {nullptr};//{new RooArgSet{}};//{nullptr};
    RooArgSet*  pois        {nullptr};//{new RooArgSet{}};//{nullptr};
    RooAbsReal* nll         {nullptr};
    Errors      errors      {Errors::DEFAULT};
    uint8_t     retry       {0};
    uint8_t     strategy    {1};
    double      eps         {1E-3};
    bool        reuse_nll   {true};
    bool        save_res    {false};
};

} // eft::stats::fit

#endif //EFTPROFILER_FITSETTINGS_H
