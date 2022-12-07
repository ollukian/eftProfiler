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
    //es.hess
    auto cov = res.reducedCovarianceMatrix(params);
    //EFT_PROF_DEBUG("Invert covariance", params.size());
    //cov.Invert();
    //EFT_PROF_DEBUG("Available {} params", params.size());
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

    HesseStudyResult hesseStudyResult {cov, params, std::move(covariances)};
    hesseStudyResult.fitResult = dynamic_cast<RooFitResult*>(res.Clone());
    return hesseStudyResult;
    //return {cov, params, std::move(covariances)};
}

void HesseStudyResult::Sort() noexcept {
    EFT_PROFILE_FN();

    vector<pair<string, double>> res {covariances.begin(), covariances.end()};

    std::sort(res.begin(),
              res.end(),
              [&](const auto& l, const auto& r) -> bool {
                  //EFT_PROF_DEBUG("compare===== l > r");
                  //EFT_PROF_DEBUG("{:50} with impact {:10}", l.first, l.second);
                  //EFT_PROF_DEBUG("{:50} with impact {:10}", r.first, r.second);
                  //EFT_PROF_DEBUG("res: {}", (l.second > r.second));
       return abs(l.second) > abs(r.second);
    });

    vector<pair<string, double>> res_corrs {corr_per_np.begin(), corr_per_np.end()};
    std::sort(res_corrs.begin(),
              res_corrs.end(),
              [&](const auto& l, const auto& r) -> bool {
                  //EFT_PROF_DEBUG("compare===== l > r");
                  //EFT_PROF_DEBUG("{:50} with impact {:10}", l.first, l.second);
                  //EFT_PROF_DEBUG("{:50} with impact {:10}", r.first, r.second);
                  //EFT_PROF_DEBUG("res: {}", (l.second > r.second));
                  return abs(l.second) > abs(r.second);
              });


    sorted_names.clear();
    sorted_names.reserve(res.size());
    EFT_PROF_DEBUG("in the sorted:");
    for (const auto& [name, impact] : res) {
        EFT_PROF_INFO("[{:50}] ==> {}", name, impact);
        sorted_names.push_back(name);
    }
    sorted_names.shrink_to_fit();

    is_sorted = true;
    sorted_covariances = std::move(res);
    sorted_correlations = std::move(res_corrs);
}

const std::vector<std::pair<std::string, double>>& HesseStudyResult::GetSorted() noexcept {
    if ( ! is_sorted )
        Sort();
    return sorted_covariances;
}

const std::vector<std::string>&  HesseStudyResult::GetSortedNames() noexcept {
    if ( ! is_sorted )
        Sort();
    return sorted_names;
}


    std::ostream& operator << (std::ostream& os, const HesseStudyResult& res) {
    if (res.covariances.empty())
        return os;
    os << '{';
    size_t idx = 1;
    for (const auto& [name, cov] : res.covariances) {
        os << fmt::format("|#{:3}|{:50} ==> {}\n", idx++, name, cov);
    }
    return os << '}';
}

} // eft::stats

