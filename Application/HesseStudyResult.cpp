//
// Created by Aleksei Lukianchuk on 29-Nov-22.
//

#include "HesseStudyResult.h"

#include "RooArgList.h"
#include "RooRealVar.h"
#include "RooFitResult.h"

#include "Logger.h"
#include "Profiler.h"

using namespace std;

namespace eft::stats {

HesseStudyResult
HesseStudyResult::ExtractFromRooFitResult(const RooFitResult& res, const RooArgList& params)
{
    EFT_PROFILE_FN();
    EFT_PROF_INFO("Extract hesse study result from RooFitResult");

    EFT_PROF_DEBUG("Available {} params", params.size());
    auto cov = res.reducedCovarianceMatrix(params);
    map<string, double> covariances;

    for (size_t idx_np {0}; idx_np < params.size(); ++idx_np) {
        auto np = dynamic_cast<RooRealVar*>(params.at(idx_np));
        double np_cov = cov.operator()(idx_np, idx_np);
        string np_name = np->GetName();
        EFT_PROF_DEBUG("For: idx: {:3}  => {:20} with value: {}",
                       idx_np,
                       np_name,
                       np_cov);
        covariances[std::move(np_name)] = np_cov;
    }


    return {cov, params, std::move(covariances)};
}


std::ostream& operator << (std::ostream& os, const HesseStudyResult& res) {
    if (res.covariances.empty())
        return os;
    os << '{';
    for (const auto& [name, cov] : res.covariances) {
        os << fmt::format("{:40} ==> {}", name, cov);
    }
    return os << '}';
}

} // eft::stats

