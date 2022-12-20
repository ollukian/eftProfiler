//
// Created by Aleksei Lukianchuk on 20-Dec-22.
//

#include "Core.h"
#include "CorrelationStudyProcessor.h"
#include "IWorkspaceWrapper.h"
#include "../../Fitter/Fitter.h"
#include "../../Utils/Scene.h"

#include "../Core/EftRooFitPreCompHeaders.h"
#include "../Core/EftRootDrawingPreCompHeaders.h"

#include "../FitManager.h"

#include "CorrelationStudyPlotSettings.h"

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
    res.poi = poi_name_;
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
    auto poi_var = ws_->GetVar(poi_name_);
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

    // pdf_total is a key to the total pdf, which is extracted by the manager
    SetPdf(const_cast<RooAbsPdf *>(manager->GetPdf("pdf_total")));
    EFT_PROF_CRITICAL("step 1");
    SetWS(manager->ws());
    EFT_PROF_CRITICAL("step 2");
    SetGlobs(manager->GetListAsArgSet("paired_globs"));
    EFT_PROF_CRITICAL("step 3");
    SetNPs(manager->GetListAsArgSet("paired_nps"));
    EFT_PROF_CRITICAL("step 4");
    SetData(&manager->GetData(settings_.prePostFit));
    EFT_PROF_CRITICAL("step 5");
    SetPOIs(const_cast<RooArgSet *>(ws_->GetPOIs()));
    EFT_PROF_CRITICAL("step 6");
    SetPOI(ws_->GetVar(settings_.poi));
    EFT_PROF_CRITICAL("step 7");
    SetPOIname(settings_.poi);
    EFT_PROF_CRITICAL("it's ok");

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

CorrelationStudyProcessor::NpCorrelations
CorrelationStudyProcessor::GetSortedCorrelationsFromFile(const std::string& path, const std::string& version)
{
    EFT_PROFILE_FN();
    EFT_PROF_INFO("GetSortedCorrelationsFromFile from {}, encoding: {}", path, version);
    if (version == "v1")
        return GetSortedCorrelationsFromFileV1(path);
    else
        throw std::runtime_error("Encoding only in v1 is currently available");
}

CorrelationStudyProcessor::NpCorrelations
CorrelationStudyProcessor::GetSortedCorrelationsFromFileV1(const std::string& path)
{
    // particular implementation for the v1 encoding (from 01Dec22)
    EFT_PROFILE_FN();
    EFT_PROF_INFO("Read sorted correlation from {}", path);
    ifstream fs(path);
    if ( ! fs.is_open() )
        throw std::runtime_error(fmt::format("cannot open: {}", path));

    string header_line;
    string poi_line;
    string np_name;
    double corr = 0.;
    string line;

    getline(fs, header_line);
    fs >> poi_line >> corr;
    EFT_PROF_DEBUG("poi: {}, corr: {}", poi_line, corr);
    vector<pair<string, double>> res;

    while (getline(fs, line)) {
        stringstream ss{line};
        ss >> np_name >> corr;
        EFT_PROF_DEBUG("Read: {}, parsed as: np: {:40}, cor: {}", line, np_name, corr);
        if ( ! np_name.empty() )
            res.emplace_back(std::move(np_name), corr);
    }
    EFT_PROF_INFO("Read {} values", res.size());
    for (const auto& [name, corr_] : res) {
        EFT_PROF_DEBUG("{:40} ==> {}", name, corr_);
    }
    return res;
}

CorrelationStudyProcessor::NpCorrelations
CorrelationStudyProcessor::FromVecNpInfo(const std::vector<NpInfoForPlot>& infos, const std::string& field) {
    EFT_PROFILE_FN();
    NpCorrelations res;
    EFT_PROF_INFO("Convert vector<NpInfoForPlot> to NpCorrelations, using: {} field for impact", field);
    EFT_PROF_INFO("Available {} pairs np <==> corr", infos.size());
    for (const auto& np_info : infos) {
        if (field == "+sigma")
            res.emplace_back(np_info.name, np_info.impact_plus_sigma_var);
        else if (field == "-sigma")
            res.emplace_back(np_info.name, np_info.impact_minus_sigma_var);
        else if (field == "+1")
            res.emplace_back(np_info.name, np_info.impact_plus_one_var);
        else if (field == "-1")
            res.emplace_back(np_info.name, np_info.impact_minus_one_var);
        else {
            EFT_PROF_CRITICAL("Field: [{}] is not know. Use: {+sigma, -sigma, +1, -1}");
            throw std::runtime_error("");
        }
    }
    EFT_PROF_INFO("As a result: {} pairs", res.size());

    return res;
};

