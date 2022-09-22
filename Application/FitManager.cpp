//
// Created by Aleksei Lukianchuk on 14-Sep-22.
//

#include "FitManager.h"
#include "../Fitter/IFitter.h"
#include "../Fitter/Fitter.h"

#include "../Core/Logger.h"

#include "NpRankingStudyRes.h"

#include <fstream>
#include <iomanip>
#include "CommandLineArgs.h"
#include "FitManagerConfig.h"

using namespace std;

namespace eft::stats {

void FitManager::DoGlobalFit()
{
    cout << "[DoGlobalFit]" << endl;
    auto* pdf = funcs_["pdf_total"];
    auto* ds = data_["ds_total"];
    auto* globObs = (args_["globObs"]);
    auto* np = (args_["np"]);

    cout << "print pdf:" << endl;
    pdf->Print("V");

    cout << "print ds:" << endl;
    ds->Print("V");

    cout << "print global obs:" << endl;
    globObs->Print("V");

    cout << "[create nll]" << endl;

    fit::Fitter fitter;
    auto nll = fitter.CreatNll(ds, pdf, globObs, np);
    cout << "[minimize it]" << endl;
    auto res = fitter.Minimize(nll, pdf);
    cout << "[minimisation done]" << endl;
    cout << "res: " << endl;
    res->Print("v");
}

void FitManager::ComputeNpRankingOneWorker(NpRankingStudySettings settings, size_t workerId)
{
    EFT_PROF_TRACE("[ComputeNpRanking] worker: {}", workerId);
    RooAbsData* data;
    RooAbsPdf*  pdf = funcs_["pdf_total"];
    auto* globObs = (args_["globObs"]);

    if (settings.studyType == StudyType::EXPECTED) {
        assert(data_["asimov_full"]);
        data = data_["asimov_full"];
    }
    else {
        assert(data_["ds_total"]);
        data = data_["ds_total"];
    }

    NpRankingStudyRes res;
    res.poi_name = settings.poi;
    res.statType = settings.statType;
    res.studyType = settings.studyType;
    res.np_name = args_["np"]->operator[](workerId)->GetName();


    EFT_PROF_INFO("[ComputeNpRanking] worker: {}, set all np float", workerId);
    SetAllNuisanceParamsFloat();

    EFT_PROF_INFO("[ComputeNpRanking] worker: {}, set all POIs float", workerId);
    SetAllPOIsConst();
    EFT_PROF_INFO("[ComputeNpRanking] worker: {}, float POI: {}", workerId, res.poi_name);
    ws_->FloatVal(res.poi_name);

    EFT_PROF_INFO("[ComputeNpRanking] compute free fit values and errors on all nps");
    fit::Fitter fitter;
    {
        EFT_PROF_INFO("[ComputeNpRanking] create Nll for free fit");
        auto nll = fitter.CreatNll(data, pdf, globObs, args_[ "np" ]);
        EFT_PROF_INFO("[ComputeNpRanking] create Nll for free fit DONE");
        EFT_PROF_INFO("[ComputeNpRanking] print nps before fit:");
        args_["np"]->Print("v");
        EFT_PROF_INFO("[ComputeNpRanking] minimize nll");
        auto fitRes = fitter.Minimize(nll, pdf);
        EFT_PROF_INFO("[ComputeNpRanking] print nps after fit:");
        args_["np"]->Print("v");
    }

    EFT_PROF_INFO("[ComputeNpRanking] worker: {}, Fix np: {} const", workerId, res.np_name);
    ws_->FixValConst(res.np_name);

    EFT_PROF_INFO("[ComputeNpRanking] create nll with np: {} fixed", res.np_name);
    auto nll = fitter.CreatNll(data, pdf, globObs, args_["np"]);
    EFT_PROF_INFO("[ComputeNpRanking] minimize nll with {} fixed", res.np_name);
    auto fitRes = fitter.Minimize(nll, pdf);
    EFT_PROF_INFO("[ComputeNpRanking] minimization nll with {} fixed is DONE", res.np_name);
    res.poi_err = ws_->GetParErr(res.poi_name);
    res.poi_val = ws_->GetParVal(res.poi_name);
    res.nll     = nll->getVal();
    res.np_err  = ws_->GetParErr(res.np_name);
    res.np_val  = ws_->GetParVal(res.np_name);

    res.statType = StatType::NP_RANKING;
    res.prePostFit = PrePostFit::POSTFIT;
    res.studyType = StudyType::OBSERVED;

//#ifndef EFT_STRUCT_TO_JSON
//#define EFT_STRUCT_TO_JSON(j, res, field) j[#field] = res.field;
//#endif

    nlohmann::json j;
    j = res;

    const string name = fmt::format("/pbs/home/o/ollukian/public/EFT/git/eftProfiler/res_{}_{}_worker_{}.json",
                                    res.poi_name,
                                    res.np_name,
                                    workerId);

    ofstream f_res;
    f_res.exceptions(ofstream::failbit | ofstream::badbit);

    try {
        f_res.open(name);
        f_res << setw(4) << j << endl;
        cout << "duplicate to the console:" << endl;
        cout << setw(4) << j << endl;
    }
    catch (...) {
        cout << "impossible to open: " << name << endl;
        cout << "print to console:" << endl;
        cout << setw(4) << j << endl;
    }

    //EFT_STRUCT_TO_JSON(j, res, nll);

    if (settings.prePostFit == PrePostFit::POSTFIT) {
        // TODO: set np to the values found in fit;
    }

    // * take np # worker_id
    // * create nll
    // * minimise it
    // * fill in results
    // * write them down

    //settings.poi

}

void FitManager::SetAllNuisanceParamsConst() noexcept
{

    if (args_["np"]->empty())
        ExtractNP();

    cout << "[SetAllNuissConst]" << endl;
    cout << "status before:" << endl;

//    args_["np"]->Print("v");
//    for (const auto& np : *args_["np"]) {
//        const string name = {np->GetTitle()};
//        cout << fmt::format("dealing with: {} ...", name) << endl;
//        //if (string(dynamic_cast<RooRealVar*>(np)->GetTitle()).substr(0, 5) == "ATLAS")
//        if (name.substr(0, 5) == "ATLAS") {
//            cout << fmt::format("dealing with: {} Set to const", name) << endl;
//            dynamic_cast<RooRealVar *>(np)->setConstant(true);
//        }
//        else {
//            cout << fmt::format("dealing with: {} DO NOT set to const", name) << endl;
//        }
//    }

    args_["np"]->Print("v");
    for (const auto& np : *args_["np"]) {
        const string name = {np->GetTitle()};
        cout << fmt::format("Set {} to const", name) << endl;
        dynamic_cast<RooRealVar *>(np)->setConstant(true);
    }

    cout << "status after:" << endl;
    args_["np"]->Print("v");
}

void FitManager::SetAllNuisanceParamsFloat() noexcept {

    if (args_["np"]->empty())
        ExtractNP();

    EFT_PROF_TRACE("[SetAllNuissFloat]");
    cout << "status before:" << endl;

//    args_["np"]->Print("");
//    for (const auto& np : *args_["np"]) {
//        const string name = {np->GetTitle()};
//        cout << fmt::format("dealing with: {} ...", name) << endl;
//        //if (string(dynamic_cast<RooRealVar*>(np)->GetTitle()).substr(0, 5) == "ATLAS")
//        if (name.substr(0, 5) == "ATLAS") {
//            cout << fmt::format("dealing with: {:40} OK", name) << endl;
//            dynamic_cast<RooRealVar *>(np)->setConstant(false);
//        }
//        else {
//            cout << fmt::format("dealing with: {:40} DO NOT set to float", name) << endl;
//        }
//    }

    args_["np"]->Print("v");
    for (const auto& np : *args_["np"]) {
        const string name = {np->GetTitle()};
        cout << fmt::format("Set {} to float", name) << endl;
        dynamic_cast<RooRealVar *>(np)->setConstant(false);
    }

    cout << "status after:" << endl;
    args_["np"]->Print("v");
}

void FitManager::ExtractPOIs() noexcept
{
    assert(ws_ != nullptr);
    args_["pois"] = (RooArgSet *) ws_->GetPOIs();
    // create list of pois in string format
    pois_.reserve(args_["pois"]->size());
    cout << fmt::format("[FitManager] Extracted {} POIs to args[pois]", args_["pois"]->size());
    cout << fmt::format("[FitManager] create a list of POIs in string format...");
    for (const auto& poi : *args_["pois"]) {
        string name = {poi->GetTitle()};
        pois_.push_back(std::move(name));
    }
    cout << fmt::format("[FitManager] list of POIs in string format:");
    for (const auto& poi : pois_) { cout << "\t" << poi << endl; }
}

void FitManager::Init(FitManagerConfig&& config)
{
    EFT_PROF_INFO("[FitManager] init from config: path to ws: {}, name: {},model_config: {}",
                  config.ws_path, config.ws_name, config.model_config);
    SetWsWrapper();
    SetWS(std::move(config.ws_path),
          std::move(config.ws_name)
          );
    SetModelConfig(std::move(config.model_config));

    ExtractNP();
    cout << "[INFO] extract obs" << endl;
    ExtractObs();
    cout << "[INFO] extract global obs" << endl;
    ExtractGlobObs();
    cout << "[INFO] extract cats" << endl;
    ExtractCats();
    cout << "[INFO] extract pois" << endl;
    ExtractPOIs();

    cout << "[INFO] extract pdf total" << endl;
    ExtractPdfTotal("combPdf");
    cout << "[INFO] extract data total" << endl;
    ExtractDataTotal("combData");

    cout << setfill('*') << setw(45) << "" << endl;
    cout << setw(20) << "" << setw(15) << " global obs: " << setw(10) << "" << endl;
    cout << setw(45) << "" << endl;
    cout << setfill(' ');
    GetArgsClosure().at("globObs")->Print("v");
    cout << setfill('*') << setw(45) << "" << endl;

    cout << setw(20) << "" << setw(15) << " obs: " << setw(10) << "" << endl;
    cout << setw(45) << "" << endl;
    cout << setfill(' ');
    GetArgsClosure().at("obs")->Print("v");
    cout << setfill('*') << setw(45) << "" << endl;

    cout << setw(20) << "" << setw(15) << " All Np " << setw(10) << "" << endl;
    cout << setw(45) << "" << endl;
    cout << setfill(' ');
    GetArgsClosure().at("np_all")->Print("v");
    cout << setfill('*') << setw(45) << "" << endl;

    cout << setw(20) << "" << setw(15) << " real Np " << setw(10) << "" << endl;
    cout << setw(45) << "" << endl;
    cout << setfill(' ');
    GetArgsClosure().at("np")->Print("v");
    cout << setfill('*') << setw(45) << "" << endl;

    cout << setw(20) << "" << setw(15) << " dataComb " << setw(10) << "" << endl;
    cout << setw(45) << "" << endl;
    cout << setfill(' ');
    GetDataClosure().at("ds_total")->Print("v");
    cout << setfill('*') << setw(45) << "" << endl;

    cout << setw(20) << "" << setw(15) << " pdfComb " << setw(10) << "" << endl;
    cout << setw(45) << "" << endl;
    cout << setfill(' ');
    GetFuncClosure().at("pdf_total")->Print("v");
    cout << setfill('*') << setw(45) << "" << endl;

    cout << setfill(' ');
}

void FitManager::ReadConfigFromCommandLine(CommandLineArgs& commandLineArgs, FitManagerConfig& config) noexcept
{
    EFT_PROF_DEBUG("[FitManager] Read Configuration from Command Line");

#ifndef EFT_SET_VAL_IF_EXISTS
#define EFT_SET_VAL_IF_EXISTS(args, config, param)        \
    if (args.SetValIfArgExists(#param, config.param)) { \
        EFT_PROF_INFO("[FitManager] Set param: {}", config.param);     \
     }

#endif

    EFT_SET_VAL_IF_EXISTS(commandLineArgs, config, res_path);
    EFT_SET_VAL_IF_EXISTS(commandLineArgs, config, worker_id);
    EFT_SET_VAL_IF_EXISTS(commandLineArgs, config, poi);
    EFT_SET_VAL_IF_EXISTS(commandLineArgs, config, ws_path);
    EFT_SET_VAL_IF_EXISTS(commandLineArgs, config, ws_name);
    EFT_SET_VAL_IF_EXISTS(commandLineArgs, config, model_config);
    EFT_SET_VAL_IF_EXISTS(commandLineArgs, config, comb_pdf);
    EFT_SET_VAL_IF_EXISTS(commandLineArgs, config, comb_data);
    EFT_SET_VAL_IF_EXISTS(commandLineArgs, config, nb_pois_to_plot);
    EFT_SET_VAL_IF_EXISTS(commandLineArgs, config, fit_precision);

#undef EFT_SET_VAL_IF_EXISTS

//    if (commandLineArgs.SetValIfArgExists("res_path", config.res_path)) {
//        EFT_PROF_INFO("Set res_path: {}", config.res_path);
//    }
//
//    if (commandLineArgs.SetValIfArgExists("worker_id", config.worker_id)) {
//        EFT_PROF_INFO("Set worker_id: {}", config.res_path);
//    }
//
//    if (commandLineArgs.SetValIfArgExists("poi", config.poi)) {
//        EFT_PROF_INFO("Set poi: {}", config.poi);
//    }
//
//    if (commandLineArgs.SetValIfArgExists("ws_path", config.ws_path)) {
//        EFT_PROF_INFO("Set ws_path: {}", config.ws_path);
//    }
//
//    if (commandLineArgs.SetValIfArgExists("ws_name", config.ws_name)) {
//        EFT_PROF_INFO("Set ws_name: {}", config.ws_name);
//    }
}

//    inline void FitManager::SetGlobalObservablesToValueFoundInFit() {
//        for (auto& obs : *globalObservables_) {
//            cout << " # set global obs: |" << obs->GetName() << "|";
//
//            string name = string(obs->GetName());
//            name = name.substr(string("RNDM__").size(), name.size() );
//
//            double foundValue = ws_->var( name.c_str() )->getVal();
//            static_cast<RooRealVar*>( obs )->setVal( foundValue  );
//
//            cout << " to " << foundValue << endl;
//        }
//    }

} // stats