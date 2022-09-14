//
// Created by Aleksei Lukianchuk on 12-Sep-22.
//

#ifndef EFTPROFILER_TOREFACT_FITMANAGER_H
#define EFTPROFILER_TOREFACT_FITMANAGER_H

#pragma once

#include <vector>
#include <string>
#include <unordered_map>

#include <iostream>
#include <iomanip>
#include <fstream>

#include <memory> // shared_ptr
#include "RooStats/ModelConfig.h"
#include "RooStats/AsymptoticCalculator.h"

// RooFit
#include "RooWorkspace.h"
#include "RooRealVar.h"
#include "RooPlot.h"
#include "RooAbsArg.h"
#include "RooAbsPdf.h"
#include "RooAbsData.h"
#include "RooFitResult.h"
#include "RooCategory.h"
#include "RooDataSet.h"
#include "RooSimultaneous.h"
#include "RooRandom.h"
#include "RooDataHist.h"
#include "RooNLLVar.h"
#include "RooMinimizer.h"
#include "TStopwatch.h"
#include "Math/CholeskyDecomp.h"

// Graphics
#include "TH2D.h"
#include "TCanvas.h"
#include "TLine.h"
#include "TLatex.h"
#include "TLegend.h"

#include "TEllipse.h"

#include "TMatrixTSym.h"
#include "TFitResult.h"

// system
#include "TFile.h"
#include "TROOT.h"
#include "TSystem.h"
#include "TStyle.h"

//#include "CatInfoHolder.h"
#include "ToRefactor_MuTypes.h"

namespace stats {

    class RooMinimizerWrapper : public RooMinimizer {
        // From quickFit
    public:
        using RooMinimizer::applyCovarianceMatrix;

        explicit RooMinimizerWrapper(RooAbsReal& function) : RooMinimizer(function){}
        void applyCovMatrix(TMatrixDSym& V){RooMinimizer::applyCovarianceMatrix(V);}
        int  getNPar() const  {return fitterFcn()->NDim();}
        void setStrategy(Int_t i) {RooMinimizer::setStrategy(i);};
    };

    class IFitManager {
    public:
//virtual void PlotDatasetAndPdfGivenCat(std::string& cat, RooWorkspace* ws);
    };

    class FitManager : public IFitManager{
    public:

        //static FitManager* Create(const string& ws_path, const string& ws_name) { return new FitManager(ws_path, ws_name); }
        static FitManager* Create()     { return new FitManager(); }
        void   Build(); // TODO: to implement
        void   Test();

        // TODO: rewrite the following fucntion to be used in the ctor of the FitManager, which is to be built throuhg a builder class
        inline void ExtractSelectedVariablesToClosure();
        inline void ExtractVariablesToClosure();

        // inner tools
        inline TList* SplitDSonCategories();
        inline void   PrintFoundPois();
        inline void   PrintSummaryPois();
        inline void   WriteObtainedPoisToInternalStorage(ResultType resType = ResultType::OBSERVED, ErrorType errType = ErrorType::STAT);
        void          WriteObtainedPoisFromStorageToFile();
        void          WriteNLLtoFile(MuSensitivityConfig& config);

        inline void PlotDatasetAndPdfGivenCat(const std::string& cat);

        // settings up
        inline void AddWSbyAddress(const std::string& path, const std::string& name);
        inline void AddModelConfig(const std::string& name);
        inline void SetUpData(const std::string& name)        { dataSetTotal_ = dynamic_cast<RooDataSet*>      ( ws_->data( name.c_str() ) ); };
        inline void SetUpCombinedPdf(const std::string& name) { combinedPdf_  = dynamic_cast<RooSimultaneous*> ( ws_->pdf(  name.c_str() ) ); };
        inline void SetUpCats(const std::vector<std::string>& cats);
        inline void SetUpCats();
        inline void SetUpPoisToFit(std::vector<std::string>& pois); // to add
        inline void SetUpPoisToFit(stats::POIs pois);
        inline void UseBinnedData(bool b);

