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

    if (commandLineArgs.HasKey("h") || commandLineArgs.HasKey("help"))
    {
        cout << "for task, use one of the following:" << endl;
        for (const auto& task : {"compute_ranking, plot_ranking, compute_unconstrained"}) {
            cout << '\t' << task << endl;
        }
        return 0;
    }

    // TODO: add help printing:
    //  available options for each task - use a macro?

    //size_t worker_id {0};
    //commandLineArgs.SetValIfArgExists("worker_id", worker_id);
    //eft::stats::Logger::Init(worker_id);

    string task;
    if (commandLineArgs.SetValIfArgExists("task", task)) {
        EFT_PROF_INFO("Set task: {}", task);
    }

    if (task == "compute_ranking") {
        EFT_PROF_INFO("Compute ranking");

        auto manager = make_unique<eft::stats::FitManager>();
        eft::stats::FitManagerConfig config;

        config.ws_path = "/pbs/home/o/ollukian/public/EFT/git/eftProfiler/source/WS-Comb-Higgs_topU3l_obs.root";
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

        // construct Errors type from the input strings "errors" : vector<string>
        {
            EFT_PROF_INFO("Identify errors evaluation type from {} arguments:", config.errors.size());
            for (const auto& arg : config.errors) {
                EFT_PROF_DEBUG("=> [{}], arg");
            }
            bool isHesse {false};
            bool isMinos {false};
            bool refineNPs {false};
            bool refinePOIs {false};

            // todo: add stringutils: trim, tolowcase, tocapitalcase

            for (const string& arg : config.errors) {
                EFT_PROF_DEBUG("dispatch: {}", arg);
                if (arg == "Minos" || arg == "minos") {
                    EFT_PROF_INFO("found Minos argument");
                    isMinos = true;
                }
                else if (arg == "Hesse" || arg == "HESSE" || arg == "hesse") {
                    EFT_PROF_INFO("found Hesse argument");
                    isHesse = true;
                }
                else if (arg == "nps" || arg == "NPs" || arg == "Nps" || arg == "NP" || arg == "np") {
                    EFT_PROF_INFO("found NP argument");
                    refineNPs = true;
                }
                else if (arg == "POIs" || arg == "Pois" || arg == "pois" || arg == "poi") {
                    EFT_PROF_INFO("found POI argument");
                    refinePOIs = true;
                }
                else {
                    EFT_PROF_CRITICAL("Command line, key -errors, got UNKNOWN argument: {}", arg);
                    EFT_PROF_CRITICAL("Known: Minos, Hesse, nps, pois, np, poi");
                }
            } // args

            // if only Minos with no other option - refine everything
            if (config.errors.size() == 1) {
                if (isMinos) {
                    EFT_PROF_WARN("Minos with no arguments, refine everything (nps and POIs)");
                    refineNPs = true;
                    refinePOIs = true;
                }
            }

            EFT_PROF_DEBUG("status of arguments..");
            EFT_PROF_DEBUG("Minos -> {}", isMinos);
            EFT_PROF_DEBUG("Hesse -> {}", isHesse);
            EFT_PROF_DEBUG("nps -> {}", refineNPs);
            EFT_PROF_DEBUG("pois -> {}", refinePOIs);

            // check logic // TODO: to make an independent function to construct if from a string and to validate
            if (isHesse && isMinos) {
                EFT_PROF_CRITICAL("Command line, key -errors, cannot use MINOS and HESSE at the same time, use on of them");
                return 0;
            }
            if (!isHesse && !isMinos) {
                EFT_PROF_WARN("Command line, key -errors, neither MINOS nor HESSE are set, use DEFAULT - no error re-evaluation");
                settings.errors = eft::stats::fit::Errors::DEFAULT;
            }
            else if (isHesse) {
                EFT_PROF_INFO("Use HESSE");
                settings.errors = eft::stats::fit::Errors::HESSE;
            }
            else { // MINOS
                if (refineNPs && refinePOIs) {
                    EFT_PROF_INFO("Use MINOS for nps and pois");
                    settings.errors = eft::stats::fit::Errors::MINOS_ALL;
                }
                else if (refineNPs) {
                    EFT_PROF_INFO("Use MINOS only for nps");
                    settings.errors = eft::stats::fit::Errors::MINOS_NPS;
                }
                else {
                    EFT_PROF_INFO("Use MINOS only for pois");
                    settings.errors = eft::stats::fit::Errors::MINOS_POIS;
                }
            } // MINOS

        } // construction of errros type

        //settings.prePostFit = eft::stats::PrePostFit::PREFIT;
        settings.studyType = eft::stats::StudyType::OBSERVED;
        settings.poi = config.poi;
        settings.path_to_save_res = "res.json";
        settings.poi_init_val = config.poi_init_val;

        auto worker_id = config.worker_id;

        manager->Init(std::move(config));
        manager->ComputeNpRankingOneWorker(std::move(settings), worker_id);
    }
    else if (task == "plot_ranking") {
        using namespace eft::plot;
        string res_path;
        string poi;
//

        eft::plot::NpRankingPlotter plotter;
        plotter.ReadSettingsFromCommandLine(&commandLineArgs);
        plotter.ReadValues(plotter.np_ranking_settings->input);

        // TODO: fileformat is not considered yet!
        // tmp: to select only entries for the given POI
//        plotter.SetCallBack([&poi, &plotter](const NpInfoForPlot& info) -> bool {
//            if (plotter.np_ranking_settings->ignore_name.empty())
//                return info.poi == poi
//                    && (info.name.find("gamma") == std::string::npos);
//            else
//                return info.poi == poi
//                       && (info.name.find("gamma") == std::string::npos)
//                       && (info.name.find(plotter.np_ranking_settings->ignore_name[0]) == std::string::npos);
//        });
        plotter.Plot(plotter.np_ranking_settings);
    }
    else if (task == "compute_unconstrained") {
        EFT_PROF_INFO("Compute Unconstrained fit");

        auto manager = make_unique<eft::stats::FitManager>();
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
