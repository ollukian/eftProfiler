#include <iostream>
#include <NpRankingStudyRes.h>

#include "Core/WorkspaceWrapper.h"
#include "Application/FitManager.h"
#include "Core/CmdLineArgs.h"

using namespace std;

int main(int argc, char* argv[]) {
    std::cout << "Hello, World!" << std::endl;

    CmdLineArgs cmdLineArgs(argc, argv);

    size_t worker_id_nb = 0;
    if (auto workerId = cmdLineArgs.GetVal("worker_id"); workerId) {
        worker_id_nb = stoi(string(workerId.value()));
    }
    cout << "worker id: " << worker_id_nb << endl;



    //eft::stats::WorkspaceWrapper ws;
    //ws.SetWS(R"(/pbs/home/o/ollukian/public/EFT/git/eftProfiler/source/WS-Comb-STXSxBR_asimov.root)",
    //         "combWS");

    // TODO: move to manager builder, where to set up ws wrapper and ws
    auto* manager = new eft::stats::FitManager();
    manager->SetWsWrapper();
    manager->SetWS(R"(/pbs/home/o/ollukian/public/EFT/git/eftProfiler/source/WS-Comb-STXSxBR_asimov.root)",
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

    cout << "[INFO] extract pdf total" << endl;
    manager->ExtractPdfTotal("combPdf");
    cout << "[INFO] extract data total" << endl;
    manager->ExtractDataTotal("combData");

    //cout << "[INFO] try to fit..." << endl;
    //manager->DoGlobalFit();

    eft::stats::NpRankingStudySettings settings;
    settings.prePostFit = eft::stats::PrePostFit::PREFIT;
    settings.studyType = eft::stats::StudyType::OBSERVED;
    settings.poi = "mu_GG2H_0J_PTH_0_10_ZZ";
    settings.path_to_save_res = "res.json";
    manager->ComputeNpRankingOneWorker(settings, worker_id_nb);

    return 0;
}
