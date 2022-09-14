#include <iostream>

#include "Core/WorkspaceWrapper.h"

int main() {
    std::cout << "Hello, World!" << std::endl;

    eft::stats::WorkspaceWrapper ws;
    ws.SetWS(R"(/pbs/home/o/ollukian/public/EFT/git/eftProfiler/source/WS-Comb-STXSxBR_asimov.root)",
             "combWS");

    ws.raw()->Print("v");

    return 0;
}
