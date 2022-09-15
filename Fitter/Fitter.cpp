//
// Created by Aleksei Lukianchuk on 14-Sep-22.
//

// Fitting is based on quickFit:
// https://gitlab.cern.ch/atlas_higgs_combination/projects/lhc-comb-tools/-/tree/master/quickFit

#include "Fitter.h"

#include "RooNLLVar.h"
#include "RooMinimizer.h"
#include "TStopwatch.h"
#include "Math/CholeskyDecomp.h"

using namespace std;

namespace eft::stats::fit {

RooAbsReal* Fitter::CreatNll(RooAbsData *data, RooAbsPdf *pdf, RooArgSet* globalObs) {
    TStopwatch timer;
    RooAbsReal* nll = pdf->createNLL(*data,
                                    RooFit::BatchMode(true),
                                    RooFit::CloneData(false),
            //IntegrateBins(_samplingRelTol),
                                    RooFit::GlobalObservables(*globalObs)
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
    cout << "[Minimizer] create a RooMinimizerWrapper" << endl;
    RooMinimizerWrapper minim(*nll);
    cout << "[Minimizer] a RooMinimizerWrapper is created" << endl;
    //if(_errorLevel>0) minim.setErrorLevel(_errorLevel);
    minim.setStrategy( 1 );
    cout << "[Minimizer] set stratego to 1" << endl;
    //minim.setPrintLevel( _printLevel-1 );
    minim.setPrintLevel( 1 );
    //if (_printLevel < 0)
    RooMsgService::instance().setGlobalKillBelow(RooFit::FATAL);
    minim.setProfile(); /* print out time */
    minim.setEps( 1E-03 / 0.001 );
    cout << "[Minimizer] set EPS to 1E-6" << endl;
    minim.setOffsetting( true );
    cout << "[Minimizer] set offsetting true" << endl;
    //if (_optConst > 0) minim.optimizeConst( _optConst );
    minim.optimizeConst( 2 );
    cout << "[Minimizer] set optimize constant 2" << endl;
    // Line suggested by Stefan, to avoid running out function calls when there are many parameters
    minim.setMaxFunctionCalls(5000 * pdf->getVariables()->getSize());

    int _status = 0;

    /*if ( _useSIMPLEX ) {
      cout << endl << "Starting fit with SIMPLEX..." << endl;
      _status += minim.simplex();
      }*/

    cout << "[Minimizer] minimizerType = Minuit2" << endl;
    minim.setMinimizerType( "Minuit2" );
    // Perform fit with MIGRAD
    _status += minim.minimize( "Minuit2" );

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
            RooNLLVar* nllComp = dynamic_cast<RooNLLVar*>(arg);
            if (!nllComp) continue;
            nllComponents.push_back(nllComp);
        }
        delete citer;
        delete comps;

        // Calculated corrected errors for weighted likelihood fits
        RooFitResult* rw = minim.save();
        for (vector<RooNLLVar*>::iterator it = nllComponents.begin(); nllComponents.end() != it; ++it) {
            (*it)->applyWeightSquared(kTRUE);
        }
        cout << "Calculating sum-of-weights-squared correction matrix for covariance matrix" << endl;
        minim.hesse();
        RooFitResult* rw2 = minim.save();
        for (vector<RooNLLVar*>::iterator it = nllComponents.begin(); nllComponents.end() != it; ++it) {
            (*it)->applyWeightSquared(kFALSE);
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
    auto result = make_unique<RooFitResult>(
            *minim.save("fitResult","Fit Results")
            );
    //result.reset(minim.save("fitResult","Fit Results"));

    return result;
}

} // eft::stats::fit