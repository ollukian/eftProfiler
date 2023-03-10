//
// Created by Aleksei Lukianchuk on 15-Sep-22.
//

#include <nlohmann/json.hpp>
#include <spdlog/fmt/fmt.h>

#include "../Core/Logger.h"
#include "../Core/Profiler.h"
#include "NpRankingPlotter.h"
#include "../Application/FitManager.h"
#include "../Utils/FileSystemUtils.h"
#include "../Utils/ColourUtils.h"
#include "../Utils/PlotterUtils.h"
#include "../Utils/Scene.h"

#include "test_runner.h"

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
#include "TBox.h"


using namespace std;

namespace eft::plot {

    void NpRankingPlotter::ReadValues(const std::filesystem::path& path)
    {
        EFT_PROFILE_FN();
        namespace fs = std::filesystem;

        if (path.empty()) {
            EFT_PROF_CRITICAL("[NpRankingPlotter][ReadValues] no --input is set: directory to read values from");
            return;
        }
        EFT_PROF_INFO("Read result files from {}", path.string());

        for (const auto& entry : fs::directory_iterator{path}) {
            const auto filenameStr = entry.path().filename().string();
            if (entry.is_directory()) {
                EFT_PROF_INFO("{} is a directory, skip it", filenameStr);
            }
            else if (entry.is_regular_file()) {
                RegisterRes(ReadValuesOneFile(entry));
            }
        }

    }

