#include <iostream>

#include "Core/WorkspaceWrapper.h"

using namespace std;

int main() {
    std::cout << "Hello, World!" << std::endl;

    eft::stats::WorkspaceWrapper ws;
    ws.SetWS(R"(/pbs/home/o/ollukian/public/EFT/git/eftProfiler/source/WS-Comb-STXSxBR_asimov.root)",
             "combWS");

    //ws.raw()->Print("v");

    cout << "cats:" << endl;
    for(const auto & cat : ws.GetCats()) {
        cout << cat << endl;
    }


    cout << "[NP]:" << endl;
    ws.GetNp()->Print("v");
    cout << "[GlobalObs]:" << endl;
    ws.GetGlobObs()->Print("v");
    cout << "[Obs]:" << endl;
    ws.GetObs()->Print("v");
    cout << "[POIs]:" << endl;
    ws.GetPOIs()->Print("v");

    return 0;
}
