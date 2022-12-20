//
// Created by Aleksei Lukianchuk on 20-Dec-22.
//

#include "CorrelationStudyProcessor.h"
#include "Profiler.h"
#include "Logger.h"
#include "IWorkspaceWrapper.h"
#include "../../Fitter/Fitter.h"
#include "../../Utils/Scene.h"

#include "../Core/EftRooFitPreCompHeaders.h"
#include "../Core/EftRootDrawingPreCompHeaders.h"

#include "../FitManager.h"

#include <fstream>
#include <iostream>

using namespace std;

namespace eft::stats::ranking {

HesseStudyResult CorrelationStudyProcessor::ComputeHesseNps() {
    EFT_PROFILE_FN();
    EFT_PROF_INFO("Compute Hesse forNPS");

    EFT_PROF_INFO("set all POIs const");
    //SetAllPOIsConst();
    // TODO: add SetAllPoisConst to the WS wrapper
    for (const auto poi : *pois_) {
        dynamic_cast<RooRealVar*>(poi)->setConstant(true);
    }

    EFT_PROF_INFO("Float required poi: {}", poi_name_);
    ws_->FloatVal(poi_name_);

    // TODO: float all pois, if needed by a flag!!!!

    fit::Fitter fitter;
    fitter.SetNps(nps_);
    fitter.SetGlobs(globs_);

    fit::FitSettings fitSettings;
    fitSettings.pdf = const_cast<RooAbsPdf*>(pdf_);
    fitSettings.data = data_;
    fitSettings.pois = pois_; // TODO: wrap around by a function
    fitSettings.errors = settings_.errors;
    fitSettings.retry = settings_.retry;
    fitSettings.strategy = settings_.strategy;
    fitSettings.eps = settings_.eps;
    fitSettings.save_res = true;
    // TODO: use one set of settings...


    EFT_PROF_INFO("[ComputeHesseNps] perform fit saving results");
    auto fitRes = fitter.Fit(fitSettings);
    EFT_PROF_DEBUG("[ComputeHesseNps] create list of nps");
    RooArgList list_nps(*nps_);
    list_nps.add(*poi_);
    EFT_PROF_DEBUG("[ComputeHesseNps] created list of nps (and POI) with {} entries:", list_nps.size());
    for (const auto np : list_nps) {
        cout << np->GetName() << endl;
    }
    EFT_PROF_DEBUG("[ComputeHesseNps] extract results from RooFitResult");
    auto res = HesseStudyResult::ExtractFromRooFitResult(*fitRes, list_nps);
    return res;
}

void CorrelationStudyProcessor::PlotCovariances(const HesseStudyResult& res) const
{
    EFT_PROFILE_FN();
    using eft::utils::draw::Scene;

    shared_ptr<TH2D> cov = make_shared<TH2D>(res.reducedCovMatrix);
    EFT_PROF_INFO("Created cov matrix with size: [{}][{}]", res.reducedCovMatrix.GetNcols(), res.reducedCovMatrix.GetNrows());
    auto canvas = make_shared<TCanvas>("c", "c", 4000, 4000);
    //auto canvas = Scene::Create(4000, 4000);
    //Scene::Register(cov.get());
    for (size_t idx_np {0}; idx_np < res.reducedCovMatrix.GetNcols(); ++idx_np) {
        EFT_PROF_DEBUG("set label of bin: {:4} to {}", idx_np, res.params.at(idx_np)->GetName());
        cov->GetXaxis()->SetBinLabel(idx_np + 1, res.params.at(idx_np)->GetName());
        cov->GetYaxis()->SetBinLabel(idx_np + 1, res.params.at(idx_np)->GetName());
    }
    cov->SetLabelSize(0.005);
    cov->GetXaxis()->SetLabelSize(0.005);
    cov->GetYaxis()->SetLabelSize(0.005);
    EFT_PROF_INFO("All names are set, draw");
    cov->Draw("colz");
    EFT_PROF_INFO("drawn, save");
    string name_covs = fmt::format("covariances_with_{}_float.pdf", res.poi);
    canvas->SaveAs(name_covs.c_str());
    EFT_PROF_INFO("clear");
    canvas->Clear();
    //Scene::SaveAs("covariances.pdf");

//    map<string, double> corr_per_np;
//    // get correlations between POI and other things
//    EFT_PROF_INFO("Extract correlations: poi <-> nps");
//    auto poi_var = ws_->GetVar(res.poi);
//    shared_ptr<TH1D> corr_with_poi = make_shared<TH1D>("h", "h", res.covariances.size(), 0, res.covariances.size());
//    for (size_t idx_np {0}; idx_np < res.reducedCovMatrix.GetNcols(); ++idx_np) {
//        auto par = res.params.at(idx_np);
//        auto corr = res.fitResult->correlation(*poi_var, *par);
//        corr_with_poi->SetBinContent(idx_np + 1, corr);
//        corr_with_poi->GetXaxis()->SetBinLabel(idx_np + 1, par->GetName());
//        EFT_PROF_DEBUG("correlations [poi] <-> {:40} ==> {}", par->GetName(), corr);
//        corr_per_np[par->GetName()] = corr;
//    }

    shared_ptr<TH1D> corr_with_poi = make_shared<TH1D>("h", "h", res.covariances.size(), 0, res.covariances.size());
    //for (const auto& [np, corr] : res.correlations_per_nb_np) {
    for (size_t idx_np {0}; idx_np < res.correlations_per_nb_np.size(); ++idx_np) {
        corr_with_poi->SetBinContent(idx_np + 1, res.correlations_per_nb_np.at(idx_np).second);
        corr_with_poi->GetXaxis()->SetBinLabel(idx_np + 1, res.correlations_per_nb_np.at(idx_np).first.c_str());
    }

    corr_with_poi->SetLabelSize(0.005);
    corr_with_poi->Draw("H");
    string name_to_save = fmt::format("correlations_with_{}.pdf", res.poi);
    canvas->SaveAs(name_to_save.c_str());
    canvas->Clear();

    //vector<pair<string, double>> sorted_corrs {corr_per_np.begin(), corr_per_np.end()};
    vector<pair<string, double>> sorted_corrs {res.corr_per_np.begin(), res.corr_per_np.end()};
    std::sort(sorted_corrs.begin(),
              sorted_corrs.end(),
              [](const auto& l, const auto& r) -> bool
              {
                  return abs(l.second) > abs(r.second);
              });

    EFT_PROF_INFO("Sorted corrs with poi:");
    for (const auto& [name, cor] : sorted_corrs) {
        cout << fmt::format("{:60} => {}", name, cor) << endl;
    }

    EFT_PROF_INFO("Fill corr with poi sorted");
    const string h_name = fmt::format("Correlations of NPs with {}", res.poi);
    shared_ptr<TH1D> corr_with_poi_sorted = make_shared<TH1D>(h_name.c_str(),
                                                              h_name.c_str(),
                                                              res.covariances.size(),
                                                              0,
                                                              res.covariances.size());
    EFT_PROF_DEBUG("{:3} ==> {:50} ==> {:10}", "idx", "name", "corr wit mu");
    for (size_t idx {1}; idx < sorted_corrs.size(); ++idx) {
        //for (const auto& [name, cor] : sorted_corrs) {
        auto& name = sorted_corrs.at(idx).first;
        auto& corr = sorted_corrs.at(idx).second;
        EFT_PROF_DEBUG("{:3} ==> {:50} ==> {:10}", idx, name, corr);
        corr_with_poi_sorted->SetBinContent(idx, corr);
        corr_with_poi_sorted->GetXaxis()->SetBinLabel(idx, name.c_str());
    }
    corr_with_poi_sorted->SetLabelSize(0.005);
    corr_with_poi_sorted->Draw("H");
    string name_to_save_sorted = fmt::format("correlations_with_{}_sorted.pdf", res.poi);
    canvas->SaveAs(name_to_save_sorted.c_str());
    //canvas->SaveAs("correlations_with_poi_sorted.pdf");
    canvas->Clear();

    //Scene::SaveAs()
}

void CorrelationStudyProcessor::ExtractCorrelations(HesseStudyResult& res) const
{
    EFT_PROFILE_FN();
    map<string, double> corr_per_np;
    std::vector<std::pair<std::string, double>> correlations_per_nb_np;
    // get correlations between POI and other things
    EFT_PROF_INFO("Extract correlations: poi <-> nps");
    auto poi_var = ws_->GetVar(res.poi);
    for (size_t idx_np {0}; idx_np < res.reducedCovMatrix.GetNcols(); ++idx_np) {
        auto par = res.params.at(idx_np);
        auto corr = res.fitResult->correlation(*poi_var, *par);
        EFT_PROF_DEBUG("correlations [poi] <-> {:40} ==> {}", par->GetName(), corr);
        corr_per_np[par->GetName()] = corr;
        correlations_per_nb_np.emplace_back(par->GetName(), corr);
    }
    res.corr_per_np = std::move(corr_per_np);
    res.correlations_per_nb_np = std::move(correlations_per_nb_np);
}

void CorrelationStudyProcessor::PrintSuggestedNpsRanking(std::string path, const HesseStudyResult& res) const
{
    EFT_PROFILE_FN();
    EFT_PROF_INFO("Print suggested nps ranking to: [{}]", path);
    ofstream fs(path);
    if ( ! fs.is_open() ) {
        EFT_PROF_CRITICAL("Cannot open file: [{}] for writing. Print only to stdout", path);
        PrintSuggestedNpsRankingStream(cout, res);
    }
    else {
        PrintSuggestedNpsRankingStream(fs, res);
        PrintSuggestedNpsRankingStream(cout, res);
    }
}

void CorrelationStudyProcessor::PrintSuggestedNpsRankingStream(std::ostream& os, const HesseStudyResult& res)
{
    //TODO: change by a json encoding
    os << "results of ranking for " << res.poi << " with " << res.corr_per_np.size() << " nps" << endl;
    for (const auto& [np, corr] : res.sorted_correlations) {
        os << fmt::format("{:50} {}", np, corr) << endl;
    }
}

CorrelationStudyProcessor::CorrelationStudyProcessor(CommandLineArgs *cmd) {
    eft::stats::FitManagerConfig config;
    eft::stats::FitManager::ReadConfigFromCommandLine(*cmd, config);
    auto manager = make_unique<eft::stats::FitManager>();

    settings_.poi                   = config.poi;
    settings_.path_to_save_res      = config.res_path;
    settings_.poi_init_val          = config.poi_init_val;
    settings_.eps                   = config.eps;
    settings_.retry                 = config.retry;
    settings_.strategy              = config.strategy;
    settings_.reuse_nll             = config.reuse_nll;
    settings_.fit_all_pois          = config.fit_all_pois;
    settings_.fit_single_poi        = config.fit_single_poi;
    settings_.errors = eft::stats::fit::Errors::HESSE;

    manager->Init(config);

    EFT_PROF_WARN("check pdf: {}", config.comb_pdf);
    assert(manager->GetPdf(config.comb_pdf) != nullptr);
    EFT_PROF_WARN("check globs");
    assert(manager->GetListAsArgSet("paired_globs"));
    EFT_PROF_WARN("check nps");
    assert(manager->GetListAsArgSet("paired_nps"));

    SetPdf(const_cast<RooAbsPdf *>(manager->GetPdf(config.comb_pdf)))
    .SetWS(ws_)
    .SetGlobs(manager->GetListAsArgSet("paired_globs"))
    .SetNPs(manager->GetListAsArgSet("paired_nps"))
    .SetData(&manager->GetData(settings_.prePostFit))
    .SetPOIs(manager->GetListAsArgSet("pois"))
    .SetPOI(ws_->GetVar(settings_.poi))
    .SetPOIname(settings_.poi);

    if ( ! VerifyConsistency() ) {
        EFT_PROF_CRITICAL("Problem with initialising CorrelationStudyProcessor ^---");
        throw std::runtime_error("");
    }
}

bool CorrelationStudyProcessor::VerifyConsistency() const {
    if (pdf_ == nullptr) {
        EFT_PROF_CRITICAL("CorrelationStudyProcessor pdf is nullptr");
        return false;
    }
    if (data_ == nullptr) {
        EFT_PROF_CRITICAL("CorrelationStudyProcessor data is nullptr");
        return false;
    }
    if (nps_ == nullptr) {
        EFT_PROF_CRITICAL("CorrelationStudyProcessor nps is nullptr");
        return false;
    }
    if (globs_ == nullptr) {
        EFT_PROF_CRITICAL("CorrelationStudyProcessor globs_ is nullptr");
        return false;
    }
    if (pois_ == nullptr) {
        EFT_PROF_CRITICAL("CorrelationStudyProcessor pois is nullptr");
        return false;
    }
    if (poi_ == nullptr) {
        EFT_PROF_CRITICAL("CorrelationStudyProcessor poi is nullptr");
        return false;
    }
    if (poi_name_.empty()) {
        EFT_PROF_CRITICAL("CorrelationStudyProcessor poi_name is empty");
        return false;
    }
    if (ws_ == nullptr) {
        EFT_PROF_CRITICAL("CorrelationStudyProcessor ws is empty");
        return false;
    }

    return true;

}

} // ranking