    NpRankingStudyRes NpRankingPlotter::ReadValuesOneFile(const std::filesystem::path& path)
    {
        EFT_PROFILE_FN();
        const string filename = path.string();
        const string extension = path.extension().string();
        if (extension != ".json") {
            EFT_PROF_WARN("{} NOT [.json]", path.string());
            return {};
        }

        ifstream ifs(filename);
        if ( ! ifs.is_open() ) {
            throw std::runtime_error("error opening: " + filename);
        }

        nlohmann::json j;
        ifs >> j;

        NpRankingStudyRes res;

        try {
            EFT_LOG_DURATION("Reading result from JSON");
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

        EFT_PROF_DEBUG("[ReadValueOneFile] read res for poi: {:10}, np: {:30}", res.poi_name, res.np_name);
        np_study_res_[res.np_name] = res;
        return res;
    }

    void NpRankingPlotter::Plot(const unique_ptr<RankingPlotterSettings>& settings) noexcept
    {
        EFT_PROFILE_FN();
        gStyle->SetOptTitle(0);
        gStyle->SetOptStat(0000000);

        EFT_PROF_TRACE("[NpRankingPlotter]{Plot}");
        EFT_PROF_INFO("[NpRankingPlotter] before selector available {} NP, try to plot {} out of them",
                      res_for_plot_.size(),
                      settings->top);

        EFT_PROF_INFO("Select entries passing the selectors");
        auto res_for_plot_after_selector = GetSelected();

        EFT_PROF_INFO("[NpRankingPlotter] after selector available {} NP, plot {} out of them",
                      res_for_plot_after_selector.size(),
                      settings->top);

        if (res_for_plot_after_selector.empty()) {
            EFT_PROF_ERROR("No entries passing selection is available");
            return;
        }

        EFT_PROF_INFO("[NpRankingPlotter] Sort entries by their impact (quadratic sum of post-fit impacts)");
        SortEntries(res_for_plot_after_selector);

        size_t nb_systematics = settings->top;
        if (nb_systematics > res_for_plot_after_selector.size())
            nb_systematics = res_for_plot_after_selector.size();


        using eft::utils::PlotterUtils;

        bool is_vertical = settings->vertical;
        size_t nb_bins = nb_systematics;
        if (is_vertical)
            nb_bins += settings->empty_bins;

        auto histo                  = PlotterUtils::MakeHisto1D("histo", nb_bins);
        auto histo_neg              = PlotterUtils::MakeHisto1D("h_neg", nb_bins);
        auto histo_plus_sigma_var   = PlotterUtils::MakeHisto1D("h_1sigma_var", nb_bins);
        auto histo_minus_sigma_var  = PlotterUtils::MakeHisto1D("h_-1sigma_var", nb_bins);
        auto histo_minus_one_var    = PlotterUtils::MakeHisto1D("h_-1_var", nb_bins);
        auto histo_plus_one_var     = PlotterUtils::MakeHisto1D("h_+1_var", nb_bins);

        TGaxis::SetMaxDigits(settings->max_digits); // 3

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

            // for vertical plot, a few bins are to be emtpy
            // to save space for labels
            size_t idx_bin = idx_syst;
            if (is_vertical) {
                idx_bin += settings->empty_bins;
            }

            string bin_label = PlotterUtils::GetLabel(settings,
                                                      idx_syst,
                                                      res_for_plot_after_selector[idx_syst].name);

            histo->GetXaxis()->SetBinLabel(idx_bin + 1, bin_label.c_str());

            if (settings->draw_impact) {
                histo->SetBinContent(idx_bin + 1, res_for_plot_after_selector[ idx_syst ].impact);
                histo_neg->SetBinContent(idx_bin + 1, -res_for_plot_after_selector[ idx_syst ].impact);
            }
            histo_plus_sigma_var ->SetBinContent(idx_bin + 1, res_for_plot_after_selector[idx_syst].impact_plus_sigma_var);
            histo_minus_sigma_var->SetBinContent(idx_bin + 1, res_for_plot_after_selector[idx_syst].impact_minus_sigma_var);
            histo_plus_one_var   ->SetBinContent(idx_bin + 1, res_for_plot_after_selector[idx_syst].impact_plus_one_var);
            histo_minus_one_var  ->SetBinContent(idx_bin + 1, res_for_plot_after_selector[idx_syst].impact_minus_one_var);
            //EFT_PROF_DEBUG("NpRankingPlotter::Plot set {:2} to {}", idx_syst, res_for_plot_after_selector[idx_syst].impact);
        }


        float range_high =   1.5f * abs((res_for_plot_after_selector.at(0).impact_plus_one_var));
        float range_low  = - 1.5f * abs((res_for_plot_after_selector.at(0).impact_minus_one_var));

        if (settings->rmuh != 0)
            range_high = settings->rmuh;
        if (settings->rmul != 0)
            range_low  = settings->rmul;


        float scaling = abs(range_high) / 1.5f;
        if (settings->np_scale > 1e-8)
            scaling = settings->np_scale;

        if (is_vertical)
            histo->GetXaxis()->LabelsOption("v");

        histo->GetYaxis()->SetLabelSize(0.025); // 0.04 by default
        histo->GetYaxis()->SetLabelOffset(0.01); // 0.005 by default

        if (is_vertical) {
            int nb_labels = histo->GetYaxis()->GetNdivisions();
            for (int idx_label {0}; idx_label < nb_labels; ++idx_label) {
                histo->GetYaxis()->ChangeLabel(idx_label, 90);
            }
        }

        histo->GetYaxis()->SetRangeUser(range_low, range_high);
        histo->GetYaxis()->SetTitleOffset(settings->mu_offset); // 1.4
        if (settings->mu_latex.empty())
            histo->GetYaxis()->SetTitle(fmt::format("#Delta {}", settings->poi).c_str());
        else
            histo->GetYaxis()->SetTitle(fmt::format("#Delta {}", settings->mu_latex).c_str());

        //histo->SetFillColor(kBlue);
        histo->SetFillColorAlpha(kGray, 0.6); // used to be blue // gray
        histo->SetLineColor(kGray);
        histo->SetLineWidth(3);

        histo_neg->SetFillColorAlpha(kGray, 0.6); // used to be blue // gray
        histo_neg->SetLineColor(kGray);
        histo_neg->SetLineWidth(3);

        //histo_minus_one_var->SetFillColorAlpha(kGreen, 0.5); // used to be green
        histo_minus_one_var->SetLineColor(settings->color_prefit_minus); // kGreen
        histo_minus_one_var->SetLineWidth(2);

        //histo_plus_one_var->SetFillColorAlpha(kBlue, 0.5); // used to be magenta
        histo_plus_one_var->SetLineColor(settings->color_prefit_plus); // kBlue
        histo_plus_one_var->SetLineWidth(2);

        size_t a_plus = utils::ColourUtils::GetColourByIdx(settings->color_postfit_plus).a();
        if (a_plus == 255)
            histo_plus_sigma_var->SetFillColorAlpha(settings->color_postfit_plus, 0.6); // used to be red // 0.6
        else {
            histo_plus_sigma_var->SetFillColorAlpha(settings->color_postfit_plus,
                                                    utils::ColourUtils::
                                                    GetColourByIdx(settings->color_postfit_plus)
                                                    .a_as_fraction()); // used to be red // 0.6
        }
        histo_plus_sigma_var->SetLineColor(settings->color_postfit_plus); // kBlue
        histo_plus_sigma_var->SetLineWidth(1);

        size_t a_minus = utils::ColourUtils::GetColourByIdx(settings->color_postfit_minus).a();
        if (a_plus == 255)
            histo_minus_sigma_var->SetFillColorAlpha(settings->color_postfit_minus, 0.6); // used to be violet // 0.6
        else
            histo_minus_sigma_var->SetFillColorAlpha(settings->color_postfit_minus,
                                                    utils::ColourUtils::
                                                    GetColourByIdx(settings->color_postfit_minus)
                                                            .a_as_fraction()); // used to be red // 0.6
        histo_minus_sigma_var->SetLineColor(settings->color_postfit_minus); // kGreen
        histo_minus_sigma_var->SetLineWidth(1);


        std::filesystem::create_directories(settings->out_dir); // figures -> by default


        auto canvas = eft::utils::draw::Scene::Create(
                settings->plt_size[0],
                settings->plt_size[1]
                );

//        auto canvas = std::make_unique<TCanvas>("c",
//                                                            "c",
//                                                                settings->plt_size[0],
//                                                                settings->plt_size[1]); // 1200 x 800



        if (is_vertical) {
            //if (settings->rmargin == 0.10f) // if they are default
            //    settings->rmargin = 0.20;
            if (settings->bmargin == 0.40f) // if they are default
                settings->bmargin = 0.50;

            if (settings->lmargin == 0.10f) // if they are default
                settings->lmargin = 0.05;

            if (settings->rmargin == 0.10f) // if they are default
                settings->rmargin = 0.05;
            //canvas->SetRightMargin(settings->bmargin);
            //canvas->SetLeftMargin(settings->tmargin);
            //canvas->SetTopMargin(settings->rmargin);
            //canvas->SetBottomMargin(settings->lmargin);
        }

        canvas->SetRightMargin  (settings->rmargin); // 0.10
        canvas->SetLeftMargin   (settings->lmargin); // 0.10
        canvas->SetTopMargin    (settings->tmargin); // 0.05
        canvas->SetBottomMargin (settings->bmargin); // 0.4


        float legend_x_low  = 1.f - settings->rmargin - 0.2f;
        float legend_y_low  = 1.f - settings->tmargin - 0.1f;
        float legend_x_high = 1.f - settings->rmargin;
        float legend_y_high = 1.f - settings->tmargin;

        if (is_vertical) {
            legend_x_low = legend_x_high;
            legend_x_high = legend_x_low + 0.1f;
            legend_y_low = 0.6f;
            legend_y_high = 1.f - settings->tmargin;
        }

        auto legend = make_unique<TLegend>(legend_x_low,
                                           legend_y_low,
                                           legend_x_high,
                                           legend_y_high);
        legend->AddEntry(histo.get(), "impact (#delta_{#theta})");
        legend->AddEntry(histo_plus_sigma_var.get(), "+#sigma impact (#theta = #hat{#theta} + #sigma_{#hat{#theta}})");
        legend->AddEntry(histo_minus_sigma_var.get(), "-#sigma impact #theta = #hat{#theta} - #sigma_{#hat{#theta}})");
        legend->AddEntry(histo_plus_one_var.get(), "+1 impact (#theta = #hat{#theta} + 1)");
        legend->AddEntry(histo_minus_one_var.get(), "-1 impact (#theta = #hat{#theta} - 1)");

        histo->GetXaxis()->SetLabelSize(settings->label_size); // 0.02 by default

        if (settings->h_draw_options.empty()) {
            //if(settings->vertical) {
                histo->Draw("HBAR same");
                histo_neg->Draw("HBAR same");

                histo_plus_one_var->Draw("HBAR same");
                histo_minus_one_var->Draw("HBAR same");
                histo_plus_sigma_var->Draw("HBAR same");
                histo_minus_sigma_var->Draw("HBAR same");
            //} else {
                //histo->Draw("H same");
                //histo_neg->Draw("H same");

                histo_plus_one_var->Draw("H same");
                histo_minus_one_var->Draw("H same");
                histo_plus_sigma_var->Draw("H same");
                histo_minus_sigma_var->Draw("H same");

            if (settings->draw_impact) {
                histo       ->Draw("H same");
                histo_neg   ->Draw("H same");
            }
            //}
        } else {
            string draw_options = StringUtils::Join(' ', settings->h_draw_options);
            draw_options += " same";

            for (auto* h : { &histo,
                                  // &histo_neg,
                                  &histo_plus_sigma_var,
                                  &histo_plus_one_var,
                                  &histo_minus_sigma_var,
                                  &histo_minus_one_var})
            {
                (*h)->Draw(draw_options.c_str());
            } // over all the histograms

            if (settings->draw_impact) {
                histo       ->Draw(draw_options.c_str());
                histo_neg   ->Draw(draw_options.c_str());
            }

        } // if draw options are forces


        // lines to show full 1 sigma error

        float x_low = 0.f;
        if (is_vertical)
            x_low = 2.f;

        TLine l1(x_low, - 1 * scaling, nb_bins, - 1 * scaling);
        TLine l2(x_low, scaling, nb_bins, scaling);

        for (auto l : {&l1, &l2}) {
            l->SetLineStyle(kDashed);
            l->SetLineWidth(1);
            l->SetLineColorAlpha(kGray, 0.9f);
            l->Draw("same");
        }

        auto graph_nps_obs = make_shared<TH1D>("h_nps_obs", "",
                                               nb_bins,
                                               0,
                                               nb_bins
        );
        //auto graph_nps_obs = make_shared<TGraphErrors>(settings->top);
        for (int idx_syst {0}; idx_syst != nb_systematics; ++idx_syst) {
            size_t idx_bin = idx_syst;
            if (is_vertical) {
                idx_bin += settings->empty_bins;
            }
            EFT_PROF_DEBUG("[NpRankingPlotter]{Plot} set np pull {:3} with name {:40} to {:8} +- {:8}",
                           idx_syst,
                           res_for_plot_after_selector[idx_syst].name,
                           scaling * res_for_plot_after_selector.at(idx_syst).obs_value,
                           scaling * res_for_plot_after_selector.at(idx_syst).obs_error);
            graph_nps_obs->SetBinContent(idx_bin + 1,res_for_plot_after_selector.at(idx_syst).obs_value);
            graph_nps_obs->SetBinError(idx_bin   + 1,  res_for_plot_after_selector.at(idx_syst).obs_error);
        }

        cout << "scaling: " << scaling << endl;
        graph_nps_obs->Scale(scaling);
        //histo_plus_sigma_var->Scale(scaling);
        //histo_minus_sigma_var->Scale(scaling);

        graph_nps_obs->SetLineColorAlpha(kBlack, 0.9);
        graph_nps_obs->SetMarkerStyle(20);
        graph_nps_obs->SetMarkerSize(1); // 2
        graph_nps_obs->SetLineWidth(2); // 4
        graph_nps_obs->Draw("same E1 X0");

        if ( ! is_vertical )
            legend->Draw("same");

        nb_bins = nb_systematics;
        if (is_vertical)
            nb_bins += settings->empty_bins;

        // draw second axes for nps
        auto axis_nps = make_unique<TGaxis>(
                                    nb_bins,
                                        - 1 * scaling,
                                        nb_bins,
                                        1 * scaling,
                                        -1.f,
                                        1.f,
                                        510,
                                        "+L");



        if (is_vertical) {
            axis_nps->ChangeLabel(0, 90);
        }
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

        TBox box {0.02f, 0.99 * range_low, (settings->empty_bins - 0.03), 0.99 * range_high};
        box.SetFillColor(kWhite);

        if (is_vertical)
            box.Draw();

        TLatex latex;
        float y = 1.f - settings->tmargin - 0.04f;
        float dy = settings->dy; // 0.03
        float x = 0.02f + settings->lmargin; // 0.12
        float dx = 0;

        if (is_vertical) {
             y = settings->bmargin + 0.02;
             dx = -0.012;
             dy = 0.f;
        }

        latex.SetNDC();
        latex.SetTextSize(0.040); //0.045 is std
        latex.SetTextFont(72);
        latex.SetTextColor(kBlack);

        if (is_vertical)
            latex.SetTextSize(0.025); //0.045 is std

        if (is_vertical)
            latex.SetTextAngle(90);
        EFT_PROF_WARN("latex.DrawLatex(x, y, settings->experiment.c_str()); at {},{}", x, y);
        latex.DrawLatex(x, y, settings->experiment.c_str());
        latex.SetTextFont(settings->text_font); //put back the font 42

        if (is_vertical) {
            latex.DrawLatex(x, y + 0.09, settings->res_status.c_str());
            EFT_PROF_WARN("latex.DrawLatex(x, y, settings->res_status.c_str()); at {}, {}", x, y + 0.09);
        }
        else {
            latex.DrawLatex(x += 0.10, y, settings->res_status.c_str());
            EFT_PROF_WARN("latex.DrawLatex(x, y, settings->res_status.c_str()); at {}, {}", x, y);
        }

        latex.SetTextSize(settings->text_size); // 0.030

        if (is_vertical)
            latex.SetTextSize(0.015); //0.045 is std

        //latex.SetTextSize(0.030); // 0.030
        latex.DrawLatex(x -= dx, y -= dy, "SMEFT, top symmetry");
        EFT_PROF_WARN("latex.DrawLatex(x, y, SMEFT; at {}, {}", x, y);

        string text_ds_energy_lumi = fmt::format("{} (#sqrt{{s}} = {} TeV, {} fb^{{-1}})",
                                                 settings->ds_title,
                                                 settings->energy,
                                                 settings->lumi);

        if (is_vertical) {
            text_ds_energy_lumi = fmt::format("{}", settings->ds_title);
            latex.DrawLatex(x -= dx, y -= dy, text_ds_energy_lumi.c_str());
            EFT_PROF_WARN("latex.DrawLatex(x, y, ds; at {}, {}", x, y);
            text_ds_energy_lumi = fmt::format("(#sqrt{{s}} = {} TeV, {} fb^{{-1}})",
                                              settings->energy,
                                              settings->lumi);
            latex.DrawLatex(x -= dx, y -= dy, text_ds_energy_lumi.c_str());
            EFT_PROF_WARN("latex.DrawLatex(x, y, energy,lumi; at {}, {}", x, y);
        }
        else {
            latex.DrawLatex(x -= dx, y -= dy, text_ds_energy_lumi.c_str());
        }

        string selection_info = fmt::format("Top {} Nuissance parameters", nb_systematics);
        if ( ! settings->match_names.empty() ) {
            selection_info = "NPs: ";
            for (const string& match : settings->match_names) {
                selection_info += R"(")" + match + R"(" )";
            }
        }



        // TODO: wrap by draw legend....
        const float x_start_init = 0.3f;

        float x_start = x_start_init;
        float x_size_one_block = settings->dx_legend; // 0.15
        float dx_between_markers = settings->dx_legend;  // 0.15

        float y_start_multiplier = 0.05f;
        float y_end_multiplier   = 0.25f;
        float dy_text = 0.05f;



        TBox marker_prefit_plus  {x_start, range_high * y_start_multiplier,  x_start += x_size_one_block, range_high * y_end_multiplier};
        TBox marker_prefit_minus {x_start += dx_between_markers, range_high * y_start_multiplier,  x_start += x_size_one_block, range_high * y_end_multiplier};
        TBox marker_posfit_plus {x_start += dx_between_markers, range_high * y_start_multiplier,  x_start += x_size_one_block, range_high * y_end_multiplier};
        TBox marker_posfit_minus {x_start += dx_between_markers, range_high * y_start_multiplier,  x_start += x_size_one_block, range_high * y_end_multiplier};

        marker_prefit_plus. SetLineColor(settings->color_prefit_plus);
        marker_prefit_minus.SetLineColor(settings->color_prefit_minus);
        marker_prefit_plus. SetFillColor(settings->color_prefit_plus);
        marker_prefit_minus.SetFillColor(settings->color_prefit_minus);
        marker_prefit_plus. SetFillStyle(0);
        marker_prefit_minus.SetFillStyle(0);
        marker_prefit_plus. SetLineWidth(1);
        marker_prefit_minus.SetLineWidth(1);


        marker_posfit_plus. SetFillColor(settings->color_postfit_plus);
        marker_posfit_minus.SetFillColor(settings->color_postfit_minus);
        marker_posfit_plus. SetLineColor(settings->color_postfit_plus);
        marker_posfit_minus.SetLineColor(settings->color_postfit_minus);




//        marker_posfit_plus.SetFillColorAlpha(settings->color_postfit_plus,
//                                             utils::ColourUtils::GetColourByIdx(
//                                                     settings->color_postfit_plus)
//                                                     .a_as_fraction()
//        );
//
//        marker_posfit_minus.SetFillColorAlpha(settings->color_postfit_minus,
//                                              utils::ColourUtils::GetColourByIdx(
//                                                      settings->color_postfit_minus)
//                                                      .a_as_fraction()
//        );


        // draw pseudo-legend
        if (is_vertical) {
            marker_prefit_plus.Draw();
            marker_prefit_minus.Draw();
            marker_posfit_plus.Draw();
            marker_posfit_minus.Draw();

            EFT_PROF_WARN("draw latex +1 at {}, {}",x_start_init + dx_between_markers * 0,range_high * (y_end_multiplier + 0.1));
            EFT_PROF_WARN("draw latex -1 at {}, {}",x_start_init + dx_between_markers * 1,range_high * (y_end_multiplier + 0.1));
            EFT_PROF_WARN("draw latex +s at {}, {}",x_start_init + dx_between_markers * 2,range_high * (y_end_multiplier + 0.1));
            EFT_PROF_WARN("draw latex -s at {}, {}",x_start_init + dx_between_markers * 3,range_high * (y_end_multiplier + 0.1));



            latex.SetNDC(false);
            latex.SetTextAlign(12);
            latex.DrawLatex(x_start_init + (dx_between_markers + x_size_one_block) * 0, range_high * (y_end_multiplier + dy_text), "+1 impact (#theta = #hat{#theta} + 1)");
            latex.DrawLatex(x_start_init + (dx_between_markers + x_size_one_block) * 1, range_high * (y_end_multiplier + dy_text), "-1 impact (#theta = #hat{#theta} - 1)");
            latex.DrawLatex(x_start_init + (dx_between_markers + x_size_one_block) * 2, range_high * (y_end_multiplier + dy_text), "+#sigma impact (#theta = #hat{#theta} + #sigma)");
            latex.DrawLatex(x_start_init + (dx_between_markers + x_size_one_block) * 3, range_high * (y_end_multiplier + dy_text), "-#sigma impact (#theta = #hat{#theta} - #sigma)");
            latex.SetNDC(true);
            latex.SetTextAlign(11); // default
        }

        float x_selection_info = 0.35;
        float y_selection_info = y;

        if (is_vertical) {
            x_selection_info = x_start_init + (dx_between_markers + x_size_one_block) * 4;
            y_selection_info = range_high * (y_start_multiplier);
            latex.SetNDC(false);
            latex.SetTextAlign(12);
            latex.DrawLatex(x_selection_info, y_selection_info, selection_info.c_str());
            latex.SetNDC(true);
            latex.SetTextAlign(11); // default
        } else {
            latex.DrawLatex(x_selection_info, y_selection_info, selection_info.c_str());
        }


        EFT_PROF_WARN("latex.DrawLatex(x, y, selection_info; at {}, {}", x_selection_info, y_selection_info);


        if (settings->mu_latex.empty())
            latex.DrawLatex(x -= dx, y -= dy, settings->poi.c_str());
        else
            latex.DrawLatex(x -= dx, y -= dy, settings->mu_latex.c_str());

        const string stem_name = eft::utils::PlotterUtils::FormName(settings);
        for (const std::string& fileformat : settings->fileformat) {
            string name = stem_name + '.' + fileformat;
            eft::utils::draw::Scene::SaveAs(settings->out_dir + "//" + name);
        }
    }

