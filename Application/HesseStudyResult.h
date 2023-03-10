//
// Created by Aleksei Lukianchuk on 29-Nov-22.
//

#ifndef EFTPROFILER_HESSESTUDYRESULT_H
#define EFTPROFILER_HESSESTUDYRESULT_H

#include "TMatrixDSym.h"
#include "RooArgList.h"
#include "RooFitResult.h"

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
    std::map<std::string, double>   corr_per_np;
    std::string                     poi;

    std::vector<std::pair<std::string, double>> sorted_covariances;
    std::vector<std::pair<std::string, double>> sorted_correlations;
    std::vector<std::pair<std::string, double>> correlations_per_nb_np;
    std::vector<std::string>                    sorted_names;

    bool is_sorted {false};
    RooFitResult*                    fitResult;


    void                                               Sort() noexcept;
    const std::vector<std::pair<std::string, double>>& GetSorted() noexcept;
    const std::vector<std::string>&                    GetSortedNames() noexcept;
    const std::vector<std::pair<std::string, double>>& GetNBiggestImpact(size_t n) const noexcept;
    const std::vector<std::pair<std::string, double>>& GetNSmallestImpact(size_t n) const noexcept;
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
