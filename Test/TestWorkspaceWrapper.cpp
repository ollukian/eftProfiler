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
    EFT_PROF_DEBUG("Initiate factories...");
    ws->factory("Exponential:bkg_pdf(x[0,10], a[-0.5,-1000,0])");
    ws->factory("Gaussian:sig_pdf(x, mass[2], sigma[0.5])");
    ws->factory("SUM:model(nsig[0,1000]*sig_pdf, nbkg[0,1000000]*bkg_pdf)");  // for extended model

    EFT_PROF_DEBUG("create pdf and variable");
    RooAbsPdf* pdf = ws->pdf("model");
    RooRealVar* x = ws->var("x");  // the observable

    // generate the data (nsig = 30, nbkg=1000)
    EFT_PROF_DEBUG("generate the data (nsig = 30, nbkg=1000)");
    ws->var("nsig")->setVal(30);
    ws->var("nbkg")->setVal(1000);
    // use fixed random numbers for reproducibility
    RooRandom::randomGenerator()->SetSeed(111);
    RooDataSet* data = pdf->generate( *x);  // will generate accordint to total S+B events
    data->SetName("test_data");
    EFT_PROF_DEBUG("import data");
    ws->import(*data);

    ModelConfig mc("ModelConfig", ws);
    mc.SetPdf(*pdf);
    mc.SetParametersOfInterest(*ws->var("nsig"));
    mc.SetObservables(*ws->var("x"));
    // define set of nuisance parameters
    ws->defineSet("nuisParams","a,nbkg");

    mc.SetNuisanceParameters(*ws->set("nuisParams"));

    ws->import(mc);
    ws->writeToFile(filename.c_str(), true);
    EFT_PROF_DEBUG("ws is written to: {}", filename);
    EFT_PROF_DEBUG("ls -tlhr");
    std::system("ls -ltrh");
    return ws;
   // auto data = ws.
}

bool DeleteWS(const std::string& path) {
    return std::filesystem::remove(path);
}

void Finalise(const std::string& filename) {
    if ( ! DeleteWS(filename) ) {
        EFT_PROF_CRITICAL("cannot delete ws: {}", filename);
        throw std::runtime_error("deleting ws is not possible");
    }
}

void TestWSreading() {
    const string path {"__temp_ws_for_eftTests.root"};
    const string ws_name {"ws_test"};
    auto ws_ = std::make_unique<WorkspaceWrapper>();
    ASSERT(ws_.get());

    std::system("ls -lthr");
    ASSERT(std::filesystem::exists(path));

    ASSERT_NO_THROW(ws_->SetWS(path, ws_name));
    ASSERT(ws_->SetWS("__temp_ws_for_eftTests", "ws_test"));
    ASSERT_NO_THROW(ws_->raw()->var("nsig")->getVal());
    ASSERT_NO_THROW(ws_->raw()->var("nbkg")->getVal());
    ASSERT_EQUAL(ws_->raw()->var("nsig")->getVal(), 30);
    ASSERT_EQUAL(ws_->raw()->var("nbkg")->getVal(), 1000);
}

EFT_IMPLEMENT_TESTFILE(WorkSpaceWrapper) {
    eft::stats::Logger::SetFullPrinting();
    const string filename = fmt::format("__temp_ws_for_eftTests.root");
    CreateWS(filename);
    EFT_PROF_DEBUG("after createws({})", filename);
    std::system("ls -lthr");
    EFT_PROF_DEBUG("try launching: {}", "TestWSreading");
    TestWSreading();

    EFT_ADD_TEST(TestWSreading, "WorkSpaceWrapper");

    //Finalise(filename);
    eft::stats::Logger::SetSilent();
}
EFT_END_IMPLEMENT_TESTFILE(WorkSpaceWrapper);

//}