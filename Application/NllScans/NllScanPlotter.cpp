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
    EFT_PROF_DEBUG("NllScanPlotter register 1D. Poi: [{}] at [{:4}] with nll: {}",
                   poi,
                   nllScanRes.poi_configs[0].Value(),
                   nllScanRes.nll_val);

    results1D_per_poi_[poi].insert(nllScanRes);
    results1D_.insert(std::move(nllScanRes));
}

void NllScanPlotter::PlotNll1D(const NllScanPlotter::Nll1Dresults& configs) {
    EFT_PROFILE_FN();
    EFT_PROF_INFO("Plot Nll 1D results for {} entries", configs.size());

    EFT_PROF_INFO("Scan entries by poi vals");

    const string& poi_name = configs.begin()->poi_configs.at(0).Name();

    vector<NllScanResult> configs_sorted {configs.begin(), configs.end()};

    std::sort(configs_sorted.begin(), configs_sorted.end(), [](NllScanResult& l, NllScanResult& r) -> bool{
        return l.poi_configs[0].Value() < r.poi_configs[0].Value();
    });

    vector<double> nll_vals;
    nll_vals.reserve(configs_sorted.size());

    vector<double> mu_vals;
    mu_vals.reserve(configs_sorted.size());

    for (const auto& config : configs_sorted) {
        nll_vals.emplace_back(config.nll_val);
        mu_vals. emplace_back(config.poi_configs.at(0).Value());
    }

    EFT_PROF_DEBUG("{:3} | {:5} | {:10}", "idx", "mu", "nll");
    for (size_t idx {0}; idx < nll_vals.size(); ++idx) {
        EFT_PROF_DEBUG("{:3} | {:5} | {:10}", idx, mu_vals.at(idx), nll_vals.at(idx));
    }

    const auto min_nll = *std::min_element(nll_vals.begin(), nll_vals.end());
    const auto max_nll = *std::max_element(nll_vals.begin(), nll_vals.end());
    const auto min_poi = *std::min_element(mu_vals.begin(), mu_vals.end());
    const auto max_poi = *std::max_element(mu_vals.begin(), mu_vals.end());

    EFT_PROF_INFO("min_nll: {}", min_nll);
    EFT_PROF_INFO("max_nll: {}", max_nll);
    EFT_PROF_INFO("min_poi: {}", min_poi);
    EFT_PROF_INFO("max_poi: {}", max_poi);

    std::for_each(nll_vals.begin(), nll_vals.end(), [&min_nll](double& val) -> void
        {
            auto val_copy = val;
            val -= min_nll;
            val *= 2;
            EFT_PROF_DEBUG("transform: {} into {}", val_copy, val);
        }
    );

    EFT_PROF_DEBUG("{:3} | {:.4} | {:.6}", "idx", "mu", "2dnll");
    for (size_t idx {0}; idx < nll_vals.size(); ++idx) {
        EFT_PROF_DEBUG("{:3} | {:5} | {:10}", idx, mu_vals.at(idx), nll_vals.at(idx));
    }

    auto mg = make_shared<TMultiGraph>("mg", "mg");
    auto gr = make_shared<TGraph>(nll_vals.size(), mu_vals.data(), nll_vals.data());

    TCanvas c("c", "c", 1800, 1200);
    c.SetRightMargin(0.03);
    c.SetLeftMargin(0.07);
    c.SetTopMargin(0.02);
    c.SetBottomMargin(0.10);

    gr->GetXaxis()->SetRangeUser(0.85 * min_poi, 1.05 * max_poi);
    //grFull->GetYaxis()->SetRangeUser(-1, 15);

    gr->GetXaxis()->SetTitle(("#mu_{" + poi_name.substr(3, poi_name.length()) + "}").c_str());
    gr->GetYaxis()->SetTitle("2 #Delta ln L");

    gr->SetMarkerColor(kBlue);

//    if (grStat) {
//        grStat->SetMarkerColor(kRed);
//        grStat->SetLineColor(kRed);
//        grStat->SetLineWidth(6);
//        grStat->SetMarkerSize(2);
//        grStat->SetMarkerStyle(24);
//        mg->Add(grStat);
//    }

    gr->SetLineWidth(6);
    gr->SetLineColor(kBlue);
    gr->SetMarkerSize(2);
    gr->SetMarkerStyle(24);

    mg->Add(gr.get());


    mg->SetTitle("");
    mg->GetXaxis()->SetTitle(("#mu_{" + poi_name.substr(3, poi_name.length()) + "}").c_str());
    mg->GetYaxis()->SetTitle("2 #Delta ln L");

    mg->GetXaxis()->SetRangeUser(0.85 * min_poi, 1.05 * max_poi);
    mg->GetYaxis()->SetRangeUser(0.f,          max_nll);

    gr->SetTitle("");
    gr->Draw("A P C");
    //grStat->Draw("A P C same");
    //grFull->Draw("A L P");

    mg->Draw("APC");
    //mg->Draw("L P C");

    TLine l1;
    l1.SetLineWidth(4);
    l1.SetLineColor(kBlack);
    l1.SetLineStyle(kDashed);
    l1.DrawLine(0.85 * min_poi, 1, 1.05 * max_poi, 1);

    TLine l2;
    l2.SetLineWidth(4);
    l2.SetLineColor(kGray);
    l1.DrawLine(0.85 * min_poi, 3.84, 1.05 * max_poi, 3.84);

    TLatex tex;
    tex.SetNDC();
    //  tex.SetTextAlign(3);
    tex.SetTextFont(72);
    tex.SetTextSize(0.055); //0.045 is std
    tex.DrawLatex(0.13, 0.9,"ATLAS");

    tex.SetTextSize(0.045); //0.045 is std
    tex.SetTextFont(42);
    tex.DrawLatex(0.28, 0.9, "Internal");


    tex.DrawLatex(0.13, 0.85, "#sqrt{s} = 13 TeV, 139 fb^{-1}");
    tex.DrawLatex(0.13, 0.80, "m_{H} = 125.09 GeV");
    tex.DrawLatex(0.13, 0.75, "|y_{H}| < 2.5");

    // tex.SetTextSize()

    tex.SetTextSize(0.025);
    tex.DrawLatex(0.40, 0.8, "stat + syst");
    tex.DrawLatex(0.40, 0.75, "stat");

    TLine lStat(0.80, 0.81, 0.85, 0.81);
    lStat.SetNDC();
    lStat.SetLineColor(kBlue);
    lStat.SetLineStyle(kDashed);
    lStat.SetLineWidth(3);
    lStat.Draw();

    TLine lFull(0.80, 0.76, 0.85, 0.76);
    lFull.SetNDC();
    lFull.SetLineColor(kRed);
    lFull.SetLineStyle(kDashed);
    lFull.SetLineWidth(3);
    lFull.Draw();


//    static const map<string, double> muErrors = {
//            {"mu_ggF", 0.128564},
//            {"mu_VBF", 0.278759},
//            {"mu_WH",  0.529406},
//            {"mu_ZH",  0.515287},
//            {"mu_ttH", 0.305285},
//            {"mu_tH",  3.30348}
//    };
//
//    tex.SetTextSize(0.045);
//    const string muStr = "#mu_{" + mu.substr(3, mu.length()) + "}";
//    tex.DrawLatex(0.50, 0.8, (muStr + " = " + to_string(sens.full.muHat).substr(0, 5)
//                              + " #pm " + to_string(muErrors.at(mu)).substr(0, 5)
//                  ).c_str()
//    );

    /*tex.DrawLatex(0.50, 0.75, (muStr + " = " + to_string(sens.full.muHat).substr(0, 5)
                  + " #pm " + to_string(muErrors.at(mu)).substr(0, 5)
                  ).c_str()
                  );*/


    tex.SetNDC(false);

    tex.DrawLatex(0.87 * min_poi, 1.05, "1#sigma");
    tex.DrawLatex(0.87 * max_poi, 3.94, "2#sigma");

    c.SaveAs(("LikelihoodScan1D_" + poi_name + ".png").c_str());
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
    }

    return res;
}

} // eft::stats::scans