//
// Created by Aleksei Lukianchuk on 16-Jan-23.
//
// @ lukianchuk.aleksei@gmail.com

#include "NllScanPlotterSettings.h"

#include "CommandLineArgs.h"

#include "TGraph.h"
#include "TAxis.h"

using namespace std;

namespace eft::stats::scans {

void NllScanPlotterSettings::ReadSettingsFromCommandLine(shared_ptr<CommandLineArgs>& cmdLine) {
    EFT_PROFILE_FN();

    if (cmdLine->HasKey("rmul"))
        cmdLine->SetValIfArgExists("rmul",range_mu_l)   ;
    if (cmdLine->HasKey("rmuh"))
        cmdLine->SetValIfArgExists("rmuh",range_mu_h)   ;
    if (cmdLine->HasKey("yl"))
        cmdLine->SetValIfArgExists("yl",range_2dnll_l);
    if (cmdLine->HasKey("yh"))
        cmdLine->SetValIfArgExists("yh",range_2dnll_h);
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

    float min_nll = *std::min_element(nll_values.begin(), nll_values.end());
    float max_nll = *std::max_element(nll_values.begin(), nll_values.end());
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

    EFT_PROF_DEBUG("elements before substracting");
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

        graph->GetXaxis()->SetRangeUser(0.85 * min_poi, 1.05 * max_poi);
        is_graph_ready = true;
    }
    return graph;
}

} // eft::stats::scan