//
// Created by Aleksei Lukianchuk on 15-Sep-22.
//

#include <nlohmann/json.hpp>
#include <spdlog/fmt/fmt.h>

#include "../Core/Logger.h"
#include "NpRankingPlotter.h"

#include <iostream>
#include <fstream>
#include <iomanip>
#include <filesystem>

#include <TCanvas.h>
#include <TAxis.h>
#include "TH1D.h"
#include "THStack.h"
#include "TLegend.h"
#include "TLatex.h"
#include "TStyle.h"
#include "TLine.h"
#include "TGraphErrors.h"


using namespace std;

namespace eft::plot {

void NpRankingPlotter::ReadValues(const std::filesystem::path& path)
{
    namespace fs = std::filesystem;

    if (path.empty()) {
        EFT_PROF_WARN("[NpRankingPlotter][ReadValues] no --res_path is set: directory to read values from");
        return;
    }

    cout << "[NpRankingPlotter] read values from: " << path.string() << endl;
    cout << "[NpRankingPlotter] start looping over the directory" << endl;

    for (const auto& entry : fs::directory_iterator{path}) {
        const auto filenameStr = entry.path().filename().string();
        cout << filenameStr;
        if (entry.is_directory()) {
            cout << " ==> is a directory" << endl;
        }
        else if (entry.is_regular_file()) {
            cout << " ==> is a regular file, try to parse it" << endl;
            RegisterRes(ReadValuesOneFile(entry));
            //if (callback(res)) {
            //    EFT_PROF_INFO("NpRankingPlotter::ReadValues passes selection set by the callback. Register it");
            //    RegisterRes(res);
            //}
            ///RegisterRes(ReadValuesOneFile(entry), callback);
        }
    }

}

NpRankingStudyRes NpRankingPlotter::ReadValuesOneFile(const std::filesystem::path& path)
{
    const string filename = path.string();
    cout << fmt::format("[ReadValuesOneFile] read from {}", filename) << endl;
    const string extension = path.extension().string();
    cout << fmt::format("[ReadValuesOneFile] extension: [{}]", extension);
    if (extension != ".json") {
        cout << fmt::format(" NOT [.json]") << endl;
        return {};
    }

    cout << " => is [.json]" << endl;
    cout << "[ReadValuesOneFile] try to open: " << filename << endl;
    ifstream ifs(filename);
    if ( ! ifs.is_open() ) {
        cout << "[ReadValuesOneFile] cannot open: " << filename << endl;
        throw std::runtime_error("error opening: " + filename);
    }

    nlohmann::json j;
    ifs >> j;

    NpRankingStudyRes res;

    try {
        res = j.get<NpRankingStudyRes>();
    }
    catch (nlohmann::json::type_error& e) {
        EFT_PROF_WARN("NpRankingPlotter::ReadValuesOneFile{} error: {}. Replace nll by zero"
        , path.string()
        , e.what()
        );

        j.at("studyType").get_to(res.studyType);
        j.at("statType").get_to(res.statType);
        j.at("prePostFit").get_to(res.prePostFit);
        j.at("poi_name").get_to(res.poi_name);
        j.at("np_name").get_to(res.np_name);
        j.at("poi_val").get_to(res.poi_val);
        j.at("poi_err").get_to(res.poi_err);
        j.at("np_val").get_to(res.np_val);
        j.at("np_err").get_to(res.np_err);
        if (!j.at("nll").is_null())
            j.at("nll").get_to(res.nll);
    }


    cout << fmt::format("[ReavValueOneFile] read res for poi: {}, np: {}", res.poi_name, res.np_name) << endl;
    cout << setw(4) << j << endl;
    np_study_res_[res.np_name] = res;
    return res;
    //RegisterRes(np_study_res_[res.np_name]);
}

void NpRankingPlotter::Plot(const std::shared_ptr<RankingPlotterSettins>& settings) noexcept
{

    gStyle->SetOptTitle(0);
    gStyle->SetOptStat(0000000);

    EFT_PROF_TRACE("[NpRankingPlotter]{Plot}");
    EFT_PROF_INFO("[NpRankingPlotter] before selector available {} NP, plot {} out of them",
                  res_for_plot_.size(),
                  settings->nb_nps_to_plot);

    vector<stats::NpInfoForPlot> res_for_plot_after_selector;

    std::copy_if(res_for_plot_.begin(),
                 res_for_plot_.end(),
                 std::back_inserter(res_for_plot_after_selector),
                 [&](const NpInfoForPlot& info) {
                     EFT_PROF_DEBUG("callback for info for poi: {:10}, np: {:40} -> {}", info.poi, info.name, callback_(info));
                     return callback_(info);
                 }
    );

    EFT_PROF_INFO("[NpRankingPlotter] after selector available {} NP, plot {} out of them",
                  res_for_plot_after_selector.size(),
                  settings->nb_nps_to_plot);

    EFT_PROF_INFO("[NpRankingPlotter] Sort entries by their impact");

    EFT_PROF_DEBUG("impacts before sorting:");
    for (const auto& res : res_for_plot_after_selector) {
        EFT_PROF_DEBUG("{:30} ==> {:5}", res.name, res.impact);
    }

    std::sort(res_for_plot_after_selector.begin(), res_for_plot_after_selector.end(),
              [&](const NpInfoForPlot& l, const NpInfoForPlot& r)
              {
                return l.impact > r.impact;
              }
              );

    EFT_PROF_DEBUG("impacts after sorting:");
    for (const auto& res : res_for_plot_after_selector) {
        EFT_PROF_DEBUG("{:30} ==> {:5}", res.name, res.impact);
    }

    //EFT_PROF_DEBUG("[NpRankingPlotter] sorte");

//    std::sort(res_for_plot_.begin(), res_for_plot_.end(),
//              [&](const NpInfoForPlot& l, const NpInfoForPlot& r) {
//                  return l.impact < r.impact;
//    });

    auto histo = make_shared<TH1D>("h", "",
                                   settings->nb_nps_to_plot,
                                   0,
                                   settings->nb_nps_to_plot
                                   );

    auto histo_neg = make_shared<TH1D>("h_neg", "",
                                   settings->nb_nps_to_plot,
                                   0,
                                   settings->nb_nps_to_plot
    );

    for (int idx_syst {0}; idx_syst != settings->nb_nps_to_plot; ++idx_syst) {
        EFT_PROF_DEBUG("[NpRankingPlotter]{Plot} set {:3} with name {:40} to {}",
                       idx_syst,
                       res_for_plot_after_selector[idx_syst].name,
                       res_for_plot_after_selector[idx_syst].impact);
        histo->SetBinContent(idx_syst + 1, res_for_plot_after_selector[idx_syst].impact);
        histo_neg->SetBinContent(idx_syst + 1, - res_for_plot_after_selector[idx_syst].impact);
        histo->GetXaxis()->SetBinLabel(idx_syst + 1, res_for_plot_after_selector[idx_syst].name.c_str());
        //EFT_PROF_DEBUG("NpRankingPlotter::Plot set {:2} to {}", idx_syst, res_for_plot_after_selector[idx_syst].impact);
    }

    constexpr float range_high = 0.05f;
    constexpr float range_low  = -0.05f;
    constexpr float scaling = (range_high - range_low) / 2.f;


    histo->GetXaxis()->LabelsOption("v");
    //histo->GetYaxis()->SetRangeUser(-1.5, 1.5);
    histo->GetYaxis()->SetRangeUser(range_low, range_high);

    //histo->SetFillColor(kBlue);
    histo->SetFillColorAlpha(kBlue, 0.6);
    histo->SetLineColor(kBlue);
    histo->SetLineWidth(2);

    histo_neg->SetFillColorAlpha(kBlue, 0.6);
    histo_neg->SetLineColor(kBlue);
    histo_neg->SetLineWidth(2);


    std::filesystem::create_directory("figures");

    auto canvas = std::make_unique<TCanvas>("c", "c", 1200, 800);

    canvas->SetRightMargin(0.05f);
    canvas->SetLeftMargin(0.05f);
    canvas->SetTopMargin(0.05f);
    canvas->SetBottomMargin(0.4f);

    histo->Draw("H TEXT same");
    histo_neg->Draw("H same");

    // lines to show full 1 sigma error
    TLine l1(0, - 1 * scaling, settings->nb_nps_to_plot, - 1 * scaling);
    TLine l2(0, scaling, settings->nb_nps_to_plot, scaling);

    for (auto l : {&l1, &l2}) {
        l->SetLineStyle(kDashed);
        l->SetLineWidth(2);
        l->SetLineColorAlpha(kGray, 0.7f);
        //l->DrawLine(l->GetX1(), l->GetY1(), l->GetX2(), l->GetY2());
        l->Draw("same");
    }

    auto graph_nps_obs = make_shared<TH1D>("h_neg", "",
                                       settings->nb_nps_to_plot,
                                       0,
                                       settings->nb_nps_to_plot
    );
    //auto graph_nps_obs = make_shared<TGraphErrors>(settings->nb_nps_to_plot);
    for (int idx_syst {0}; idx_syst != settings->nb_nps_to_plot; ++idx_syst) {
        EFT_PROF_DEBUG("[NpRankingPlotter]{Plot} set np pull {:3} with name {:40} to {:8} +- {:8}",
                       idx_syst,
                       res_for_plot_after_selector[idx_syst].name,
                       scaling * res_for_plot_after_selector.at(idx_syst).obs_value,
                       scaling * res_for_plot_after_selector.at(idx_syst).obs_error);
        graph_nps_obs->SetBinContent(idx_syst, idx_syst, scaling* res_for_plot_after_selector.at(idx_syst).obs_value);
        graph_nps_obs->SetBinError(idx_syst, idx_syst, scaling * res_for_plot_after_selector.at(idx_syst).obs_error);
    }

    graph_nps_obs->SetLineColorAlpha(kBlack, 0.9);
    graph_nps_obs->Draw("same E");



    canvas->SaveAs("histo.pdf");
}

void NpRankingPlotter::RegisterRes(const NpRankingStudyRes& res) noexcept {
    EFT_PROF_TRACE("[NpPlotter]{RegisterRes} register: {}", res.np_name);
    NpInfoForPlot info;
    info.name = res.np_name;
    info.poi = res.poi_name;
    if (res.prePostFit == PrePostFit::PREFIT) {
        info.post_fit_value = res.np_val;
        info.post_fit_error = res.np_err;
    }
    else if (res.prePostFit == PrePostFit::POSTFIT){
        info.pre_fit_value = res.np_val;
        info.pre_fit_error = res.np_err;
    }
    else
    {
        info.obs_value = res.np_val;
        info.obs_error = res.np_err;
    }

    EFT_PROF_WARN("[NpPlotter]{RegisterRes} put real formulae for  => now we just plot it's error");
    EFT_PROF_WARN("[NpPlotter]{RegisterRes} now we use predef value for");


    //static constexpr float error_full = 0.677982275;
    static constexpr float error_full = 0.0932585782834731;

    if (res.poi_err < error_full)
        info.impact = sqrt( error_full * error_full - res.poi_err * res.poi_err);
    else
        info.impact = 0;

    EFT_PROF_DEBUG("NpRankingPlotter::RegisterRes poi.err: {:5}, full_err: {:5} ==> impact: {:5}",
                   res.poi_err,
                   error_full,
                   info.impact);

    res_for_plot_.push_back(std::move(info));
}


}