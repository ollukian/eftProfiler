//
// Created by Aleksei Lukianchuk on 23-Nov-22.
//

#include "Tester.h"
#include "../Utils/ColourUtils.h"
#include "../Core/WorkspaceWrapper.h"
#include "RooRandom.h"


using namespace eft::utils;
using namespace eft::stats;
using namespace std;

//namespace eft::inner::tests {

RooWorkspace * CreateWS(const string& filename)
{

    using RooStats::ModelConfig;
    using WS = RooWorkspace;

    auto ws = new RooWorkspace("ws_test");
    //auto ws = make_unique<RooWorkspace>("ws_test");

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
    ws->factory("Gaussian::constraint_b(nuisance_b[41.7,0,100],41.7,4.6)"); //constrained b to be positive - "truncated gaussian"
    ws->factory("Gaussian::constraint_acc(nuisance_acc[0.71,0,1],0.71,0.09)"); //constrained acc in range 0-1
    ws->factory("Gaussian::constraint_lumi(nuisance_lumi[5.0,0.0,10.0],5.0,0.195)"); //constrained lumi from 0 to 10.0
    ws->factory("prod::s(sigma[0,100],nuisance_lumi,nuisance_acc)"); //constructs a function
    ws->factory("sum::mean(s,nuisance_b)"); //another function
    ws->factory("Poisson::pois(n[61,0,100],mean)"); //a pdf (special function)
    ws->factory("PROD::model(pois,constraint_b,constraint_lumi,constraint_acc)"); //another pdf

    ws->var("n")->setVal(65);

    //define RooArgSets for convenience
    ws->defineSet("obs","n"); //observables
    ws->defineSet("poi","sigma"); //parameters of interest
    ws->defineSet("np","nuisance_b,nuisance_lumi,nuisance_acc"); //nuisance parameters

    auto pdf = ws->pdf("model");
    auto n = ws->var("n");
    RooDataSet* data = pdf->generate(*n);  // will generate accordint to total S+B events
    data->SetName("test_data");
    //EFT_PROF_DEBUG("import data");
    ws->import(*data);

    //
    mc.SetNuisanceParameters("nuisance_b,nuisance_lumi,nuisance_acc");
    mc.SetPdf("model");
    mc.SetObservables("n");
    mc.SetParametersOfInterest("sigma");
    mc.SetProtoData("test_data");

    // RooDataSet data("data", "data", *ws->set("obs"));
    // data.add(*ws->set("obs")); //actually add the data

   /*
    _combWS->defineSet("Observables", Observables);
    _combWS->defineSet("globalObservables", globalObservables);
    _combWS->defineSet("POI", POI);

    _mConfig->SetPdf(*_combWS->pdf("CombinedPdf"));
    _mConfig->SetObservables(*_combWS->set("Observables"));
    _mConfig->SetParametersOfInterest(*_combWS->set("POI"));
    _mConfig->SetGlobalObservables(*_combWS->set("globalObservables"));
    */


    ws->import(mc);
    ws->writeToFile(filename.c_str(), true);
    EFT_PROF_DEBUG("ws is written to: {}", filename);
    return ws;
   // auto data = ws.
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
}

void TestWSreading() {
    const string path {"__temp_ws_for_eftTests.root"};
    const string ws_name {"ws_test"};
    auto ws_ = std::make_unique<WorkspaceWrapper>();
    ASSERT(ws_.get());
    ASSERT(std::filesystem::exists(path));

    ASSERT_NO_THROW(ws_->SetWS(path, ws_name));
    ASSERT(ws_->SetWS(path, ws_name));
    ASSERT_NO_THROW(ws_->raw()->var("n")->getVal());
    ASSERT_EQUAL(ws_->raw()->var("n")->getVal(), 65);
    // try to get Model Config
    ASSERT_NO_THROW(ws_->raw()->obj("ModelConfig"));
    ASSERT_NO_THROW(ws_->SetModelConfig("ModelConfig"));
    ws_->raw()->Print("v");
    EFT_PROF_INFO("pois:");
    ws_->GetPOIs()->Print();
    EFT_PROF_INFO("nps:");
    ws_->GetNp()->Print();
    EFT_PROF_INFO("obs:");
    ws_->GetObs()->Print();
    EFT_PROF_INFO("pdf:");
    ws_->GetCombinedPdf("model");
    EFT_PROF_INFO("data:");
    ws_->GetData("test_data");
}

EFT_IMPLEMENT_TESTFILE(WorkSpaceWrapper) {
    eft::stats::Logger::SetFullPrinting();
    const string filename = fmt::format("__temp_ws_for_eftTests.root");
    CreateWS(filename);
    TestWSreading();

    EFT_ADD_TEST(TestWSreading, "WorkSpaceWrapper");
    EFT_ADD_TEST(Finalise,      "WorkSpaceWrapper");

    //Finalise(filename);
    eft::stats::Logger::SetSilent();
}
EFT_END_IMPLEMENT_TESTFILE(WorkSpaceWrapper);

//}