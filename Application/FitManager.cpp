//
// Created by Aleksei Lukianchuk on 14-Sep-22.
//

#include "FitManager.h"
#include "../Fitter/IFitter.h"
#include "../Fitter/Fitter.h"
#include "../Utils/FitUtils.h"

#include "../Core/Logger.h"

#include "NpRankingStudyRes.h"

#include <fstream>
#include <iomanip>
#include "CommandLineArgs.h"
#include "FitManagerConfig.h"
#include "RooStats/AsymptoticCalculator.h"

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
    {
        auto* nps = GetListAsArgSet("paired_nps");
        EFT_PROF_TRACE("[ComputeNpRanking] worker: {} save snapshot tmp_nps", workerId);
        ws_->raw()->saveSnapshot("tmp_nps", *nps);
    }
    EFT_PROF_TRACE("[ComputeNpRanking] worker: {}", workerId);
    EFT_PROF_INFO("[ComputeNpRanking] worker: {} do unconditional fit", workerId);
    EFT_PROF_INFO("[ComputeNpRanking] {} before uncond fit: {} +- {}",
                  settings.poi,
                  ws()->GetParVal(settings.poi),
                  ws()->GetParErr(settings.poi)
    );
    DoFitAllNpFloat(settings);

    {
        EFT_PROF_DEBUG("[ComputeNpRanking] worker: {} load snapshot tmp_nps after free fit", workerId);
        EFT_PROF_DEBUG("[ComputeNpRanking] nps before loading:");
        GetListAsArgSet("paired_nps")->Print("v");
        ws_->raw()->loadSnapshot("tmp_nps");
        EFT_PROF_DEBUG("[ComputeNpRanking] nps after loading:");
        GetListAsArgSet("paired_nps")->Print("v");
    }

    EFT_PROF_INFO("[ComputeNpRanking] worker: {} unconditional fit is done, fit required np", workerId);
    EFT_PROF_INFO("[ComputeNpRanking] {} after uncond fit: {} +- {}",
                  settings.poi,
                  ws()->GetParVal(settings.poi),
                  ws()->GetParErr(settings.poi)
                  );
    //SetUpGlobObs(settings.prePostFit);
    RooAbsData& data = GetData(settings.prePostFit);
    //auto pdf = GetPdf("pdf_total");
    RooAbsPdf*  pdf = funcs_["pdf_total"];
    //auto* globObs = (args_["globObs"]);

    auto* globObs = GetListAsArgSet("paired_globs");
    auto* nps = GetListAsArgSet("paired_nps");

    /*auto* globObs_list = (lists_["paired_globs"]);
    auto* nps_list     = (lists_["paired_nps"]);

    auto* globObs = new RooArgSet();
    for (const auto glob : *globObs_list) { globObs->add(*glob); }

    auto* nps = new RooArgSet();
    for (const auto np : *nps_list) { nps->add(*np); }*/

    /*if (settings.studyType == StudyType::EXPECTED) {
        assert(data_["asimov_full"]);
        data = data_["asimov_full"];
    }
    else {
        assert(data_["ds_total"]);
        data = data_["ds_total"];
    }*/

    NpRankingStudyRes res;
    res.poi_name = settings.poi;
    res.statType = settings.statType;
    res.studyType = settings.studyType;
    res.prePostFit = settings.prePostFit;
    res.np_name = nps->operator[](workerId)->GetName();

    //EFT_PROF_INFO("[ComputeNpRanking], set all globs to nps");
    //SetGlobsToNPs();

    //EFT_PROF_INFO("[ComputeNpRanking] worker: {}, set all np float", workerId);
    //SetAllNuisanceParamsFloat();
    EFT_PROF_INFO("[ComputeNpRanking] worker: {}, set all globs to zero", workerId);
    SetAllGlobObsTo(0);

    EFT_PROF_INFO("[ComputeNpRanking] worker: {}, set all POIs const", workerId);
    SetAllPOIsConst();
    EFT_PROF_INFO("[ComputeNpRanking] worker: {}, float single POI: {}", workerId, res.poi_name);
    ws_->FloatVal(res.poi_name);
    ws_->SetVarVal(res.poi_name, 0.f);
    ws_->SetVarErr(res.poi_name,  0.f);
    //ws_->SetVarVal(res.poi_name, -1.14955385407254);
    //ws_->SetVarErr(res.poi_name, 0.669981);
    //ws_->FloatVal(res.poi_name);

    fit::Fitter fitter;

    /*EFT_PROF_INFO("[ComputeNpRanking] compute free fit values and errors on all nps");
    fit::Fitter fitter;
    {
        EFT_PROF_INFO("[ComputeNpRanking] create Nll for free fit");
        auto nll = fitter.CreatNll(data, pdf, globObs, args_[ "np" ]);
        EFT_PROF_INFO("[ComputeNpRanking] print nps before free fit:");
        args_["np"]->Print("v");
        EFT_PROF_INFO("[ComputeNpRanking] minimize nll for free fit");
        auto fitRes = fitter.Minimize(nll, pdf);
        EFT_PROF_INFO("[ComputeNpRanking] print nps after free fit:");
        args_["np"]->Print("v");
    }*/

    EFT_PROF_INFO("[ComputeNpRanking] worker: {}, Fix single np: {} const", workerId, res.np_name);
    ws_->FixValConst(res.np_name);

    EFT_PROF_INFO("[ComputeNpRanking] create nll with np: {} fixed", res.np_name);
    auto nll = fitter.CreatNll(&data, pdf, globObs, nps);
    EFT_PROF_INFO("[ComputeNpRanking] minimize nll with {} fixed", res.np_name);
    auto fitRes = fitter.Minimize(nll, pdf);
    EFT_PROF_INFO("[ComputeNpRanking] minimization nll with {} fixed is DONE", res.np_name);

    // TODO: create:
    //  * [] create: print results
    //  * [] create: prepare nps, globs and so on
    res.ExtractPoiValErr(ws_, res.poi_name);
    res.ExtractNPValErr(ws_, res.np_name);
    res.nll     = nll->getVal();

    EFT_PROF_INFO("[ComputeNpRanking] after fixed np fit, poi: {} +- {}", res.poi_val, res.poi_err);
    EFT_PROF_INFO("[ComputeNpRanking] after fixed np fit, nll: {}", res.nll);

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
}

