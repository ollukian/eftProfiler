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
#include "TGaxis.h"


using namespace std;

namespace eft::plot {

    void NpRankingPlotter::ReadValues(const std::filesystem::path& path)
    {
        namespace fs = std::filesystem;

        if (path.empty()) {
            EFT_PROF_CRITICAL("[NpRankingPlotter][ReadValues] no --input is set: directory to read values from");
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
        //cout << fmt::format("[ReadValuesOneFile] read from {}", filename) << endl;
        const string extension = path.extension().string();
        //cout << fmt::format("[ReadValuesOneFile] extension: [{}]", extension);
        if (extension != ".json") {
            cout << fmt::format(" NOT [.json]") << endl;
            return {};
        }

        //cout << " => is [.json]" << endl;
        //cout << "[ReadValuesOneFile] try to open: " << filename << endl;
        ifstream ifs(filename);
        if ( ! ifs.is_open() ) {
            //cout << "[ReadValuesOneFile] cannot open: " << filename << endl;
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
            j.at("poi_fixed_np_val").get_to(res.poi_fixed_np_val);
            j.at("poi_fixed_np_err").get_to(res.poi_fixed_np_err);
            j.at("np_val").get_to(res.np_val);
            j.at("np_err").get_to(res.np_err);
            if (!j.at("nll").is_null())
                j.at("nll").get_to(res.nll);
        }


        EFT_PROF_INFO("[ReadValueOneFile] read res for poi: {}, np: {}", res.poi_name, res.np_name);
        //cout << setw(4) << j << endl;
        np_study_res_[res.np_name] = res;
        return res;
        //RegisterRes(np_study_res_[res.np_name]);
    }

    void NpRankingPlotter::Plot(const unique_ptr<RankingPlotterSettings>& settings) noexcept
    {

        gStyle->SetOptTitle(0);
        gStyle->SetOptStat(0000000);

        EFT_PROF_TRACE("[NpRankingPlotter]{Plot}");
        EFT_PROF_INFO("[NpRankingPlotter] before selector available {} NP, plot {} out of them",
                      res_for_plot_.size(),
                      settings->top);

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
                      settings->top);

        EFT_PROF_INFO("[NpRankingPlotter] Sort entries by their impact");

        EFT_PROF_DEBUG("impacts before sorting:");
        for (const auto& res : res_for_plot_after_selector) {
            EFT_PROF_DEBUG("{:30} ==> {:5}", res.name, res.impact);
        }

        std::sort(res_for_plot_after_selector.begin(), res_for_plot_after_selector.end(),
                  [&](const NpInfoForPlot& l, const NpInfoForPlot& r)
                  {
                      return ((l.impact_plus_sigma_var * l.impact_plus_sigma_var)
                              +
                              (l.impact_minus_sigma_var * l.impact_minus_sigma_var))
                             >
                             ((r.impact_plus_sigma_var * r.impact_plus_sigma_var)
                              +
                              (r.impact_minus_sigma_var * r.impact_minus_sigma_var));

                      //return l.impact > r.impact;
                  }
        );

        EFT_PROF_DEBUG("impacts after sorting:");
        for (const auto& res : res_for_plot_after_selector) {
            EFT_PROF_DEBUG("{:30} ==> {:5}", res.name, res.impact);
        }


        auto histo = MakeHisto1D("histo", settings->top);
        auto histo_neg = MakeHisto1D("h_neg", settings->top);
        auto histo_plus_sigma_var = MakeHisto1D("h_1sigma_var", settings->top);
        auto histo_minus_sigma_var = MakeHisto1D("h_-1sigma_var", settings->top);
        auto histo_minus_one_var = MakeHisto1D("h_-1_var", settings->top);
        auto histo_plus_one_var = MakeHisto1D("h_+1_var", settings->top);


        for (int idx_syst {0}; idx_syst != settings->top; ++idx_syst) {
            EFT_PROF_DEBUG("[NpRankingPlotter]{Plot} set {:3} with name {:40} to {}",
                           idx_syst,
                           res_for_plot_after_selector[idx_syst].name,
                           res_for_plot_after_selector[idx_syst].impact);
            histo->GetXaxis()->SetBinLabel(idx_syst + 1, res_for_plot_after_selector[idx_syst].name.c_str());

            histo->SetBinContent(idx_syst + 1, res_for_plot_after_selector[idx_syst].impact);
            histo_neg->SetBinContent(idx_syst + 1, - res_for_plot_after_selector[idx_syst].impact);
            histo_plus_sigma_var ->SetBinContent(idx_syst + 1, res_for_plot_after_selector[idx_syst].impact_plus_sigma_var);
            histo_minus_sigma_var->SetBinContent(idx_syst + 1, res_for_plot_after_selector[idx_syst].impact_minus_sigma_var);
            histo_plus_one_var   ->SetBinContent(idx_syst + 1, res_for_plot_after_selector[idx_syst].impact_plus_one_var);
            histo_minus_one_var  ->SetBinContent(idx_syst + 1, res_for_plot_after_selector[idx_syst].impact_minus_one_var);
            //EFT_PROF_DEBUG("NpRankingPlotter::Plot set {:2} to {}", idx_syst, res_for_plot_after_selector[idx_syst].impact);
        }

        constexpr float range_high = 0.002f;
        constexpr float range_low  = -0.002f;
        //constexpr float scaling = (range_high - range_low) / 2.f;
        //const double scaling = abs(res_for_plot_after_selector.at(0).obs_value);
        const float scaling = abs(res_for_plot_after_selector.at(0).impact_plus_one_var);
        //const auto range_high = 1.5f * (res_for_plot_after_selector.at(0).obs_value +  res_for_plot_after_selector.at(0).obs_error);
        //const auto range_high = 1.5f * (res_for_plot_after_selector.at(0).obs_value);
        //const auto range_low = -range_high;

        EFT_PROF_INFO("range_high: {}", range_high);
        EFT_PROF_INFO("range_low: {}", range_low);
        EFT_PROF_INFO("scaling: {}", scaling);
        EFT_PROF_INFO("[0]obs_value = {}", res_for_plot_after_selector.at(0).obs_value);
        EFT_PROF_INFO("[0]impact_plus_sigma_var = {}", res_for_plot_after_selector.at(0).impact_plus_sigma_var);
        EFT_PROF_INFO("[0]impact_minus_sigma_var = {}", res_for_plot_after_selector.at(0).impact_minus_sigma_var);
        EFT_PROF_INFO("[0]impact_plus_one_var = {}", res_for_plot_after_selector.at(0).impact_plus_one_var);
        EFT_PROF_INFO("[0]impact_minus_one_var = {}", res_for_plot_after_selector.at(0).impact_minus_one_var);


        histo->GetXaxis()->LabelsOption("v");
        //histo->GetYaxis()->SetRangeUser(-1.5, 1.5);
        histo->GetYaxis()->SetRangeUser(range_low, range_high);
        histo->GetYaxis()->SetTitle("#Delta #mu");

        //histo->SetFillColor(kBlue);
        histo->SetFillColorAlpha(kGray, 0.6); // used to be blue // gray
        histo->SetLineColor(kGray);
        histo->SetLineWidth(3);

        histo_neg->SetFillColorAlpha(kGray, 0.6); // used to be blue // gray
        histo_neg->SetLineColor(kGray);
        histo_neg->SetLineWidth(3);

        //histo_minus_one_var->SetFillColorAlpha(kGreen, 0.5); // used to be green
        histo_minus_one_var->SetLineColor(kGreen);
        histo_minus_one_var->SetLineWidth(2);

        //histo_plus_one_var->SetFillColorAlpha(kBlue, 0.5); // used to be magenta
        histo_plus_one_var->SetLineColor(kBlue);
        histo_plus_one_var->SetLineWidth(2);

        histo_plus_sigma_var->SetFillColorAlpha(kBlue, 0.6); // used to be red
        histo_plus_sigma_var->SetLineColor(kBlue);
        histo_plus_sigma_var->SetLineWidth(1);

        histo_minus_sigma_var->SetFillColorAlpha(kGreen, 0.6); // used to be violet
        histo_minus_sigma_var->SetLineColor(kGreen);
        histo_minus_sigma_var->SetLineWidth(1);



        auto legend = make_unique<TLegend>(0.7, 0.85, 0.95, 0.95);
        legend->AddEntry(histo.get(), "impact (#delta_{#theta})");
        legend->AddEntry(histo_plus_sigma_var.get(), "+#sigma impact (#theta = #hat{#theta} + #sigma_{#hat{#theta}})");
        legend->AddEntry(histo_minus_sigma_var.get(), "-#sigma impact #theta = #hat{#theta} - #sigma_{#hat{#theta}})");
        legend->AddEntry(histo_plus_one_var.get(), "+1 impact (#theta = #hat{#theta} + 1)");
        legend->AddEntry(histo_minus_one_var.get(), "-1 impact (#theta = #hat{#theta} - 1)");

        std::filesystem::create_directory("figures");

        auto canvas = std::make_unique<TCanvas>("c", "c", 1200, 800);

        canvas->SetRightMargin(0.05f);
        canvas->SetLeftMargin(0.10f);
        canvas->SetTopMargin(0.05f);
        canvas->SetBottomMargin(0.4f);

        histo->GetXaxis()->SetLabelSize(0.02);

        histo->Draw("H same");
        histo_neg->Draw("H same");

        histo_plus_one_var->Draw("H same");
        histo_minus_one_var->Draw("H same");
        histo_plus_sigma_var->Draw("H same");
        histo_minus_sigma_var->Draw("H same");


        // lines to show full 1 sigma error
        TLine l1(0, - 1 * scaling, settings->top, - 1 * scaling);
        TLine l2(0, scaling, settings->top, scaling);

        for (auto l : {&l1, &l2}) {
            l->SetLineStyle(kDashed);
            l->SetLineWidth(1);
            l->SetLineColorAlpha(kGray, 0.9f);
            l->Draw("same");
        }

        auto graph_nps_obs = make_shared<TH1D>("h_nps_obs", "",
                                               settings->top,
                                               0,
                                               settings->top
        );
        //auto graph_nps_obs = make_shared<TGraphErrors>(settings->top);
        for (int idx_syst {0}; idx_syst != settings->top; ++idx_syst) {
            EFT_PROF_DEBUG("[NpRankingPlotter]{Plot} set np pull {:3} with name {:40} to {:8} +- {:8}",
                           idx_syst,
                           res_for_plot_after_selector[idx_syst].name,
                           scaling * res_for_plot_after_selector.at(idx_syst).obs_value,
                           scaling * res_for_plot_after_selector.at(idx_syst).obs_error);
            graph_nps_obs->SetBinContent(idx_syst + 1,res_for_plot_after_selector.at(idx_syst).obs_value);
            graph_nps_obs->SetBinError(idx_syst   + 1,  res_for_plot_after_selector.at(idx_syst).obs_error);
        }

        graph_nps_obs->Scale(scaling);
        //histo_plus_sigma_var->Scale(scaling);
        //histo_minus_sigma_var->Scale(scaling);

        graph_nps_obs->SetLineColorAlpha(kBlack, 0.9);
        graph_nps_obs->SetMarkerStyle(20);
        graph_nps_obs->SetMarkerSize(1); // 2
        //graph_nps_obs->SetLineColorAlpha(kGreen, 0.6);
        graph_nps_obs->SetLineWidth(2); // 4
        graph_nps_obs->Draw("same E1 X0");


        legend->Draw("same");

        // draw second axes for nps
//    auto axis_nps = make_unique<TGaxis>(gPad->GetUxmin(),
//                                        gPad->GetUymin(),
//                                        gPad->GetUxmax(),
//                                        gPad->GetUymax(),
//                                        -1.2f,
//                                        1.2f,
//                                        510,
//                                        "+L");
        //axis_nps->SetLineColor(kRed);
        //axis_nps->SetTextColor(kRed);
        //axis_nps->SetTitle("#hat{#theta}");
        //axis_nps->Draw();

        string name = fmt::format("Impact_{}_{}_nps.pdf", res_for_plot_after_selector[0].poi, settings->top);
        if ( ! settings->ignore_name.empty() )
        {
            string ignore_in_one_string = fmt::format("Ignore_{}_patterns__", settings->ignore_name.size());
            for (const auto& patter : settings->ignore_name)
                ignore_in_one_string += patter + "__";

            name = fmt::format("Impact_{}_{}_nps__{}.pdf",
                               res_for_plot_after_selector[0].poi,
                               settings->top,
                               ignore_in_one_string);
        }

        canvas->SaveAs(std::move(name).c_str());
    }

