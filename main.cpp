#include <iostream>

#include "Core/WorkspaceWrapper.h"
#include "Application/FitManager.h"

using namespace std;

int main() {
    std::cout << "Hello, World!" << std::endl;

    //eft::stats::WorkspaceWrapper ws;
    //ws.SetWS(R"(/pbs/home/o/ollukian/public/EFT/git/eftProfiler/source/WS-Comb-STXSxBR_asimov.root)",
    //         "combWS");

    eft::stats::FitManager* manager;
    cout << "set ws wrapper" << endl;
    manager->SetWsWrapper();
    cout << "set" << endl;
    manager->SetWS(R"(/pbs/home/o/ollukian/public/EFT/git/eftProfiler/source/WS-Comb-STXSxBR_asimov.root)",
                          "combWS");
    cout << "set model config" << endl;
    manager->SetModelConfig("ModelConfig");

    //manager->ws()
    //ws.raw()->Print("v");

    cout << "cats:" << endl;
    for(const auto & cat : manager->ws()->GetCats()) {
        cout << cat << endl;
    }




    cout << "[NP]:" << endl;
    manager->ws()->GetNp()->Print("v");
    cout << "[GlobalObs]:" << endl;
    manager->ws()->GetGlobObs()->Print("v");
    cout << "[Obs]:" << endl;
    manager->ws()->GetObs()->Print("v");
    cout << "[POIs]:" << endl;
    manager->ws()->GetPOIs()->Print("v");

    return 0;
}
