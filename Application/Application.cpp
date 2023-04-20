//
// Created by Aleksei Lukianchuk on 20-Apr-23.
//

#include "Application.h"

#include <iostream>
#include <filesystem>

#include <NpRankingStudyRes.h>
#include <Ranking/CorrelationStudyPlotSettings.h>

#include "../Core/WorkspaceWrapper.h"
#include "FitManager.h"
#include "../Utils/NpRankingPlotter.h"
#include "../Core/CommandLineArgs.h"
#include "../Test/Tester.h"
#include "../Core/Logger.h"
#include "Profiler.h"

#include "Ranking/MissingNpsProcessor.h"
#include "Ranking/CorrelationStudyProcessor.h"

#include "NllScans/NllScanManager.h"
#include "NllScans/NllScanPlotter.h"
#include "NllScans/NllScanPlotterSettings.h"

#include "FreeFit/FreeFitManager.h"

#include "spdlog/fmt/bundled/format.h"
#include "spdlog/fmt/bundled/core.h"
#include "spdlog/fmt/bundled/color.h"

#include "TError.h"
#include "TStyle.h"

namespace eft {

void Application::Init(int argc, char **argv) {
    gErrorIgnoreLevel = kError;
    gStyle->SetHistMinimumZero(true);

    eft::stats::Logger::Init();
    eft::stats::Logger::GetLogger()->set_level(spdlog::level::level_enum::err);
    commandLineArgs_ = std::make_shared<CommandLineArgs>(argc, argv);
    eft::stats::Logger::Init(commandLineArgs_);
    eft::stats::Logger::SetRelease();

    InitDebugLevel();
    std::filesystem::current_path("../");

    if (commandLineArgs_->HasKey("test")) {
        applicationState_ = ApplicationState::TEST;
        ProcessTest();
        applicationState_ = ApplicationState::FINISHED;
        return;
    }

    if (commandLineArgs_->HasKey("help") || commandLineArgs_->HasKey("h")) {
        applicationState_ = ApplicationState::HELP;
        PrintHelp(std::cout);
        applicationState_ = ApplicationState::FINISHED;
        return;
    }
    applicationState_ = ApplicationState::WAITING_REQUEST;
}

void Application::InitDebugLevel() {
    if (commandLineArgs_->HasKey("release")) {
        eft::stats::Logger::SetRelease();
        debugState_ = DebugState::RELEASE;
    }

    if (commandLineArgs_->HasKey("debug")) {
        eft::stats::Logger::SetFullPrinting();
        debugState_ = DebugState::DEBUG;
    }

    if (commandLineArgs_->HasKey("silent")) {
        eft::stats::Logger::SetSilent();
        debugState_ = DebugState::SILENT;
    }
}


void Application::ProcessTest() {
    eft::stats::Logger::SetLevel(spdlog::level::level_enum::info);
    EFT_RUN_TESTS();
    setSuccess();
}

void Application::ProcessRequest(const std::string &request) {
    if (       request == "compute_ranking") {
        ProcessRankingComputation();
    } else if (request == "plot_ranking") {
        ProcessRankingPlotting();
    } else if (request == "free_fit") {
        ProcessFreeFitComputation();
    } else if (request == "nll_scan") {
        ProcessNllScanComputation();
    } else if (request == "plot_scan") {
        ProcessNllScanPlotting();
    } else if (request == "compare_ratings") {
        ProcessCompareRatings();
    } else if (request == "get") {
        ProcessGet();
    } else if (request == "compute_hesse_nps") {
        ProcessComputeHesseNps();
    } else if (request == "get_missing_nps") {
        ProcessGetMissingNps();
    } else if (request == "compute_unconstrained") {
        EFT_PROF_CRITICAL("compute_unconstrained - is not implemented  - consider [free_fit]");
        setError();
    } else {
        std::cerr << "Unknown request: " << request << std::endl;
        setError();
        PrintHelp(std::cout);
    }
}

void Application::PrintHelp(std::ostream& os) {
    using std::cout;
    using std::endl;
    cout << "for task, use one of the following:" << endl;
    for (const auto& task : {"compute_ranking", "plot_ranking", "compute_unconstrained", "get_missing_nps"}) {
        cout << '\t' << task << endl;
    }
    cout << "* Available keys" << endl;
    cout << fmt::format("+{:=^16}==={:=^20}===={:=^15}===={:=^80}=", "=", "=", "=", "=") << '\n';
    cout << fmt::format("|{:^16} | {:^20} | {:^15} | {:^80}|", "type", "key", "default value", "comment") << '\n';
    cout << fmt::format("+{:=^16}=+={:=^20}=+=={:=^15}=+=={:=^80}+", "=", "=", "=", "=") << '\n';
    for (const auto& options : std::vector<std::array<std::string, 4>>{
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
            {"double",          "error_level",      "1.0",          "For MINOS: level of 2 Delta ln L to find an intersection with."},
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
        cout << fmt::format("|{:^16} | {:<20} | {:^15} | {:<80}|", options[0], options[1], options[2], options[3]) << '\n';
    }
    cout << fmt::format("+{:=^16}==={:=^20}===={:=^15}===={:=^80}+", "=", "=", "=", "=") << endl;
    setSuccess();
}

void Application::ProcessGet() {
    eft::stats::FitManagerConfig config;
    auto manager = std::make_unique<eft::stats::FitManager>();
    eft::stats::FitManager::ReadConfigFromCommandLine(*commandLineArgs_, config);
    manager->Init(std::move(config));
    setSuccess();
}

void Application::ProcessRankingComputation() {
    EFT_PROF_INFO("Compute ranking");

    auto manager = std::make_unique<eft::stats::FitManager>();
    eft::stats::FitManagerConfig config;
    eft::stats::FitManager::ReadConfigFromCommandLine(*commandLineArgs_, config);

    eft::stats::NpRankingStudySettings settings;
    settings.GetFromConfig(&config);

    auto worker_id = config.worker_id;

    manager->Init(std::move(config));
    manager->ComputeNpRankingOneWorker(settings, worker_id);
    setSuccess();
}

void Application::ProcessRankingPlotting() {
    using namespace eft::plot;
    std::string res_path;
    std::string poi;

    eft::plot::NpRankingPlotter plotter;
    plotter.ReadSettingsFromCommandLine(commandLineArgs_);
    plotter.ReadValues(plotter.np_ranking_settings->input);
    plotter.Plot(plotter.np_ranking_settings);
}

void Application::ProcessNllScanComputation() {
    using namespace eft::stats::scans;
    using std::vector;
    using std::string;

    commandLineArgs_->RegisterKey("pois");
    commandLineArgs_->RegisterKey("pois_float");
    commandLineArgs_->RegisterKey("prefit");
    commandLineArgs_->RegisterKey("postfit");
    commandLineArgs_->RegisterKey("stat_only");
    commandLineArgs_->RegisterKey("out");
    commandLineArgs_->RegisterKey("one_at_time");
    commandLineArgs_->RegisterKey("fit_all_pois");
    commandLineArgs_->RegisterKey("force_data");
    vector<string> pois_float;
    vector<string> pois;

    NllScanManager scanManager = NllScanManager::InitFromCommandLine(commandLineArgs_);

    commandLineArgs_->SetValIfArgExists("pois", pois);
    if (! pois.empty() ) {
        EFT_PROF_INFO("read POIs from cmdline");
        PoiConfig poi2 = PoiConfig::readFromString(eft::StringUtils::Join(' ', pois));
        scanManager.AddPoi(poi2);
    }
    size_t worker_id;
    commandLineArgs_->SetValIfArgExists("worker_id", worker_id);

    scanManager.SetWorkerId(worker_id);
    scanManager.RunScan();
    string path_res = "results/NllScans/1D";
    if (commandLineArgs_->HasKey("out")) {
        commandLineArgs_->SetValIfArgExists("out", path_res);
    }
    scanManager.SaveRes(path_res);
}

    void Application::ProcessNllScanPlotting() {
        using namespace eft::stats::scans;
        using std::string;

        string poi;
        commandLineArgs_->SetValIfArgExists("poi", poi);

        string path_res;
        commandLineArgs_->SetValIfArgExists("input", path_res);

        NllScanPlotter plotter;
        NllScanPlotterSettings plotSettings;
        plotSettings.ReadSettingsFromCommandLine(commandLineArgs_);

        plotter.UseSettings(std::move(plotSettings));

        plotter.ReadFiles(path_res);
        plotter.PlotNll1D(poi);
    }

void Application::ProcessFreeFitComputation() {
    commandLineArgs_->RegisterKey("pois_float");
    commandLineArgs_->RegisterKey("postfit");
    commandLineArgs_->RegisterKey("prefit");
    commandLineArgs_->RegisterKey("pois_float");
    commandLineArgs_->RegisterKey("errors_for");
    commandLineArgs_->RegisterKey("error_level");

    auto fitManager = eft::stats::freefit::FreeFitManager::InitFromCommandLine(commandLineArgs_);
    fitManager.RunFit();
}

void Application::ProcessCompareRatings() {
    using eft::stats::ranking::CorrelationStudyProcessor;
    using eft::plot::NpRankingPlotter;
    using eft::stats::ranking::SelectorSettings;
    using eft::plot::NpInfoForPlot;
    using std::string;

    NpRankingPlotter plotter;
    plotter.ReadSettingsFromCommandLine(commandLineArgs_);
    plotter.ReadValues(plotter.np_ranking_settings->input);
    auto res_from_computation_vec = plotter.GetSelectedSorted();
    auto res_from_computation = CorrelationStudyProcessor::FromVecNpInfo(res_from_computation_vec);

    // Get ordering based on the "prefit" sorting
    NpRankingPlotter::EntriesSorter sorter_prefit { [&](const NpInfoForPlot&l, const NpInfoForPlot& r) -> bool {
        return ((l.impact_plus_one_var * l.impact_plus_one_var)
                +
                (l.impact_minus_one_var * l.impact_minus_one_var))
               >
               ((r.impact_plus_one_var * r.impact_plus_one_var)
                +
                (r.impact_minus_one_var * r.impact_minus_one_var));
    }  };

    auto res_from_computation_prefit_vec = plotter.GetSelected();
    plotter.SortEntries(res_from_computation_prefit_vec, sorter_prefit);
    auto res_from_computation_prefit = CorrelationStudyProcessor::FromVecNpInfo(res_from_computation_prefit_vec);


    string path_suggestion;
    commandLineArgs_->SetValIfArgExists("suggestions", path_suggestion);
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
    plotSettings->label1 = "Computation: Post-fit";
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
    if (plotter.np_ranking_settings->plt_size != std::vector<size_t> {1200, 800})
        plotSettings->plt_size = plotter.np_ranking_settings->plt_size;

    plotSettings->label_size = plotter.np_ranking_settings->label_size;

    if (commandLineArgs_->HasKey("weighted"))
        plotSettings->weighted = true;

    //if ( ! plotter.np_ranking_settings->h_draw_options.empty() )
    //    plotSettings->draw_options = eft::StringUtils::Join(' ', plotter.np_ranking_settings->h_draw_options);
    plotSettings->name_to_save = fmt::format("PostFit_{}_nps.pdf", plotSettings->np_nps_plot);
    CorrelationStudyProcessor::DrawCorrsComparison(plotSettings);

    plotSettings->correlations1 = std::move(res_from_computation_prefit);
    plotSettings->label1 = "Computation: Pre-fit";
    plotSettings->name_to_save = fmt::format("PreFit_{}_nps.pdf", plotSettings->np_nps_plot);
    CorrelationStudyProcessor::DrawCorrsComparison(plotSettings);
}

void Application::Finalize() {
    using std::cout;
    if (commandLineArgs_->HasKey("debug")) {
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
}

void Application::Run() {
    if (isReady()) {
        std::string task;
        if (commandLineArgs_->SetValIfArgExists("task", task)) {
            EFT_PROF_INFO("Set task to: {}", task);
            ProcessRequest(task);
        } else {
            EFT_PROF_ERROR("No task specified");
            setError();
            PrintHelp(std::cout);
        }

    }
}

} // eft