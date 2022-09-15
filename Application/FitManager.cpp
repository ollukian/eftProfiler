//
// Created by Aleksei Lukianchuk on 14-Sep-22.
//

#include "FitManager.h"
#include "../Fitter/IFitter.h"
#include "../Fitter/Fitter.h"

#include "NpRankingStudyRes.h"

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

void FitManager::ComputeNpRankingOneWorker(NpRankingStudySettings settings, size_t workerId)
{
    RooAbsData* data;
    RooAbsPdf*  pdf = funcs_["pdf_total"];
    auto* globObs = (args_["globObs"]);

    if (settings.studyType == StudyType::EXPECTED) {
        data = data_["asimov_full"];
    }
    else {
        data = data_["ds_total"];
    }

    NpRankingStudyRes res;
    res.poi_name = settings.poi;
    res.statType = settings.statType;
    res.studyType = settings.studyType;

    SetAllNuisanceParamsFloat();
    ws_->FixValConst(res.np_name);

    if (settings.prePostFit == PrePostFit::POSTFIT) {
        // TODO: set np to the values found in fit;
    }

    // * take np # worker_id
    // * create nll
    // * minimise it
    // * fill in results
    // * write them down

    //settings.poi

}

void FitManager::SetAllNuisanceParamsConst() noexcept
{

    if (args_["np"]->empty())
        ExtractNP();

    cout << "[SetAllNuissConst]" << endl;
    cout << "status before:" << endl;

    args_["np"]->Print("v");
    for (const auto& np : *args_["np"]) {
        const string name = {np->GetTitle()};
        cout << fmt::format("dealing with: {} ...", name) << endl;
        //if (string(dynamic_cast<RooRealVar*>(np)->GetTitle()).substr(0, 5) == "ATLAS")
        if (name.substr(0, 5) == "ATLAS") {
            cout << fmt::format("dealing with: {} Set to const", name) << endl;
            dynamic_cast<RooRealVar *>(np)->setConstant(true);
        }
        else {
            cout << fmt::format("dealing with: {} DO NOT set to const", name) << endl;
        }
    }
    cout << "status after:" << endl;
    args_["np"]->Print("v");
}

void FitManager::SetAllNuisanceParamsFloat() noexcept {

    if (args_["np"]->empty())
        ExtractNP();

    cout << "[SetAllNuissFloat]" << endl;
    cout << "status before:" << endl;

    args_["np"]->Print("v");
    for (const auto& np : *args_["np"]) {
        const string name = {np->GetTitle()};
        cout << fmt::format("dealing with: {} ...", name) << endl;
        //if (string(dynamic_cast<RooRealVar*>(np)->GetTitle()).substr(0, 5) == "ATLAS")
        if (name.substr(0, 5) == "ATLAS") {
            cout << fmt::format("dealing with: {} Set to float", name) << endl;
            dynamic_cast<RooRealVar *>(np)->setConstant(false);
        }
        else {
            cout << fmt::format("dealing with: {} DO NOT set to float", name) << endl;
        }
    }
    cout << "status after:" << endl;
    args_["np"]->Print("v");
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