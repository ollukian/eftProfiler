#include <iostream>
#include <NpRankingStudyRes.h>

#include "Core/WorkspaceWrapper.h"
#include "Application/FitManager.h"
#include "Utils/NpRankingPlotter.h"
#include "Core/CommandLineArgs.h"
#include "Core/Logger.h"

using namespace std;

int main(int argc, char* argv[]) {
    std::cout << "Hello, World!" << std::endl;

    eft::stats::Logger::Init();

    CommandLineArgs commandLineArgs(argc, argv);

    string task;
    if (commandLineArgs.SetValIfArgExists("task", task)) {
        EFT_PROF_INFO("Set task: {}", task);
    }

    //if (auto task_opt = commandLineArgs.GetVal("task"); task_opt) {
    //    if (task_opt.has_value())
    //        task = task_opt.value();
    //}
    //EFT_PROF_INFO("task: {}", task);

    //manager->SetWsWrapper();
    //manager->SetWS(R"(/pbs/home/o/ollukian/public/EFT/git/eftProfiler/source/WS-Comb-Higgs_topU3l_obs.root)",
    //                      "combWS");
    //manager->SetModelConfig("ModelConfig");

    if (task == "compute_ranking") {
        EFT_PROF_INFO("Compute ranking");

        auto* manager = new eft::stats::FitManager();
        eft::stats::FitManagerConfig config;
        config.ws_name = "combWS";
        config.ws_path = "/pbs/home/o/ollukian/public/EFT/git/eftProfiler/source/WS-Comb-Higgs_topU3l_obs.root";
        config.model_configi_name = "ModelConfig";
        manager->Init(std::move(config));

        string res_path;
        //if (auto res_path_opt = commandLineArgs.GetVal("res_path"); res_path_opt) {
        //    if (res_path_opt.has_value())
        //        res_path = res_path_opt.value();
        //}
        if (commandLineArgs.SetValIfArgExists("res_path", res_path)) {
            EFT_PROF_INFO("Set res_path: {}", res_path);
        }

        size_t worker_id = 0;
        //if (auto workerId = commandLineArgs.GetVal("worker_id"); workerId) {
        //    if (workerId.has_value())
        //        worker_id_nb = stoi(string(workerId.value()));
        //}
        if (commandLineArgs.SetValIfArgExists("worker_id", worker_id)) {
            EFT_PROF_INFO("Set worker_id: {}", res_path);
        }
        //cout << "[INFO] worker id: " << worker_id_nb << endl;
        //EFT_PROF_INFO("worker id: {}", worker_id_nb);

        string poi = "cG";
        if (commandLineArgs.SetValIfArgExists("poi", poi)) {
            EFT_PROF_INFO("Set poi: {}", poi);
        }
        //if (auto poi_opt = commandLineArgs.GetVal("poi"); poi_opt) {
        //    if (poi_opt.has_value())
        //        poi = poi_opt.value();
       //}
        //cout << "[INFO] poi: " << poi << endl;
        //EFT_PROF_INFO("poi: {}", poi);

        cout << "[INFO] compute ranking " << endl;
        eft::stats::NpRankingStudySettings settings;
        settings.prePostFit = eft::stats::PrePostFit::PREFIT;
        settings.studyType = eft::stats::StudyType::OBSERVED;
        //settings.poi = "mu_GG2H_0J_PTH_0_10_ZZ";
        settings.poi = poi;
        settings.path_to_save_res = "res.json";
        manager->ComputeNpRankingOneWorker(settings, worker_id);
    }
    else if (task == "plot_ranking") {
        string res_path;
        if (commandLineArgs.SetValIfArgExists("res_path", res_path)) {
            EFT_PROF_INFO("Set res_path: {}", res_path);
        }

        eft::plot::NpRankingPlotter plotter;
        plotter.ReadValues(res_path);
    }
    else {
        EFT_PROF_CRITICAL("Task: [{}] is unknown, use: [plot_ranking] or [compute_ranking]", task);
    }

    return 0;
}
