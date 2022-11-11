//
// Created by Aleksei Lukianchuk on 14-Sep-22.
//

// Fitting is based on quickFit:
// https://gitlab.cern.ch/atlas_higgs_combination/projects/lhc-comb-tools/-/tree/master/quickFit

#include "Fitter.h"
#include "IFitResult.h"

#include "RooNLLVar.h"
#include "RooMinimizer.h"
#include "TStopwatch.h"
#include "Math/CholeskyDecomp.h"
#include "../Core/Logger.h"

using namespace std;

namespace eft::stats::fit {

RooAbsReal* Fitter::CreatNll(const FitSettings& settings) {
    EFT_PROF_TRACE("[CreateNll]");
    TStopwatch timer;
    RooAbsReal* nll = settings.pdf->createNLL(*settings.data,
                                    RooFit::BatchMode(true),
                                    RooFit::CloneData(false),
            //IntegrateBins(_samplingRelTol),
                                    RooFit::GlobalObservables(*globs_),
                                    RooFit::Constrain(*nps_) // try with this line
            //ConditionalObservables(),
            //ExternalConstraints(*_externalConstraint)
    );
    nll->enableOffsetting(true);
    timer.Stop();
    double t_cpu_  = timer.CpuTime()/60.;
    double t_real_ = timer.RealTime()/60.;
return nll;
}

IFitter::FitResPtr Fitter::Minimize(const FitSettings& settings) {
    EFT_PROF_TRACE("[Minimize]");

    if (settings.nll == nullptr) {
        EFT_PROF_CRITICAL("minimize, no nll is set");
        throw std::runtime_error("no nll is set for minimisation");
    }

    EFT_PROF_INFO("[Minimizer] create a RooMinimizerWrapper. Error handling: {}", settings.errors);
    RooMinimizerWrapper minim(*settings.nll);
    EFT_PROF_TRACE("[Minimizer] a RooMinimizerWrapper is created");
    minim.setStrategy( 1 );
    EFT_PROF_INFO("[Minimizer] set strategy to 1");
    minim.setPrintLevel( 1 );
    RooMsgService::instance().setGlobalKillBelow(RooFit::FATAL);
    minim.setProfile(); /* print out time */
    minim.setEps(1E-03 / 0.001); // used to be 1E-3 ==> minimise until 1E-6
    minim.setOffsetting( true );
    EFT_PROF_INFO("[Minimizer] allow offseting");
    minim.optimizeConst( 2 );
    EFT_PROF_INFO("[Minimizer] set optimize constant 2");
    // Line suggested by Stefan, to avoid running out function calls when there are many parameters
    minim.setMaxFunctionCalls(5000 * settings.pdf->getVariables()->getSize());

    int _status = 0;

    /*if ( _useSIMPLEX ) {
      cout << endl << "Starting fit with SIMPLEX...");
      _status += minim.simplex();
      }*/

    EFT_PROF_INFO("[Minimizer] minimizerType = Minuit2, alg: Migrag");
    minim.setMinimizerType( "Minuit2" );
    // Perform fit with MIGRAD
    _status += minim.minimize( "Minuit2", "Migrad" );
    //_status += minim.minimize( "Minuit2");
    EFT_PROF_INFO("[Minimizer] fit status: {}", _status);
    /*if ( _useHESSE ) {
      cout << endl << "Starting fit with HESSE..." << endl;
      _status += minim.hesse();
      minim.save("hesse","")->Print();
      }*/

    switch (settings.errors) {
        case Errors::HESSE:
            EFT_PROF_INFO("[Minimizer] start HESSE...");
            minim.hesse();
            EFT_PROF_INFO("[Minimizer] HESSE is done");
            //minim.save("hesse", "")->Print();
            break; // nothing
        case Errors::MINOS_NPS:
            assert(settings.nps);
            EFT_PROF_INFO("[Minimizer] re-estimate errors for NPS");
            minim.minos(*settings.nps);
            break;
        case Errors::MINOS_POIS:
            EFT_PROF_INFO("[Minimizer] re-estimate errors for POIS");
            assert(settings.pois);
            minim.minos(*settings.pois);
            break;
        case Errors::MINOS_ALL:
            EFT_PROF_INFO("[Minimizer] re-estimate errors for NPS & POIS");
            assert(settings.nps);
            assert(settings.pois);
            minim.minos(RooArgSet{*settings.pois, *settings.nps});
            break;
        case Errors::DEFAULT:
            EFT_PROF_INFO("[Minimizer] no need to re-estimate errors, default strategy is set");
            break;
    }

//    if (settings.errors == Errors::HESSE) {
//        EFT_PROF_INFO("[Minimizer] start HESSE...");
//        minim.hesse();
//        EFT_PROF_INFO("[Minimizer] HESSE is done");
//        //minim.save("hesse", "")->Print();
//    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// COPIED from QuickFit:                                                                                        //
    // https://gitlab.cern.ch/atlas_higgs_combination/projects/lhc-comb-tools/-/blob/master/quickFit/src/fitTool.cxx //
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Copied from RooAbsPdf::fitTo()
    //if (_doSumW2==1 && minim.getNPar()>0) {
#if 0
    if (settings.errors != Errors::DEFAULT) {
    //if (false) {

        if (settings.errors == Errors::HESSE) {
            EFT_PROF_INFO("[Minimizer] start HESSE...");
            minim.hesse();
            EFT_PROF_INFO("[Minimizer] HESSE is done");
            //minim.save("hesse", "")->Print();
        }



        EFT_PROF_INFO("[Minimizer] Evaluating SumW2 error...");
        // Make list of RooNLLVar components of FCN
        EFT_PROF_DEBUG("[Minimizer] extract nll components");
        unique_ptr<RooArgSet> comps = make_unique<RooArgSet>(*settings.nll->getComponents());
        EFT_PROF_DEBUG("[Minimizer] extracted {} nll components", comps->getSize());
        EFT_PROF_DEBUG("[Minimizer] add nll components..");
        vector<unique_ptr<RooNLLVar>> nllComponents;
        nllComponents.reserve(comps->getSize());
        for (const auto nll_comp : *comps) {
            auto* nllComp = dynamic_cast<RooNLLVar*>(nll_comp);
            if(!nllComp)
                continue;
            nllComponents.emplace_back(make_unique<RooNLLVar>(*nllComp));
        }
        EFT_PROF_DEBUG("[Minimizer] {} nll components are added", nllComponents.size());
        // Calculated corrected errors for weighted likelihood fits
        EFT_PROF_DEBUG("[Minimizer] save minim results");
        RooFitResult* rw = minim.save();
        EFT_PROF_DEBUG("[Minimizer] results are save");
        EFT_PROF_DEBUG("[Minimizer] apply covariances");
        for (auto& nllComponent : nllComponents) {
            EFT_PROF_DEBUG("apply cov to {}...", nllComponent->GetName());
            nllComponent->applyWeightSquared(kTRUE);
            EFT_PROF_DEBUG("apply cov to {} DONE", nllComponent->GetName());
        }
        EFT_PROF_DEBUG("[Minimizer] covariances are added");
        EFT_PROF_INFO("Calculating sum-of-weights-squared correction matrix for covariance matrix");
        minim.hesse();
        EFT_PROF_INFO("[Minimizer] hesse successfully finished working, applying the sum-of-weights-squared correction matrix");
        RooFitResult* rw2 = minim.save();
        for (auto& nllComponent : nllComponents) {
            nllComponent->applyWeightSquared(kFALSE);
        }

        // Apply correction matrix
        const TMatrixDSym& matV = rw->covarianceMatrix();
        TMatrixDSym matC = rw2->covarianceMatrix();
        using ROOT::Math::CholeskyDecompGenDim;
        CholeskyDecompGenDim<Double_t> decomp(matC.GetNrows(), matC);
        if (!decomp) {
            cerr << "ERROR: Cannot apply sum-of-weights correction to covariance matrix: correction matrix calculated with weight-squared is singular" << endl;
        } else {
            // replace C by its inverse
            decomp.Invert(matC);
            // the class lies about the matrix being symmetric, so fill in the
            // part above the diagonal
            for (int i = 0; i < matC.GetNrows(); ++i) {
                for (int j = 0; j < i; ++j) {
                    matC(j, i) = matC(i, j);
                }
            }
            matC.Similarity(matV);
            // C now contiains V C^-1 V
            // Propagate corrected errors to parameters objects
            minim.applyCovarianceMatrix(matC);
        }

        delete rw;
        delete rw2;

        switch (settings.errors) {
            case Errors::HESSE:
                EFT_PROF_INFO("[Minimizer] no need to re-estimate errors with MINOS: already done Hesse");
                break; // nothing
            case Errors::MINOS_NPS:
                assert(settings.nps);
                EFT_PROF_INFO("[Minimizer] re-estimate errors for NPS");
                minim.minos(*settings.nps);
                break;
            case Errors::MINOS_POIS:
                EFT_PROF_INFO("[Minimizer] re-estimate errors for POIS");
                assert(settings.pois);
                minim.minos(*settings.pois);
                break;
            case Errors::MINOS_ALL:
                EFT_PROF_INFO("[Minimizer] re-estimate errors for NPS & POIS");
                assert(settings.nps);
                assert(settings.pois);
                minim.minos(RooArgSet{*settings.pois, *settings.nps});
                break;
            case Errors::DEFAULT:
                EFT_PROF_INFO("[Minimizer] no need to re-estimate errors, default strategy is set");
                break;
        }


    }
#endif

    auto result = make_unique<RooFitResult>(
            *minim.save("fitResult","Fit Results")
            );

    EFT_PROF_INFO("[Minimizer] fit is finished. Min nll: {}", result->minNll());
    return result;
}

IFitter::FitResPtr Fitter::Fit(FitSettings& settings) {
    EFT_PROF_TRACE("Fitter::Fit");
    if ( ! globs_ ) {
        EFT_PROF_CRITICAL("Fitter::Fit globals observables are not set");
        throw std::runtime_error("global obs are not set");
    }
    if ( ! nps_ ) {
        EFT_PROF_CRITICAL("Fitter::Fit nps are not set");
        throw std::runtime_error("nps are not set");
    }
    settings.globalObs = globs_;
    settings.nps = nps_;

    auto nll = CreatNll(settings);
    settings.nll = nll;
    auto res = Minimize(settings);
    return res;
    //FitResPtr to_return;
    //to_return.rooFitResult = res.rooFitResult;
    //to_return.nll = nll;
    //FitResPtr res;
    //res.rooFitResult = Mi
    //return  {Minimize(nll, pdf), nll->getVal()};
}

} // eft::stats::fit