void FitManager::DoFitAllNpFloat(NpRankingStudySettings settings)
{
    EFT_PROF_TRACE("[DoFitAllNpFloat]");
    SetAllGlobObsTo(0); // to find values for np preferred by data
    EFT_PROF_INFO("[DoFitAllNpFloat] all global observables set to zero");
    //SetAllNuisanceParamsFloat();
    //SetAllNuisanceParamsToValue(0);
    //EFT_PROF_INFO("[DoFitAllNpFloat] all nuisance parameters let to float and set to zero");
   RooAbsData* data = data_["ds_total"];
   RooAbsPdf*  pdf = funcs_["pdf_total"];

    //auto* data = GetData("ds_total");
    //auto* pdf  = GetPdf("pdf_total");
    auto* globObs = GetListAsArgSet("paired_globs");
    auto* nps = GetListAsArgSet("paired_nps");

//    EFT_PROF_WARN("[DoFitAllNpFloat] fit to");
//    pdf->fitTo(*data,
//               RooFit::Constrain(*args_["np"]),
//               RooFit::CloneData(false),
//               RooFit::GlobalObservables(*globObs),
//               RooFit::PrintLevel(1),
//               RooFit::Offset(true),
//               RooFit::BatchMode(true),
//               RooFit::Timer(true));
//    //pdf->fitTo(*data, *globObs, );
//    EFT_PROF_WARN("[DoFitAllNpFloat] after fit to");
//    return;

    NpRankingStudyRes res;
    res.poi_name = settings.poi;
    res.statType = StatType::FULL;
    res.studyType = StudyType::NOTDEF;
    res.prePostFit = PrePostFit::PREFIT;

    //EFT_PROF_INFO("[DoFitAllNpFloat], set all globs to nps");
    //SetGlobsToNPs();

    EFT_PROF_INFO("[DoFitAllNpFloat], set all POIs const");
    SetAllPOIsConst();
    EFT_PROF_INFO("[DoFitAllNpFloat], float single POI: {}", res.poi_name);
    ws_->FloatVal(res.poi_name);

    fit::Fitter fitter;

    EFT_PROF_INFO("[DoFitAllNpFloat] compute free fit values and errors on all nps");
    EFT_PROF_INFO("[DoFitAllNpFloat] create Nll for free fit");
    auto nll = fitter.CreatNll(data, pdf, globObs, nps);
    //EFT_PROF_INFO("[DoFitAllNpFloat] print nps before free fit:");
    //args_["np"]->Print("v");
    //EFT_PROF_INFO("[DoFitAllNpFloat] minimize nll for free fit");
    auto fitRes = fitter.Minimize(nll, pdf);
    //EFT_PROF_INFO("[DoFitAllNpFloat] print nps after free fit:");
    //args_["np"]->Print("v");

    res.ExtractPoiValErr(ws_, res.poi_name);
    res.nll = nll->getVal();

    EFT_PROF_INFO("[ComputeNpRanking] after fixed np fit, poi: {} +- {}", res.poi_val, res.poi_err);
    EFT_PROF_INFO("[ComputeNpRanking] after fixed np fit, nll: {}", res.nll);

    nlohmann::json j;
    j = res;

    const string name = fmt::format("/pbs/home/o/ollukian/public/EFT/git/eftProfiler/res_no_constrains__{}_{}.json",
                                    res.poi_name,
                                    res.np_name);

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
}

