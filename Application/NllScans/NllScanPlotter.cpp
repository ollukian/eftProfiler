//
// Created by Aleksei Lukianchuk on 15-Jan-23.
//

#include "NllScanPlotter.h"

#include "nlohmann/json.hpp"

#include <fstream>
#include <iostream>

#include "TMultiGraph.h"
#include "TGraph.h"
#include "TCanvas.h"
#include "TLatex.h"
#include "TLegend.h"
#include "TLine.h"
#include "TAxis.h"

using namespace std;
namespace fs =  std::filesystem;

namespace eft::stats::scans {

void NllScanPlotter::ReadFiles(std::filesystem::path& path) {
    EFT_PROFILE_FN();
    EFT_PROF_INFO("NllScanPlotter: read files from: {}", path.string());


    if(path.empty()) {
        EFT_PROF_CRITICAL("[NpRankingPlotter][ReadValues] no --input is set: directory to read values from");
        return;
    }
    EFT_PROF_INFO("Read result files from {}", path.string());

    for (const auto& entry: fs::directory_iterator{path}) {
        const auto filenameStr = entry.path().filename().string();
        if(entry.is_directory()) {
            EFT_PROF_INFO("{} is a directory, skip it", filenameStr);
        } else if(entry.is_regular_file()) {
            RegisterRes(ReadValuesOneFile(entry));
        }
    }
}

void NllScanPlotter::RegisterRes(NllScanResult nllScanRes) {
    EFT_PROFILE_FN();
    if (nllScanRes.nll_val == 0) {
        EFT_PROF_WARN("Skip failed result");
        return;
    }
    if (nllScanRes.poi_configs.size() == 1) {
        RegisterRes1D(std::move(nllScanRes));
    }
    else {
        EFT_PROF_CRITICAL("NllScanPlotter::RegisterRes for {}D case is not supported yet",
                          nllScanRes.poi_configs.size());
        //throw std::runtime_error("");
    }
}

void NllScanPlotter::RegisterRes1D(NllScanResult nllScanRes) {
    EFT_PROFILE_FN();
    const string& poi = nllScanRes.poi_configs[0].Name();
    if (nllScanRes.fit_status != 0) {
        EFT_PROF_WARN("NllScanPlotter: skip 1D - fit_status: |{}|. Poi: [{}] at [{:4}]",
                      nllScanRes.fit_status,
                      poi,
                      nllScanRes.poi_configs[0].Value());
        return;
    }
    EFT_PROF_DEBUG("NllScanPlotter: register 1D. Poi: [{}] at [{:4}] with nll: {}",
                   poi,
                   nllScanRes.poi_configs[0].Value(),
                   nllScanRes.nll_val);
    // not sure these guys are needed....
    results1D_per_poi_[poi].insert(nllScanRes);
    results1D_.insert(std::move(nllScanRes));
    //results1D_per_poi_[poi].insert(nllScanRes);
    //results1D_.insert(std::move(nllScanRes));
}

NllScanPlotter::Nll1Dresults NllScanPlotter::GetSelectedEntries(const string& mu) {
    EFT_PROFILE_FN();
    EFT_PROF_INFO("NllScanPlotter::GetSelectedEntries based only on POI name: {}", mu);
    NllScanPlotter::Nll1Dresults selected;
    for (const auto& entry : results1D_) {
        if (entry.poi_configs[0].Name() != mu) {
            EFT_PROF_DEBUG("entry {} DOESN'T PASS name selection for {}", entry.PrintAsString(), mu);
            continue;
        }
        EFT_PROF_DEBUG("entry {} PASSES        name selection for {}", entry.PrintAsString(), mu);
        selected.insert(entry);
    }
    EFT_PROF_INFO("NllScanPlotter::GetSelectedEntries selected {} out of {}", selected.size(), results1D_.size());
    return selected;
}

void NllScanPlotter::SplitEntriesObservedExpectedPrefit(const NllScanPlotter::Nll1Dresults& results) {
    EFT_PROFILE_FN();
    EFT_PROF_INFO("NllScanPlotter split all entries on full/stat + observed/prefit/postfit");
    string curve_type_key;
    // TODO: to really think how to make it better....

    NllCurveSettings curve_full_observed ;
    NllCurveSettings curve_full_prefit   ;
    NllCurveSettings curve_full_postfit  ;
    NllCurveSettings curve_stat_observed ;
    NllCurveSettings curve_stat_prefit   ;
    NllCurveSettings curve_stat_postfit  ;
    for (auto& entry : results) {
        auto mu_val = entry.poi_configs[ 0 ].Value();
        auto nll_val = entry.nll_val;

        if (entry.version != "v2" && entry.version != "v1") {
            EFT_PROF_DEBUG("entry: {} is of version: {} ==> not v1 or v2", entry.PrintAsString(), entry.version);
            continue;
        }
            //EFT_PROF_DEBUG("entry: {} is of version: {} ==> not v2", entry.PrintAsString(), entry.version);
        if (entry.version == "v1") {
            EFT_PROF_DEBUG("entry: {} goes to OBSERVED FULL [v1]", entry.PrintAsString());
            curve_full_observed.AddPoint(mu_val, nll_val);
            continue;
        }

        if      (entry.prePostFit == PrePostFit::OBSERVED   && entry.statType == StatType::FULL) {
            EFT_PROF_DEBUG("entry: {} goes to OBSERVED FULL", entry.PrintAsString());
            curve_full_observed.AddPoint(mu_val, nll_val);
        }
        else if (entry.prePostFit == PrePostFit::OBSERVED   && entry.statType == StatType::STAT) {
            EFT_PROF_DEBUG("entry: {} goes to OBSERVED STAT", entry.PrintAsString());
            curve_stat_observed.AddPoint(mu_val, nll_val);
        }
        else if (entry.prePostFit == PrePostFit::PREFIT     && entry.statType == StatType::FULL) {
            EFT_PROF_DEBUG("entry: {} goes to PREFIT FULL", entry.PrintAsString());
            curve_full_prefit.AddPoint(mu_val, nll_val);
        }
        else if (entry.prePostFit == PrePostFit::PREFIT     && entry.statType == StatType::STAT) {
            EFT_PROF_DEBUG("entry: {} goes to PREFIT STAT", entry.PrintAsString());
            curve_stat_prefit.AddPoint(mu_val, nll_val);
        }
        else if (entry.prePostFit == PrePostFit::POSTFIT    && entry.statType == StatType::FULL) {
            EFT_PROF_DEBUG("entry: {} goes to POSTFIT FULL", entry.PrintAsString());
            curve_full_postfit.AddPoint(mu_val, nll_val);
        }
        else if (entry.prePostFit == PrePostFit::POSTFIT    && entry.statType == StatType::STAT) {
            EFT_PROF_DEBUG("entry: {} goes to POSTFIT STAT", entry.PrintAsString());
            curve_stat_postfit.AddPoint(mu_val, nll_val);
        }
    }
    EFT_PROF_INFO("All {} entries have been sorted:", results.size());
    EFT_PROF_INFO("{:4} {:8} ==> {:4} entries", "stat", "observed",   curve_stat_observed.NbPoints());
    EFT_PROF_INFO("{:4} {:8} ==> {:4} entries", "full", "observed",   curve_full_observed.NbPoints());
    EFT_PROF_INFO("{:4} {:8} ==> {:4} entries", "stat", "prefit",     curve_stat_prefit.  NbPoints());
    EFT_PROF_INFO("{:4} {:8} ==> {:4} entries", "full", "prefit",     curve_full_prefit.  NbPoints());
    EFT_PROF_INFO("{:4} {:8} ==> {:4} entries", "stat", "postfit",    curve_stat_postfit. NbPoints());
    EFT_PROF_INFO("{:4} {:8} ==> {:4} entries", "full", "postfit",    curve_full_postfit. NbPoints());
    curve_stat_postfit. colour = kRed;
    curve_stat_prefit.  colour = kRed;
    curve_stat_observed.colour = kRed;

#define EFT_MOVE_TO_MAP(name) curve_##name.title = #name; \
    curves_[#name] = std::move(curve_##name);

    EFT_MOVE_TO_MAP(full_observed);
    EFT_MOVE_TO_MAP(full_prefit);
    EFT_MOVE_TO_MAP(full_postfit);
    EFT_MOVE_TO_MAP(stat_observed);
    EFT_MOVE_TO_MAP(stat_prefit);
    EFT_MOVE_TO_MAP(stat_postfit);
#undef EFT_MOVE_TO_MAP
    EFT_PROF_DEBUG("leave SplitEntriesObservedExpectedPrefit");
}

NllScanPlotter::NllScanPlotter() {
//    for (const string& stat_type : {"full", "stat"}) {
//        for (const string& prefit_type : {"prefit", "postfit", "observed"}) {
//            curves_[stat_type + "_" + prefit_type] = {};
//        }
//    }
}

void NllScanPlotter::PlotNll1D(const string& poi_name) {
    EFT_PROFILE_FN();
    EFT_PROF_INFO("Plot Nll 1D results for {} entries", results1D_.size());

    EFT_PROF_INFO("Get selected entries by poi...");
    auto selected = GetSelectedEntries(poi_name);
    EFT_PROF_INFO("available {} selected entries", selected.size());
    SplitEntriesObservedExpectedPrefit(selected);
    EFT_PROF_INFO("split entries is done");
    //EFT_PROF_INFO("Scan entries by poi vals");
    EFT_PROF_INFO("Prepare mu-nll pairs");
    for (const string& key : {"full_observed",
                              "full_prefit",
                              "full_postfit",
                              "stat_observed",
                              "stat_prefit",
                              "stat_postfit"})
    {
        if (curves_.find(key) == curves_.end()) {
            EFT_PROF_CRITICAL("NllScanPlotter:: no key: {} in curves_ found at line: {}", key, __LINE__);
        }
        else {
            EFT_PROF_INFO("deal with {:15} type", key);
            curves_[ key ].PrepareMuNllValues();
            curves_[ key ].title = key + "_" + poi_name;
            curves_[ key ].GetGraph();
//            auto central_value = curves_[ key ].GetCentralValue();
//            auto central_error = curves_[ key ].GetCentralError();
//            EFT_PROF_INFO("{} => central value: {} +/- {}",
//                          key,
//                          central_value,
//                          central_error
//            );
        }
    }

    EFT_PROF_INFO("Setting: stat=> {} obs {}", settings_.draw_stat, settings_.draw_obs);
    EFT_PROF_INFO("Setting: full=> {} exp {}", settings_.draw_full, settings_.draw_exp);
    EFT_PROF_INFO("Setting: stat=> {} obs {}", settings_.draw_stat, settings_.draw_obs);
    EFT_PROF_INFO("Setting: full=> {} exp {}", settings_.draw_full, settings_.draw_exp);

    if (settings_.draw_stat && settings_.draw_obs) {
        EFT_PROF_INFO("to draw: {}", "stat_observed");
        curves_[ "stat_observed" ].to_draw = true;
    }
    if (settings_.draw_stat && settings_.draw_exp) {
        EFT_PROF_INFO("to draw: {}", "stat_expected");
        EFT_PROF_INFO("Setting: stat=> {} obs {}");
        curves_[ "stat_postfit" ].to_draw = true;
        curves_[ "stat_prefit" ]. to_draw = true;
    }
    if (settings_.draw_full && settings_.draw_obs) {
        EFT_PROF_INFO("to draw: {}", "full_observed");
        EFT_PROF_INFO("Setting: stat=> {} obs {}");
        curves_[ "full_observed" ].to_draw = true;
    }
    if (settings_.draw_full && settings_.draw_exp) {
         EFT_PROF_INFO("to draw: {}", "full_expected");
        EFT_PROF_INFO("Setting: stat=> {} obs {}");
        curves_[ "full_postfit" ].to_draw = true;
        curves_[ "full_prefit" ]. to_draw = true;
    }

    //const string& poi_name = configs.begin()->poi_configs.at(0).Name();

//    vector<NllScanResult> configs_sorted {configs.begin(), configs.end()};
//
//    std::sort(configs_sorted.begin(), configs_sorted.end(), [](NllScanResult& l, NllScanResult& r) -> bool{
//        return l.poi_configs[0].Value() < r.poi_configs[0].Value();
//    });
//
//    vector<double> nll_vals;
//    nll_vals.reserve(configs_sorted.size());
//
//    vector<double> mu_vals;
//    mu_vals.reserve(configs_sorted.size());
//
//    for (const auto& config : configs_sorted) {
//        nll_vals.emplace_back(config.nll_val);
//        mu_vals. emplace_back(config.poi_configs.at(0).Value());
//    }
//
//    EFT_PROF_DEBUG("{:3} | {:5} | {:10}", "idx", "mu", "nll");
//    for (size_t idx {0}; idx < nll_vals.size(); ++idx) {
//        EFT_PROF_DEBUG("{:3} | {:5} | {:10}", idx, mu_vals.at(idx), nll_vals.at(idx));
//    }
//
//    const auto min_nll = *std::min_element(nll_vals.begin(), nll_vals.end());
//    const auto max_nll = *std::max_element(nll_vals.begin(), nll_vals.end());
//    const auto min_poi = *std::min_element(mu_vals.begin(), mu_vals.end());
//    const auto max_poi = *std::max_element(mu_vals.begin(), mu_vals.end());
//
//    EFT_PROF_INFO("min_nll: {}", min_nll);
//    EFT_PROF_INFO("max_nll: {}", max_nll);
//    EFT_PROF_INFO("min_poi: {}", min_poi);
//    EFT_PROF_INFO("max_poi: {}", max_poi);
//
//    std::for_each(nll_vals.begin(), nll_vals.end(), [&min_nll](double& val) -> void
//        {
//            auto val_copy = val;
//            val -= min_nll;
//            val *= 2;
//            EFT_PROF_DEBUG("transform: {} into {}", val_copy, val);
//        }
//    );
//
//    EFT_PROF_DEBUG("{:3} | {:.4} | {:.6}", "idx", "mu", "2dnll");
//    for (size_t idx {0}; idx < nll_vals.size(); ++idx) {
//        EFT_PROF_DEBUG("{:3} | {:5} | {:10}", idx, mu_vals.at(idx), nll_vals.at(idx));
//    }

    auto mg = make_shared<TMultiGraph>("mg", "mg");
    auto legend = make_shared<TLegend>(0.8, 0.8, 0.97, 0.98);
    //auto gr = make_shared<TGraph>(nll_vals.size(), mu_vals.data(), nll_vals.data());

    TCanvas c("c", "c", 1800, 1200);
    c.SetRightMargin(0.03);
    c.SetLeftMargin(0.07);
    c.SetTopMargin(0.02);
    c.SetBottomMargin(0.10);

    //gr->GetXaxis()->SetRangeUser(0.85 * min_poi, 1.05 * max_poi);
    //grFull->GetYaxis()->SetRangeUser(-1, 15);

    //gr->GetXaxis()->SetTitle(("#mu_{" + poi_name.substr(3, poi_name.length()) + "}").c_str());
    //gr->GetYaxis()->SetTitle("2 #Delta ln L");

    //gr->SetMarkerColor(kBlue);

//    if (grStat) {
//        grStat->SetMarkerColor(kRed);
//        grStat->SetLineColor(kRed);
//        grStat->SetLineWidth(6);
//        grStat->SetMarkerSize(2);
//        grStat->SetMarkerStyle(24);
//        mg->Add(grStat);
//    }

//    gr->SetLineWidth(6);
//    gr->SetLineColor(kBlue);
//    gr->SetMarkerSize(2);
//    gr->SetMarkerStyle(24);

    for (auto& [name, curve] : curves_) {
        ofstream res(name + "_" + poi_name + ".txt");
        if (curve.NbPoints() != 0) {
            EFT_PROF_INFO("Add curve: {:15} with {} points to the scene", name, curve.NbPoints());
            EFT_PROF_INFO("points:");
            EFT_PROF_DEBUG("{:5} ==> {:5}", "mu", "2dnll");
            res  << fmt::format("{:5} {:10} {:5}", poi_name, "dnll", "fit_status") << endl;
            cout << fmt::format("{:5} {:10} {:5}", poi_name, "dnll", "fit_status") << endl;
            //EFT_PROF_DEBUG("{:5} {:10} {:5}", curve.poi_name, "nll", "fit_status");
            for (size_t idx {0}; idx < curve.NbPoints(); ++idx) {
                //EFT_PROFILE_FN()
                res  << fmt::format("{} {:10} {:5}", curve.mu_values.at(idx), curve.nll_values.at(idx) / 2., 0) << endl;
                cout << fmt::format("{} {:10} {:5}", curve.mu_values.at(idx), curve.nll_values.at(idx) / 2., 0)  << endl;
                //EFT_PROF_DEBUG("{:.3} ==> {:.3}", curve.mu_values.at(idx), curve.nll_values.at(idx));
            }
            if (curve.to_draw) {
                EFT_PROF_INFO("To     draw curve: {} due to the settings", curve.title);
                mg->Add(curve.GetGraph().get());
                curve.GetGraph()->Draw("A P C");
                {
                    string text_legend;
                    if (curve.title.find("stat") != string::npos)
                        text_legend = "Stat";
                    else {
                        text_legend = "Full";
                    }
                    legend->AddEntry(curve.GetGraph().get(), text_legend.c_str());
                }

            }
            else {
                EFT_PROF_INFO("To NOT draw curve: {}  due to the settings", curve.title);
            }
        }
    }


    mg->SetTitle("");
    mg->GetXaxis()->SetTitle(poi_name.c_str());
    mg->GetYaxis()->SetTitle("2 #Delta ln L");

    //mg->GetXaxis()->SetRangeUser(0.85 * min_poi, 1.05 * max_poi);
    //mg->GetYaxis()->SetRangeUser(0.f,          max_nll);

    if (settings_.range_2dnll_h != 0) {
        mg->GetYaxis()->SetRangeUser(0.f, settings_.range_2dnll_h);
    }
    if (settings_.range_mu_l != 0.f) {
        EFT_PROF_INFO("Change POI low bound to: {}", settings_.range_mu_l);
        auto current_range_h = mg->GetXaxis()->GetXmax();
        mg->GetXaxis()->SetRangeUser(settings_.range_mu_l, current_range_h);
        mg->GetXaxis()->SetLimits(settings_.range_mu_l, current_range_h);
        EFT_PROF_DEBUG("==>Range for POI axis: [{} {}]", mg->GetXaxis()->GetXmin(), mg->GetXaxis()->GetXmax());
    }
    if (settings_.range_mu_h != 0.f) {
        EFT_PROF_INFO("Change POI high bound to: {}", settings_.range_mu_h);
        auto current_range_l = mg->GetXaxis()->GetXmin();
        mg->GetXaxis()->SetRangeUser(current_range_l, settings_.range_mu_h);
        mg->GetXaxis()->SetLimits(current_range_l, settings_.range_mu_h);
        EFT_PROF_DEBUG("==>Range for POI axis: [{} {}]", mg->GetXaxis()->GetXmin(), mg->GetXaxis()->GetXmax());
    }

    EFT_PROF_DEBUG("Range for POI axis: [{} {}]", mg->GetXaxis()->GetXmin(), mg->GetXaxis()->GetXmax());

    //gr->SetTitle("");
    //gr->Draw("A P C");

    //grStat->Draw("A P C same");
    //grFull->Draw("A L P");

    mg->Draw("AC");
    legend->Draw("same");
    //mg->Draw("L P C");

    TLine l1;
    l1.SetLineWidth(4);
    l1.SetLineColor(kBlack);
    l1.SetLineStyle(kDashed);
    l1.DrawLine(mg->GetXaxis()->GetXmin(), 1,  mg->GetXaxis()->GetXmax(), 1);

    TLine l2;
    l2.SetLineWidth(4);
    l2.SetLineColor(kGray);
    l2.DrawLine(mg->GetXaxis()->GetXmin(), 3.84,  mg->GetXaxis()->GetXmax(), 3.84);

    float y_text = 0.93f;
    float x_text = 0.13f;
    float dy     = 0.05f;

    TLatex tex;
    tex.SetNDC();
    //  tex.SetTextAlign(3);
    tex.SetTextFont(72);
    tex.SetTextSize(0.055); //0.045 is std
    tex.DrawLatex(x_text, y_text,"ATLAS"); // 0.13, 0.9

    tex.SetTextSize(0.045); //0.045 is std
    tex.SetTextFont(42);
    tex.DrawLatex(x_text + 0.15f, y_text, "Internal"); // 0.28, 0.9


    tex.DrawLatex(x_text, y_text -= dy, "#sqrt{s} = 13 TeV, 31-139 fb^{-1}");
    tex.DrawLatex(x_text, y_text -= dy, "SMEFT, top symmetry");
    //tex.DrawLatex(x_text, y_text -= dy, "Higgs Combination");
    tex.DrawLatex(x_text, y_text -= dy, "H#rightarrow #gamma#gamma & H#rightarrow 4l STXS combination");
    tex.DrawLatex(x_text, y_text -= dy, "m_{H} = 125.09 GeV, |y_{H}| < 2.5");

    // tex.SetTextSize()

    tex.SetTextSize(0.025);
    //tex.DrawLatex(0.40, 0.8, "stat + syst");
    //tex.DrawLatex(0.40, 0.75, "stat");

//    TLine lStat(0.80, 0.81, 0.85, 0.81);
//    lStat.SetNDC();
//    lStat.SetLineColor(kBlue);
//    lStat.SetLineStyle(kDashed);
//    lStat.SetLineWidth(3);
//    lStat.Draw();
//
//    TLine lFull(0.80, 0.76, 0.85, 0.76);
//    lFull.SetNDC();
//    lFull.SetLineColor(kRed);
//    lFull.SetLineStyle(kDashed);
//    lFull.SetLineWidth(3);
//    lFull.Draw();

    tex.SetNDC(false);

    tex.DrawLatex(mg->GetXaxis()->GetXmin() * 0.90, 1.05f, "1#sigma");
    tex.DrawLatex(mg->GetXaxis()->GetXmin() * 0.90, 3.90f, "95% CL");

    string stat_regime_str;
    if (settings_.draw_stat) {
        stat_regime_str += "stat_";
    }
    if (settings_.draw_full) {
        stat_regime_str += "full_";
    }

    string expected_regime_str;
    if (settings_.draw_exp) {
        expected_regime_str += "exp_";
    }
    if (settings_.draw_obs) {
        expected_regime_str += "obs_";
    }

    for (string format : {"png", "pdf"}) {
        string save_name = fmt::format("{}_LikelihoodScan1D_{}_{}_{}.{}",
                                       settings_.output,
                                       stat_regime_str,
                                       expected_regime_str,
                                       poi_name,
                                       format);

        c.SaveAs(save_name.c_str());
    }
}

NllScanResult NllScanPlotter::ReadValuesOneFile(const std::filesystem::path& path)
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

    NllScanResult res;

    try {
        EFT_LOG_DURATION("Reading result from JSON");
        res = j.get<NllScanResult>();
    }
    catch (nlohmann::json::type_error& e) {
        EFT_PROF_WARN("NpRankingPlotter::ReadValuesOneFile{} error: {}.", path.string(), e.what()
        );
        return {};
    }

    return res;
}

} // eft::stats::scans