        inline void FloatVars(std::vector<std::string> vars) { for (const std::string& var : vars) { ws_->var( var.c_str() )->setConstant(0); } };
        inline void FloatVars(const char* var)                                                 { ws_->var( var)         ->setConstant(0); };
        inline void FloatPOIs()                        { for (const std::string& var : poisToFit_) { cout << " -- float " << var << endl; ws_->var( var.c_str() )->setConstant(0); } };
        inline void FixPois() { for (const std::string& poi :  poisToFit_) { ws_->var( poi.c_str() )->setConstant(true); } };
        inline void FixPois(const std::vector<std::string>& pois) { for (const std::string& poi : pois) { ws_->var( poi.c_str() )->setConstant(true); } };
        inline void FixPois(const std::string& poi) { std::vector<std::string> _poi {poi}; FixPois(move(_poi));  };
        inline void SetPOIsToValue(double val) { for (const std::string& var : poisToFit_) { ws_->var( var.c_str() )->setVal(val); cout << " - set " << var << " to " << val << endl;}; }
        inline void SetGivenPOItoValue(const std::string& poi, double val) { ws_->var( poi.c_str() )->setVal(val); };
        inline void SetGivenPOItoValue(std::pair<std::string, double> var_val) {  SetGivenPOItoValue(var_val.first, var_val.second); };
        inline void SetGivenPOItoValue(std::vector<std::pair<std::string, double>> vars) { for (const auto& name_val : vars) { SetGivenPOItoValue(name_val); }; }

        inline void SetVarVal(const std::string& name, double val) { ws_->var( name.c_str() )->setVal(val); };
        inline void SetVarErr(const std::string& name, double err) { ws_->var( name.c_str() )->setError(err); };

        inline void SetStatOnly       (bool b = true) { isStatOnly = b;}
        /////inline void SetNP

        void        SetMuFitConfig(); // to add

        void        AddVarToClosure(const std::string& name, const std::string& targetName); // to add
        RooAbsData* GetValFromClosure(const std::string& name); // to add

        // Running modes
        inline void PlotDSandPdfGivenCats(const std::vector<std::string>& cats);

        // toys
        void        DoToyStudy(size_t nbToys = 1e6);
        void        DoToyStudyGivenCategory(std::vector<std::string>& cats, size_t nbToys = 1e6);

        // asimov
        void        DoAsimovStudyBKUP();
        void        DoAsimovStudy();
        void        DoAsimovStudy(std::vector<std::string>& cats);
        void        DoAsimovStudy(size_t nbCats);
        //void        DoAsimovStudy(RooAbsPdf* combinedPdf, RooAbsData* );
        void        DoAsimovStudyGivenCategoriesBKUP(std::vector<std::string>& cats);

        // observed
        RooFitResult*        DoObservedFit(RooAbsPdf* combinedPdf, RooAbsData* dataSet_);
        RooFitResult*        DoObservedFit(std::vector<std::string>& cats);
        inline RooFitResult* DoObservedFit();

        // sensitivity study
        std::shared_ptr<RooFitResult> ComputeObservedValuesGivenPOIs(MuConfig& mu1, MuConfig& mu2, RooAbsPdf* pdf, RooAbsData* ds);
        void          ComputeSensitivityGivenPOI(const std::string& poi1, const std::string& poi2, const std::vector<std::string>& cats);
        void          ComputeSensitivityGivenPOI(const std::string& poi1, const std::string& poi2, size_t nb_cats = 0);
        //double        ComputeNLLgivenConfiguration(MuSensitivityConfig& config); //trash
        double        ComputeNllAnd1Mu();
        double        ComputeGlobalNll(bool statOnly = false);
        double        ComputeNllGivenMuGivenValue(const std::string& muName, double muVal);
        double        ComputeNll2D(const std::string& mu1, const std::string& mu2, double muVal1, double muVal2, bool isExpected = false);
        void          ComputeCentralValuesErrors(const std::string& mu = "", bool isExpected = false);
        void          ComputeCentralValuesErrors(size_t jobID, bool isExpected = false)
        {ComputeCentralValuesErrors(poisToFit_[jobID], isExpected);}
        void          ComputeNPforSM();
        void          ComputeExpectedNll(const std::string&& muToZero);
        // fitting instead of fitTo
        RooAbsReal*              CreateNll(RooAbsData& data, RooAbsPdf& pdf);
        std::shared_ptr<RooFitResult> Minimize(RooAbsReal& nll);