    void NpRankingPlotter::RegisterRes(const NpRankingStudyRes& res) noexcept {
        EFT_PROF_TRACE("[NpPlotter]{RegisterRes} register: {}", res.np_name);
        auto info = ComputeInfoForPlot(res);


        //EFT_PROF_WARN("[NpPlotter]{RegisterRes} put real formulae for  => now we just plot it's error");
        //EFT_PROF_WARN("[NpPlotter]{RegisterRes} now we use predef value for");


        //static constexpr float error_full = 0.677982275;

//    EFT_PROF_DEBUG("NpRankingPlotter::RegisterRes poi.err: {:5}, full_err: {:5} ==> impact: {:5}",
//                   res.poi_fixed_np_err,
//                   error_full,
//                   info.impact);

        res_for_plot_.push_back(std::move(info));
    }

    NpInfoForPlot NpRankingPlotter::ComputeInfoForPlot(const NpRankingStudyRes& res) noexcept
    {
        NpInfoForPlot info;
        info.name = res.np_name;
        info.poi = res.poi_name;
        /*if (res.prePostFit == PrePostFit::PREFIT) {
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
        }*/

        const auto error_full = res.poi_fixed_np_err;
        //static constexpr float error_full = 0.0932585782834731;
        //auto error_full = res.poi_fixed_np_err;

        if (res.poi_fixed_np_err < error_full)
            info.impact = sqrt( error_full * error_full - res.poi_fixed_np_err * res.poi_fixed_np_err);
        else
            info.impact = 0;

        info.obs_value = res.np_val;
        info.obs_error = res.np_err;

//    EFT_PROF_DEBUG("NpRankingPlotter::ComputeInfoForPlot for {:30} with {} {} {} {} {}",
//                   res.poi_name,
//                   res.poi_fixed_np_val,
//                   res.poi_plus_sigma_variation_val,
//                   res.poi_minus_sigma_variation_val,
//                   res.poi_plus_one_variation_val,
//                   res.poi_minus_one_variation_val);


        info.impact_plus_sigma_var  = res.poi_plus_sigma_variation_val  - res.poi_fixed_np_val;
        info.impact_minus_sigma_var = res.poi_minus_sigma_variation_val - res.poi_fixed_np_val;
        info.impact_plus_one_var    = res.poi_plus_one_variation_val    - res.poi_fixed_np_val;
        info.impact_minus_one_var   = res.poi_minus_one_variation_val   - res.poi_fixed_np_val;

        EFT_PROF_DEBUG("name: {:30}, fixed: {:10}, +sigma: {:10} ==> impact: {:10}",
                       res.np_name,
                       res.poi_fixed_np_val,
                       res.poi_plus_sigma_variation_val,
                       info.impact_plus_sigma_var);

        EFT_PROF_DEBUG("name: {:30}, fixed: {:10}, -sigma: {:10} ==> impact: {:10}",
                       res.np_name,
                       res.poi_fixed_np_val,
                       res.poi_minus_sigma_variation_val,
                       info.impact_minus_sigma_var);

        EFT_PROF_DEBUG("name: {:30}, fixed: {:10}, +1    : {:10} ==> impact: {:10}",
                       res.np_name,
                       res.poi_fixed_np_val,
                       res.poi_plus_one_variation_val,
                       info.impact_plus_one_var);

        EFT_PROF_DEBUG("name: {:30}, fixed: {:10}, -1    : {:10} ==> impact: {:10}",
                       res.np_name,
                       res.poi_fixed_np_val,
                       res.poi_minus_one_variation_val,
                       info.impact_minus_one_var);


        return info;
    }

