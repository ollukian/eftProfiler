#include <iostream>
#include <NpRankingStudyRes.h>
#include <Ranking/CorrelationStudyPlotSettings.h>

#include "Core/WorkspaceWrapper.h"
#include "Application/FitManager.h"
#include "Utils/NpRankingPlotter.h"
#include "Core/CommandLineArgs.h"
#include "Test/Tester.h"
#include "Core/Logger.h"
#include "Profiler.h"

#include "Application/Ranking/MissingNpsProcessor.h"
#include "Application/Ranking/CorrelationStudyProcessor.h"

#include "spdlog/fmt/bundled/format.h"
#include "spdlog/fmt/bundled/core.h"
#include "spdlog/fmt/bundled/color.h"

#include "TError.h"
#include "TStyle.h"

using namespace std;

int main(int argc, char* argv[]) {
    //
    gErrorIgnoreLevel = kError;
    gStyle->SetHistMinimumZero(true);

    eft::stats::Logger::Init();
    eft::stats::Logger::GetLogger()->set_level(spdlog::level::level_enum::err);
    auto commandLineArgs = std::make_shared<CommandLineArgs>(argc, argv);
    eft::stats::Logger::Init(commandLineArgs);
    //eft::stats::Logger::SetRelease();
    if (commandLineArgs->HasKey("test")) {
    //if (argc >= 2 && string(argv[1]) == "--test") {
        eft::stats::Logger::SetLevel(spdlog::level::level_enum::info);
        EFT_RUN_TESTS();
        return 0;
    }
    if (commandLineArgs->HasKey("h") || commandLineArgs->HasKey("help"))
    {
        cout << "for task, use one of the following:" << endl;
        for (const auto& task : {"compute_ranking", "plot_ranking", "compute_unconstrained", "get_missing_nps"}) {
            cout << '\t' << task << endl;
        }
        cout << "* Available keys" << endl;
        cout << fmt::format("+{:=^16}==={:=^20}===={:=^15}===={:=^80}=", "=", "=", "=", "=") << endl;
        cout << fmt::format("|{:^16} | {:^20} | {:^15} | {:^80}|", "type", "key", "default value", "comment") << endl;
        cout << fmt::format("+{:=^16}=+={:=^20}=+=={:=^15}=+=={:=^80}+", "=", "=", "=", "=") << endl;
        for (const auto& options : std::vector<std::array<string, 4>>{
                {"",                "",                 "",             "WARNING: x in front of the comment: not supported yet"},
                {"string",          "task",             "",             "compute_ranking | plot_ranking | compute_unconstrained"},
                {"string",          "log_path",         "",             "x Path to save log files"},
                {"string",          "log_name",         "eft_profiler", "x name of the logger"},
                {"string",          "ws_path",          "",             ""},
                {"string",          "ws_name",          "combWS",       "Name of the workspace in the file"},
                {"string",          "model_config",     "ModelConfig",  "Name of the RooStats::ModelConfig in the file"},
                {"string",          "comb_pdf",         "combPdf",      "Name of the pdf  to be used (usually, combined pdf)"},
                {"string",          "comb_data",        "combData",     "Name of the data to be used (usually, combined pdf)"},
                {"string",          "res_path",         ".",            "path where to save the resulting json file"},
                {"string",          "poi",              "cHG",          "POI name in the workspace"},
                {"vector<string>",  "errors",           "",             "Minos nps | Minos pois | Minos | Hesse"},
                {"string",          "np_name",          "",             "Name of the Np to compute ranking for."},
                {"size_t",          "worker_id",        "0",            ""},
                {"size_t",          "top",              "20",           "number of pois to plot"},
                {"double",          "eps",              "1E-3",         "Fit precision"},
                {"size_t",          "strategy",         "1",            "Strategy from RooMinimizer (0, 1, 2)"},
                {"size_t",          "retry",            "0",            "Nb of times to retry fit in case of failure (0, 1, 2)"},
                {"double",          "poi_init_val",     "0.",           "Initial value of the POI to be set before each fit"},
                {"float",           "rmargin",          "0.05",         "Canvas margin RIGHT"},
                {"float",           "lmargin",          "0.10",         "Canvas margin LEFT"},
                {"float",           "tmargin",          "0.03",         "Canvas margin TOP"},
                {"float",           "bmargin",          "0.40",         "Canvas margin BOTTOM"},
                {"vector<size_t>",  "plt_size",         "1200 800",     "Size of the canvas (x, y). Example: --plt_size 1200 800"},
                {"string",          "study_type",       "",             ""},
                {"string",          "snapshot",         "",             "x Snapshot to be load before each fit"},
                {"bool",            "no_gamma",        "false",         "If skip gamma-constrained systematics"},
                {"bool",            "fit_all_pois",    "false",         "If fit all pois available in the Workspace"},
                {"bool",            "fit_single_poi",  "true",          "If fix all pois available in the Workspace, apart from the one to fit"},
                {"vector<string>",  "fit_pois",        "[POI]",         "x List of POIs to be float during each fit. Impact only on the [--poi] is reported"},
                {"string",          "input",            "",             "Folder to read json results from (for plotting)"},
                {"vector<string>",  "fileformat",       "pdf",          "Format(s) of the output plot"},
                {"vector<string>",  "ignore_name",      "",             "Patterns in the names of systematics to be ignored (not regex yet), just string.find()"},
                {"vector<string>",  "match_names",      "",             "Patterns in the names of systematics to be matched (not regex yet), just string.find()"},
                {"bool",            "vertical",         "false",        "Orientation of the impact plot: whether to be vertical or standard (horizontal)"},
                {"string",          "color_prefit_plus", "kBlue",       "Colour for +1     variation. Formats: kBlue | RGB(r, g, b) | RGBA(r, g, b, a) || x in [0..255]"},
                {"string",          "color_prefit_minus", "kGreen",     "Colour for -1     variation. Formats: kBlue | RGB(r, g, b) | RGBA(r, g, b, a) || x in [0..255]"},
                {"string",          "color_postfit_plus", "kBlue",      "Colour for +sigma variation. Formats: kBlue | RGB(r, g, b) | RGBA(r, g, b, a) || x in [0..255]"},
                {"string",          "color_postfit_minus","kGreen",     "Colour for -sigma variation. Formats: kBlue | RGB(r, g, b) | RGBA(r, g, b, a) || x in [0..255]"},
                {"string",          "color_np",         "kBlack",       "x Colour for NP graph.    Formats: kBlue | RGB(x, y, z) | RGBA(x, y, z, a) || x in [0..255]"},
                {"bool",            "reuse_nll",        "true",         "Do not create new nll for each fit in the impact study (pre-, post-fits, initial fit)"},
                {"float",           "rmul",             "[-1.5 * impact#1]", "LOW  value for the POI axis on the ranking plot"},
                {"float",           "rmuh",             "[ 1.5 * impact#1]", "HIGH value for the POI axis on the ranking plot"},
                {"float",           "np_scale",         "[post fit of top np]", "Force scale at which +- 1 for np axis is drawn wrt to the POI axis"},
                {"bool",            "save_prelim",      "false",        "x To force saving results after each fit stage (free, fixed np, pre-fit, post-fit)"},
                {"string",          "out_dir",          "figures",      "Directory to save result plot"},
                {"string",          "output",           "",             "Force using specified name (without format, format is to be set by --fileformat)"},
                {"float",           "label_size",       "",             "Size of the label (where np names are printed)"},
                {"size_t",          "label_font",       "62",           "Np axis label's font; See ROOT Fonts: https://root.cern.ch/doc/master/classTAttText.html#ATTTEXT5"},
                {"vector<string>",  "remove_prefix",    "",             R"(Prefix to be cut from the names of nps (ex: "ATLAS_Hgg_bias_**" with "ATLAS_" being option will become: "Hgg_bias_*")"},
                {"vector<string>",  "remove_suffix",    "",             R"(Suffix to be cut from the names of nps (ex: "**_HComb" with "HComb" being option will become: "**_")"},
                {"vector<string>",  "replace",          "",             R"(Replace in labels. Format: "key1:val2 Key2:val2 ...". Ex:"ATLAS_:LHC" replaces "ATLAS_" by "LHC")"},
                {"string",          "ds_title",         "Higgs Combination", "Text description of the dataset. Allowed to use commands from TLatex"},
                {"float",           "energy",           "13",           R"(Center-of-mass energy [TeV] x To change units use []. Ex: "27[Mev]" => will force MeV)"},
                {"string",          "lumi",             "36.1-139",     R"(Luminosity [fb^-1]          x To change units use []. Ex: "25[pb]   => will force pb^-1"  )"},
                {"string",          "experiment",       "ATLAS",        "Name of the experiment to be printed."},
                {"string",          "res_status",       "Internal",     "Work-in-progress, Internal, Simulation, ..."},
                {"float",           "mu_offset",        "",             "Offset of the POI label"},
                {"float",           "np_offset",        "",             "Offset of the NP label"},
                {"string",          "mu_latex",         "[POI]",        "Name of the POI to be printed on the 3rd line in TLatex format."},
                {"vector<string>",  "np_names",         "",             R"(Labels for bins. Either list (Ex: cute_np1 cool_np2 #mu_latex ...) or "file:my_filename.txt" - to read from file)"},
                {"float",           "text_size",        "0.030",        R"(Text size of the: energy, dataset title, POI, luminosity)"},
                {"float",           "text_font",        "42",           R"(Font of the: energy, dataset title, POI, luminosity)"},
                {"float",           "dy",               "0.03",         R"(Distance between text lines)"},
                {"vector<string>",  "add_text",         "",             R"(x [x y text [size][font]] to be added. Ex: "2 4 abc" or: "1 2 xyz 27 34")"},
                {"vector<string>",  "add_text_ndc",     "",             R"(x [x y text [size][font]] to be added. (X, Y) - in NDC. Example: "0.1 0.4 WriteTextInNDC")"},
                {"bool",            "silent",           "false",        R"(MUST me FIRST option. To prevent any output, except demanded: to use as wrapper around wc.)"},
                {"bool",            "release",          "false",        R"(Set message level to "INFO". Available only for debugging during development)"},
                {"string",          "get",              "",             R"(POI | NPS | GLOBS | CATS | PAIRED_NPS | PAIRED_GLOBS + [COUNT] <== prints them to stdout)"},
                {"vector<string>",  "h_draw_options",   "H",            "Options for the draw of histograms. Note: [HBAR] is default for --vertical"},
                {"size_t",          "empty_bins",       "3",            "Number of first empty bins, where the Legend and text will be written"},
                {"float",           "dx_legend",        "0.15",         "x-distance between lines of legend in the units of the plot"},
                {"float",           "dy_legend",        "0.00",         "y-distance between lines of legend in the units of the plot"},
                {"size_t",          "max_digits",       "3",            "Maximum number of digits on the POI axis. If more, exponential form is used"},
                {"bool",            "draw_impact",      "false",        "To draw impact being quadratic difference of the total error on the POI (free fit and with this NP fixed)"},
                {"string",          "suggestions",      "",             "File with suggestions, to read ranking from Hesse from"},
        })
        {
            cout << fmt::format("|{:^16} | {:<20} | {:^15} | {:<80}|", options[0], options[1], options[2], options[3]) << endl;
        }
        cout << fmt::format("+{:=^16}==={:=^20}===={:=^15}===={:=^80}+", "=", "=", "=", "=") << endl;
        return 0;
    }

    if (commandLineArgs->HasKey("get")) {
        eft::stats::FitManagerConfig config;
        auto manager = make_unique<eft::stats::FitManager>();
        eft::stats::FitManager::ReadConfigFromCommandLine(*commandLineArgs, config);
        manager->Init(std::move(config));
        return 0;
    }

    string task;
    if (commandLineArgs->SetValIfArgExists("task", task)) {
        EFT_PROF_INFO("Set task: {}", task);
    }

    if (task == "compute_ranking") {
        EFT_PROF_INFO("Compute ranking");

        auto manager = make_unique<eft::stats::FitManager>();
        eft::stats::FitManagerConfig config;
        eft::stats::FitManager::ReadConfigFromCommandLine(*commandLineArgs, config);

        eft::stats::NpRankingStudySettings settings;
        settings.GetFromConfig(&config);

        auto worker_id = config.worker_id;

        manager->Init(std::move(config));
        manager->ComputeNpRankingOneWorker(std::move(settings), worker_id);
    }
    else if (task == "plot_ranking") {
        using namespace eft::plot;
        string res_path;
        string poi;

        eft::plot::NpRankingPlotter plotter;
        plotter.ReadSettingsFromCommandLine(commandLineArgs);
        plotter.ReadValues(plotter.np_ranking_settings->input);
        plotter.Plot(plotter.np_ranking_settings);
    }
    else if (task == "compute_unconstrained") {
        EFT_PROF_INFO("Compute Unconstrained fit");

        auto manager = make_unique<eft::stats::FitManager>();
        eft::stats::FitManagerConfig config;

        eft::stats::FitManager::ReadConfigFromCommandLine(*commandLineArgs, config);

        eft::stats::NpRankingStudySettings settings;

        settings.poi = config.poi;
        manager->Init(std::move(config));
        manager->DoFitAllNpFloat(std::move(settings));
    }
    else if (task == "compute_hesse_nps") {
        using eft::stats::ranking::CorrelationStudyProcessor;
        EFT_PROF_INFO("Compute hesse nps");

        CorrelationStudyProcessor correlationStudyProcessor(commandLineArgs.get());
        auto res = correlationStudyProcessor.ComputeHesseNps();
        correlationStudyProcessor.ExtractCorrelations(res);
        const auto sorted = res.GetSorted();
        correlationStudyProcessor.PlotCovariances(res);
        string name_to_save = fmt::format("suggested_covariances_{}.txt", res.poi);
        correlationStudyProcessor.PrintSuggestedNpsRanking(std::move(name_to_save), res);
    }
    else if (task == "get_missing_nps") {
        eft::stats::ranking::MissingNpsProcessor missingNpsProcessor;
        missingNpsProcessor.ReadSettingsFromCommandLine(commandLineArgs.get());
        missingNpsProcessor.ComputeMissingNPs();
        missingNpsProcessor.PrintMissingNps(cout, " \\ \n");
    }
    else if (task == "compare_ratings") {
        using eft::stats::ranking::CorrelationStudyProcessor;
        using eft::plot::NpRankingPlotter;
        using eft::stats::ranking::SelectorSettings;

        NpRankingPlotter plotter;
        plotter.ReadSettingsFromCommandLine(commandLineArgs);
        plotter.ReadValues(plotter.np_ranking_settings->input);
        auto res_from_computation_vec = plotter.GetSelectedSorted();
        auto res_from_computation = CorrelationStudyProcessor::FromVecNpInfo(res_from_computation_vec);

        string path_suggestion;
        commandLineArgs->SetValIfArgExists("suggestions", path_suggestion);
        auto res_from_correlation_before_selector = CorrelationStudyProcessor::GetSortedCorrelationsFromFile(path_suggestion);
        SelectorSettings selectorSetting;
        selectorSetting.poi = plotter.np_ranking_settings->poi;
        selectorSetting.selector = plotter.GetSelector();
        auto res_from_correlation
            = CorrelationStudyProcessor::ApplySelector(res_from_correlation_before_selector,
                                                       selectorSetting);

        auto plotSettings = std::make_shared<CorrelationStudyPlotSettings>();
        plotSettings->correlations1 = std::move(res_from_computation);
        plotSettings->correlations2 = std::move(res_from_correlation);
        plotSettings->label1 = "From Computation";
        plotSettings->label2 = "From Correlation matrix";
        if ( ! plotter.np_ranking_settings->output.empty() )
            plotSettings->name_to_save = plotter.np_ranking_settings->output;
        plotSettings->np_nps_plot = plotter.np_ranking_settings->top;

        if (plotter.np_ranking_settings->lmargin != 0.1f)
            plotSettings->lmargin = plotter.np_ranking_settings->lmargin;
        if (plotter.np_ranking_settings->rmargin != 0.05f)
            plotSettings->rmargin = plotter.np_ranking_settings->rmargin;
        if (plotter.np_ranking_settings->tmargin != 0.03f)
            plotSettings->tmargin = plotter.np_ranking_settings->tmargin;
        if (plotter.np_ranking_settings->bmargin != 0.4f)
            plotSettings->bmargin = plotter.np_ranking_settings->bmargin;
        if (plotter.np_ranking_settings->plt_size != vector<size_t>{1200, 800})
            plotSettings->plt_size = plotter.np_ranking_settings->plt_size;
        //if ( ! plotter.np_ranking_settings->h_draw_options.empty() )
        //    plotSettings->draw_options = eft::StringUtils::Join(' ', plotter.np_ranking_settings->h_draw_options);
        CorrelationStudyProcessor::DrawCorrsComparison(plotSettings);
        // in this way, we apply the same selection to the read values
       // auto res_from_computation_after_selector = plotter.GetSelected(res_from_computation_before_selector);
    }
    else {
        EFT_PROF_CRITICAL("Task: [{}] is unknown, use: [plot_ranking], [compute_ranking], [compute_unconstrained], get_missing_nps", task);
        return 0;
    }

    if (commandLineArgs->HasKey("debug")) {
        EFT_PROF_INFO("Statistics for function calls:");
        fmt::print(cout, "| {:-^30} | {:-<10} | {:-<15} |\n", "-", "-", "-");
        fmt::print(cout, "| {:^30} | {:<10} | {:<15} |\n", "Function", "Duration", "Avg duration");
        fmt::print(cout, "| {:-^30} | {:-<10} | {:-<15} |\n", "-", "-", "-");
        const auto& durations = eft::utils::Profiler::GetDurations();
        const auto& avg_durations = eft::utils::Profiler::GetAvgDurations();
        for (const auto& [name, duration]: durations) {
            fmt::print(fmt::fg(fmt::color::light_green), "| {:^30} | {:10} | {:10} | \n",
                       name,
                       duration.count(),
                       avg_durations.at(name).count()
            );
        }
        fmt::print(cout, "| {:-^30} | {:-<10} | {:-<15} |\n", "-", "-", "-");
    }

    EFT_PROF_INFO("[Application] execution successfully finished");
    return 0;
}
