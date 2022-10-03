//
// Created by Aleksei Lukianchuk on 03-Oct-22.
//

#include "RooStats/ModelConfig.h"
#include "TFile.h"
#include "RooFitResult.h"

#include <iostream>

using namespace std;

int TestRooFit()
{
    auto file = TFile::Open("source/WS-Comb-Higgs-topU3l_obs.root");
    auto combWS = dynamic_cast<RooWorkspace*>(file->Get("combWS"));
    auto pdf = combWS->pdf("combPdf");
    auto data = combWS->data("combData");
    auto mc = dynamic_cast<RooStats::ModelConfig*>(combWS->obj("ModelConfig"));
    auto globs = mc->GetNuisanceParameters();

    cout << "[Info] before fit to" << endl;
    auto res = pdf->fitTo(*data,
               RooFit::Constrain(*globs),
               RooFit::BatchMode(true),
               RooFit::Offset(true),
               RooFit::SumW2Error(true),
               RooFit::Save(true)
               );

    cout << "[Info] after fit to" << endl;

    return 0;
}