    std::shared_ptr<TH1D> NpRankingPlotter::MakeHisto1D(const string& name, size_t nb_bins) noexcept {
        return std::make_shared<TH1D>(name.c_str(), name.c_str(),
                                      nb_bins,
                                      0,
                                      nb_bins
        );
    }

    void NpRankingPlotter::ReadSettingsFromCommandLine(CommandLineArgs* cmdLineArgs) {
        EFT_PROF_INFO("NpRankingPlotter::ReadSettingsFromCommandLine");

        np_ranking_settings = make_unique<RankingPlotterSettings>();

        if (cmdLineArgs->SetValIfArgExists("input", np_ranking_settings->input)) {
            EFT_PROF_INFO("Set input: {}", np_ranking_settings->input);
        }
        if (cmdLineArgs->SetValIfArgExists("poi", np_ranking_settings->poi)) {
            EFT_PROF_INFO("Set poi: {}", np_ranking_settings->poi);
        }

        if (cmdLineArgs->SetValIfArgExists("top", np_ranking_settings->top)) {
            EFT_PROF_INFO("Set top: {}", np_ranking_settings->top);
        }

        if (cmdLineArgs->SetValIfArgExists("fileformat", np_ranking_settings->fileformat)) {
            EFT_PROF_INFO("Set fileformat: {}", np_ranking_settings->fileformat);
        }
        if (cmdLineArgs->SetValIfArgExists("ignore_name", np_ranking_settings->ignore_name)) {
            EFT_PROF_INFO("Set ignore_name with: {} elements", np_ranking_settings->ignore_name.size());
            EFT_PROF_INFO("It will modify the callback, by requiring this string not to be present in the filenames");
        }
        if (cmdLineArgs->SetValIfArgExists("match_names", np_ranking_settings->match_names)) {
            EFT_PROF_INFO("Set match_names with: {} elements", np_ranking_settings->match_names.size());
            EFT_PROF_INFO("It will modify the callback, by requiring this string to be present in the filenames");
        }

        vector<EntriesSelector> callbacks;
        if ( ! np_ranking_settings->ignore_name.empty() )
            callbacks.push_back(CreateLambdaForIgnoringNpNames(np_ranking_settings->ignore_name));
        //if ( ! np_ranking_settings->match_names.empty() )
        //    callbacks.push_back(CreateLambdaForMatchingNpNames(np_ranking_settings->match_names));

        //callbacks.push_back([this](const NpInfoForPlot& info) -> bool {
        //   return info.poi == np_ranking_settings->poi;
        //});

        callbacks.push_back(std::move([&](const NpInfoForPlot& info) -> bool {
            EFT_PROF_INFO("callback for gamma for np: {} result: {}", info.name, info.name.find("gamma") == std::string::npos);
            return info.name.find("gamma") == std::string::npos;
        }));

        EFT_PROF_INFO("NpRankingPlotter::ReadSettingsFromCommandLine created with {} callbacks to be joined", callbacks.size());

        SetCallBack(std::move([&](const NpInfoForPlot& info) -> bool
                              {
                                  return std::all_of(callbacks.begin(),
                                                     callbacks.end(),
                                                     [&](const auto& cb) -> bool
                                                     {
                                                         EFT_PROF_INFO("callback for {} res: {}",
                                                                       info.name, cb(info));
                                                         return cb(info);
                                                     });
                              }));

//    SetCallBack([this](const NpInfoForPlot& info) -> bool {
//        if (np_ranking_settings->ignore_name.empty())
//            return info.poi == np_ranking_settings->poi
//                   && (info.name.find("gamma") == std::string::npos);
//        else
//            return info.poi == poi
//                   && (info.name.find("gamma") == std::string::npos)
//                   && (info.name.find(plotter.np_ranking_settings->ignore_name[0]) == std::string::npos);
//    });

    }


}
