//
// Created by Aleksei Lukianchuk on 14-Sep-22.
//

#include "FitManager.h"
#include "../Fitter/IFitter.h"
#include "../Fitter/Fitter.h"

using namespace std;

namespace eft::stats {

void FitManager::DoGlobalFit()
{
    cout << "[DoGlobalFit]" << endl;
    auto* pdf = funcs_["pdf_total"];
    auto* ds = data_["ds_total"];
    auto* globObs = (args_["globObs"]);

    cout << "print pdf:" << endl;
    pdf->Print("V");

    cout << "print ds:" << endl;
    ds->Print("V");

    cout << "print global obs:" << endl;
    globObs->Print("V");

    cout << "[create nll]" << endl;

    fit::Fitter fitter;
    auto nll = fitter.CreatNll(ds, pdf, globObs);
    cout << "[minimize it]" << endl;
    auto res = fitter.Minimize(nll, pdf);
    cout << "[minimisation done]" << endl;
    cout << "res: " << endl;
    res->Print("v");
}

//    inline void FitManager::SetGlobalObservablesToValueFoundInFit() {
//        for (auto& obs : *globalObservables_) {
//            cout << " # set global obs: |" << obs->GetName() << "|";
//
//            string name = string(obs->GetName());
//            name = name.substr(string("RNDM__").size(), name.size() );
//
//            double foundValue = ws_->var( name.c_str() )->getVal();
//            static_cast<RooRealVar*>( obs )->setVal( foundValue  );
//
//            cout << " to " << foundValue << endl;
//        }
//    }

} // stats