CorrelationStudyProcessor::NpCorrelations
CorrelationStudyProcessor::ApplySelector(const CorrelationStudyProcessor::NpCorrelations& corrs,
                                         const SelectorSettings& selectorSettings)
                                          noexcept
{
    EFT_PROFILE_FN();
    NpCorrelations res;
    const auto& poi = selectorSettings.poi;
    const auto& selector = selectorSettings.selector;

    for (const auto& [np_name, corr] : corrs) {
        NpInfoForPlot info;
        info.name = np_name;
        info.poi = poi;
        bool is_passing = selector(std::move(info));
        EFT_PROF_DEBUG("Result of applysing selector to: {:50} -> ", is_passing);
        if (is_passing)
            res.emplace_back(np_name, corr);
    }

    return res;
}

//vector<NpInfoForPlot>
//CorrelationStudyProcessor::ToVecNpInfo(const CorrelationStudyProcessor::NpCorrelations& infos,
//                                       const std::string& field)
//{
//    EFT_PROFILE_FN();
//    vector<NpInfoForPlot> res;
//    res.reserve(infos.size());
//
//
//    for (const auto& [np_name, corr] : infos) {
//        NpInfoForPlot info;
//        if (field == "+sigma")
//            info.impact_plus_sigma_var = co);
//        else if (field == "-sigma")
//            info.impact_minus_sigma_var = co);
//        else if (field == "+1")
//            info.impact_plus_one_var = co);
//        else if (field == "-1")
//            info.impact_minus_one_var = co);
//        else {
//            EFT_PROF_CRITICAL("Field: [{}] is not know. Use: {+sigma, -sigma, +1, -1}");
//            throw std::runtime_error("");
//        }
//    }
//}

void CorrelationStudyProcessor::DrawCorrsComparison(const shared_ptr<CorrelationStudyPlotSettings>& settings)
{
    EFT_PROFILE_FN();

    gStyle->SetOptTitle(0);
    gStyle->SetOptStat(0000000);

    using eft::utils::draw::Scene;

    size_t nb_bins = settings->correlations1.size();
    //ASSERT_EQUAL(nb_bins, settings->correlations2.size());
    if (settings->correlations2.size() < nb_bins)
        nb_bins = settings->correlations2.size();

    if (settings->np_nps_plot < nb_bins)
        nb_bins = settings->np_nps_plot;

    const auto& corrs1 = settings->correlations1;
    const auto& corrs2 = settings->correlations2;


    const auto& names1 = settings->sorted_names_1;
    const auto& names2 = settings->sorted_names_2;

    if (settings->sorted_names_1.empty())
        settings->FormSortedNames();

    auto canvas = Scene::Create(settings->plt_size[0], settings->plt_size[1]);
    canvas->SetGrid();

    Scene::SetLeftMargin(settings->lmargin);
    Scene::SetRightMargin(settings->rmargin);
    Scene::SetBottomMargin(settings->bmargin);
    Scene::SetTopMargin(settings->bmargin);


    auto h = make_shared<TH2D>("h", "h",
                               nb_bins,
                               0,
                               nb_bins,
                               nb_bins,
                               0,
                               nb_bins);

    size_t nb_guessed {0};

    EFT_PROF_INFO("Idx for: {:40} ==> {:3} & {:3}", "np_name", settings->label1, settings->label2);
    for (size_t idx {0}; idx < nb_bins; ++idx) {
        auto np_name = names1.at(idx);
        auto idx_1 = idx;
        auto idx_2 = GetIdx(names2, np_name);
        EFT_PROF_DEBUG("Idx for: {:40} ==> {:3} & {:3}", np_name, idx_1, idx_2);
        if (idx_2 != -1) {
            h->Fill(idx_1 + 1, idx_2 + 1, 1);
            h->GetXaxis()->SetBinLabel(idx_1, np_name.c_str());
            h->GetYaxis()->SetBinLabel(idx_1, names2.at(idx).c_str());
            nb_guessed++;
        }
        else {
            EFT_PROF_WARN("NP: {:50} is not present in the second list", np_name);
        }
    }

    h->GetXaxis()->SetTitle(settings->label1.c_str());
    h->GetYaxis()->SetTitle(settings->label2.c_str());

    h->GetXaxis()->SetLabelSize(settings->label_size);
    h->GetYaxis()->SetLabelSize(settings->label_size);

    h->Draw("colz");
    Scene::SaveAs(settings->name_to_save);
    Scene::Clear();

    EFT_PROF_INFO("Guessed {} nps out of {} ==> {} fraction", nb_guessed, nb_bins, (float) nb_bins / (float) nb_guessed);
}

} // ranking