void FitManager::SetAllNuisanceParamsConst() noexcept
{

    if (args_["np"]->empty())
        ExtractNP();

    EFT_PROF_TRACE("[SetAllNuissConst]");

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

    //args_["np"]->Print("v");
    for (const auto& np : *args_["np"]) {
        const string name = {np->GetTitle()};
        EFT_PROF_DEBUG("Set {} to const", name);
        dynamic_cast<RooRealVar *>(np)->setConstant(true);
    }

    //cout << "status after:" << endl;
    //args_["np"]->Print("v");
}

void FitManager::SetAllNuisanceParamsFloat() noexcept {

    if (args_["np"]->empty())
        ExtractNP();

    EFT_PROF_TRACE("[SetAllNuissFloat]");

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

    //args_["np"]->Print("v");
    for (const auto& np : *args_["np"]) {
        const string name = {np->GetTitle()};
        EFT_PROF_DEBUG("Set {} to float", name);
        dynamic_cast<RooRealVar *>(np)->setConstant(false);
    }

    //cout << "status after:" << endl;
    //args_["np"]->Print("v");
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

    EFT_PROF_INFO("[FitManager] extract NPs");
    ExtractNP();
    EFT_PROF_INFO("[FitManager] extract obs");
    ExtractObs();
    EFT_PROF_INFO("[FitManager] extract global obs");
    ExtractGlobObs();
    EFT_PROF_INFO("[FitManager] extract cats");
    ExtractCats();
    EFT_PROF_INFO("[FitManager] extract POIs");
    ExtractPOIs();

    EFT_PROF_INFO("[FitManager] extract pdf total: {}", config.comb_pdf);
    ExtractPdfTotal(config.comb_pdf);
    EFT_PROF_INFO("[FitManager] extract data total: {}", config.comb_data);
    ExtractDataTotal( config.comb_data);

    EFT_PROF_INFO("[FitManager] get constrains");
    auto pairConstr = FitUtils::GetPairConstraints(funcs_["pdf_total"], args_["np_all"], args_["globObs"], args_["obs"]);
    EFT_PROF_INFO("[FitManager] print obtained constrains");
    EFT_PROF_INFO("[FitManager] paired_constr_pdf {}:", pairConstr.paired_constr_pdf->size());
    EFT_PROF_INFO("[FitManager] paired_globs {}:",      pairConstr.paired_globs->size());
    EFT_PROF_INFO("[FitManager] paired_nps {}:",        pairConstr.paired_nps->size());

    //auto paired_globs = new RooArgSet();
    //auto paired_nps = new RooArgSet();

    lists_[ "paired_globs" ] = pairConstr.paired_globs;
    lists_[ "paired_nps"   ] = pairConstr.paired_nps;

//    for (const auto& pdf : *pairConstr.paired_constr_pdf)
//    {
//        pdf->Print();
//    }
//
//    EFT_PROF_INFO("[FitManager] paired_globs {}:", pairConstr.paired_globs->size());
//    for (const auto& pdf : *pairConstr.paired_globs)
//    {
//        pdf->Print();
//    }
//
//    EFT_PROF_INFO("[FitManager] paired_nps {}:", pairConstr.paired_nps->size());
//    for (const auto& pdf : *pairConstr.paired_nps)
//    {
//        pdf->Print();
//    }

    //throw std::runtime_error("enough ;)");

   /* cout << setfill('*') << setw(45) << "" << endl;
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

    cout << setfill(' ');*/
    EFT_PROF_INFO("[FitManager] INIT DONE");
}