    void NpRankingPlotter::RegisterRes(const NpRankingStudyRes& res) noexcept {
        EFT_PROFILE_FN();
        EFT_PROF_TRACE("[NpPlotter]{RegisterRes} register: {}", res.np_name);
        auto info = ComputeInfoForPlot(res);
        res_for_plot_.push_back(std::move(info));
    }

    NpInfoForPlot NpRankingPlotter::ComputeInfoForPlot(const NpRankingStudyRes& res) noexcept
    {
        EFT_PROFILE_FN();
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

        EFT_PROF_TRACE("name: {:30}, fixed: {:10}, np val: {:10} ==> np error: {:10}",
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

    void NpRankingPlotter::ReadSettingsFromCommandLine(CommandLineArgs* cmdLineArgs) {
        EFT_PROFILE_FN();
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
        EFT_GET_FROM_CONFIG(config, np_ranking_settings, h_draw_options);
        EFT_GET_FROM_CONFIG(config, np_ranking_settings, dx_legend);
        EFT_GET_FROM_CONFIG(config, np_ranking_settings, dy_legend);
        EFT_GET_FROM_CONFIG(config, np_ranking_settings, empty_bins);
        EFT_GET_FROM_CONFIG(config, np_ranking_settings, max_digits);
#undef EFT_GET_FROM_CONFIG
#endif


        np_ranking_settings->replacements = eft::utils::PlotterUtils::ParseReplacements(config.replace);

#ifndef EFT_PROCESS_COLOUR
#define EFT_PROCESS_COLOUR(cfg, settings, colour_str) \
    settings->colour_str = utils::ColourUtils::GetColourFromString(config.colour_str);

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
            bool res = info.name.find("gamma_stat") == std::string::npos;
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

void NpRankingPlotter::ReadNpNamesFromFile(const std::string& path) const
{
    EFT_PROFILE_FN();
    EFT_PROF_INFO("Read np names from: {}", path);
    np_ranking_settings->np_names = utils::FileSystemUtils::ReadLines(path).value();
}

vector<NpInfoForPlot>
NpRankingPlotter::GetSelected(const vector<NpInfoForPlot>& entries,
                              const NpRankingPlotter::EntriesSelector& selector) noexcept
{
    EFT_PROFILE_FN();
    ASSERT_NOT_EQUAL(entries.size(), 0);
    vector<stats::NpInfoForPlot> res_for_plot_after_selector;
    res_for_plot_after_selector.reserve(entries.size());

    {
        EFT_LOG_DURATION("Selecting results");
        std::copy_if(entries.begin(),
                     entries.end(),
                     std::back_inserter(res_for_plot_after_selector),
                     [ & ](const NpInfoForPlot& info) {
                         bool res = selector(info);
                         if(res) {
                             EFT_PROF_INFO("{:70} for poi: {} passes      name selection",
                                           info.name,
                                           info.poi);
                         } else {
                             EFT_PROF_WARN("{:70} for poi: {} DOESNT pass name selection",
                                           info.name,
                                           info.poi);
                         }
                         return res;
                     }
        );
    }
    res_for_plot_after_selector.shrink_to_fit();
    return res_for_plot_after_selector;
}

vector<NpInfoForPlot> NpRankingPlotter::GetSelected(const vector<NpInfoForPlot>& entries) const noexcept {
    EFT_PROFILE_FN();
    return GetSelected(entries, callback_);
}

vector<NpInfoForPlot> NpRankingPlotter::GetSelected(const EntriesSelector& selector) const noexcept {
    EFT_PROFILE_FN();
    return GetSelected(res_for_plot_, selector);
//    ASSERT_NOT_EQUAL(res_for_plot_.size(), 0);
//
//    vector<stats::NpInfoForPlot> res_for_plot_after_selector;
//    res_for_plot_after_selector.reserve(res_for_plot_.size());
//
//    size_t total_nb_systematics_in_folder = res_for_plot_.size();
//
//    {
//        EFT_LOG_DURATION("Selecting results");
//        std::copy_if(res_for_plot_.begin(),
//                     res_for_plot_.end(),
//                     std::back_inserter(res_for_plot_after_selector),
//                     [ & ](const NpInfoForPlot& info) {
//                         bool res = selector(info);
//                         if(res) {
//                             EFT_PROF_INFO("{:70} for poi: {} passes      name selection",
//                                           info.name,
//                                           info.poi);
//                         } else {
//                             EFT_PROF_WARN("{:70} for poi: {} DOESNT pass name selection",
//                                           info.name,
//                                           info.poi);
//                         }
//                         return res;
//                     }
//        );
//    }
//    res_for_plot_after_selector.shrink_to_fit();
//    return res_for_plot_after_selector;
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
    EFT_PROF_CRITICAL("NpRankingPlotter::RemovePrefix(string& s, const vector<string>& prefixes) is not implemented");
}
string NpRankingPlotter::RemovePrefixCopy(std::string s, const std::vector<std::string>& prefixes)
{
    EFT_PROF_CRITICAL("NpRankingPlotter::RemovePrefixCopy(std::string s, const std::vector<std::string>& prefixes) is not implemented");
}

}
