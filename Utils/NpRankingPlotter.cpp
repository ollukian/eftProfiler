//
// Created by Aleksei Lukianchuk on 15-Sep-22.
//

#include <nlohmann/json.hpp>
#include <spdlog/fmt/fmt.h>

#include "../Core/Logger.h"
#include "NpRankingPlotter.h"
#include "../Application/FitManager.h"
#include "../Utils/FileSystemUtils.h"
#include "../Utils/ColourUtils.h"

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
        EFT_PROF_INFO("Read result files from {}", path.string());
        //cout << "[NpRankingPlotter] read values from: " << path.string() << endl;

        for (const auto& entry : fs::directory_iterator{path}) {
            const auto filenameStr = entry.path().filename().string();
            //cout << filenameStr;
            if (entry.is_directory()) {
                EFT_PROF_INFO("{} is a directory, skip it", filenameStr);
            }
            else if (entry.is_regular_file()) {
                //EFT_PROF_DEBUG();
                //cout << " ==> is a regular file, try to parse it" << endl;
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

        if (res.np_name == "none")
        {
            EFT_PROF_INFO("[ReadValueOneFile] read res for poi: {:10}, np: {:30} => skip not constrained", res.poi_name, res.np_name);
        }

        EFT_PROF_INFO("[ReadValueOneFile] read res for poi: {:10}, np: {:30}", res.poi_name, res.np_name);
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

        // TODO: wrap by "GetSelected"
        std::copy_if(res_for_plot_.begin(),
                     res_for_plot_.end(),
                     std::back_inserter(res_for_plot_after_selector),
                     [&](const NpInfoForPlot& info) {
                         bool res = callback_(info);
                         EFT_PROF_INFO("callback [{:12}][{:10}] for POI: {:10}, np: {:20} result: {}",
                                       "overall",
                                       info.poi,
                                       info.name,
                                       res);
                         return res;
                     }
        );

        EFT_PROF_INFO("[NpRankingPlotter] after selector available {} NP, plot {} out of them",
                      res_for_plot_after_selector.size(),
                      settings->top);

        if (res_for_plot_after_selector.empty()) {
            EFT_PROF_ERROR("No entries passing selection is available");
            return;
        }

        EFT_PROF_INFO("[NpRankingPlotter] Sort entries by their impact");

//        EFT_PROF_DEBUG("impacts before sorting:");
//        for (const auto& res : res_for_plot_after_selector) {
//            EFT_PROF_DEBUG("{:30} ==> {:5}", res.name, res.impact);
//        }

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

                  }
        );

        EFT_PROF_DEBUG("impacts after sorting:");
        for (const auto& res : res_for_plot_after_selector) {
            EFT_PROF_DEBUG("{:30} ==> {:5}", res.name, res.impact);
        }

        size_t nb_systematics = settings->top;
        if (nb_systematics > res_for_plot_after_selector.size())
            nb_systematics = res_for_plot_after_selector.size();

        auto histo = MakeHisto1D("histo", nb_systematics);
        auto histo_neg = MakeHisto1D("h_neg", nb_systematics);
        auto histo_plus_sigma_var = MakeHisto1D("h_1sigma_var", nb_systematics);
        auto histo_minus_sigma_var = MakeHisto1D("h_-1sigma_var", nb_systematics);
        auto histo_minus_one_var = MakeHisto1D("h_-1_var", nb_systematics);
        auto histo_plus_one_var = MakeHisto1D("h_+1_var", nb_systematics);

        if ( ! settings->np_names.empty() ) {
            if (settings->np_names.size() != nb_systematics) {
                EFT_PROF_CRITICAL("Number of provided systematics names: {} doesn't match the amount to be plot: {}",
                                  settings->np_names.size(),
                                  nb_systematics);
                return;
            }
        }

        // idx_syst -> int, because TH1D::SetBinContent requires int as an argument
        // no idea why not size_t
        for (int idx_syst {0}; idx_syst != nb_systematics; ++idx_syst) {
            EFT_PROF_DEBUG("[NpRankingPlotter]{Plot} set {:3} with name {:40} to {}",
                           idx_syst,
                           res_for_plot_after_selector[idx_syst].name,
                           res_for_plot_after_selector[idx_syst].impact);

            string bin_label;
            if (settings->np_names.empty()) {
                bin_label = res_for_plot_after_selector[ idx_syst ].name;
                if(!settings->replacements.empty())
                    ReplaceStrings(bin_label, settings->replacements);
                if(!settings->remove_prefix.empty())
                    RemovePrefix(bin_label, settings->remove_prefix);
                if(!settings->remove_suffix.empty())
                    RemoveSuffix(bin_label, settings->remove_suffix);
            }
            else {
                bin_label = settings->np_names.at(idx_syst);
            }

            histo->GetXaxis()->SetBinLabel(idx_syst + 1, bin_label.c_str());

            histo->SetBinContent(idx_syst + 1, res_for_plot_after_selector[idx_syst].impact);
            histo_neg->SetBinContent(idx_syst + 1, - res_for_plot_after_selector[idx_syst].impact);
            histo_plus_sigma_var ->SetBinContent(idx_syst + 1, res_for_plot_after_selector[idx_syst].impact_plus_sigma_var);
            histo_minus_sigma_var->SetBinContent(idx_syst + 1, res_for_plot_after_selector[idx_syst].impact_minus_sigma_var);
            histo_plus_one_var   ->SetBinContent(idx_syst + 1, res_for_plot_after_selector[idx_syst].impact_plus_one_var);
            histo_minus_one_var  ->SetBinContent(idx_syst + 1, res_for_plot_after_selector[idx_syst].impact_minus_one_var);
            //EFT_PROF_DEBUG("NpRankingPlotter::Plot set {:2} to {}", idx_syst, res_for_plot_after_selector[idx_syst].impact);
        }

        const float range_high = settings->rmuh; //  0.002f;
        const float range_low  = settings->rmul; // -0.002
        //constexpr float scaling = (range_high - range_low) / 2.f;
        //const double scaling = abs(res_for_plot_after_selector.at(0).obs_value);
        float scaling = abs(res_for_plot_after_selector.at(0).impact_plus_one_var);
        if (settings->np_scale > 1E-9)
            scaling = settings->np_scale;
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
        histo->GetYaxis()->SetTitleOffset(settings->mu_offset); // 1.4
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



        auto legend = make_unique<TLegend>(1 - settings->rmargin - 0.2,
                                           1 - settings->tmargin - 0.1,
                                           1 - settings->rmargin,
                                           1 - settings->tmargin);
        legend->AddEntry(histo.get(), "impact (#delta_{#theta})");
        legend->AddEntry(histo_plus_sigma_var.get(), "+#sigma impact (#theta = #hat{#theta} + #sigma_{#hat{#theta}})");
        legend->AddEntry(histo_minus_sigma_var.get(), "-#sigma impact #theta = #hat{#theta} - #sigma_{#hat{#theta}})");
        legend->AddEntry(histo_plus_one_var.get(), "+1 impact (#theta = #hat{#theta} + 1)");
        legend->AddEntry(histo_minus_one_var.get(), "-1 impact (#theta = #hat{#theta} - 1)");

        std::filesystem::create_directory(settings->out_dir); // figures -> by default

        auto canvas = std::make_unique<TCanvas>("c",
                                                            "c",
                                                                settings->plt_size[0],
                                                                settings->plt_size[1]); // 1200 x 800

        canvas->SetRightMargin  (settings->rmargin); // 0.10
        canvas->SetLeftMargin   (settings->lmargin); // 0.10
        canvas->SetTopMargin    (settings->tmargin); // 0.05
        canvas->SetBottomMargin (settings->bmargin); // 0.4

        histo->GetXaxis()->SetLabelSize(settings->label_size); // 0.02 by default

        histo->Draw("H same");
        histo_neg->Draw("H same");

        histo_plus_one_var->Draw("H same");
        histo_minus_one_var->Draw("H same");
        histo_plus_sigma_var->Draw("H same");
        histo_minus_sigma_var->Draw("H same");


        // lines to show full 1 sigma error
        TLine l1(0, - 1 * scaling, nb_systematics, - 1 * scaling);
        TLine l2(0, scaling, nb_systematics, scaling);

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
        for (int idx_syst {0}; idx_syst != nb_systematics; ++idx_syst) {
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
        graph_nps_obs->SetLineWidth(2); // 4
        graph_nps_obs->Draw("same E1 X0");

        legend->Draw("same");

        // draw second axes for nps
        auto axis_nps = make_unique<TGaxis>(
                                        nb_systematics,
                                        - 1 * scaling,
                                        nb_systematics,
                                        1 * scaling,
                                        -1.f,
                                        1.f,
                                        510,
                                        "+L");
        //axis_nps->SetLineColor(kRed);
        //axis_nps->SetTextColor(kRed);
        axis_nps->SetTitle("#hat{#theta} - #theta_{0}");
        //axis_nps->SetTextSize(0.5);
        //axis_nps->SetLabelColor(kRed);
        axis_nps->SetLabelFont(histo->GetLabelFont());
        axis_nps->SetLabelSize(0.02);
        axis_nps->SetTitleOffset(settings->np_offset); // 1.0
        axis_nps->SetTitleSize(0.02);
        axis_nps->Draw();

        TLatex latex;
        float y = 1.f - settings->tmargin - 0.04f;
        float dy = settings->dy; // 0.03
        float x = 0.02f + settings->lmargin; // 0.12
        latex.SetNDC();
        latex.SetTextSize(0.040); //0.045 is std
        latex.SetTextFont(72);
        latex.SetTextColor(kBlack);
        latex.DrawLatex(x, y, settings->experiment.c_str());
        latex.SetTextFont(settings->text_font); //put back the font 42
        latex.DrawLatex(x + 0.10, y, settings->res_status.c_str());

        latex.SetTextSize(settings->text_size); // 0.030
        //latex.SetTextSize(0.030); // 0.030
        latex.DrawLatex(x, y - dy, "SMEFT, top symmetry");

        string text_ds_energy_lumi = fmt::format("{} (#sqrt{{s}} = {} TeV, {} fb^{{-1}})",
                                                 settings->ds_title,
                                                 settings->energy,
                                                 settings->lumi);

        latex.DrawLatex(x, y - 2 * dy, text_ds_energy_lumi.c_str());

        string selection_info = "All nuissance parameters";
        if ( ! settings->match_names.empty() ) {
            selection_info = "Group of Nuissance parameters: ";
            for (const string& match : settings->match_names) {
                selection_info += match + " ";
            }
        }

        latex.DrawLatex(0.35, y, selection_info.c_str());

        if (settings->mu_latex.empty())
            latex.DrawLatex(x, y - 3 * dy, settings->poi.c_str());
        else
            latex.DrawLatex(x, y - 3 * dy, settings->mu_latex.c_str());


        string ignore_part;
        string select_part;

        if ( ! settings->ignore_name.empty() )
        {
            string ignore_in_one_string = fmt::format("Ignore_{}_patterns__", settings->ignore_name.size());
            for (const auto& patter : settings->ignore_name)
                ignore_in_one_string += patter + "__";

            ignore_part = "__" + ignore_in_one_string.substr(0, ignore_in_one_string.size() - 2);
            //name = fmt::format("Impact_{}_{}_nps__{}.pdf",
            //                   res_for_plot_after_selector[0].poi,
            //                   settings->top,
            //                   ignore_in_one_string);
        }

        if ( ! settings->match_names.empty() )
        {
            string matches_in_one_string = fmt::format("Select_{}_patterns__", settings->match_names.size());
            for (const auto& patter : settings->match_names)
                matches_in_one_string += patter + "__";

            select_part = "__" + matches_in_one_string.substr(0, matches_in_one_string.size() - 2);
            //name = fmt::format("Impact_{}_{}_nps__{}.pdf",
            //                   res_for_plot_after_selector[0].poi,
            //                   settings->top,
            //                   matches_in_one_string);
        }

        std::string stem_name;
        if (settings->output.empty())
            stem_name = fmt::format("Impact_{}_{}_nps{}{}",
                                    settings->poi,
                                    settings->top,
                                    select_part,
                                    ignore_part);
        else
            stem_name = settings->output;


        for (const std::string& fileformat : settings->fileformat) {
            string name = stem_name + '.' + fileformat;

            canvas->SaveAs(std::move(name).c_str());
        }
    }

    void NpRankingPlotter::RegisterRes(const NpRankingStudyRes& res) noexcept {
        EFT_PROF_TRACE("[NpPlotter]{RegisterRes} register: {}", res.np_name);
        auto info = ComputeInfoForPlot(res);
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

        const auto error_full = res.poi_free_fit_err;
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

        EFT_PROF_DEBUG("name: {:30}, fixed: {:10}, np val: {:10} ==> np error: {:10}",
                       res.np_name,
                       res.poi_fixed_np_val,
                       res.np_val,
                       res.np_err);

//        EFT_PROF_DEBUG("name: {:30}, fixed: {:10}, +sigma: {:10} ==> impact: {:10}",
//                       res.np_name,
//                       res.poi_fixed_np_val,
//                       res.poi_plus_sigma_variation_val,
//                       info.impact_plus_sigma_var);
//
//        EFT_PROF_DEBUG("name: {:30}, fixed: {:10}, -sigma: {:10} ==> impact: {:10}",
//                       res.np_name,
//                       res.poi_fixed_np_val,
//                       res.poi_minus_sigma_variation_val,
//                       info.impact_minus_sigma_var);
//
//        EFT_PROF_DEBUG("name: {:30}, fixed: {:10}, +1    : {:10} ==> impact: {:10}",
//                       res.np_name,
//                       res.poi_fixed_np_val,
//                       res.poi_plus_one_variation_val,
//                       info.impact_plus_one_var);
//
//        EFT_PROF_DEBUG("name: {:30}, fixed: {:10}, -1    : {:10} ==> impact: {:10}",
//                       res.np_name,
//                       res.poi_fixed_np_val,
//                       res.poi_minus_one_variation_val,
//                       info.impact_minus_one_var);


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

        eft::stats::FitManagerConfig config;
        eft::stats::FitManager::ReadConfigFromCommandLine(*cmdLineArgs, config);

        np_ranking_settings = make_unique<RankingPlotterSettings>();

#ifndef EFT_GET_FROM_CONFIG
#define EFT_GET_FROM_CONFIG(config, settings, param) \
    settings->param = config.param;

        EFT_GET_FROM_CONFIG(config, np_ranking_settings, fileformat);
        EFT_GET_FROM_CONFIG(config, np_ranking_settings, top);
        EFT_GET_FROM_CONFIG(config, np_ranking_settings, ignore_name);
        EFT_GET_FROM_CONFIG(config, np_ranking_settings, match_names);
        EFT_GET_FROM_CONFIG(config, np_ranking_settings, poi);
        EFT_GET_FROM_CONFIG(config, np_ranking_settings, color_np);
        EFT_GET_FROM_CONFIG(config, np_ranking_settings, lmargin);
        EFT_GET_FROM_CONFIG(config, np_ranking_settings, rmargin);
        EFT_GET_FROM_CONFIG(config, np_ranking_settings, tmargin);
        EFT_GET_FROM_CONFIG(config, np_ranking_settings, bmargin);
        EFT_GET_FROM_CONFIG(config, np_ranking_settings, plt_size);
        EFT_GET_FROM_CONFIG(config, np_ranking_settings, rmul);
        EFT_GET_FROM_CONFIG(config, np_ranking_settings, rmuh);
        EFT_GET_FROM_CONFIG(config, np_ranking_settings, np_scale);
        EFT_GET_FROM_CONFIG(config, np_ranking_settings, vertical);
        EFT_GET_FROM_CONFIG(config, np_ranking_settings, output);
        EFT_GET_FROM_CONFIG(config, np_ranking_settings, out_dir);
        EFT_GET_FROM_CONFIG(config, np_ranking_settings, input);
        EFT_GET_FROM_CONFIG(config, np_ranking_settings, np_scale);
        EFT_GET_FROM_CONFIG(config, np_ranking_settings, label_size);
        EFT_GET_FROM_CONFIG(config, np_ranking_settings, remove_prefix);
        EFT_GET_FROM_CONFIG(config, np_ranking_settings, remove_suffix);
        EFT_GET_FROM_CONFIG(config, np_ranking_settings, ds_title);
        EFT_GET_FROM_CONFIG(config, np_ranking_settings, energy);
        EFT_GET_FROM_CONFIG(config, np_ranking_settings, lumi);
        EFT_GET_FROM_CONFIG(config, np_ranking_settings, experiment);
        EFT_GET_FROM_CONFIG(config, np_ranking_settings, res_status);
        EFT_GET_FROM_CONFIG(config, np_ranking_settings, np_offset);
        EFT_GET_FROM_CONFIG(config, np_ranking_settings, mu_offset);
        EFT_GET_FROM_CONFIG(config, np_ranking_settings, mu_latex);
        EFT_GET_FROM_CONFIG(config, np_ranking_settings, np_names);
        EFT_GET_FROM_CONFIG(config, np_ranking_settings, text_size);
        EFT_GET_FROM_CONFIG(config, np_ranking_settings, text_font);
        EFT_GET_FROM_CONFIG(config, np_ranking_settings, add_text);
        EFT_GET_FROM_CONFIG(config, np_ranking_settings, add_text_ndc);
        EFT_GET_FROM_CONFIG(config, np_ranking_settings, dy);
#undef EFT_GET_FROM_CONFIG
#endif

        np_ranking_settings->replacements = ParseReplacements(config.replace);

#ifndef EFT_PROCESS_COLOUR
#define EFT_PROCESS_COLOUR(cfg, settings, colour_str) \
    settings->colour_str = utils::ColourUtils::RegisterColourFromString(config.colour_str);

        EFT_PROCESS_COLOUR(config, np_ranking_settings, color_prefit_minus );
        EFT_PROCESS_COLOUR(config, np_ranking_settings, color_prefit_plus );
        EFT_PROCESS_COLOUR(config, np_ranking_settings, color_postfit_minus );
        EFT_PROCESS_COLOUR(config, np_ranking_settings, color_postfit_plus );
#endif
#undef EFT_PROCESS_COLOUR


        if (np_ranking_settings->np_names.size() == 1) {
            const string& np_names_string = np_ranking_settings->np_names.at(0);
            const auto pos_filename = np_names_string.find("file:");
            if (pos_filename != std::string::npos)
            {
                ReadNpNamesFromFile(np_names_string.substr(pos_filename + 1, np_names_string.length()));
            }
        }

        vector<EntriesSelector> callbacks;
        if ( ! np_ranking_settings->ignore_name.empty() )
            callbacks.emplace_back(CreateLambdaForIgnoringNpNames(np_ranking_settings->ignore_name));
        if ( ! np_ranking_settings->match_names.empty() )
            callbacks.emplace_back(CreateLambdaForMatchingNpNames(np_ranking_settings->match_names));

        // TODO: add function: add callback poi match
        callbacks.emplace_back([this](const NpInfoForPlot& info) -> bool {
            bool res = (info.poi == np_ranking_settings->poi);
            EFT_PROF_DEBUG("callback [{:12}][{:10}] for POI: {:10}, np: {:20} result: {}",
                           "poi match",
                           np_ranking_settings->poi,
                           info.poi,
                           info.name,
                           res);
            return res;
        });

        callbacks.emplace_back(std::move([&](const NpInfoForPlot& info) -> bool {
            bool res = info.name.find("gamma") == std::string::npos;
            EFT_PROF_DEBUG("callback [{:12}][{:10}] for POI: {:10}, np: {:20} result: {}",
                           "no gamma",
                           info.poi,
                           info.name,
                           res);
            return res;
        }));

        EFT_PROF_INFO("NpRankingPlotter::ReadSettingsFromCommandLine created with {} callbacks to be joined", callbacks.size());

        SetCallBack(std::move([callbacks](const NpInfoForPlot& info) -> bool
                              {
                                  //EFT_PROF_INFO("Global callback for {} with {} components",
                                  //              info.name, callbacks.size());
                                  return std::all_of(callbacks.begin(),
                                                     callbacks.end(),
                                                     [&](const auto& cb) -> bool
                                                     {
                                                         //EFT_PROF_INFO("callback for {} res: {}",
                                                         //              info.name, cb(info));
                                                         return cb(info);
                                                     });
                              }));



    }


