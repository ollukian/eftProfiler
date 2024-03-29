//
// Created by Aleksei Lukianchuk on 16-Jan-23.
//
// @ lukianchuk.aleksei@gmail.com

#include "NllScanPlotterSettings.h"

#include "CommandLineArgs.h"

#include "TGraph.h"
#include "TAxis.h"
#include "TF1.h"
#include <memory>

using namespace std;

namespace eft::stats::scans {

void NllScanPlotterSettings::ReadSettingsFromCommandLine(shared_ptr<CommandLineArgs>& cmdLine) {
    EFT_PROFILE_FN();
    EFT_PROF_DEBUG("ReadSettingsFromCommandLine");

    cmdLine->RegisterKey("yl");
    cmdLine->RegisterKey("yh");
    cmdLine->RegisterKey("out");
    cmdLine->RegisterKey("stat");
    cmdLine->RegisterKey("obs");
    cmdLine->RegisterKey("exp");
    cmdLine->RegisterKey("force_data");
    cmdLine->RegisterKey("snapshot");

#ifndef EFT_SET_VAL_IF_ARG_EXISTS_FROM_CMD_LINE
#define EFT_SET_VAL_IF_ARG_EXISTS_FROM_CMD_LINE(cmdline, key, val) \
    if (cmdline->HasKey(key)) {                                    \
        EFT_PROF_INFO("Set: {:15} to {}", #val, val);              \
        cmdline->SetValIfArgExists(key, val); \
    }

    EFT_SET_VAL_IF_ARG_EXISTS_FROM_CMD_LINE(cmdLine, "rmul", range_mu_l);
    EFT_SET_VAL_IF_ARG_EXISTS_FROM_CMD_LINE(cmdLine, "rmuh", range_mu_h);
    EFT_SET_VAL_IF_ARG_EXISTS_FROM_CMD_LINE(cmdLine, "yl", range_2dnll_l);
    EFT_SET_VAL_IF_ARG_EXISTS_FROM_CMD_LINE(cmdLine, "yh", range_2dnll_h);
    EFT_SET_VAL_IF_ARG_EXISTS_FROM_CMD_LINE(cmdLine, "out", output);
    EFT_SET_VAL_IF_ARG_EXISTS_FROM_CMD_LINE(cmdLine, "ds", ds_title);
    EFT_SET_VAL_IF_ARG_EXISTS_FROM_CMD_LINE(cmdLine, "energy", energy);
    EFT_SET_VAL_IF_ARG_EXISTS_FROM_CMD_LINE(cmdLine, "status", res_status); // Internal, ...
    EFT_SET_VAL_IF_ARG_EXISTS_FROM_CMD_LINE(cmdLine, "comment", obs_expected); // Observed, Expected, whatever
    EFT_SET_VAL_IF_ARG_EXISTS_FROM_CMD_LINE(cmdLine, "experiment", experiment); // ATLAS, CMS, ...
    EFT_SET_VAL_IF_ARG_EXISTS_FROM_CMD_LINE(cmdLine, "lumi", lumi); // Intergated luminosity

#undef EFT_SET_VAL_IF_ARG_EXISTS_FROM_CMD_LINE
#endif

#ifndef EFT_SET_BOOL_FLAG_IF_ARG_EXISTS_FROM_CMD_LINE
#define EFT_SET_BOOL_FLAG_IF_ARG_EXISTS_FROM_CMD_LINE(cmdline, key, val) \
    if (cmdline->HasKey(key)) {                                    \
        EFT_PROF_INFO("Use flag: {:15}", #val, val);              \
        val = true; \
    }


    EFT_SET_BOOL_FLAG_IF_ARG_EXISTS_FROM_CMD_LINE(cmdLine, "full", draw_full);
    EFT_SET_BOOL_FLAG_IF_ARG_EXISTS_FROM_CMD_LINE(cmdLine, "stat", draw_stat);
    EFT_SET_BOOL_FLAG_IF_ARG_EXISTS_FROM_CMD_LINE(cmdLine, "exp", draw_exp);
    EFT_SET_BOOL_FLAG_IF_ARG_EXISTS_FROM_CMD_LINE(cmdLine, "obs", draw_obs);

#undef EFT_SET_BOOL_FLAG_IF_ARG_EXISTS_FROM_CMD_LINE
#endif

}

void NllCurveSettings::PrepareMuNllValues() {
    EFT_PROFILE_FN();
    if (NbPoints() == 0) {
        EFT_PROF_INFO("No entries for: {}", title);
        return;
    }
    EFT_PROF_INFO("Prepare MuNllValues by sorting them for |{}|", title);
    EFT_PROF_INFO("Available {} entries", mu_nll_values.size());

    std::sort(mu_nll_values.begin(), mu_nll_values.end(), [](auto& l, auto& r) -> bool{
        return l.first < r.first;
    });

    nll_values.reserve(mu_nll_values.size());
    mu_values.reserve(mu_nll_values.size());

    for (const auto& [mu, nll] : mu_nll_values) {
        mu_values. emplace_back(mu);
        nll_values.emplace_back(nll);
    }

    double min_nll = *std::min_element(nll_values.begin(), nll_values.end());
    double max_nll = *std::max_element(nll_values.begin(), nll_values.end());
    min_poi = *std::min_element(mu_values.begin(), mu_values.end());
    max_poi = *std::max_element(mu_values.begin(), mu_values.end());

    EFT_PROF_DEBUG("for: {}", title);
    EFT_PROF_DEBUG("mu  range: {:.4f} {:.4f}", min_poi, max_poi);
    EFT_PROF_DEBUG("nll range: {:.4f} {:.4f} <= nll", min_nll, max_nll);
    EFT_PROF_DEBUG("elements before substracting");
    EFT_PROF_DEBUG("{:5} ==> {:5}", "mu", "nll");
    for (size_t idx {0}; idx < NbPoints(); ++idx) {
        EFT_PROF_DEBUG("{:10} ==> {:.10}", mu_values.at(idx), nll_values.at(idx));
    }

    for (auto& nll : nll_values) {
        nll = 2 * (nll - min_nll);
    }

    EFT_PROF_DEBUG("elements after substracting");
    EFT_PROF_DEBUG("{:5} ==> {:5}", "mu", "2dnll");
    for (size_t idx {0}; idx < NbPoints(); ++idx) {
        EFT_PROF_DEBUG("{:10} ==> {:.10}", mu_values.at(idx), nll_values.at(idx));
    }

    min_nll = *std::min_element(nll_values.begin(), nll_values.end());
    max_nll = *std::max_element(nll_values.begin(), nll_values.end());
    EFT_PROF_DEBUG("nll range: {:.4f} {:.4f} <= 2dnll", min_nll, max_nll);
}

shared_ptr<TGraph> NllCurveSettings::GetGraph() {
    //EFT_PROF_WARN("GetGraph. Is ready? ==> {} | nll_values.size() = {}", is_graph_ready, nll_values.size());
    if (! is_graph_ready ) {
        graph = make_shared<TGraph>(nll_values.size(),
                                    mu_values.data(),
                                    nll_values.data()
                                    );
        graph->SetTitle(title.c_str());
        graph->SetLineColor(colour);
        graph->SetLineWidth(6);
        graph->SetMarkerSize(2);
        graph->SetMarkerStyle(24);

        if (title.find("observed") == string::npos) {
            graph->SetLineStyle(kDashed);
        }

        graph->GetXaxis()->SetRangeUser(0.85 * min_poi, 1.05 * max_poi);
        is_graph_ready = true;
    }
    return graph;
}

void NllCurveSettings::ComputeCentralValueAndError() {
    EFT_PROFILE_FN();
    auto fParab = std::make_shared<TF1>("parab", Parabola2, -100., 100., 2);
    fParab->SetParNames("mean", "sigma");
    auto graph_ = GetGraph();
    auto res = graph_->Fit(fParab.get(), "S +");
    central_value = fParab->GetParameter("mean");
    central_error = fParab->GetParameter("sigma");
    EFT_PROF_INFO("Compute central value and error for: {} => {} +- {}",
                  title,
                  central_value,
                  central_error);
}

} // eft::stats::scan

