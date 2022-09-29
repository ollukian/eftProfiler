#include <iostream>
#include <NpRankingStudyRes.h>

#include "Core/WorkspaceWrapper.h"
#include "Application/FitManager.h"
#include "Utils/NpRankingPlotter.h"
#include "Core/CommandLineArgs.h"
#include "Core/Logger.h"

using namespace std;

int main(int argc, char* argv[]) {

    eft::stats::Logger::Init();
    CommandLineArgs commandLineArgs(argc, argv);

    //size_t worker_id {0};
    //commandLineArgs.SetValIfArgExists("worker_id", worker_id);
    //eft::stats::Logger::Init(worker_id);

    string task;
    if (commandLineArgs.SetValIfArgExists("task", task)) {
        EFT_PROF_INFO("Set task: {}", task);
    }

    if (task == "compute_ranking") {
        EFT_PROF_INFO("Compute ranking");

        auto* manager = new eft::stats::FitManager();
        eft::stats::FitManagerConfig config;

        config.ws_name = "combWS";
        config.ws_path = "/pbs/home/o/ollukian/public/EFT/git/eftProfiler/source/WS-Comb-Higgs_topU3l_obs.root";
        config.model_config = "ModelConfig";

        eft::stats::FitManager::ReadConfigFromCommandLine(commandLineArgs, config);

        eft::stats::NpRankingStudySettings settings;

        const string postFit = config.study_type;
        if (postFit == "prefit")
            settings.prePostFit = eft::stats::PrePostFit::PREFIT;
        else if (postFit == "postfit")
            settings.prePostFit = eft::stats::PrePostFit::POSTFIT;
        else if (postFit == "observed")
            settings.prePostFit = eft::stats::PrePostFit::OBSERVED;
        else {
            EFT_PROF_CRITICAL("Prepostfit: {} is not known. Use: prefit, postfit, observed", postFit);
            EFT_PROF_CRITICAL("use OBSERVED");
            settings.prePostFit = eft::stats::PrePostFit::OBSERVED;
           // throw std::runtime_error("wrong --study_type flag option");
        }

        //settings.prePostFit = eft::stats::PrePostFit::PREFIT;
        settings.studyType = eft::stats::StudyType::OBSERVED;
        settings.poi = config.poi;
        settings.path_to_save_res = "res.json";

        auto worker_id = config.worker_id;

        manager->Init(std::move(config));
        EFT_PROF_WARN("try start compute one worker");
        manager->ComputeNpRankingOneWorker(std::move(settings), worker_id);
    }
    else if (task == "plot_ranking") {
        using namespace eft::plot;
        string res_path;
        string poi;
        if (commandLineArgs.SetValIfArgExists("res_path", res_path)) {
            EFT_PROF_INFO("Set res_path: {}", res_path);
        }
        if (commandLineArgs.SetValIfArgExists("poi", poi)) {
            EFT_PROF_INFO("Set poi: {}", poi);
        }

        eft::plot::NpRankingPlotter plotter;
        plotter.ReadValues(res_path);
        auto settings = std::make_shared<RankingPlotterSettins>();
        size_t nb_nps_to_plot = 20;
        if (commandLineArgs.SetValIfArgExists("nb_nps_to_plot", nb_nps_to_plot)) {
            EFT_PROF_INFO("Set nb_nps_to_plot: {}", nb_nps_to_plot);
        }
        settings->nb_nps_to_plot = nb_nps_to_plot;
        // tmp: to select only entries for the given POI
        plotter.SetCallBack([&poi](const NpInfoForPlot& info) -> bool { return info.poi == poi;  });
        plotter.Plot(settings);
    }
    else if (task == "compute_unconstrained") {
        EFT_PROF_INFO("Compute Unconstrained fit");

        auto* manager = new eft::stats::FitManager();
        eft::stats::FitManagerConfig config;

        config.ws_name = "combWS";
        config.ws_path = "/pbs/home/o/ollukian/public/EFT/git/eftProfiler/source/WS-Comb-Higgs_topU3l_obs.root";
        config.model_config = "ModelConfig";

        eft::stats::FitManager::ReadConfigFromCommandLine(commandLineArgs, config);

        eft::stats::NpRankingStudySettings settings;

        settings.poi = config.poi;
        manager->Init(std::move(config));
        manager->DoFitAllNpFloat(std::move(settings));
    }
    else {
        EFT_PROF_CRITICAL("Task: [{}] is unknown, use: [plot_ranking], [compute_ranking] or [compute_unconstrained]", task);
    }

    EFT_PROF_INFO("[Application] execution successfully finished");
    return 0;
}
