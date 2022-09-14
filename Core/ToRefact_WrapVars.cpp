//
// Created by Aleksei Lukianchuk on 12-Sep-22.
//

#include "ToRefact_WrapVars.h"
#include "ToRefact_FitManager.h"

#include <RooAbsData.h>

using namespace std;

//#include "FitManager.C"
//#include "Toy.C"

void DumpDataset(RooAbsData *absdata);

//TFile* f;

int WrapVars(size_t jobID = 0 /*string mu = ""*/) {

    //gSystem->Exec("source /afs/cern.ch/user/o/olukianc/public/HGam/xmlAnaWSBuilder/setup_lxplus.sh");
    //gSystem->Exec("source /afs/cern.ch/user/o/olukianc/public/HGam/Workspace/setup.sh");

    //TFile* f = new TFile("/afs/cern.ch/user/o/olukianc/public/HGam/FullRun2/ws_inputs/Davide/workspace/CouplingWS.root");
    //f = new TFile("/afs/cern.ch/user/o/olukianc/public/HGam/Workspace/source/WSfromDavide/Unblinded_mu_latestTheo/workspace/CouplingWS.root");
    //f->ls();

    /* RooWorkspace* ws = (RooWorkspace*) f->Get("combWS");*/

    //stats::FitManager fitManager;
    std::vector<std::string> cats =  {
            //"GG2H_0J_PTH_0_10__0",
            //"GG2H_GE2J_MJJ_350_700_PTH_0_200_PTHJJ_0_25__2",
            //"UNSELECTED_ZH",
            //"QQ2HQQ_0J__0",
            "GG2H_1J_PTH_0_60__0",
            "GG2H_0J_PTH_0_10__0",
            "QQ2HQQ_GE2J_MJJ_0_60__0",
            "QQ2HLNU_PTV_0_75__0",
            "HLL_PTV_0_75__0",
            "TTH_PTH_0_60__0",
            //"GG2H_GE2J_MJJ_0_350_PTH_0_60__0"
    };

    stats::FitManager* fitManager = stats::FitManager::Create();
    cout << "here we are created" << endl;
    //std::unique_ptr<stats::FitManager> fitManager = make_unique<stats::FitManager>( stats::FitManager::Create() );

    /*fitManager->AddWSbyAddress("/afs/cern.ch/user/o/olukianc/public/HGam/Workspace/source/WSfromDavide/Unblinded_mu_latestTheo/workspace/CouplingWS.root",
                  "combWS"
                  );*/
    //fitManager->AddWSbyAddress("CouplingWS.root", "combWS");
    //fitManager->AddWSbyAddress("CouplingWS_mu.root", "combWS");
    //fitManager->AddWSbyAddress("CouplingWS_xs_6XS.root", "combWS");
    fitManager->AddWSbyAddress("CouplingWS_xs_STXS.root", "combWS");
    cout << "ws is added" << endl;
    fitManager->AddModelConfig("ModelConfig");
    fitManager->SetUpData("combData");
    //fitManager->UseBinnedData(true);
    fitManager->SetUpCombinedPdf("CombinedPdf");
    //fitManager->SetUpPoisToFit(stats::POIs::GLOBAL);
    //fitManager->SetUpPoisToFit(stats::POIs::PROD_MODE_6_POI);
    fitManager->SetUpPoisToFit(stats::POIs::STXS);
    fitManager->FloatPOIs();
    fitManager->SetStatOnly(false);
    //fitManager->LoadNPifNeeded("source/NPvalues.txt");

    fitManager->SetUpCats();
    fitManager->ExtractSelectedVariablesToClosure();
    fitManager->ExtractObservables();
    fitManager->ExtractGlobalObservables();
    fitManager->ExtractNuisanseParameters();
    fitManager->ExtractPOIs();
    //cout << " global obs:" << endl;
    //fitManager->PrintGlobalObservables();
    //exit(1);
    //fitManager->FloatPOIs();

    //fitManager->Test();


    //fitManager->FloatVars("mu");
    //cout << " # before do test" << endl;
    //fitManager->DoTest();
    //cout << " # after the test" << endl;

    //std::vector<std::string> Cats;
    //fitManager->GetListOfCategories(Cats);

    /// put all these functions into a one "setup" or "build"

    //fitManager->PlotDSandPdfGivenCats(cats);

    //fitManager->DoToyStudy(1);  //fitManager->DoToyStudyGivenCategory(cats, 1);

    //  fitManager->DoAsimovStudyGivenCategories(cats);
    //fitManager->DoAsimovStudy(cats);
    // fitManager->DoObservedFit(cats);

    //static const vector<string> mus = {"mu_ggF", "mu_VBF", "mu_top", "mu_VH"};
    static const vector<string> mus = {"mu_ggF", "mu_VBF", "mu_WH", "mu_ZH", "mu_ttH", "mu_tH"};

    //fitManager->ComputeExpectedNll( move(mus[jobID]) );

    //fitManager->ComputeExpectedNll("mu");

    fitManager->PrintNP();
    fitManager->PrintObservables();
    fitManager->PrintGlobalObservables();
    //fitManager->ComputeNPforSM();
    //return 0;
    //fitManager->ComputeCentralValuesErrors(mus[jobID]);
    //fitManager->ComputeCentralValuesErrors(jobID);
    fitManager->ComputeCentralValuesErrors("", true);
    return 0;
    //return 0;
    //fitManager->SetUpPoisToFit(stats::POIs::PROD_MODE_6_POI);
    //fitManager->FloatPOIs();

    //fitManager->DoSignificanceStudy("mu");
    //fitManager->DoSignificanceStudy(mus[jobID]);
    //fitManager->ComputeGlobalNll();
    //  return 0;

    //fitManager->SetAllNuisanceParamsFloat();
    //static const vector<string> mus = {"mu_ggF", "mu_VBF", "mu_WH", "mu_ZH", "mu_ttH", "mu_tH"};

    //size_t muNb   = jobID / 15;
    //size_t muStep = jobID % 15;

    //cout << "muNb   = " << muNb   << endl;
    //cout << "muStep = " << muStep << endl;
    //const string mu = mus[muNb];
    //cout << "mu = " << mu << endl;

    //fitManager->LoadNPvalues("source/NPvalues.txt");
    //exit(0);

    // fitManager->RunWorker1DSensitivity("mu_ZH", muStep);
    //fitManager->RunWorker1DSensitivity(mu, muStep);
    //exit(0);
    size_t mu1Step = jobID / 15;
    size_t mu2Step = jobID % 15;

    bool isExpected = true;

    fitManager->RunWorker2DSensitivity("mu_ggF", "mu_VBF", mu1Step, mu2Step, isExpected);
    //fitManager->DoSensitivity1DStudy( mus[jobID] );
    //fitManager->DoSensitivityStudy("mu_ggF", "mu_VBF");


    fitManager->~FitManager();
    //fitManager->DoAsimovStudy();

    /*
    for (const string& cat : {
          "GG2H_0J_PTH_0_10__0",
      "GG2H_GE2J_MJJ_350_700_PTH_0_200_PTHJJ_0_25__2",
      "UNSELECTED_ZH",
      "QQ2HQQ_0J__0" }
      )
      {
        DSperCat[cat] = (RooDataSet*) dsPerCat->operator()( cat.c_str() );
        fitManager->AddCatToClosure(cat,  DSperCat[cat] );

        fitManager->PlotDatasetAndPdfGivenCat(cat, ws);
        }*/

    // reduced DS:
    //RooDataSet* dsSmall = (RooDataSet*) combData->reduce(RooFit::EventRange(1, 1E4));

    // POI

    //combDataBinned->Print();
    //throw std::runtime_error("exit");
    /*
    RooRealVar* mu = dynamic_cast<RooRealVar*>( ws->allVars().find("mu") );
    mu->setConstant(false);
    */

    //combData->Print("v");
    //combinedPdf->Print("v");

    //combinedPdf->fitTo(*combData, RooFit::SumW2Error(true));
    //combinedPdf->fitTo((RooAbsData&) *dsSmall, RooFit::SumW2Error(true));
    ////RooFitResult* fitResult = combinedPdf->fitTo((RooAbsData&) *combDataBinned, RooFit::SumW2Error(true));

    //TCanvas* c = new TCanvas("c", "c", 1200, 800);

    //TH2D* h_corr = (TH2D*) fitResult->correlationHist();
    //const TMatrixDSym covMatrix = fitResult->covarianceMatrix();

    //h_corr->Draw("colz");
    //c->SaveAs("corr.png");

    return 0;
}
//########################################################################################################################################

void DumpDataset(RooAbsData *absdata)
{
    int nb_entries=absdata->numEntries();

    for (int index_entry=0;index_entry<nb_entries;index_entry++) {
        //for (int index_entry=0;index_entry<10;index_entry++) {
        RooArgSet *argset = (RooArgSet *)absdata->get(index_entry);

        //argset->Print();

        //    cout << "m_yy=" << argset->getRealValue("vec_roorealvar_finalDV[0]->GetName());
        cout << "wt=" << absdata->weight() << endl;
        cout << "shit=" << argset->getRealValue("shit") << endl;

    } //end loop events
}
