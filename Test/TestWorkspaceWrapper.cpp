//
// Created by Aleksei Lukianchuk on 23-Nov-22.
//

#include "Tester.h"
#include "../Utils/ColourUtils.h"
#include "../Core/WorkspaceWrapper.h"
#include "RooRandom.h"
#include "RooFitResult.h"
#include "RooFormulaVar.h"
#include "RooPlot.h"
#include "TLine.h"
#include "RooProfileLL.h"
#include "TCanvas.h"
#include "RooGaussian.h"
#include "RooProdPdf.h"
#include "RooAddPdf.h"
#include "RooConstVar.h"

using namespace eft::utils;
using namespace eft::stats;
using namespace std;

//namespace eft::inner::tests {

RooWorkspace* CreateWS(const string& filename)
{

    using RooStats::ModelConfig;
    using WS = RooWorkspace;

    auto ws = new RooWorkspace("ws_test");
    //auto ws = make_unique<RooWorkspace>("ws_test");
    //RooRealVar myy("myy", "myy", 125.f, 105.f, 160.f);
    //RooRealVar mH("mH", "mH", 125.09);
    //RooRealVar width("width", "width", 2);


    //RooGaussian pdf_bkg("bkg_pdf", "bkg_pdf", myy, mH, width);


    EFT_PROF_INFO("Create pdfs for sig and bkg...");
    ws->factory("Gaussian::pdf_sig(myy[125, 105, 160], mH[125.09], width[2])");
    ws->factory("Exponential::pdf_bkg(myy, bkg_slope[-0.01,-10,0])");
    EFT_PROF_INFO("Create pdfs for sig and bkg DONE");
    ws->Print("");
    //s->factory("expr::bkg('mu*s_nom',mu[1,-5,5],s_nom[50])") ;

    EFT_PROF_INFO("Create lumi block..");
    ws->factory( "lumi_nom[5000.0, 4000.0, 6000.0]" );
    ws->factory( "lumi_kappa[1.045]" );
    ws->factory( "cexpr::alpha_lumi('pow(lumi_kappa,beta_lumi)',lumi_kappa,beta_lumi[0,-5,5])" );
    ws->factory( "prod::lumi(lumi_nom,alpha_lumi)" );
    ws->factory( "Gaussian::constr_lumi(beta_lumi,glob_lumi[0,-5,5],1)" );
    EFT_PROF_INFO("Create lumi block DONE");

//    // lumi
//    EFT_PROF_INFO("Create lumi block..");
//    EFT_PROF_INFO("lumi_nom[5000.0, 4000.0, 6000.0] try");
//    ws->factory( "lumi_nom[5000.0, 4000.0, 6000.0]" );
//    //EFT_PROF_INFO("lumi_nom[5000.0, 4000.0, 6000.0] DONE:"); ws->Print("");
//
//    EFT_PROF_INFO("lumi_kappa[1.045] try");
//    ws->factory( "lumi_kappa[1.045]" );
//
//    EFT_PROF_INFO("cexpr::alpha_lumi('pow(lumi_kappa,beta_lumi)',lumi_kappa,beta_lumi[0,-5,5]) try");
//    ws->factory( "cexpr::alpha_lumi('pow(lumi_kappa,beta_lumi)',lumi_kappa,beta_lumi[0,-5,5])" );
//
//    EFT_PROF_INFO("PROD::lumi(lumi_nom,alpha_lumi) try");
//    ws->factory( "PROD::lumi(lumi_nom,alpha_lumi)" );
//
//    EFT_PROF_INFO("Gaussian::constr_lumi(beta_lumi,glob_lumi[0,-5,5],1) try");
//    ws->factory( "Gaussian::constr_lumi(beta_lumi,glob_lumi[0,-5,5],1)" );
//    EFT_PROF_INFO("Create lumi block DONE");
//    ws->Print("");

    // efficience
    EFT_PROF_INFO("Create efficiency block..");
    ws->factory( "efficiency_nom[0.1, 0.05, 0.15]" );
    ws->factory( "efficiency_kappa[1.10]" );
    ws->factory( "cexpr::alpha_efficiency('pow(efficiency_kappa,beta_efficiency)',efficiency_kappa,beta_efficiency[0,-5,5])" );
    ws->factory( "prod::efficiency(efficiency_nom,alpha_efficiency)" );
    ws->factory( "Gaussian::constr_efficiency(beta_efficiency,glob_efficiency[0,-5,5],1)" );
    EFT_PROF_INFO("Create efficiency block DONE");
    ws->Print("");


    // signal
    EFT_PROF_INFO("Create signal term");
    ws->factory("expr::S('mu*s_nom*lumi',mu[1,-5,5],s_nom[50], lumi)") ;
    EFT_PROF_INFO("Create signal term DONE");
    ws->Print("");
    // (s + b) model
    EFT_PROF_INFO("Create (s+b) model");
    EFT_PROF_INFO("PROD::model_sig(S, pdf_sig)");
    ws->factory("PROD::model_sig(S, pdf_sig)");

    EFT_PROF_INFO("PROD::model_bkg(n_bkg[10, 1000], pdf_bkg)");
    ws->factory("PROD::model_bkg(n_bkg[10, 1000], pdf_bkg)");

    EFT_PROF_INFO("SUM::model_s_b(model_sig, model_bkg)");
    ws->factory("SUM::model_s_b(model_sig, model_bkg)");

    EFT_PROF_INFO("Create (s+b) model DONE");
    ws->Print("");

    EFT_PROF_INFO("Create full model");
    // full model: (s+b) * constrains
    ws->factory("PROD::model(model_s_b, constr_lumi, constr_efficiency)");
    EFT_PROF_INFO("Create full model done");
    ws->Print("");

    EFT_PROF_INFO("write to file");
    ws->writeToFile(filename.c_str(), true);
    EFT_PROF_INFO("DONE");
    return ws;


    //ws->factory("PROD::model_s_b_lumi(model_s_b, constr_lumi))");
    //ws->factory("PROD::sig(S)")
    //ws->factory("exper::lumi(lumi_nominal*nuissance_lumi)")

    /*
     *  ws->factory("expr::mean('sigma*nuisance_lumi*nuisance_acc+nuisance_b', \
                  sigma[0,100], nuisance_lumi[5.0,0.0,10.0],nuisance_acc[0.71,0,1],nuisance_b[41.7,0,100])");

    ws->factory("SUM::model(mean*PROD::constraints(Gaussian::constraint_b(nuisance_b,b0[41.7,0,100],4.6), \
               Gaussian::constraint_acc(nuisance_acc,acc0[0.71,0,1],0.09), \
               Gaussian::constraint_lumi(nuisance_lumi,lumi0[5.0,0,10],0.195) \
               ))");
     *
     *
     * */

    RooRealVar n_sig("n_sig", "n_sig", 10, -100, 100);
    RooRealVar n_bkg("n_bkg", "n_bkg", 10, -100, 100);
    RooRealVar mu("mu", "mu", 1, -5, 5);

    ws->factory("expr::mean('sigma*nuisance_lumi*nuisance_acc+nuisance_b',"
             "sigma[0,100], nuisance_lumi[5.0,0.0,10.0],nuisance_acc[0.71,0,1],nuisance_b[41.7,0,100])");

    // from:
    // https://twiki.cern.ch/twiki/bin/view/RooStats/RooStatsTutorialsAugust2012
    //EFT_PROF_DEBUG("Initiate factories...");
    //ws->factory("Exponential:bkg_pdf(x[0,10], a[-0.5,-1000,0])");
    //ws->factory("Gaussian:sig_pdf(x, mass[2], sigma[0.5])");
    //ws->factory("SUM:model(nsig[0,1000]*sig_pdf, nbkg[0,1000000]*bkg_pdf)");  // for extended model

    //EFT_PROF_DEBUG("create pdf and variable");
    //RooAbsPdf* pdf = ws->pdf("model");
    //RooRealVar* x = ws->var("x");  // the observable

    // generate the data (nsig = 30, nbkg=1000)
    //EFT_PROF_DEBUG("generate the data (nsig = 30, nbkg=1000)");
    //ws->var("nsig")->setVal(30);
    //ws->var("nbkg")->setVal(1000);
    // use fixed random numbers for reproducibility
    //RooRandom::randomGenerator()->SetSeed(111);
    //RooDataSet* data = pdf->generate( *x);  // will generate accordint to total S+B events
    //data->SetName("test_data");
    //EFT_PROF_DEBUG("import data");
    //ws->import(*data);

    ModelConfig mc("ModelConfig", ws);
    //mc.SetPdf(*pdf);
    //mc.SetParametersOfInterest(*ws->var("nsig"));
    //mc.SetObservables(*ws->var("x"));
    // define set of nuisance parameters
    //ws->defineSet("nuisParams","a,nbkg");
    //mc.SetNuisanceParameters(*ws->set("nuisParams"));

    // from:
    // https://twiki.cern.ch/twiki/bin/view/Main/LearningRoostats

//    ws->factory("expr::mean('sigma*nuisance_lumi*nuisance_acc+nuisance_b',"
//              "sigma[0,100], nuisance_lumi[5.0,0.0,10.0],nuisance_acc[0.71,0,1],nuisance_b[41.7,0,100])");
//
//    ws->factory("SUM::model(mean*PROD::constraints(Gaussian::constraint_b(nuisance_b,41.7,4.6),"
//                "Gaussian::constraint_acc(nuisance_acc,0.71,0.09),"
//                "Gaussian::constraint_lumi(nuisance_lumi,5.0,0.195)"
//                "))");



    ws->factory("expr::mean('sigma*nuisance_lumi*nuisance_acc+nuisance_b', \
                  sigma[0,100], nuisance_lumi[5.0,0.0,10.0],nuisance_acc[0.71,0,1],nuisance_b[41.7,0,100])");

    ws->factory("SUM::model(mean*PROD::constraints(Gaussian::constraint_b(nuisance_b,b0[41.7,0,100],4.6), \
               Gaussian::constraint_acc(nuisance_acc,acc0[0.71,0,1],0.09), \
               Gaussian::constraint_lumi(nuisance_lumi,lumi0[5.0,0,10],0.195) \
               ))");

    ws->pdf("model")->expectedEvents(ws->set("obs"));

    //ws->defineSet("obs",""); //observables
    ws->defineSet("poi","sigma"); //parameters of interest
    ws->defineSet("np","nuisance_b,nuisance_lumi,nuisance_acc"); //nuisance parameters


    RooArgSet* globalObs = ws->pdf("model")
            ->getParameters(RooArgSet(RooArgSet(
                                                *ws->set("obs"),
                                                *ws->set("np")),
                                                *ws->set("poi")
                                                )
                                                );


//    ws->factory("Gaussian::constraint_b(nuisance_b[41.7,0,100],41.7,4.6)"); //constrained b to be positive - "truncated gaussian"
//    ws->factory("Gaussian::constraint_acc(nuisance_acc[0.71,0,1],0.71,0.09)"); //constrained acc in range 0-1
//    ws->factory("Gaussian::constraint_lumi(nuisance_lumi[5.0,0.0,10.0],5.0,0.195)"); //constrained lumi from 0 to 10.0
//    ws->factory("prod::s(sigma[0,100],nuisance_lumi,nuisance_acc)"); //constructs a function
//    ws->factory("sum::mean(s,nuisance_b)"); //another function
//    ws->factory("Poisson::pois(n[61,0,100],mean)"); //a pdf (special function)
//    ws->factory("PROD::model(pois,constraint_b,constraint_lumi,constraint_acc)"); //another pdf

    //ws->var("n")->setVal(65);

    //define RooArgSets for convenience
    //ws->defineSet("obs","n"); //observables
    //ws->defineSet("poi","sigma"); //parameters of interest
    //ws->defineSet("np","nuisance_b,nuisance_lumi,nuisance_acc"); //nuisance parameters

//    auto pdf = ws->pdf("model");
//    auto n = ws->var("n");
//    RooDataSet* data = pdf->generate(*n);  // will generate accordint to total S+B events
//    data->SetName("test_data");
//    //EFT_PROF_DEBUG("import data");
//    ws->import(*data);

    RooDataSet data("data", "data", *ws->set("obs"));
    data.add(*ws->set("obs")); //actually add the data
    ws->import(data);

    RooFitResult* res = ws->pdf("model")->fitTo(data,
                                                RooFit::Constrain(*ws->set("np")),
                                                RooFit::GlobalObservables(*globalObs),
                                                RooFit::Minos(ws->set("poi")),
                                                RooFit::Save(),
                                                RooFit::Hesse(false),
                                                RooFit::PrintLevel(1));

    if(res->status()==0) {
        ws->var("sigma")->Print();
    } else {
        cout << "Likelihood maximization failed" << endl;
    }

    RooAbsReal* nll = ws->pdf("model")->createNLL(data);
    auto pll = dynamic_cast<RooProfileLL*>(nll->createProfile(*ws->set("poi")));

    RooFormulaVar p_mu("p_mu","p_{#mu} using asymptotic formulae","TMath::Prob(2*@0,1.)",RooArgList(*pll));

    TCanvas c("c", "c", 1200, 800);

    RooPlot* frame = ws->var("sigma")->frame(RooFit::Range(0.f,30.f));
    p_mu.plotOn(frame,RooFit::LineColor(kGreen));
    frame->Draw();

    TLine l;
    l.SetLineStyle(2);
    l.DrawLine(0,0.05,30,0.05);
    c.SaveAs("test_draw.png");
    //

    EFT_PROF_INFO("after setting up the model, pdf [model]:");
    ws->pdf("model")->Print("");
    EFT_PROF_INFO("after setting up the model, poi [sigma]:");
    ws->var("sigma")->Print("");
    EFT_PROF_INFO("after setting up the model, obs [n]:");
    ws->var("n")->Print("");
    EFT_PROF_INFO("after setting up the model, nps [nuisance_b,nuisance_lumi,nuisance_acc]:");
    ws->var("nuisance_b")->Print("");
    ws->var("nuisance_lumi")->Print("");
    ws->var("nuisance_acc")->Print("");

    mc.SetPdf(*ws->pdf("model"));
    mc.SetParametersOfInterest(*ws->var("sigma"));
    mc.SetObservables(*ws->var("n"));
    mc.SetNuisanceParameters("nuisance_b,nuisance_lumi,nuisance_acc");
    mc.SetGlobalObservables(*globalObs);
    //mc.SetPdf("model");
    //mc.SetObservables("n");
    //mc.SetParametersOfInterest("sigma");
    auto obs = ws->var("n");
    auto data_generated = ws->pdf("model")->generate(*obs, 1000);

    // RooDataSet data("data", "data", *ws->set("obs"));
    // data.add(*ws->set("obs")); //actually add the data
    //ws->import(*data_generated);

   /*
    _combWS->defineSet("Observables", Observables);
    _combWS->defineSet("globalObservables", globalObservables);
    _combWS->defineSet("POI", POI);

    _mConfig->SetPdf(*_combWS->pdf("CombinedPdf"));
    _mConfig->SetObservables(*_combWS->set("Observables"));
    _mConfig->SetParametersOfInterest(*_combWS->set("POI"));
    _mConfig->SetGlobalObservables(*_combWS->set("globalObservables"));
    */

    EFT_PROF_INFO("import mc", true);
    ws->import(mc);
    ws->writeToFile(filename.c_str(), true);
    EFT_PROF_DEBUG("ws is written to: {}", filename);
    return ws;
}

