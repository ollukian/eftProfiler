//
// Created by Aleksei Lukianchuk on 29-Nov-22.
//

#ifndef EFTPROFILER_HESSESTUDYRESULT_H
#define EFTPROFILER_HESSESTUDYRESULT_H

#include "TMatrixDSym.h"
#include "RooArgList.h"

class RooFitResult;
class RooRealVar;
//class RooArgList;

#include <map>
#include <vector>
#include <iostream>

namespace eft::stats {

struct HesseStudyResult {
    TMatrixDSym                     reducedCovMatrix;
    RooArgList                      params;
    std::map<std::string, double>   covariances;

    bool is_sorted {false};

    //void                          Sort() noexcept;
    //std::map<std::string, double> GetNSorted(size_t n) const;
    //std::vector<std::string> GetNSortedNames(size_t n) const;
    //std::vector<std::string>        np_names;

    HesseStudyResult()  = default;
    ~HesseStudyResult() = default;

    HesseStudyResult(const TMatrixDSym& reducedCov, const RooArgList& params_ext, std::map<std::string, double> covariances_ext)
        : reducedCovMatrix(reducedCov)
        , params(params_ext)
        , covariances(std::move(covariances_ext))
    {}

    static HesseStudyResult ExtractFromRooFitResult(const RooFitResult& res, const RooArgList& params);
};

std::ostream& operator << (std::ostream& os, const HesseStudyResult& res);

} // eft::stats

#endif //EFTPROFILER_HESSESTUDYRESULT_H
