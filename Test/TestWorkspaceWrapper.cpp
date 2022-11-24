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

namespace eft::utils::internal {
    static const set<string> _test_globs_names_ {
            "glob_lumi",
            "glob_efficiency",
            "glob_nbkg"
    };

    static const set<string> _test_np_names_ {
            "beta_lumi",
            "beta_efficiency",
            "beta_nbkg"
    };

    static const set<string> _test_fixed_names_ {
            "lumi_nom",
            "efficiency_nom",
            "nbkg_nom",
            "lumi_kappa",
            "efficiency_kappa",
            "nbkg_kappa"
    };

    static const set<string> _test_poi_names_ {
            "xsec"
    };
}

//namespace eft::inner::tests {

void CreateWS(const string& filename)
{

    using RooStats::ModelConfig;
    using WS = RooWorkspace;

    auto ws = new RooWorkspace("ws_test");
    //auto ws = new RooWorkspace();
    //auto ws = make_shared<RooWorkspace>("ws_test");
    //RooRealVar myy("myy", "myy", 125.f, 105.f, 160.f);
    //RooRealVar mH("mH", "mH", 125.09);
    //RooRealVar width("width", "width", 2);

    // WS creating is inspired by:
    // https://indico.in2p3.fr/event/6315/contributions/37810/attachments/30394/37380/sos-lista-app.pdf

    //RooGaussian pdf_bkg("bkg_pdf", "bkg_pdf", myy, mH, width);


    /*
    EFT_PROF_INFO("Create pdfs for sig and bkg...");
    ws->factory("Gaussian::pdf_sig(myy[125, 105, 160], mH[125.09], width[2])");
    ws->factory("Exponential::pdf_bkg(myy, bkg_slope[-0.01,-10,0])");
    EFT_PROF_INFO("Create pdfs for sig and bkg DONE");
    ws->Print("");
    //s->factory("expr::bkg('mu*s_nom',mu[1,-5,5],s_nom[50])") ;
    */

    Logger::SetLevel(spdlog::level::level_enum::info);
    EFT_PROF_INFO("Create lumi block..");
    ws->factory( "lumi_nom[5000.0, 4000.0, 6000.0]" );
    ws->factory( "lumi_kappa[1.045]" );
    ws->factory( "cexpr::alpha_lumi('pow(lumi_kappa,beta_lumi)',lumi_kappa,beta_lumi[0,-5,5])" );
    ws->factory( "prod::lumi(lumi_nom,alpha_lumi)" );
    ws->factory( "Gaussian::constr_lumi(beta_lumi,glob_lumi[0,-5,5],1)" );
    //EFT_PROF_INFO("Create lumi block DONE");

    // efficience
    EFT_PROF_INFO("Create efficiency block..");
    ws->factory( "efficiency_nom[0.1, 0.05, 0.15]" );
    ws->factory( "efficiency_kappa[1.10]" );
    ws->factory( "cexpr::alpha_efficiency('pow(efficiency_kappa,beta_efficiency)',efficiency_kappa,beta_efficiency[0,-5,5])" );
    ws->factory( "prod::efficiency(efficiency_nom,alpha_efficiency)" );
    ws->factory( "Gaussian::constr_efficiency(beta_efficiency,glob_efficiency[0,-5,5],1)" );
    //EFT_PROF_INFO("Create efficiency block DONE");
    //ws->Print("");

    // bkg with syst
    EFT_PROF_INFO("Create background block..");
    ws->factory( "nbkg_nom[10.0, 5.0, 15.0]" );
    ws->factory( "nbkg_kappa[1.10]" );
    ws->factory( "cexpr::alpha_nbkg('pow(nbkg_kappa,beta_nbkg)',nbkg_kappa,beta_nbkg[0,-5,5])" );
    ws->factory( "prod::nbkg(nbkg_nom,alpha_lumi,alpha_nbkg)" );
    ws->factory( "Gaussian::constr_nbkg(beta_nbkg,glob_nbkg[0,-5,5],1)" );


    // cross section - parameter of interest
    ws->factory( "xsec[0,0,0.1]" );
    // selection efficiency * acceptance
    // signal yield
    ws->factory( "prod::nsig(lumi,xsec,efficiency)" );
    //ws->factory( "nbkg_nom[10]" );
    //ws->factory( "prod::nbkg(nbkg_nom,alpha_lumi)" );

    ws->factory( "n[0]" );
    // signal yield
    // background yield
    // full event yield
    ws->factory( "sum::yield(nsig,nbkg)" );
    ws->factory( "Uniform::prior(xsec)" );
    // NOTE: lower-case "sum" create a function. Upper-case "SUM" would create a PDF
    // Core model: Poisson probability with mean signal+bkg
    ws->factory( "Poisson::model_core(n,yield)" );

    //EFT_PROF_INFO("core model (+ lumi):");
    //ws->Print();
    EFT_PROF_INFO("Create final model: Poisson core + lumi + efficienty + bkg systematics");
    ws->factory( "PROD::model(model_core,constr_lumi,constr_efficiency, constr_nbkg)" );
    //EFT_PROF_INFO("Full model");
    //ws->Print();

    RooRealVar* n = ws->var("n");
    RooArgSet obs("observables");
    obs.add(*n);

    n->setVal(11);
    //auto data = new RooDataSet("data", "data", obs);
    //data->add( *n );
    //ws->import(*data, RooFit::Silence());

    // define glob obs

    using namespace eft::utils::internal;
    
    RooArgSet globalObs("global_obs");
    for (const auto& glob_name : _test_globs_names_) {
        ws->var(glob_name.c_str())->setConstant(true);
        globalObs.add( *ws->var(glob_name.c_str()) );
    }

    RooArgSet poi("poi");
    for (const auto& poi_name : _test_poi_names_) {
        poi.add(*ws->var(poi_name.c_str()));
    }
    //poi.add( *ws->var("xsec") );

    RooArgSet nuis("nuis");
    for (const auto& np_name : _test_np_names_) {
        nuis.add(*ws->var(np_name.c_str()));
    }

    RooArgSet fixed("fixed");
    for (const auto& fixed_name : _test_fixed_names_) {
        ws->var(fixed_name.c_str())->setConstant(true);
        fixed.add(*ws->var(fixed_name.c_str()));
    }

//    ws->var("glob_lumi")->setConstant(true);
//    ws->var("glob_efficiency")->setConstant(true);
//    ws->var("glob_nbkg")->setConstant(true);
//    globalObs.add( *ws->var("glob_lumi") );
//    globalObs.add( *ws->var("glob_efficiency") );
//    globalObs.add( *ws->var("glob_nbkg") );
    // create set of parameters of interest (POI)


    // create set of nuisance parameters



    //nuis.add( *ws->var("beta_lumi") );
    //nuis.add( *ws->var("beta_efficiency") );
    //nuis.add( *ws->var("beta_nbkg") );

    // fix all other variables in model:
    // everything except observables, POI, and
    //nuisance parameters
    // must be constant



//    ws->var("lumi_nom")->setConstant(true);
//    ws->var("efficiency_nom")->setConstant(true);
//    ws->var("nbkg_nom")->setConstant(true);
//    ws->var("lumi_kappa")->setConstant(true);
//    ws->var("efficiency_kappa")->setConstant(true);
//    ws->var("nbkg_kappa")->setConstant(true);

//    fixed.add( *ws->var("lumi_nom") );
//    fixed.add( *ws->var("efficiency_nom") );
//    fixed.add( *ws->var("nbkg_nom") );
//    fixed.add( *ws->var("lumi_kappa") );
//    fixed.add( *ws->var("efficiency_kappa") );
//    fixed.add( *ws->var("nbkg_kappa") );
    // create signal+background Model Config
    RooStats::ModelConfig modelConfig("ModelConfig");
    modelConfig.SetWorkspace( *ws );
    modelConfig.SetPdf( *ws->pdf("model") );
    modelConfig.SetObservables( obs );
    modelConfig.SetGlobalObservables( globalObs );
    modelConfig.SetParametersOfInterest( poi );
    modelConfig.SetNuisanceParameters( nuis );
    // this is optional, for Bayesian analysis
    modelConfig.SetPriorPdf( *ws->pdf("prior") );
    // import ModelConfig into workspace
    ws->import( modelConfig );

    //EFT_PROF_INFO("print all vars:");

    for (const auto container : {&_test_globs_names_,
                                 &_test_np_names_,
                                 &_test_fixed_names_,
                                 &_test_poi_names_})
    {
        //EFT_PROF_INFO("change container");
        for (const auto& elem_name : *container) {
            //ws->var(elem_name.c_str())->Print();
        }
    }


    //EFT_PROF_INFO("create nll");
    //RooAbsReal* nll = ws->pdf("model")->createNLL(*data);

    //EFT_PROF_INFO("write to file:");
    //ws->Print();
    ws->writeToFile(filename.c_str(), true);
    //gDirectory->Add(ws);
    EFT_PROF_INFO("Creation is done, the next steps should take less time");
    Logger::SetSilent();
    return;// ws;
#if 0
    // signal
    EFT_PROF_INFO("Create signal term");
    ws->factory("expr::S('mu*s_nom*lumi',mu[1,-5,5],s_nom[50], lumi)") ;
    EFT_PROF_INFO("Create signal term DONE");
    ws->Print("");
    // (s + b) model
    EFT_PROF_INFO("Create (s+b) model");
    EFT_PROF_INFO("prod::model_sig(S, pdf_sig)");
    ws->factory("prod::model_sig(S, pdf_sig)");
    ws->Print("");

    EFT_PROF_INFO("prod::model_bkg(n_bkg[10, 1000], pdf_bkg)");
    ws->factory("prod::model_bkg(n_bkg[10, 1000], pdf_bkg)");
    ws->Print("");

    EFT_PROF_INFO("sum::model_s_b(model_sig, model_bkg)");
    ws->factory("sum::model_s_b(model_sig, model_bkg)");
    ws->Print("");

    EFT_PROF_INFO("Create (s+b) model DONE");
    ws->Print("");

    EFT_PROF_INFO("Create full model");
    // full model: (s+b) * constrains
    ws->factory("prod::model(model_s_b, constr_lumi, constr_efficiency)");
    EFT_PROF_INFO("Create full model done");
    ws->Print("");

    EFT_PROF_INFO("extract mu");
    RooRealVar* mu = ws->var("mu");
    RooArgSet obs("observables");
    EFT_PROF_INFO("add mu to obs");
    obs.add(*mu);

    mu->setVal(1);
    EFT_PROF_INFO("set mu to 1");
    RooDataSet * data = new RooDataSet("data", "data", obs);
    EFT_PROF_INFO("add mu to data");
    data->add( *mu );
    EFT_PROF_INFO("import data");
    ws->import(*data);

    EFT_PROF_INFO("create nll");
    RooAbsReal* nll = ws->pdf("model")->createNLL(*data);

    EFT_PROF_INFO("write to file");
    ws->writeToFile(filename.c_str(), true);
    EFT_PROF_INFO("DONE");

    return ws;
#endif

#if 0
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

    //RooRealVar n_sig("n_sig", "n_sig", 10, -100, 100);
    //RooRealVar n_bkg("n_bkg", "n_bkg", 10, -100, 100);
    //RooRealVar mu("mu", "mu", 1, -5, 5);

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
    return;// ws;
#endif
}