void FitManager::ReadConfigFromCommandLine(CommandLineArgs& commandLineArgs, FitManagerConfig& config) noexcept
{
    EFT_PROF_DEBUG("[FitManager] Read Configuration from Command Line");

#ifndef EFT_SET_VAL_IF_EXISTS
#define EFT_SET_VAL_IF_EXISTS(args, config, param)                                  \
    if (args.SetValIfArgExists(#param, config.param)) {                             \
        EFT_PROF_INFO("[FitManager] Set param: {:15} to {}", #param, config.param); \
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
    EFT_SET_VAL_IF_EXISTS(commandLineArgs, config, study_type);

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

void FitManager::CreateAsimovData(PrePostFit studyType) noexcept
{
    EFT_PROF_TRACE("[FitManager]{AsimovData}");
    assert(studyType != PrePostFit::OBSERVED);

    if (studyType == PrePostFit::PREFIT && data_.find("asimov_prefit") == data_.end()) {
        EFT_PROF_INFO("[FitManager]{AsimovData} return cached asimov_prefit");
        return;
    }
    if (studyType == PrePostFit::POSTFIT && data_.find("asimov_postfit") == data_.end()) {
        EFT_PROF_INFO("[FitManager]{AsimovData} return cached asimov_postfit");
        return;
    }

    SetUpGlobObs(studyType);

    string ds_name;
    if (studyType == PrePostFit::POSTFIT)
        ds_name = "asimov_postfit";
    else
        ds_name = "asimov_prefit";

    EFT_PROF_INFO("[FitManager]{AsimovData} compute asimov ds: {}", ds_name);
    data_[std::move(ds_name)] =
            (RooDataSet*)
            RooStats::AsymptoticCalculator::MakeAsimovData(*data_["ds_total"],
                                                           ws_->GetModelConfig(),
                                                           *args_["pois"],
                                                           *args_["globObs"]
    );
};

RooArgSet* FitManager::GetListAsArgSet(const std::string& name) const
{
    EFT_PROF_TRACE("FitManager::GetListAsArgSet for {}", name);
    auto* res = new RooArgSet();

    if (lists_.find(name) == lists_.end())
    {
        EFT_PROF_WARN("FitManager::GetListAsArgSet no list: {} is available", name);
        string error_message = fmt::format("FitManager::GetListAsArgSet no list: {name} is available,"
                                           "Use one of the {} followings:", lists_.size());

        for (const auto& list : lists_) error_message += "[" + list.first + "], ";

        throw std::out_of_range(error_message);
    }
    EFT_PROF_DEBUG("FitManager::GetListAsArgSet for {} - found");
    const auto* list = lists_.at(name);
    for (const auto elem : *list) {
        res->add(*elem);
    }
    return res;
}



} // stats