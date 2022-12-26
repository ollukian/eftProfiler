//
// Created by Aleksei Lukianchuk on 22-Dec-22.
//

#include "Tester.h"
#include "../Utils/ColourUtils.h"
#include "../Core/WorkspaceWrapper.h"
//#include "RooRandom.h"
//#include "RooFitResult.h"
//#include "RooFormulaVar.h"
//#include "RooPlot.h"
//#include "TLine.h"
//#include "RooProfileLL.h"
//#include "TCanvas.h"
//#include "RooGaussian.h"
//#include "RooProdPdf.h"
//#include "RooAddPdf.h"
//#include "RooConstVar.h"
//#include "RooSimultaneous.h"

#include "Core.h"

#include "spdlog/fmt/bundled/color.h"
#include "spdlog/fmt/bundled/ostream.h"

#include "FitManagerConfig.h"
#include "FitManager.h"

#include "StringUtils.h"

#include <sstream>

using namespace std;
using namespace eft::stats;


//EFT_BEGIN_TESTFILE(FitManager)

const vector<string> keys_vals{
        "res_path",
        "worker_id",
        "np_name",
        "poi",
        "ws_path",
        "ws_name",
        "model_config",
        "comb_pdf",
        "comb_data",
        "top",
        "eps",
        "study_type",
        "snapshot",
        "poi_init_val",
        "color_prefit_plus",
        "color_prefit_minus",
        "color_postfit_plus",
        "color_postfit_minus",
        "rmul",
        "rmuh",
        "np_scale",
        "out_dir",
        "output",
        "input",
        "np_scale",
        "label_size",
        "lmargin",
        "rmargin",
        "tmargin",
        "bmargin",
        "retry",
        "strategy",
        "ds_title",
        "energy",
        "lumi",
        "experiment",
        "res_status",
        "np_offset",
        "mu_offset",
        "mu_latex",
        "text_size",
        "text_font",
        "dy",
        "empty_bins",
        "dx_legend",
        "dy_legend",
        "max_digits",
        "suggestions",
};

const vector<string> keys_vecs {
        "errors",
        "fileformat",
        "ignore_name",
        "match_names",
        "replace",
        "remove_prefix",
        "remove_suffix",
        "np_names",
        "add_text",
        "add_text_ndc",
        "get",
        "h_draw_options",
        "plt_size"
};

const vector<string> keys_bools {
        "no_gamma",
        "fit_all_pois",
        "fit_single_poi",
        "vertical",
        "reuse_nll",
        "silent",
        "release",
        "save_prelim",
        "draw_impact",
        "weighted",
};

EFT_IMPLEMENT_TEST(FitManager, ReadFromConfig) {
    {
        vector<string> keys_all(keys_vals);
        std::copy(keys_vecs.begin(), keys_vecs.end(), back_inserter(keys_all));
        std::copy(keys_bools.begin(), keys_bools.end(), back_inserter(keys_all));

        ASSERT_EQUAL(keys_all.size(), keys_vals.size() + keys_vecs.size() + keys_bools.size());


        auto string_all_keys = eft::StringUtils::Join(" --", keys_all);

        istringstream arguments{string_all_keys};
        int argc{0};
        char **argv = nullptr;
        GetArgcArgvFromVecCharStars(arguments, argc, argv);
        ASSERT_NO_THROW(CommandLineArgs(argc, argv));
        CommandLineArgs cmd(argc, argv);

        auto commandLineArgs = std::make_shared<CommandLineArgs>(argc, argv);
        eft::stats::FitManagerConfig config;
        auto manager = make_unique<eft::stats::FitManager>();
        eft::stats::FitManager::ReadConfigFromCommandLine(*commandLineArgs, config);
        manager->Init(std::move(config));
    }
}



EFT_START_TEST_GROUP(FitManager)
    EFT_RUNTEST(ReadFromConfig, FitManager);
}
EFT_END_IMPLEMENT_TESTFILE(FitManager);