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
            ReadValuesOneFile(entry);
        }
    }

}

void NpRankingPlotter::ReadValuesOneFile(const std::filesystem::path& path)
{
    const string filename = path.string();
    cout << fmt::format("[ReadValuesOneFile] read from {}", filename) << endl;
    const string extension = path.extension().string();
    cout << fmt::format("[ReadValuesOneFile] extension: [{}]", extension);
    if (extension != ".json") {
        cout << fmt::format(" NOT [.json]") << endl;
        return;
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

    auto res = j.get<NpRankingStudyRes>();


    cout << fmt::format("[ReavValueOneFile] read res for poi: {}, np: {}", res.poi_name, res.np_name) << endl;
    cout << setw(4) << j << endl;
    np_study_res_[res.np_name] = res;
    RegisterRes(np_study_res_[res.np_name]);
}

void NpRankingPlotter::Plot(const std::shared_ptr<RankingPlotterSettins>& settings) noexcept
{

    gStyle->SetOptTitle(0);
    gStyle->SetOptStat(0000000);

    EFT_PROF_TRACE("[NpRankingPlotter]{Plot}");
    EFT_PROF_INFO("[NpRankingPlotter] available {} NP, plot {} out of them",
                  res_for_plot_.size(),
                  settings->nb_nps_to_plot);


    std::sort(res_for_plot_.begin(), res_for_plot_.end(),
              [&](const NpInfoForPlot& l, const NpInfoForPlot& r) {
                  return l.impact < r.impact;
    });

    auto histo = make_shared<TH1D>("h", "",
                                   settings->nb_nps_to_plot,
                                   0,
                                   settings->nb_nps_to_plot
                                   );

    for (int idx_syst {0}; idx_syst != settings->nb_nps_to_plot; ++idx_syst) {
        EFT_PROF_DEBUG("[NpRankingPlotter]{Plot} set {:3} with name {:40} to {}",
                       idx_syst,
                       res_for_plot_[idx_syst].name,
                       res_for_plot_[idx_syst].impact);
        histo->SetBinContent(idx_syst + 1, res_for_plot_[idx_syst].impact);
        histo->GetXaxis()->SetBinLabel(idx_syst + 1, res_for_plot_[idx_syst].name.c_str());
        EFT_PROF_DEBUG("NpRankingPlotter::Plot set {:2} to {}", idx_syst, res_for_plot_[idx_syst].impact);
    }

    histo->GetXaxis()->LabelsOption("v");
    histo->GetYaxis()->SetRangeUser(-1.5, 1.5);

    //histo->SetFillColor(kBlue);
    histo->SetFillColorAlpha(kBlue, 0.6);
    histo->SetLineColor(kBlue);
    histo->SetLineWidth(2);


    std::filesystem::create_directory("figures");

    auto canvas = std::make_unique<TCanvas>("c", "c", 1200, 800);

    canvas->SetRightMargin(0.2f);
    canvas->SetLeftMargin(0.2f);
    canvas->SetTopMargin(0.2f);
    canvas->SetBottomMargin(0.4f);

    // lines to show full 1 sigma error
    TLine l1(0, -1, settings->nb_nps_to_plot, -1);
    TLine l2(0, 1, settings->nb_nps_to_plot, 1);

    for (auto l : {&l1, &l2}) {
        l->SetLineStyle(kDashed);
        l->SetLineWidth(2);
        l->SetLineColorAlpha(kGray, 0.7f);
        //l->Draw("same");
    }


    histo->Draw("H TEXT same");
    canvas->SaveAs("histo.pdf");
}

void NpRankingPlotter::RegisterRes(const NpRankingStudyRes& res) noexcept {
    EFT_PROF_TRACE("[NpPlotter]{RegisterRes} register: {}", res.np_name);
    NpInfoForPlot info;
    info.name = res.np_name;
    if (res.prePostFit == PrePostFit::PREFIT) {
        info.post_fit_value = res.np_val;
        info.post_fit_error = res.np_err;
    }
    else {
        info.pre_fit_value = res.np_val;
        info.pre_fit_error = res.np_err;
    }

    EFT_PROF_WARN("[NpPlotter]{RegisterRes} put real formulae for  => now we just plot it's error");
    EFT_PROF_WARN("[NpPlotter]{RegisterRes} now we use predef value for");


    static constexpr float error_full = 0.677982275;
    EFT_PROF_DEBUG("NpRankingPlotter::RegisterRes np.err: {}, full_err: {}", res.np_err, error_full);

    if (res.np_err < error_full)
        info.impact = res.np_err;
        //info.impact = sqrt( error_full * error_full - res.np_err * res.np_err);
    else
        info.impact = 0;

    res_for_plot_.push_back(std::move(info));
}


}