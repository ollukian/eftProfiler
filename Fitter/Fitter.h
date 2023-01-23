//
// Created by Aleksei Lukianchuk on 14-Sep-22.
//

#ifndef EFTPROFILER_FITTER_H
#define EFTPROFILER_FITTER_H

#include "IFitter.h"
#include "FitSettings.h"

#include "RooMinimizer.h"
#include "RooAbsReal.h"
#include "TMatrixDSym.h"

namespace eft::stats::fit {

class RooMinimizerWrapper;
class Fitter;


class Fitter : public IFitter {
public:
    Fitter() = default;
    ~Fitter() noexcept override = default;

     RooAbsReal* CreatNll(const FitSettings& settings) override;
     //RooAbsReal* CreatNll(std::string&& data, std::string&& pdf, RooArgSet* globalObs) override{ return nullptr; }
     IFitter::FitResPtr Minimize(const FitSettings& settings, RooAbsReal *nll) override;
     FitResPtr   Fit(FitSettings& settings) override;
     //FitResPtr   Fit(std::string&& data, std::string&& pdf)      override{return {};}
     void SetGlobs(RooArgSet* globs) noexcept override { globs_ = globs; };
     void SetNps(RooArgSet* nps) noexcept override { nps_ = nps; }
     int  GetLastFitStatus() noexcept override { return last_fit_status_; }
private:
    RooArgSet* nps_   = nullptr;
    RooArgSet* globs_ = nullptr;
    int        last_fit_status_ {-999};
};

class RooMinimizerWrapper : public RooMinimizer {
 // From quickFit: (with some modifications: attributes + inlining)
 // https://gitlab.cern.ch/atlas_higgs_combination/projects/lhc-comb-tools/-/tree/master/quickFit
public:
    using RooMinimizer::applyCovarianceMatrix;

    explicit RooMinimizerWrapper(RooAbsReal& function) : RooMinimizer(function){}
    void applyCovMatrix(TMatrixDSym& V){RooMinimizer::applyCovarianceMatrix(V);}
    [[nodiscard]] inline size_t getNPar() const  { return fitterFcn()->NDim(); }
    inline void setStrategy(Int_t i) { RooMinimizer::setStrategy(i); };
};

} // eft::stats::fit

#endif //EFTPROFILER_FITTER_H
