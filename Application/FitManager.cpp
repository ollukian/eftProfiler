//
// Created by Aleksei Lukianchuk on 14-Sep-22.
//

#include "FitManager.h"
#include "../Fitter/IFitter.h"
#include "../Fitter/Fitter.h"
#include "../Utils/FitUtils.h"
#include "../Utils/StringUtils.h"
#include "../Core/Logger.h"
#include "Profiler.h"
#include "../Test/test_runner.h"

#include "Scene.h"
#include "TH2D.h"

#include "../Vendors/spdlog/fmt/fmt.h"
#include "spdlog/fmt/ostr.h"

#include "NpRankingStudyRes.h"
#include "Ranking/OneNpManager.h"

#include "FitSettings.h"

#include <fstream>
#include <iomanip>

#include "CommandLineArgs.h"
#include "FitManagerConfig.h"
#include "RooStats/AsymptoticCalculator.h"

using namespace std;

namespace eft::stats {

void FitManager::DoGlobalFit()
{

//    ranking::OneNpManager npraning = eft::stats::ranking::OneNpManager::create()
//            .ForNP()
//            .ForPOI()
//            .UsingGlobalObservables()
//            .UsingData()
//            .UsingNPs()
//            .UsingPdf();

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
    fitter.SetGlobs(globObs);
    fitter.SetNps(np);
    fit::FitSettings settings;
    settings.data = ds;
    settings.pdf = pdf;
    settings.nps = args_["pois"];
    auto res = fitter.Fit(settings);
    //auto nll = fitter.CreatNll(ds, pdf, globObs, np);
    //cout << "[minimize it]" << endl;
    //auto res = fitter.Minimize(nll, pdf);
    cout << "[minimisation done]" << endl;
    cout << "res: " << endl;
    res->Print("v");
}

void FitManager::ComputeNpRankingOneWorker(const NpRankingStudySettings& settings, size_t workerId) {
    EFT_PROFILE_FN();
    NpRankingStudyRes res;
    res.poi_name = settings.poi;
    res.statType = settings.statType;
    res.studyType = settings.studyType;
    res.prePostFit = settings.prePostFit;
    auto *globObs = GetListAsArgSet("paired_globs");
    auto *nps = GetListAsArgSet("paired_nps"); // TODO: refactor to get nps
    auto *non_gamma_nps = GetListAsArgSet("non_gamma_nps");

    if (settings.np_name.empty()) { // if np_name to be deduced from worker_id
        EFT_PROF_WARN("Name of Nuissance parameter is not set directly, it will be deduced from worker_id");
        if (settings.no_gamma) {
            res.np_name = non_gamma_nps->operator[](workerId)->GetName();
            EFT_PROF_INFO("No-gamma option ==> choose NP number from the non-gamma list. Deal with {} ==> {}",
                          workerId,
                          res.np_name);
        } else {
            res.np_name = nps->operator[](workerId)->GetName();
            EFT_PROF_INFO("All nps, including gamma ==> choose NP number from the full list. Deal with {} ==> {}",
                          workerId,
                          res.np_name);
        }
    }
    else {
        EFT_PROF_WARN("Name of Nuissance parameter is directly set");
        res.np_name = settings.np_name;

        workerId = 0;
        bool is_found = false;

        RooArgSet* nps_to_use = nullptr;
        if (settings.no_gamma) {
            EFT_PROF_WARN("Choose np number from the NON-GAMMA list");
            nps_to_use = non_gamma_nps;
        }
        else {
            EFT_PROF_WARN("Choose np number from the FULL list (including gamma-nps)");
            nps_to_use = nps;
        }

        while (nps_to_use->operator[](workerId)->GetName() != res.np_name) {
            workerId++;
            is_found = true;
        }
        if (is_found) {
            EFT_PROF_INFO("Corresponded worker id: {}", workerId);
        }
        else {
            EFT_PROF_INFO("Didn't find the np: {} in the np list", res.np_name);
            throw std::runtime_error("");
        }
    }
    //res.np_name = nps->operator[](workerId)->GetName();
    {
        if (settings.prePostFit != PrePostFit::POSTFIT) {
            EFT_PROF_INFO("For study type set pois to init values and globs to 0");
            SetAllPoisTo(settings.poi_init_val, 0);
            SetAllGlobObsTo(0, 0); // to find values for np preferred by data
        }
        else {
            EFT_PROF_INFO("For study type [potsfit] no need to set pois to init values and globs to 0");
        }
        auto args = new RooArgSet();
        args->add(*globObs);
        args->add(*nps);
        EFT_PROF_TRACE("[ComputeNpRanking] worker: {} save snapshot tmp_nps with globs and obs", workerId);
        ws_->raw()->saveSnapshot("tmp_nps", *args, true);
    }
    eft::stats::Logger::SetFullPrinting();
    EFT_PROF_TRACE("[ComputeNpRanking] worker: {}", workerId);
    EFT_PROF_INFO("[ComputeNpRanking] worker: {} do unconditional fit", workerId);
    EFT_PROF_INFO("[ComputeNpRanking] {} before uncond fit: {} +- {}",
                  settings.poi,
                  ws()->GetParVal(settings.poi),
                  ws()->GetParErr(settings.poi)
    );

    EFT_PROF_DEBUG("Nps:");
    for (const auto np : *nps) {
        auto np_var = dynamic_cast<RooRealVar*>(np);
        EFT_PROF_TRACE("{:40}, {} +- {}, const => {}",
                       np_var->GetName(),
                       np_var->getVal(),
                       np_var->getError(),
                       np_var->isConstant());
    }

    RooAbsData& data = GetData(settings.prePostFit);
    auto pdf = GetPdf("pdf_total");

    EFT_PROF_DEBUG("create np manager");
    ranking::OneNpManager npManager = ranking::OneNpManager::create()
            .UsingPdf(const_cast<RooAbsPdf *>(pdf))
            .UsingData(&data)
            .UsingGlobalObservables(globObs)
            .UsingNPs(nps)
            .ForNP(res.np_name)
            .UsingErrors(settings.errors)
            .UsingWS(ws_)
            .UsingSnapshotWithInitVals("tmp_nps")
            .ForPOI(res.poi_name)
            .UsingPOIs(const_cast<RooArgSet*>(ws_->GetPOIs()))
            //.UsingPOIs(new RooArgSet(*ws()->GetVar(res.poi_name)))
            .UsingFitSettings(settings);

    npManager.SetPoiPreferredValue(settings.poi_init_val, 0.);

    npManager.RunFreeFit();

    const auto np_val_free = ws()->GetParVal(res.np_name);
    const auto np_err_free = ws()->GetParErr(res.np_name);
    //res.poi_free_fit_val = ws()->GetParVal(res.poi_name);
    //res.poi_free_fit_err = ws()->GetParErr(res.poi_name);

    res.np_val = np_val_free;
    res.np_err = np_err_free;

    npManager.SetNpPreferredValue(np_val_free, np_err_free);

    npManager.RunFitFixingNpAtCentralValue();

    EFT_PROF_INFO("Now we're only interested in central values of the POI => set error type to default");
    npManager.SetErrors(fit::Errors::DEFAULT);
    npManager.RunPreFit('+');
    npManager.RunPreFit('-');
    npManager.RunPostFit('+');
    npManager.RunPostFit('-');

    // TODO: get NpRankingStudyRes directly from the NpManager
    res.poi_free_fit_val = npManager.GetResult("free_fit").poi_val;
    res.poi_free_fit_err = npManager.GetResult("free_fit").poi_err;

    res.poi_fixed_np_val = npManager.GetResult("fixed_np_fit").poi_val;
    res.poi_fixed_np_err = npManager.GetResult("fixed_np_fit").poi_err;

    res.poi_plus_one_variation_val = npManager.GetResult("prefit_+").poi_val;
    res.poi_plus_one_variation_err = npManager.GetResult("prefit_+").poi_err;

    res.poi_minus_one_variation_val = npManager.GetResult("prefit_-").poi_val;
    res.poi_minus_one_variation_err = npManager.GetResult("prefit_-").poi_err;

    res.poi_plus_sigma_variation_val = npManager.GetResult("postfit_+").poi_val;
    res.poi_plus_sigma_variation_err = npManager.GetResult("postfit_+").poi_err;

    res.poi_minus_sigma_variation_val = npManager.GetResult("postfit_-").poi_val;
    res.poi_minus_sigma_variation_err = npManager.GetResult("postfit_-").poi_err;


    EFT_PROF_INFO("[ComputeNpRanking] results:");
    EFT_PROF_INFO("+{:=^15}==={:=^15}===={:=^15}+", "=", "=", "=");
    EFT_PROF_INFO("|{:^15} | {:^15} | {:^15}|", "Study", "poi value", "poi error");
    EFT_PROF_INFO("+{:=^15}==={:=^15}===={:=^15}+", "=", "=", "=");
    EFT_PROF_INFO("|{:^15} | {:^3} +- {:^6}|", "free fit", res.poi_free_fit_val,              res.poi_free_fit_err);
    EFT_PROF_INFO("|{:^15} | {:^3} +- {:^6}|", "fixed np", res.poi_fixed_np_val,              res.poi_fixed_np_err);
    EFT_PROF_INFO("|{:^15} | {:^3} +- {:^6}|", "+1 sigma", res.poi_plus_sigma_variation_val,  res.poi_plus_sigma_variation_err);
    EFT_PROF_INFO("|{:^15} | {:^3} +- {:^6}|", "-1 sigma", res.poi_minus_sigma_variation_val, res.poi_minus_sigma_variation_err);
    EFT_PROF_INFO("|{:^15} | {:^3} +- {:^6}|", "+1 ",      res.poi_plus_one_variation_val,    res.poi_plus_sigma_variation_err);
    EFT_PROF_INFO("|{:^15} | {:^3} +- {:^6}|", "-1 ",      res.poi_minus_one_variation_val,   res.poi_minus_sigma_variation_err);
    EFT_PROF_INFO("+{:=^15}==={:=^15}===={:=^15}+", "=", "=", "=");
    EFT_PROF_INFO("|{:^15} | {:^15} | {:^15}|", "Study", "np value", "np error");
    EFT_PROF_INFO("|{:^15} | {:^3} +- {:^6}|", " * ", res.np_val, res.np_err);
    EFT_PROF_INFO("+{:=^15}==={:=^15}===={:=^15}+", "=", "=", "=");

    std::filesystem::path path_res = settings.path_to_save_res;

    if ( !std::filesystem::exists(path_res) ) {
        EFT_PROF_INFO("Required path directory {} needs to be created", path_res);
        std::filesystem::create_directories(path_res);
    }

    const string name = fmt::format("{}/res__{}__worker_{}__{}.json",
                                    path_res.string(),
                                    res.poi_name,
                                    workerId,
                                    res.np_name
                                    );

    nlohmann::json j;
    j = res;
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

void FitManager::DoFitAllNpFloat(const NpRankingStudySettings& settings)
{
    EFT_PROFILE_FN();
    EFT_PROF_TRACE("[DoFitAllNpFloat]");
    SetUpGlobObs(settings.prePostFit);
    //SetAllGlobObsTo(0, 0); // to find values for np preferred by data
    EFT_PROF_INFO("[DoFitAllNpFloat] all global observables set to zero");

    //RooAbsData* data = data_["ds_total"];
    //RooAbsPdf*  pdf = funcs_["pdf_total"];

    auto& data = GetData(settings.prePostFit);
    auto* pdf  = GetPdf("pdf_total");
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
    res.prePostFit = settings.prePostFit;

    //EFT_PROF_INFO("[DoFitAllNpFloat], set all globs to nps");
    //SetGlobsToNPs();

    EFT_PROF_INFO("[DoFitAllNpFloat], set all POIs const");
    SetAllPOIsConst();
    // TODO: float all pois, if needed by a flag
    EFT_PROF_INFO("[DoFitAllNpFloat], float single POI: {}", res.poi_name);
    ws_->FloatVal(res.poi_name);

    fit::Fitter fitter;
    fitter.SetNps(nps);
    fitter.SetGlobs(globObs);

    fit::FitSettings fitSettings;
    fitSettings.pdf = const_cast<RooAbsPdf*>(pdf);
    fitSettings.data = &data;
    fitSettings.pois = args_["pois"]; // TODO: wrap around by a function
    fitSettings.errors = settings.errors;
    fitSettings.retry = settings.retry;
    fitSettings.strategy = settings.strategy;
    fitSettings.eps = settings.eps;
    // TODO: use one set of settings...


    EFT_PROF_INFO("[DoFitAllNpFloat] compute free fit values and errors on all nps");
    EFT_PROF_INFO("[DoFitAllNpFloat] create Nll for free fit");
    fitter.Fit(fitSettings);
    EFT_PROF_WARN("[DoFitAllNpFloat] after fit");
    //auto fitRes = fitter.Fit(fitSettings_);
    //auto nll = fitter.CreatNll(fitSettings_);
    //auto fitRes = fitter.Minimize(fitSettings_);
    //EFT_PROF_INFO("[DoFitAllNpFloat] print nps after free fit:");
    //args_["np"]->Print("v");

    res.poi_free_fit_val = ws()->GetParVal(res.poi_name);
    res.poi_free_fit_err = ws()->GetParErr(res.poi_name);
    //res.ExtractPoiValErr(ws_, res.poi_name);
    //res.nll = fitSettings_.nll->getVal();

    EFT_PROF_INFO("[DoFitAllNpFloat] after free fit, poi: {} +- {}", res.poi_free_fit_val, res.poi_free_fit_err);
    EFT_PROF_INFO("[DoFitAllNpFloat] after free fit, nll: {}", res.nll);

    nlohmann::json j;
    j = res;

    //std::filesystem::path path_res = std::filesystem::current_path();
    std::filesystem::path path_res = settings.path_to_save_res;
    //if ( !settings.path_to_save_res.empty() )
    //    path_res /= settings.path_to_save_res;
    EFT_PROF_INFO("Save res to {}", path_res.string());


    if ( !std::filesystem::exists(path_res) ) {
        EFT_PROF_INFO("Required path directory {} needs to be created", path_res);
        std::filesystem::create_directories(path_res);
    }

    //const string name = fmt::format("/pbs/home/o/ollukian/public/EFT/git/eftProfiler/res_no_constrains__{}_{}.json",
    const string name = fmt::format("{}/res_no_constrains__{}_{}.json",
                                    path_res.string(),
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

//HesseStudyResult FitManager::ComputeHesseNps(const NpRankingStudySettings& settings) {
//    EFT_PROFILE_FN();
//    EFT_PROF_INFO("Compute Hesse forNPS");
//    auto& data = GetData(settings.prePostFit);
//    auto* pdf  = GetPdf("pdf_total");
//    auto* globObs = GetListAsArgSet("paired_globs");
//    auto* nps = GetListAsArgSet("paired_nps");
//
//
//    EFT_PROF_INFO("set all POIs const");
//    SetAllPOIsConst();
//
//    EFT_PROF_INFO("Float required poi: {}", settings.poi);
//    ws_->FloatVal(settings.poi);
//
//    // TODO: float all pois, if needed by a flag!!!!
//
//    fit::Fitter fitter;
//    fitter.SetNps(nps);
//    fitter.SetGlobs(globObs);
//
//    fit::FitSettings fitSettings_;
//    fitSettings_.pdf = const_cast<RooAbsPdf*>(pdf);
//    fitSettings_.data = &data;
//    fitSettings_.pois = args_["pois"]; // TODO: wrap around by a function
//    fitSettings_.errors = settings.errors;
//    fitSettings_.retry = settings.retry;
//    fitSettings_.strategy = settings.strategy;
//    fitSettings_.eps = settings.eps;
//    fitSettings_.save_res = true;
//    // TODO: use one set of settings...
//
//
//    EFT_PROF_INFO("[ComputeHesseNps] perform fit saving results");
//    auto fitRes = fitter.Fit(fitSettings_);
//    EFT_PROF_DEBUG("[ComputeHesseNps] create list of nps");
//    RooArgList list_nps(*nps);
//    list_nps.add(*ws_->GetVar(settings.poi));
//    EFT_PROF_DEBUG("[ComputeHesseNps] created list of nps (and POI) with {} entries:", list_nps.size());
//    for (const auto np : list_nps) {
//        cout << np->GetName() << endl;
//    }
//    EFT_PROF_DEBUG("[ComputeHesseNps] extract results from RooFitResult");
//    auto res = HesseStudyResult::ExtractFromRooFitResult(*fitRes, list_nps);
//    return res;
//}
//
//void FitManager::PlotCovariances(const HesseStudyResult& res) const
//{
//    EFT_PROFILE_FN();
//    using eft::utils::draw::Scene;
//
//    shared_ptr<TH2D> cov = make_shared<TH2D>(res.reducedCovMatrix);
//    EFT_PROF_INFO("Created cov matrix with size: [{}][{}]", res.reducedCovMatrix.GetNcols(), res.reducedCovMatrix.GetNrows());
//    auto canvas = make_shared<TCanvas>("c", "c", 4000, 4000);
//    //auto canvas = Scene::Create(4000, 4000);
//    //Scene::Register(cov.get());
//    for (size_t idx_np {0}; idx_np < res.reducedCovMatrix.GetNcols(); ++idx_np) {
//        EFT_PROF_DEBUG("set label of bin: {:4} to {}", idx_np, res.params.at(idx_np)->GetName());
//        cov->GetXaxis()->SetBinLabel(idx_np + 1, res.params.at(idx_np)->GetName());
//        cov->GetYaxis()->SetBinLabel(idx_np + 1, res.params.at(idx_np)->GetName());
//    }
//    cov->SetLabelSize(0.005);
//    cov->GetXaxis()->SetLabelSize(0.005);
//    cov->GetYaxis()->SetLabelSize(0.005);
//    EFT_PROF_INFO("All names are set, draw");
//    cov->Draw("colz");
//    EFT_PROF_INFO("drawn, save");
//    string name_covs = fmt::format("covariances_with_{}_float.pdf", res.poi);
//    canvas->SaveAs(name_covs.c_str());
//    EFT_PROF_INFO("clear");
//    canvas->Clear();
//    //Scene::SaveAs("covariances.pdf");
//
////    map<string, double> corr_per_np;
////    // get correlations between POI and other things
////    EFT_PROF_INFO("Extract correlations: poi <-> nps");
////    auto poi_var = ws_->GetVar(res.poi);
////    shared_ptr<TH1D> corr_with_poi = make_shared<TH1D>("h", "h", res.covariances.size(), 0, res.covariances.size());
////    for (size_t idx_np {0}; idx_np < res.reducedCovMatrix.GetNcols(); ++idx_np) {
////        auto par = res.params.at(idx_np);
////        auto corr = res.fitResult->correlation(*poi_var, *par);
////        corr_with_poi->SetBinContent(idx_np + 1, corr);
////        corr_with_poi->GetXaxis()->SetBinLabel(idx_np + 1, par->GetName());
////        EFT_PROF_DEBUG("correlations [poi] <-> {:40} ==> {}", par->GetName(), corr);
////        corr_per_np[par->GetName()] = corr;
////    }
//
//    shared_ptr<TH1D> corr_with_poi = make_shared<TH1D>("h", "h", res.covariances.size(), 0, res.covariances.size());
//    //for (const auto& [np, corr] : res.correlations_per_nb_np) {
//    for (size_t idx_np {0}; idx_np < res.correlations_per_nb_np.size(); ++idx_np) {
//        corr_with_poi->SetBinContent(idx_np + 1, res.correlations_per_nb_np.at(idx_np).second);
//        corr_with_poi->GetXaxis()->SetBinLabel(idx_np + 1, res.correlations_per_nb_np.at(idx_np).first.c_str());
//    }
//
//    corr_with_poi->SetLabelSize(0.005);
//    corr_with_poi->Draw("H");
//    string name_to_save = fmt::format("correlations_with_{}.pdf", res.poi);
//    canvas->SaveAs(name_to_save.c_str());
//    canvas->Clear();
//
//    //vector<pair<string, double>> sorted_corrs {corr_per_np.begin(), corr_per_np.end()};
//    vector<pair<string, double>> sorted_corrs {res.corr_per_np.begin(), res.corr_per_np.end()};
//    std::sort(sorted_corrs.begin(),
//              sorted_corrs.end(),
//              [](const auto& l, const auto& r) -> bool
//    {
//        return abs(l.second) > abs(r.second);
//    });
//
//    EFT_PROF_INFO("Sorted corrs with poi:");
//    for (const auto& [name, cor] : sorted_corrs) {
//        cout << fmt::format("{:60} => {}", name, cor) << endl;
//    }
//
//    EFT_PROF_INFO("Fill corr with poi sorted");
//    const string h_name = fmt::format("Correlations of NPs with {}", res.poi);
//    shared_ptr<TH1D> corr_with_poi_sorted = make_shared<TH1D>(h_name.c_str(),
//                                                              h_name.c_str(),
//                                                              res.covariances.size(),
//                                                              0,
//                                                              res.covariances.size());
//    EFT_PROF_DEBUG("{:3} ==> {:50} ==> {:10}", "idx", "name", "corr wit mu");
//    for (size_t idx {1}; idx < sorted_corrs.size(); ++idx) {
//    //for (const auto& [name, cor] : sorted_corrs) {
//        auto& name = sorted_corrs.at(idx).first;
//        auto& corr = sorted_corrs.at(idx).second;
//        EFT_PROF_DEBUG("{:3} ==> {:50} ==> {:10}", idx, name, corr);
//        corr_with_poi_sorted->SetBinContent(idx, corr);
//        corr_with_poi_sorted->GetXaxis()->SetBinLabel(idx, name.c_str());
//    }
//    corr_with_poi_sorted->SetLabelSize(0.005);
//    corr_with_poi_sorted->Draw("H");
//    string name_to_save_sorted = fmt::format("correlations_with_{}_sorted.pdf", res.poi);
//    canvas->SaveAs(name_to_save_sorted.c_str());
//    //canvas->SaveAs("correlations_with_poi_sorted.pdf");
//    canvas->Clear();
//
//    //Scene::SaveAs()
//}
//
//void FitManager::ExtractCorrelations(HesseStudyResult& res) const
//{
//    EFT_PROFILE_FN();
//    map<string, double> corr_per_np;
//    std::vector<std::pair<std::string, double>> correlations_per_nb_np;
//    // get correlations between POI and other things
//    EFT_PROF_INFO("Extract correlations: poi <-> nps");
//    auto poi_var = ws_->GetVar(res.poi);
//    for (size_t idx_np {0}; idx_np < res.reducedCovMatrix.GetNcols(); ++idx_np) {
//        auto par = res.params.at(idx_np);
//        auto corr = res.fitResult->correlation(*poi_var, *par);
//        EFT_PROF_DEBUG("correlations [poi] <-> {:40} ==> {}", par->GetName(), corr);
//        corr_per_np[par->GetName()] = corr;
//        correlations_per_nb_np.emplace_back(par->GetName(), corr);
//    }
//    res.corr_per_np = std::move(corr_per_np);
//    res.correlations_per_nb_np = std::move(correlations_per_nb_np);
//}
//
//void FitManager::PrintSuggestedNpsRanking(std::string path, const HesseStudyResult& res) const
//{
//    EFT_PROFILE_FN();
//    EFT_PROF_INFO("Print suggested nps ranking to: [{}]", path);
//    ofstream fs(path);
//    if ( ! fs.is_open() ) {
//        EFT_PROF_CRITICAL("Cannot open file: [{}] for writing. Print only to stdout", path);
//        PrintSuggestedNpsRankingStream(cout, res);
//    }
//    else {
//        PrintSuggestedNpsRankingStream(fs, res);
//        PrintSuggestedNpsRankingStream(cout, res);
//    }
//}
//
//void FitManager::PrintSuggestedNpsRankingStream(std::ostream& os, const HesseStudyResult& res) const
//{
//    //TODO: change by a json encoding
//    os << "results of ranking for " << res.poi << " with " << res.corr_per_np.size() << " nps" << endl;
//    for (const auto& [np, corr] : res.sorted_correlations) {
//        os << fmt::format("{:50} {}", np, corr) << endl;
//    }
//}

void FitManager::SetAllNuisanceParamsConst() noexcept
{
    EFT_PROFILE_FN();
    if (args_["np_all"]->empty())
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
    for (const auto& np : *args_["np_all"]) {
        const string name = {np->GetTitle()};
        EFT_PROF_DEBUG("Set {} to const", name);
        dynamic_cast<RooRealVar *>(np)->setConstant(true);
    }

    //cout << "status after:" << endl;
    //args_["np"]->Print("v");
}

void FitManager::SetAllNuisanceParamsFloat() noexcept {
    EFT_PROFILE_FN();
    if (args_["np_all"]->empty())
        ExtractNP();

    EFT_PROF_TRACE("[SetAllNuissFloat]");
    for (const auto& np : *args_["np_all"]) {
        const string name = {np->GetTitle()};
        EFT_PROF_DEBUG("Set {} to float ==> {}", name, *dynamic_cast<RooRealVar *>(np));
        dynamic_cast<RooRealVar *>(np)->setConstant(false);
    }
}

void FitManager::ExtractPOIs() noexcept
{
    EFT_PROFILE_FN();
    assert(ws_ != nullptr);
    args_["pois"] = (RooArgSet *) ws_->GetPOIs();
    // create list of pois in string format
    pois_.reserve(args_["pois"]->size());
    EFT_PROF_INFO("[FitManager] Extracted {} POIs to args[pois]", args_["pois"]->size());
    EFT_PROF_DEBUG("[FitManager] create a list of POIs in string format...");
    for (const auto& poi : *args_["pois"]) {
        string name = {poi->GetTitle()};
        pois_.push_back(std::move(name));
    }
    EFT_PROF_DEBUG("[FitManager] list of POIs in string format:");
    for (const auto& poi : pois_) { EFT_PROF_DEBUG("[{}]", poi); }
}

void FitManager::Init(FitManagerConfig config)
{
    EFT_PROFILE_FN();
    //eft::stats::Logger::SetFullPrinting();
    if (config.ws_name.empty()) {
        EFT_PROF_CRITICAL("ws_name is empty");
        throw std::logic_error("no ws_name set");
    }
    if (config.ws_path.empty()) {
        EFT_PROF_CRITICAL("ws_path is empty");
        throw std::logic_error("no ws_path set");
    }
    if (config.model_config.empty()) {
        EFT_PROF_CRITICAL("model_config is empty");
        throw std::logic_error("no model_config set");
    }

    EFT_PROF_INFO("[FitManager] init from config: path to ws: [{}], name: [{}], model_config: [{}]",
                  config.ws_path, config.ws_name, config.model_config);
    SetWsWrapper();
    SetWS(std::move(config.ws_path),
          std::move(config.ws_name)
          );
    SetModelConfig(std::move(config.model_config));

    EFT_PROF_DEBUG("[FitManager] extract NPs");
    ExtractNP();
    EFT_PROF_DEBUG("[FitManager] extract obs");
    ExtractObs();
    EFT_PROF_DEBUG("[FitManager] extract global obs");
    ExtractGlobObs();
    EFT_PROF_DEBUG("[FitManager] extract cats");
    ExtractCats();
    EFT_PROF_DEBUG("[FitManager] extract POIs");
    ExtractPOIs();

    EFT_PROF_INFO("[FitManager] extract pdf total: {}", config.comb_pdf);
    ExtractPdfTotal(config.comb_pdf);
    EFT_PROF_INFO("[FitManager] extract data total: {}", config.comb_data);
    ExtractDataTotal( config.comb_data);

    EFT_PROF_DEBUG("[FitManager] get constrains");
    auto pairConstr = FitUtils::GetPairConstraints(funcs_["pdf_total"], args_["np_all"], args_["globObs"], args_["obs"]);
    EFT_PROF_DEBUG("[FitManager] print obtained constrains");
    EFT_PROF_INFO("[FitManager] Extracted {} paired_constr_pdf", pairConstr.paired_constr_pdf->size());
    EFT_PROF_INFO("[FitManager] Extracted {} paired_globs",      pairConstr.paired_globs->size());
    EFT_PROF_INFO("[FitManager] Extracted {} paired_nps",        pairConstr.paired_nps->size());

    lists_[ "paired_globs" ] = pairConstr.paired_globs;
    lists_[ "paired_nps"   ] = pairConstr.paired_nps;
    ExtractNotGammaNps();

    if ( !config.get.empty() ) {
        ProcessGetCommand(config);
        return;
    }

    EFT_PROF_TRACE("[FitManager] INIT DONE");
}

void FitManager::ReadConfigFromCommandLine(CommandLineArgs& commandLineArgs, FitManagerConfig& config) noexcept
{
    EFT_PROFILE_FN();
    EFT_PROF_DEBUG("[FitManager] Read Configuration from Command Line");

    // Use macros of a few kinds:
    // - trivial values (of a trivial types)
    // - bool values (which do not require key)
    // - arrays (and all decaying types)

#ifndef EFT_SET_VAL_IF_EXISTS
#define EFT_SET_VAL_IF_EXISTS(args, config, param)                                  \
    if (args.SetValIfArgExists(#param, config.param)) {                             \
        EFT_PROF_INFO("[FitManager] Set param: {:15} to {}", #param, config.param); \
     }

#endif
    EFT_SET_VAL_IF_EXISTS(commandLineArgs, config, res_path);
    EFT_SET_VAL_IF_EXISTS(commandLineArgs, config, worker_id);
    EFT_SET_VAL_IF_EXISTS(commandLineArgs, config, np_name);
    EFT_SET_VAL_IF_EXISTS(commandLineArgs, config, poi);
    EFT_SET_VAL_IF_EXISTS(commandLineArgs, config, ws_path);
    EFT_SET_VAL_IF_EXISTS(commandLineArgs, config, ws_name);
    EFT_SET_VAL_IF_EXISTS(commandLineArgs, config, model_config);
    EFT_SET_VAL_IF_EXISTS(commandLineArgs, config, comb_pdf);
    EFT_SET_VAL_IF_EXISTS(commandLineArgs, config, comb_data);
    EFT_SET_VAL_IF_EXISTS(commandLineArgs, config, top);
    EFT_SET_VAL_IF_EXISTS(commandLineArgs, config, eps);
    EFT_SET_VAL_IF_EXISTS(commandLineArgs, config, error_level);
    EFT_SET_VAL_IF_EXISTS(commandLineArgs, config, study_type);
    EFT_SET_VAL_IF_EXISTS(commandLineArgs, config, snapshot);
    EFT_SET_VAL_IF_EXISTS(commandLineArgs, config, poi_init_val);
    EFT_SET_VAL_IF_EXISTS(commandLineArgs, config, color_prefit_plus);
    EFT_SET_VAL_IF_EXISTS(commandLineArgs, config, color_prefit_minus);
    EFT_SET_VAL_IF_EXISTS(commandLineArgs, config, color_postfit_plus);
    EFT_SET_VAL_IF_EXISTS(commandLineArgs, config, color_postfit_minus);
    EFT_SET_VAL_IF_EXISTS(commandLineArgs, config, rmul);
    EFT_SET_VAL_IF_EXISTS(commandLineArgs, config, rmuh);
    EFT_SET_VAL_IF_EXISTS(commandLineArgs, config, np_scale);
    EFT_SET_VAL_IF_EXISTS(commandLineArgs, config, out_dir);
    EFT_SET_VAL_IF_EXISTS(commandLineArgs, config, output);
    EFT_SET_VAL_IF_EXISTS(commandLineArgs, config, input);
    EFT_SET_VAL_IF_EXISTS(commandLineArgs, config, np_scale);
    EFT_SET_VAL_IF_EXISTS(commandLineArgs, config, label_size);
    EFT_SET_VAL_IF_EXISTS(commandLineArgs, config, lmargin);
    EFT_SET_VAL_IF_EXISTS(commandLineArgs, config, rmargin);
    EFT_SET_VAL_IF_EXISTS(commandLineArgs, config, tmargin);
    EFT_SET_VAL_IF_EXISTS(commandLineArgs, config, bmargin);
    EFT_SET_VAL_IF_EXISTS(commandLineArgs, config, retry);
    EFT_SET_VAL_IF_EXISTS(commandLineArgs, config, strategy);
    EFT_SET_VAL_IF_EXISTS(commandLineArgs, config, ds_title);
    EFT_SET_VAL_IF_EXISTS(commandLineArgs, config, energy);
    EFT_SET_VAL_IF_EXISTS(commandLineArgs, config, lumi);
    EFT_SET_VAL_IF_EXISTS(commandLineArgs, config, experiment);
    EFT_SET_VAL_IF_EXISTS(commandLineArgs, config, res_status);
    EFT_SET_VAL_IF_EXISTS(commandLineArgs, config, np_offset);
    EFT_SET_VAL_IF_EXISTS(commandLineArgs, config, mu_offset);
    EFT_SET_VAL_IF_EXISTS(commandLineArgs, config, mu_latex);
    EFT_SET_VAL_IF_EXISTS(commandLineArgs, config, text_size);
    EFT_SET_VAL_IF_EXISTS(commandLineArgs, config, text_font);
    EFT_SET_VAL_IF_EXISTS(commandLineArgs, config, dy);
    EFT_SET_VAL_IF_EXISTS(commandLineArgs, config, empty_bins);
    EFT_SET_VAL_IF_EXISTS(commandLineArgs, config, dx_legend);
    EFT_SET_VAL_IF_EXISTS(commandLineArgs, config, dy_legend);
    EFT_SET_VAL_IF_EXISTS(commandLineArgs, config, max_digits);
    EFT_SET_VAL_IF_EXISTS(commandLineArgs, config, suggestions);
#undef EFT_SET_VAL_IF_EXISTS

// Parse bool options

#ifndef EFT_ADD_BOOL_OPTIONS
#define EFT_ADD_BOOL_OPTIONS(args, config, param)                       \
    if (args.HasKey(#param)) {                                          \
        config.param = true;                                            \
        EFT_PROF_INFO("[FitManager] Add flag option: {:15}", #param);   \
     }
#endif

    EFT_ADD_BOOL_OPTIONS(commandLineArgs, config, no_gamma);
    EFT_ADD_BOOL_OPTIONS(commandLineArgs, config, fit_all_pois);
    EFT_ADD_BOOL_OPTIONS(commandLineArgs, config, fit_single_poi);
    EFT_ADD_BOOL_OPTIONS(commandLineArgs, config, vertical);
    EFT_ADD_BOOL_OPTIONS(commandLineArgs, config, reuse_nll);
    EFT_ADD_BOOL_OPTIONS(commandLineArgs, config, silent);
    EFT_ADD_BOOL_OPTIONS(commandLineArgs, config, release);
    EFT_ADD_BOOL_OPTIONS(commandLineArgs, config, save_prelim);
    EFT_ADD_BOOL_OPTIONS(commandLineArgs, config, draw_impact);
    EFT_ADD_BOOL_OPTIONS(commandLineArgs, config, weighted);
#undef EFT_ADD_BOOL_OPTIONS

    // vectors
#ifndef EFT_ADD_VEC_OPTION
#define EFT_ADD_VEC_OPTION(args, config, param)                                                 \
    if (args.SetValIfArgExists(#param, config.param)) {                                         \
        EFT_PROF_DEBUG("Add vector [{}] option with: {:15} params", #param, config.param.size());  \
     }

#endif
    EFT_ADD_VEC_OPTION(commandLineArgs, config, errors);
    EFT_ADD_VEC_OPTION(commandLineArgs, config, fileformat);
    EFT_ADD_VEC_OPTION(commandLineArgs, config, ignore_name);
    EFT_ADD_VEC_OPTION(commandLineArgs, config, match_names);
    EFT_ADD_VEC_OPTION(commandLineArgs, config, replace);
    EFT_ADD_VEC_OPTION(commandLineArgs, config, remove_prefix);
    EFT_ADD_VEC_OPTION(commandLineArgs, config, remove_suffix);
    EFT_ADD_VEC_OPTION(commandLineArgs, config, np_names);
    EFT_ADD_VEC_OPTION(commandLineArgs, config, add_text);
    EFT_ADD_VEC_OPTION(commandLineArgs, config, add_text_ndc);
    EFT_ADD_VEC_OPTION(commandLineArgs, config, get);
    EFT_ADD_VEC_OPTION(commandLineArgs, config, h_draw_options);
    EFT_ADD_VEC_OPTION(commandLineArgs, config, plt_size);
#undef EFT_ADD_VEC_OPTION



    if (config.fit_all_pois) {
        EFT_PROF_WARN("Set [fit_all_pois mode]: all available pois are to be fit");
        config.fit_single_poi = false;
    }

    if (config.res_path.empty())
        EFT_PROF_WARN("save res to default (in the root folder)");
    else
        EFT_PROF_INFO("save res to: {}", config.res_path);

    if (config.release)
        eft::stats::Logger::SetRelease();

    commandLineArgs.ReportStatus();
}

void FitManager::CreateAsimovData(PrePostFit studyType) noexcept
{
    EFT_PROF_TRACE("[FitManager]{AsimovData}");
    assert(studyType != PrePostFit::OBSERVED);

    if (studyType == PrePostFit::PREFIT && data_.find("asimov_prefit") == data_.end()) {
        EFT_PROF_INFO("[FitManager]{CreateAsimovData} required Asimov [prefit] is already present, no need to generate new");
        return;
    }
    if (studyType == PrePostFit::POSTFIT && data_.find("asimov_postfit") == data_.end()) {
        EFT_PROF_INFO("[FitManager]{CreateAsimovData} required Asimov [postfit] is already present, no need to generate new");
        return;
    }

    //SetUpGlobObs(studyType);
    SetAllGlobObsTo(0., 0.);
    NpRankingStudySettings settings;
    settings.errors = fit::Errors::DEFAULT;
    //settings.poi
    DoFitAllNpFloat(std::move(settings));
    SetGlobalObservablesToValueFoundInFit();
    SetAllGlobObsConst();
    //SetUpGlobObs(studyType);

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
    ws_->raw()->saveSnapshot("condGlobObs", *args_["globObs"], kTRUE);
    ws_->raw()->loadSnapshot("condGlobObs");
};

RooArgSet* FitManager::GetListAsArgSet(const std::string& name) const
{
    EFT_PROF_TRACE("FitManager::GetListAsArgSet for {}", name);
    auto* res = new RooArgSet();

    if (lists_.find(name) == lists_.end())
    {
        EFT_PROF_WARN("FitManager::GetListAsArgSet no list: {} is available", name);
        string error_message = fmt::format("FitManager::GetListAsArgSet no list: {} is available,"
                                           "Use one of the {} followings:", name, lists_.size());

        for (const auto& list : lists_) error_message += "[" + list.first + "], ";

        throw std::out_of_range(error_message);
    }
    //EFT_PROF_DEBUG("FitManager::GetListAsArgSet for {} - found", name);
    const auto* list = lists_.at(name);
    for (const auto elem : *list) {
        res->add(*elem);
    }
    return res;
}

void FitManager::ExtractNotGammaNps() noexcept
{
    EFT_PROF_TRACE("FitManager::ExtractNotGammaNps");
    if (lists_.find("paired_nps") == lists_.end())
    {
        EFT_PROF_CRITICAL("FitManager::ExtractNotGammaNps the lists_[paired_nps] is empty.");
        EFT_PROF_CRITICAL("FitManager::ExtractNotGammaNps extract them before");
        return;
        //throw std::runtime_error("no paired nps extracted");
    }
    assert( lists_[ "paired_nps"   ]->size() != 0);

    auto non_gamma_nps = new RooArgList();

    for (const auto& np : *lists_["paired_nps"]) {
        const std::string name = {np->GetTitle()};
        if (name.find("gamma") == std::string::npos) {
            EFT_PROF_DEBUG("add {:60} as not-gamma", name);
            non_gamma_nps->add(*np);
        }
    }
    EFT_PROF_INFO("FitManager::ExtractNotGammaNps extracted {} non-gamma nps out of {}",
                  non_gamma_nps->size(),
                  lists_.at("paired_nps")->size());
    lists_["non_gamma_nps"] = non_gamma_nps;
}

void FitManager::SetGlobalObservablesToValueFoundInFit() noexcept
{
    EFT_PROF_INFO("Set global observables to the values found in fit for the corresponding Nuisance parameters");
    auto *globObs = GetListAsArgSet("paired_globs");
    auto *nps = GetListAsArgSet("paired_nps"); // TODO: refactor to use GetNps
    const size_t nb_nps = nps->size();
    for (size_t idx_np {0}; idx_np < nb_nps; ++idx_np)
    {
        auto np = dynamic_cast<RooRealVar*>(nps->operator[](idx_np));
        for (size_t idx_glob {0}; idx_glob < nb_nps; ++idx_glob)
        {
            auto glob = dynamic_cast<RooRealVar*>( globObs->operator[](idx_glob) );
            if (glob->dependsOn(*np))
            {
                std::string np_name   = np->GetName();
                std::string glob_name = glob->GetName();
                EFT_PROF_DEBUG("Set glob: {:60} to the value of np: {:1} => {:10}",
                               std::move(glob_name),
                               std::move(np_name),
                               np->getVal()
                               );
                glob->setVal(np->getVal());
            }
        }
    }

}

void FitManager::ProcessGetCommand(const FitManagerConfig& config) {
    //EFT_PROF_DEBUG("Process get command: {}", config.get);
    ASSERT_NOT(config.get.empty());
    string get_demand = config.get[0];
    bool get_count = false;
    if (config.get.size() > 1) {
        string count_candidate = config.get[1];
        eft::StringUtils::Trim(count_candidate);
        eft::StringUtils::ToLowCase(count_candidate);
        if (count_candidate == "count") {
            EFT_PROF_DEBUG("In the count mode");
            get_count = true;
        }
    }
    eft::StringUtils::Trim(get_demand);
    eft::StringUtils::ToLowCase(get_demand);
    EFT_PROF_DEBUG("after trimming and lowering: {}", get_demand);
    if (get_demand == "poi") {
        if (get_count) {
            cout << pois_.size() << endl;
            return;
        }
        EFT_PROF_DEBUG("dispatch to POI, there are {} available", pois_.size());
        for (const auto& poi : pois_) {
            std::cout << poi << std::endl;
        }
        return;
    }

    // TODO: finish printing results by "get" demand
    RooArgSet* argSet = nullptr;

    if (get_demand == "np" || get_demand == "nps") {
        argSet = args_["np_all"];
    }
    else if (get_demand == "obs" ) {
        argSet = args_["obs"];
    }
    else if (get_demand == "globs" ) {
        argSet = args_["globObs"];
    }
    else if (get_demand == "paired_globs" ) {
        argSet = args_["paired_globs"];
    }
    else if (get_demand == "paired_nps"
            || get_demand == "paired_globs"
            || get_demand == "non_gamma_nps") {
        argSet = GetListAsArgSet(get_demand);
    }


    if (argSet) {
        if (get_count) {
            cout << argSet->size() << endl;
            return;
        }

        EFT_PROF_DEBUG("for key {} available {} params", get_demand, argSet->size());
        for (const auto& arg : *argSet) {
            //EFT_PROF_DEBUG("{}", *dynamic_cast<RooRealVar*>(arg));
            cout << arg->GetTitle() << endl;
        }
        return;
    }
    else {
        EFT_PROF_CRITICAL("Argset for demand [{}] is empty", get_demand);
        throw std::logic_error("");
    } // if argset is empte even though the name is known

    // counts
    if (get_demand == "pdf") {
        funcs_[ "pdf_total" ]->Print();
        return;
    }
    else if (get_demand == "ds_total"){
        data_[ "ds_total" ]->Print();
        return;
    }

    throw std::logic_error(fmt::format("--get {} is now known. Use: np, poi, globs, obs, paired_globs, paired_nps, non_gamma_nps"));

}



} // stats