[[nodiscard]]
//std::shared_ptr<WorkspaceWrapper> LoadWS() {
WorkspaceWrapper* LoadWS() {
    RooMsgService::instance().setGlobalKillBelow(RooFit::WARNING);
    static const string path {"__temp_ws_for_eftTests.root"};
    static const string ws_name {"ws_test"};
    auto ws_ = new WorkspaceWrapper();
    //auto ws_ = std::make_shared<WorkspaceWrapper>();
    ws_->SetWS(path, ws_name);
    ws_->SetModelConfig("ModelConfig");
    return ws_;
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
    const string path {"__temp_ws_for_eftTests.root"};
    const string ws_name {"ws_test"};
    auto ws_ = new WorkspaceWrapper();
    //auto ws_ = std::make_shared<WorkspaceWrapper>();
    ASSERT(ws_);
    ASSERT(std::filesystem::exists(path));

    bool is_set = false;
    ASSERT_NO_THROW(is_set = ws_->SetWS(path, ws_name));
    ASSERT(is_set);
    ASSERT_EQUAL(ws_->raw()->var("n")->getVal(), 11);
    ASSERT_NO_THROW(ws_->SetModelConfig("ModelConfig"));
    EFT_PROF_INFO("model config is set");

    const RooArgSet* pois  = ws_->GetPOIs();
    const RooArgSet* nps   = ws_->GetNp();
    const RooArgSet* globs = ws_->GetGlobObs();
    const RooArgSet* obs   = ws_->GetObs();
    const RooSimultaneous* pdf =  ws_->GetCombinedPdf("model");

    ASSERT(pois);
    ASSERT(nps);
    ASSERT(globs);
    ASSERT(obs);
    ASSERT(pdf);

    ASSERT_EQUAL(pois->getSize(),   1);
    ASSERT_EQUAL(nps->getSize(),    3);
    ASSERT_EQUAL(globs->getSize(),  3);
    ASSERT_EQUAL(obs->getSize(),    1);
}

