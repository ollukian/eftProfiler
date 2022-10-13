//
// Created by Aleksei Lukianchuk on 04-Oct-22.
//

//#include "RooFitResult.h"

#ifndef EFTPROFILER_IFITRESULT_H
#define EFTPROFILER_IFITRESULT_H

class RooFitResult;
class RooAbsReal;

namespace eft::stats::fit {

class IFitResult {
public:
    RooFitResult* rooFitResult = nullptr;
    RooAbsReal* nll = nullptr;
    //std::unique_ptr<RooFitResult> rooFitResult {};
    //double nll {0.};
private:

};

} // eft::stats::fit

#endif //EFTPROFILER_IFITRESULT_H
