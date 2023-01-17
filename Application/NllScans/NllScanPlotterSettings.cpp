//
// Created by Aleksei Lukianchuk on 16-Jan-23.
//
// @ lukianchuk.aleksei@gmail.com

#include "NllScanPlotterSettings.h"

#include "CommandLineArgs.h"

namespace eft::stats::scans {

void NllScanPlotterSettings::ReadSettingsFromCommandLine(std::shared_ptr<CommandLineArgs>& cmdLine) {
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

} // eft::stats::scan