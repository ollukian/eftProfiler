#include <iostream>

#include "Core/WorkspaceWrapper.h"

int main() {
    std::cout << "Hello, World!" << std::endl;

    eft::stats::WorkspaceWrapper ws;
    ws.SetWS(R"(C:\!PhD\EFT\combination_exercise\wcs\WS-Comb-STXSxBR_asimov.root)",
             "combWS");

    return 0;
}