        void         DoFullStudy();
        void         DoSensitivityStudy(const std::string& mu1, const std::string& mu2);
        void         DoSensitivity1DStudy(const std::string& mu);
        void         DoSignificanceStudy(const std::string& mu, bool statOnly = false);

        inline void  RunStudyOneWorker(size_t jobID);
        void         RunStudyOnePoiScheme(stats::POIs scheme);
        void         RunWorker1DSensitivity(const std::string& mu, size_t muStep);
        void         RunWorker2DSensitivity(const std::string& mu1, const std::string& mu2, size_t nbMu1Step, size_t nbMu2Step, bool isExpected = false);
        void         RunWorkerSignificanceStudy(const std::string& mu, bool statOnly = false) const;

        void         LoadNPvalues(const std::string& filename);
        void         LoadRooRealVars(const std::string& filename);
        inline void  LoadNPifNeeded(const std::string& filename) { if (isStatOnly) LoadNPvalues(filename); }
        //map<float, float> ParseNllGivenMuAndStatRegime(const string& mu, bool statOnly = false);
        //void

        // debug
        void        DoTest();
        void        DumpDataset(RooAbsData* ds);

        // partial combined pdf/dataset
        RooSimultaneous* CreateCombinedPdfFromGivenCategories(const std::vector<std::string>& cats);
        RooSimultaneous* CreateCombinedPdfFromGivenCategories(const std::string& cat);
        RooSimultaneous* CreateCombinedPdfFromGivenNumberCategories(size_t nbCats);

        RooDataSet*     CreateCombinedDataSetFromGivenCategories(const std::vector<std::string>& cats);
        RooDataSet*     CreateCombinedDataSetFromGivenCategories(const std::string& cat);
        RooDataSet*     CreateCombinedDataSetFromGivenNumberCategories(size_t nbCats);

        void            CreateAsimovDataSet(const std::vector<std::string>& cats);
        void            PlotAsimovDataSet  (const std::vector<std::string>& cats);

        RooFitResult*   DoSimultaneouslyFitGivenCategories(std::vector<std::string>& cats);
        RooFitResult*   DoSimultaneouslyFitGivenCategories(std::string& cats);

        void            _PerformOffsetNllTest();


        ////// getters
        // PDFs:
        inline RooAbsPdf* GetPdfModelGivenCategory(const std::string& cat)  { return ws_->pdf(("_model_"          + cat + "_HGam__addConstr").c_str()); };
        inline RooAbsPdf* GetPdfSBGivenCategory(const std::string& cat)     { return ws_->pdf(("_modelSB_"        + cat + "_HGam").c_str()); };
        inline RooAbsPdf* GetPdfBkgGivenCategory(const std::string& cat)    { return ws_->pdf(("pdf__background_" + cat + "_HGam").c_str()); };
        inline RooAbsPdf* GetPdfSigGivenCategory(const std::string& cat)    { return ws_->pdf(("pdf__commonSig_"  + cat + "_HGam").c_str()); };

        // DataSets
        inline RooDataSet* GetDataSetGivenCategory(const std::string& cat) { return dsPerCat_[cat]; };

        // observables
        inline RooRealVar* GetObservable(const std::string& name) { return ws_->var( ("atlas_invMass_" + name).c_str() ); };

        // parameters (bkg, yield, ...)
        inline RooRealVar* GetNbkgGivenCategory(const std::string& name) const { return ws_->var( ("nbkg_Hgg_" + name).c_str() ); };

        inline RooRealVar* GetVar(const std::string& name) { return ws_->var(name.c_str());  };

