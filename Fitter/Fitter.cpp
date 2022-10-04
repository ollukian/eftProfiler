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

RooAbsReal* Fitter::CreatNll(RooAbsData *data, RooAbsPdf *pdf, RooArgSet* globalObs, RooArgSet* np) {
    EFT_PROF_TRACE("[CreateNll]");
    TStopwatch timer;
    RooAbsReal* nll = pdf->createNLL(*data,
                                    RooFit::BatchMode(true),
                                    RooFit::CloneData(false),
            //IntegrateBins(_samplingRelTol),
                                    RooFit::GlobalObservables(*globalObs),
                                    RooFit::Constrain(*np) // try with this line
            //ConditionalObservables(),
            //ExternalConstraints(*_externalConstraint)
    );
    nll->enableOffsetting(true);
    timer.Stop();
    double t_cpu_  = timer.CpuTime()/60.;
    double t_real_ = timer.RealTime()/60.;
return nll;
}

IFitter::FitResPtr Fitter::Minimize(RooAbsReal *nll, RooAbsPdf* pdf) {
    EFT_PROF_TRACE("[Minimize]");
    EFT_PROF_INFO("[Minimizer] create a RooMinimizerWrapper");
    RooMinimizerWrapper minim(*nll);
    EFT_PROF_TRACE("[Minimizer] a RooMinimizerWrapper is created");
    //if(_errorLevel>0) minim.setErrorLevel(_errorLevel);
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
    minim.setMaxFunctionCalls(5000 * pdf->getVariables()->getSize());

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

    // Copied from RooAbsPdf::fitTo()
    //if (_doSumW2==1 && minim.getNPar()>0) {
    if (false) {
        cout << endl << "Evaluating SumW2 error..." << endl <<endl;
        // Make list of RooNLLVar components of FCN
        RooArgSet* comps = nll->getComponents();
        vector<RooNLLVar*> nllComponents;
        nllComponents.reserve(comps->getSize());
        TIterator* citer = comps->createIterator();
        RooAbsArg* arg;
        while ((arg=(RooAbsArg*)citer->Next())) {
            auto* nllComp = dynamic_cast<RooNLLVar*>(arg);
            if (!nllComp) continue;
            nllComponents.push_back(nllComp);
        }
        delete citer;
        delete comps;

        // Calculated corrected errors for weighted likelihood fits
        RooFitResult* rw = minim.save();
        for (auto& nllComponent : nllComponents) {
            nllComponent->applyWeightSquared(kTRUE);
        }
        cout << "Calculating sum-of-weights-squared correction matrix for covariance matrix" << endl;
        minim.hesse();
        RooFitResult* rw2 = minim.save();
        for (auto & nllComponent : nllComponents) {
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
            for (int i = 0; i < matC.GetNrows(); ++i)
                for (int j = 0; j < i; ++j) matC(j, i) = matC(i, j);
            matC.Similarity(matV);
            // C now contiains V C^-1 V
            // Propagate corrected errors to parameters objects
            minim.applyCovarianceMatrix(matC);
        }

        delete rw;
        delete rw2;
    }

    /*if ( _useMINOS > 0 ) {
      cout << endl << "Starting fit with MINOS..." << endl;
      if( _useMINOS == 1){
      cout << endl << "Evaluating MINOS errors for all POIs..." << endl;
      _status += minim.minos( *_paramList );
      }
      else if( _useMINOS == 2){
      cout << endl << "Evaluating MINOS errors for all NPs..." << endl;
      _status += minim.minos( *mc->GetNuisanceParameters() );
      }
      else if( _useMINOS == 3){
      cout << endl << "Evaluating MINOS errors for both POIs and NPs..." << endl;
      _status += minim.minos( RooArgSet(*_paramList, *mc->GetNuisanceParameters()) );
      }
      else{
      cout << endl << "Unknown code for MINOS fit " << _useMINOS
      << " assume running MINOS only on POIs..." << endl;
      _status += minim.minos( *_paramList );
      }
      }*/

    //if(_saveFitResult) _result.reset(minim.save("fitResult","Fit Results"));
    //EFT_PROF_DEBUG("[Minimizer] save results");
    auto result = make_unique<RooFitResult>(
            *minim.save("fitResult","Fit Results")
            );

    EFT_PROF_INFO("[Minimizer] fit is finished. Min nll: {}", result->minNll());
    //result->Print("v");
    //EFT_PROF_INFO("[Minimizer] covariance:");
    //result->covarianceMatrix().Print("v");
    //EFT_PROF_INFO("[Minimizer] correlation:");
    //result->correlationMatrix().Print("v");

    //result.reset(minim.save("fitResult","Fit Results"));
    //eft::stats::fit::IFitResult res;
    //FitResPtr res;

    //return {};
    return result;
}

IFitter::FitResPtr Fitter::Fit(RooAbsData *data, RooAbsPdf *pdf) {
    EFT_PROF_TRACE("Fitter::Fit");
    if ( ! globs_ ) {
        EFT_PROF_CRITICAL("Fitter::Fit globals observables are not set");
        throw std::runtime_error("global obs are not set");
    }
    if ( ! nps_ ) {
        EFT_PROF_CRITICAL("Fitter::Fit nps are not set");
        throw std::runtime_error("nps are not set");
    }
    auto nll = CreatNll(data, pdf, globs_, nps_);
    auto res = Minimize(nll, pdf);
    return res;
    //FitResPtr to_return;
    //to_return.rooFitResult = res.rooFitResult;
    //to_return.nll = nll;
    //FitResPtr res;
    //res.rooFitResult = Mi
    //return  {Minimize(nll, pdf), nll->getVal()};
}

} // eft::stats::fit