void TestLoading() {
    EFT_PROF_INFO("start test loading");
    ASSERT_NO_THROW(std::ignore = LoadWS()); // std::ignore to prevent warnings about [[nodiscard]]
    ASSERT(LoadWS()->raw());
}

void TestWSGetters() {
    using namespace eft::utils::internal;
    auto ws = LoadWS();

    for (const string& glob_name : _test_globs_names_) {
        ASSERT(ws->GetVar(glob_name)->isConstant());
        ASSERT_EQUAL(ws->GetVar(glob_name)->getVal(), ws->GetParVal(glob_name));
        ASSERT_EQUAL(ws->GetVar(glob_name)->getError(), ws->GetParErr(glob_name));
    }

    for (const string& fixed_name : _test_fixed_names_) {
        ASSERT(ws->GetVar(fixed_name)->isConstant());
        ASSERT_EQUAL(ws->GetVar(fixed_name)->getVal(), ws->GetParVal(fixed_name));
        ASSERT_EQUAL(ws->GetVar(fixed_name)->getError(), ws->GetParErr(fixed_name));
    }

    for (const string& poi_name : _test_poi_names_) {
        ASSERT(ws->GetVar(poi_name)); // check that exists
        ASSERT_EQUAL(ws->GetVar(poi_name)->getVal(), ws->GetParVal(poi_name));
        ASSERT_EQUAL(ws->GetVar(poi_name)->getError(), ws->GetParErr(poi_name));
    }

    for (const string& fixedNames : _test_fixed_names_) {
        ASSERT(ws->GetVar(fixedNames)); // check that exists
        ASSERT_EQUAL(ws->GetVar(fixedNames)->getVal(), ws->GetParVal(fixedNames));
        ASSERT_EQUAL(ws->GetVar(fixedNames)->getError(), ws->GetParErr(fixedNames));
    }

    ASSERT_NO_THROW(ws->GetModelConfig());
    ASSERT_EQUAL(ws->GetModelConfig().GetName(), "ModelConfig");
}