        inline void                     GetListOfCategories(std::vector<std::string>& cats);
        inline std::vector<std::string> GetListOfFirstNcategories(size_t nbCats);

        inline double GetParameterValue(const std::string& par)   const { return ws_->var( par.c_str() )->getVal(); };
        inline double GetParameterError(const std::string& par)   const { return ws_->var( par.c_str() )->getError(); };
        inline double GetParameterErrorHi(const std::string& par) const { return ws_->var( par.c_str() )->getAsymErrorHi();}
        inline double GetParameterErrorLo(const std::string& par) const { return ws_->var( par.c_str() )->getAsymErrorLo();}

        void Fit();
        //inline void SetAllNuisanceParamsZero();
        inline void SetAllNuisanceParamsConst();
        inline void SetAllNuisanceParamsFloat();
        inline void SetAllNuisanceParamsToValue(float val);

        inline void SetGlobalObservablesToValueFoundInFit();
        //inline void SetGlobal

        //private:
        //FitManager(const string& ws_path, const string& ws_name);
        //FitManager();

        ~FitManager();

        inline void ExtractNuisanseParameters() { nuisanceParameters_ = const_cast<RooArgSet*> ( modelConfig_->GetNuisanceParameters()    ); };
        inline void ExtractObservables()        { observables_        = const_cast<RooArgSet*> ( modelConfig_->GetObservables()           ); };
        inline void ExtractGlobalObservables()  { globalObservables_  = const_cast<RooArgSet*> ( modelConfig_->GetGlobalObservables()     ); };
        inline void ExtractPOIs()               { POIs_               = const_cast<RooArgSet*> ( modelConfig_->GetParametersOfInterest() ); };

        //inline RooArgSet GetObservables() { return observables_; };
        inline RooArgSet GetObservables(const std::vector<std::string> cats);

        inline void PrintObservables()        { for (const auto& obs : *observables_)        { /*obs->Print("v");*/ obs->Print(); } };
        inline void PrintGlobalObservables()  { globalObservables_->Print("v"); /*globalObservables_->Print();*/ };
        inline void PrintNP()                 { for (const auto& np  : *nuisanceParameters_) { /*np ->Print("v");*/ np->Print(); } };
        inline void PrintPOIs()               { for (const auto& poi : *POIs_              ) { poi->Print(); } };
        inline void PrintStudyResults(std::string& path);

        inline void PrintInnerParams() {
            cout << "[Observed] \n";
            PrintObservables();
            cout << "[global obs] \n";
            PrintGlobalObservables();
            cout << "[NP] \n";
            PrintNP();
            cout << "[POIs] \n";
            PrintPOIs();
        };
    public:
        void AddCatToClosure(const std::string& cat, RooDataSet* ds);


    private:
        FitManager* gFitManager_;
        TFile* f_;
        RooWorkspace* ws_;
        RooStats::ModelConfig* modelConfig_;

        RooArgSet* nuisanceParameters_;
        RooArgSet* globalObservables_;
        RooArgSet* observables_;
        RooArgSet* POIs_;
        RooCategory* channelList_;

        RooDataSet*  dataSetTotal_;
        RooDataHist* dataHistTotal_;
        //RooAbsData* dataSetTotal_;
        RooSimultaneous* combinedPdf_;

        TList*  rawListDataPerCat_;
        //unordered_map<std::string, RooDataSet*> dsPerCat_;
        std::map<std::string, RooDataSet*>* asimovDSperCat = nullptr;
        RooDataSet* dsAsimov;
        std::map<std::string, RooDataSet*> dsPerCat_;
        // unordered_map<std::string, CatInfoHolder*> closure_; // a global closure, holding all information for a given category
        std::vector<std::string> catsForStudy_;
        std::vector<std::string> poisToFit_;

        stats::POIs study_;

        bool isStatOnly = false;

        std::map<POIs, std::map<std::string, MuResult>> muResults_f_POItype_f_mu;

        //TFitResult
    };

} // namespace stats


#endif //EFTPROFILER_TOREFACT_FITMANAGER_H