std::vector<NpRankingPlotter::Replacement>
NpRankingPlotter::ParseReplacements(const std::vector<std::string>& replacements)
{
    // convert vector of "key:val" to vector of pairs: <key, val>
    EFT_PROF_TRACE("ParseReplacements for {} objects", replacements.size());
    vector<Replacement> res;
    res.reserve(replacements.size());
    for (const auto& raw : replacements)
    {
        EFT_PROF_DEBUG("Extract replacement from: {}", raw);
        auto pos_separator = raw.find(':');
        if (pos_separator == std::string::npos) {
            throw std::logic_error(fmt::format("Cannot parse replacement string {}, {}",
                                               raw,
                                               ". Must have format: 'key1:val1, key2:val2, ...'"));

        }

        std::string key = raw.substr(0, pos_separator);
        std::string val = raw.substr(pos_separator + 1, raw.length());
        res.emplace_back(std::move(key), std::move(val));
    }

    for (const auto& key_val : res) {
        EFT_PROF_DEBUG("Add replacing: {:10} ==> {:10}", key_val.first, key_val.second);
    }

    return res;
}

void NpRankingPlotter::ReplaceStrings(std::string& s, const std::vector<Replacement>& replacements)
{
    for (const auto& replacement : replacements) {
        EFT_PROF_DEBUG("Replace {} using {:10} -> {:10} replacement", s, replacement.first, replacement.second);
        StringUtils::Replace(s, replacement.first, replacement.second);
    }
}

void NpRankingPlotter::RemovePrefix(string& s, const vector<string>& prefixes)
{
    for (const auto& prefix : prefixes) {
        EFT_PROF_DEBUG("Remove prefix: {:10} from {:10}", prefix, s);
        StringUtils::RemovePrefix(s, prefix);
    }
}
void NpRankingPlotter::RemoveSuffix(string& s, const vector<string>& suffixes)
{
    for (const auto& suffix : suffixes) {
        EFT_PROF_DEBUG("Remove suffix: {:10} from {:10}", suffix, s);
        StringUtils::RemoveSuffix(s, suffix);
    }
}

void NpRankingPlotter::ReadNpNamesFromFile(const std::string& path) const
{
    EFT_PROF_INFO("Read np names from: {}", path);
    np_ranking_settings->np_names = utils::FileSystemUtils::ReadLines(path).value();
}

}