void TestWSSetters() {
    using namespace eft::utils::internal;
    auto ws = LoadWS();

    const string poi_name = *_test_poi_names_.begin();
    const string one_np_name = *_test_np_names_.begin();
    { // set const
        ASSERT_NOT(ws->GetVar(poi_name)->isConstant());
        ws->FixValConst(poi_name);
        ASSERT(ws->GetVar(poi_name)->isConstant());
    }
    { // set values and errors of a value
        ASSERT_NOT(ws->GetVar(one_np_name)->isConstant());
        ws->SetVarVal(one_np_name, 1.2);
        ws->SetVarErr(one_np_name, 3.4);
        ASSERT_NOT(ws->GetVar(one_np_name)->isConstant());
        ASSERT_EQUAL(ws->GetVar(one_np_name)->getVal(), 1.2);
        ASSERT_EQUAL(ws->GetVar(one_np_name)->getError(), 3.4);
        ASSERT_EQUAL(ws->GetParVal(one_np_name), 1.2);
        ASSERT_EQUAL(ws->GetParErr(one_np_name), 3.4);
    }
    {
        ASSERT_NOT(ws->GetVar(one_np_name)->isConstant());
        ws->FixValConst(one_np_name);
        ASSERT(ws->GetVar(one_np_name)->isConstant());
    }
}

void Initiate() {
    EFT_PROF_WARN("{} {} {}",
                  "Running tests of the Workspace wrapper. First stage may take some non-negligible time"
                  "due to creation of a toy RooWorkspace, requiring compiling of functions for a few test",
                  "systematics: luminosity and efficiency");
    RooMsgService::instance().setGlobalKillBelow(RooFit::MsgLevel::FATAL);
    const string filename = fmt::format("__temp_ws_for_eftTests.root");
    CreateWS(filename);
}

EFT_IMPLEMENT_TESTFILE(WorkSpaceWrapper) {
    EFT_ADD_TEST(Initiate,      "WorkSpaceWrapper")
    EFT_ADD_TEST(TestWSreading, "WorkSpaceWrapper");
    EFT_ADD_TEST(TestLoading,   "WorkSpaceWrapper");
    EFT_ADD_TEST(TestWSGetters,   "WorkSpaceWrapper");
    EFT_ADD_TEST(TestWSSetters,   "WorkSpaceWrapper");
    EFT_ADD_TEST(Finalise,      "WorkSpaceWrapper");
}
EFT_END_IMPLEMENT_TESTFILE(WorkSpaceWrapper);

//}
