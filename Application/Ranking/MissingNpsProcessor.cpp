//
// Created by Aleksei Lukianchuk on 16-Dec-22.
//

#include "MissingNpsProcessor.h"

#include "MissingNpsProcessorSettings.h"
#include "../Core/CommandLineArgs.h"
#include "FitManagerConfig.h"
#include "FitManager.h"
#include "../Utils/NpRankingPlotter.h"

#include <iostream>

using namespace std;

namespace eft::stats::ranking {


void MissingNpsProcessor::ReadSettingsFromCommandLine(CommandLineArgs *cmdLineArgs) {
    EFT_PROF_TRACE("MissingNpsProcessor::ReadSettingsFromCommandLine");

    // get all nps
    auto manager = make_unique<FitManager>();
    FitManagerConfig config;
    FitManager::ReadConfigFromCommandLine(*cmdLineArgs, config);
    settings_->path_files = config.input;
    eft::stats::NpRankingStudySettings settings;
    EFT_PROF_TRACE("get settings from config");
    settings.GetFromConfig(&config);
    EFT_PROF_TRACE("init managet from config");
    manager->Init(std::move(config));
    EFT_PROF_TRACE("get paired nps");
    auto paired_nps = manager->GetListAsArgSet("paired_nps");

    EFT_PROF_DEBUG("MissingNpsProcessor: Get list of names of all nps");
    EFT_PROF_DEBUG("MissingNpsProcessor: available: {} paired_nps", paired_nps->size());
    for (const auto np : *paired_nps) {
        string np_name = np->GetName();
        EFT_PROF_TRACE("Add: {:50}", np_name);
        all_nps_.insert(std::move(np_name));
    }
    EFT_PROF_INFO("Extracted {} nps from the full list of paired nps", all_nps_.size());

    eft::plot::NpRankingPlotter plotter;
    plotter.ReadSettingsFromCommandLine(cmdLineArgs);
    plotter.ReadValues(plotter.np_ranking_settings->input);

    const auto& np_res = plotter.GetResForPlot();
    EFT_PROF_DEBUG("MissingNpsProcessor: Get list of names from the folder: {}", plotter.np_ranking_settings->input);
    for (const auto& res : np_res) {
        present_nps_.insert(res.name);
    }
    EFT_PROF_INFO("Extracted {} nps from the folder", present_nps_.size());
}

//void MissingNpsProcessor::ReadNpFiles() {
//
//}

void MissingNpsProcessor::PrintMissingNps(std::ostream& os, const std::string& separator)
{
    EFT_PROF_TRACE("MissingNpsProcessor::PrintMissingNps");
    if (missing_nps_.empty()) {
        EFT_PROF_CRITICAL("No missing nps found");
        return;
    }

    for (const auto& np : missing_nps_) {
        os << np << separator;
    }
}

void MissingNpsProcessor::ComputeMissingNPs() noexcept {
    EFT_PROF_TRACE("Compute Missing nps");
    EFT_PROF_DEBUG("Loop over a list with {} nps", all_nps_.size());
    for (const auto& np : all_nps_) {
        if (present_nps_.count(np) == 0) {
            EFT_PROF_DEBUG("Check: {:60} ==> ADD");
            missing_nps_.insert(np);
        }
        else {
            EFT_PROF_DEBUG("Check: {:60} ==> ALREADY");
        }
    }
    EFT_PROF_INFO("Found {} missing nps", missing_nps_.size());
}


} // eft::stats::ranking