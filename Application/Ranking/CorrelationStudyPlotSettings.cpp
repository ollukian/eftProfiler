//
// Created by Aleksei Lukianchuk on 20-Dec-22.
//

#include "CorrelationStudyPlotSettings.h"

#include "Core.h"

void CorrelationStudyPlotSettings::FormSortedNames()
{
    EFT_PROFILE_FN();
    EFT_PROF_TRACE("Form sorted names");

    sorted_names_1.reserve(correlations1.size());
    sorted_names_2.reserve(correlations2.size());

    for (const auto& [np_name, corr] : correlations1) {
        sorted_names_1.emplace_back(np_name);
    }

    for (const auto& [np_name, corr] : correlations2) {
        sorted_names_2.emplace_back(np_name);
    }
    EFT_PROF_DEBUG("Sorted names:");
    EFT_PROF_DEBUG("Correlation1:");
    for (const auto& name : sorted_names_1) {
        EFT_PROF_DEBUG("{}", name);
    }
    EFT_PROF_DEBUG("{:=>60}");
    EFT_PROF_DEBUG("{:=>60}");
    EFT_PROF_DEBUG("{:=>60}");
    EFT_PROF_DEBUG("Correlation2:");
    for (const auto& name : sorted_names_2) {
        EFT_PROF_DEBUG("{}", name);
    }
}