bool DeleteWS(const std::string& path) {
    return std::filesystem::remove(path);
}

void Finalise() {
    const string path {"__temp_ws_for_eftTests.root"};
    if ( ! DeleteWS(path) ) {
        EFT_PROF_CRITICAL("cannot delete ws: {}", path);
        throw std::runtime_error("deleting ws is not possible");
    }
    eft::stats::Logger::SetSilent();
}

void TestWSreading() {
    eft::stats::Logger::SetFullPrinting();
    const string path {"__temp_ws_for_eftTests.root"};
    const string ws_name {"ws_test"};
    auto ws_ = std::make_shared<WorkspaceWrapper>();
    //auto ws_ = std::make_unique<WorkspaceWrapper>();
    ASSERT(ws_.get());
    ASSERT(std::filesystem::exists(path));

    ASSERT_NO_THROW(ws_->SetWS(path, ws_name));
    ASSERT(ws_->SetWS(path, ws_name));
    ASSERT_NO_THROW(ws_->raw()->var("n")->getVal());
    ASSERT_EQUAL(ws_->raw()->var("n")->getVal(), 65);
    // try to get Model Config
    ASSERT_NO_THROW(ws_->raw()->obj("ModelConfig"));
    ASSERT_NO_THROW(ws_->SetModelConfig("ModelConfig"));
    EFT_PROF_INFO("model config is set");
    ws_->raw()->Print("v");
    EFT_PROF_INFO("pois:");
    ws_->GetPOIs()->Print();
    EFT_PROF_INFO("nps:");
    ws_->GetNp()->Print();
    EFT_PROF_INFO("obs:");
    ws_->GetObs()->Print();
    //EFT_PROF_INFO("pdf:");
    //ws_->GetCombinedPdf("model");
    //EFT_PROF_INFO("data:");
    //ws_->GetData("data");
}

void Initiate() {
    eft::stats::Logger::SetFullPrinting();
    const string filename = fmt::format("__temp_ws_for_eftTests.root");
    CreateWS(filename);
}

EFT_IMPLEMENT_TESTFILE(WorkSpaceWrapper) {
    EFT_ADD_TEST(Initiate,      "WorkSpaceWrapper")
    EFT_ADD_TEST(TestWSreading, "WorkSpaceWrapper");
    EFT_ADD_TEST(Finalise,      "WorkSpaceWrapper");

    //Finalise(filename);
}
EFT_END_IMPLEMENT_TESTFILE(WorkSpaceWrapper);

//}