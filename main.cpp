#include <iostream>
#include <NpRankingStudyRes.h>

#include "Core/WorkspaceWrapper.h"
#include "Application/FitManager.h"
#include "Utils/NpRankingPlotter.h"
#include "Core/CmdLineArgs.h"
#include "Core/Logger.h"

using namespace std;

int main(int argc, char* argv[]) {
    std::cout << "Hello, World!" << std::endl;

    eft::stats::Logger::Init();

    CmdLineArgs cmdLineArgs(argc, argv);

    size_t worker_id_nb = 0;
    if (auto workerId = cmdLineArgs.GetVal("worker_id"); workerId) {
        if (workerId.has_value())
            worker_id_nb = stoi(string(workerId.value()));
    }
    cout << "[INFO] worker id: " << worker_id_nb << endl;
    EFT_PROF_INFO("worker id: {}", worker_id_nb);

    string poi;
    if (auto poi_opt = cmdLineArgs.GetVal("poi"); poi_opt) {
        if (poi_opt.has_value())
            poi = poi_opt.value();
    }
    cout << "[INFO] poi: " << poi << endl;
    EFT_PROF_INFO("poi: {}", poi);

    string task;
    if (auto task_opt = cmdLineArgs.GetVal("task"); task_opt) {
        if (task_opt.has_value())
            task = task_opt.value();
    }
    cout << "[INFO] task: " << task << endl;
    EFT_PROF_INFO("task: {}", task);

    string res_path;
    if (auto res_path_opt = cmdLineArgs.GetVal("res_path"); res_path_opt) {
        if (res_path_opt.has_value())
            res_path = res_path_opt.value();
    }
    cout << "[INFO] res_path: " << res_path << endl;
    EFT_PROF_INFO("res_path: {}", res_path);


    //eft::stats::WorkspaceWrapper ws;
    //ws.SetWS(R"(/pbs/home/o/ollukian/public/EFT/git/eftProfiler/source/WS-Comb-STXSxBR_asimov.root)",
    //         "combWS");

    // TODO: move to manager builder, where to set up ws wrapper and ws
    auto* manager = new eft::stats::FitManager();
    manager->SetWsWrapper();
    manager->SetWS(R"(/pbs/home/o/ollukian/public/EFT/git/eftProfiler/source/WS-Comb-Higgs_topU3l_obs.root)",
                          "combWS");
    manager->SetModelConfig("ModelConfig");

    //manager->ws()
    //ws.raw()->Print("v");

    //cout << "cats:" << endl;
    //for(const auto & cat : manager->ws()->GetCats()) {
    //    cout << cat << endl;
    //}


    /*cout << "[NP]:" << endl;
    manager->ws()->GetNp()->Print("v");
    cout << "[GlobalObs]:" << endl;
    manager->ws()->GetGlobObs()->Print("v");
    cout << "[Obs]:" << endl;
    manager->ws()->GetObs()->Print("v");
    cout << "[POIs]:" << endl;
    manager->ws()->GetPOIs()->Print("v");

    cout << "[INFO] extract everything" << endl;
    cout << "[INFO] extract np" << endl;*/
    manager->ExtractNP();
    cout << "[INFO] extract obs" << endl;
    manager->ExtractObs();
    cout << "[INFO] extract global obs" << endl;
    manager->ExtractGlobObs();
    cout << "[INFO] extract cats" << endl;
    manager->ExtractCats();
    cout << "[INFO] extract pois" << endl;
    manager->ExtractPOIs();

    cout << "[INFO] extract pdf total" << endl;
    manager->ExtractPdfTotal("combPdf");
    cout << "[INFO] extract data total" << endl;
    manager->ExtractDataTotal("combData");

    cout << setfill('*') << setw(45) << "" << endl;
    cout << setw(20) << "" << setw(15) << " global obs: " << setw(10) << "" << endl;
    cout << setw(45) << "" << endl;
    cout << setfill(' ');
    manager->GetArgsClosure().at("globObs")->Print("V");
    cout << setfill('*') << setw(45) << "" << endl;

    cout << setw(20) << "" << setw(15) << " obs: " << setw(10) << "" << endl;
    cout << setw(45) << "" << endl;
    cout << setfill(' ');
    manager->GetArgsClosure().at("obs")->Print("V");
    cout << setfill('*') << setw(45) << "" << endl;

    cout << setw(20) << "" << setw(15) << " All Np " << setw(10) << "" << endl;
    cout << setw(45) << "" << endl;
    cout << setfill(' ');
    manager->GetArgsClosure().at("np_all")->Print("V");
    cout << setfill('*') << setw(45) << "" << endl;

    cout << setw(20) << "" << setw(15) << " real Np " << setw(10) << "" << endl;
    cout << setw(45) << "" << endl;
    cout << setfill(' ');
    manager->GetArgsClosure().at("np")->Print("V");
    cout << setfill('*') << setw(45) << "" << endl;

    cout << setw(20) << "" << setw(15) << " dataComb " << setw(10) << "" << endl;
    cout << setw(45) << "" << endl;
    cout << setfill(' ');
    manager->GetDataClosure().at("ds_total")->Print("V");
    cout << setfill('*') << setw(45) << "" << endl;

    cout << setw(20) << "" << setw(15) << " pdfComb " << setw(10) << "" << endl;
    cout << setw(45) << "" << endl;
    cout << setfill(' ');
    manager->GetFuncClosure().at("pdf_total")->Print("V");
    cout << setfill('*') << setw(45) << "" << endl;

    cout << setfill(' ');

    //cout << "[INFO] try to fit..." << endl;
    //manager->DoGlobalFit();

    if (task == "compute_ranking") {
        EFT_PROF_INFO("Compute ranking");
        cout << "[INFO] compute ranking " << endl;
        eft::stats::NpRankingStudySettings settings;
        settings.prePostFit = eft::stats::PrePostFit::PREFIT;
        settings.studyType = eft::stats::StudyType::OBSERVED;
        settings.poi = "mu_GG2H_0J_PTH_0_10_ZZ";
        settings.path_to_save_res = "res.json";
        manager->ComputeNpRankingOneWorker(settings, worker_id_nb);
    }
    else if (task == "plot_ranking") {
        EFT_PROF_INFO("plot ranking");
        cout << "[INFO] plot ranking " << endl;
        eft::plot::NpRankingPlotter plotter;
        plotter.ReadValues(res_path);
    }

    return 0;
}
