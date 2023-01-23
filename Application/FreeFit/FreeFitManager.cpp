//
// Created by Aleksei Lukianchuk on 23-Jan-23.
//

#include "FreeFitManager.h"
#include "FitManager.h"
#include "StringUtils.h"
#include "Fitter.h"

using namespace std;

namespace eft::stats::freefit {

FreeFitManager FreeFitManager::InitFromCommandLine(const std::shared_ptr<CommandLineArgs>& cmdLineArgs) {
    EFT_PROFILE_FN();

    auto manager = new FitManager{};
    eft::stats::FitManagerConfig config;
    eft::stats::FitManager::ReadConfigFromCommandLine(*cmdLineArgs, config);
    manager->Init(std::move(config));

    vector<string> pois_to_float_;

    if (cmdLineArgs->HasKey("pois_float")) {
        cmdLineArgs->SetValIfArgExists("pois_float", pois_to_float_);
    }

    auto globObs = (RooArgSet*) manager->GetListAsArgSet("paired_globs")->clone("globs");
    auto nps = (RooArgSet*) manager->GetListAsArgSet("paired_nps")->clone("nps"); // TODO: refactor to get nps
    auto pdf = (RooAbsPdf*) manager->GetPdf("pdf_total")->clone("pdf");
    //RooAbsData* data = ws_-
    //auto data* =  manager->GetData(prePostFit);

    if (nps == nullptr) {
        EFT_PROF_CRITICAL("FreeFitManager::InitFromCommandLine nps are nullptr");
        throw std::logic_error("Nps are not set in FreeFitManager");
    }
    if (globObs == nullptr) {
        EFT_PROF_CRITICAL("FreeFitManager::InitFromCommandLine globs are nullptr");
        throw std::logic_error("Global observables are not set in FreeFitManager");
    }
    if (pdf == nullptr) {
        EFT_PROF_CRITICAL("FreeFitManager::InitFromCommandLine pdf is nullptr");
        throw std::logic_error("PDF is not set in FreeFitManager");
    }

    auto pdf_to_use = dynamic_cast<RooAbsPdf*>(pdf->clone());

    auto nps_to_use = new RooArgSet{};
    nps_to_use->add(*nps);

    auto globs_to_use = new RooArgSet{};
    globs_to_use->add(*globObs);

    auto pois = new RooArgSet{};

    for (const auto& poi : manager->GetListPOIs()) {
        EFT_PROF_INFO("\t add: {}", poi);
        pois->add(*manager->ws()->GetVar(poi));
    }

    FreeFitManager fitManager;

    fitManager
            .SetWS(manager->GetWs())
            .SetPOIsFullList(pois)
            .SetPOIsToFloat(pois_to_float_)
            .SetGlobs(globs_to_use)
            .SetNPs(nps_to_use)
            .SetData(&manager->GetData(PrePostFit::OBSERVED))
            .SetPDF(pdf_to_use);

    string error_str;
    if (cmdLineArgs->HasKey("errors")) {
        cmdLineArgs->SetValIfArgExists("errors", error_str);
        eft::StringUtils::ToLowCase(error_str);
        if (error_str == "hesse") {
            fitManager.SetErorsEvaluation(fit::Errors::HESSE);
        }
        else {
            EFT_PROF_CRITICAL("FreeFit:: errors: {} are not supported - only hesse now", error_str);
            throw std::logic_error("inconsistent settings for errors");
        }
    }
    return fitManager;

}

FreeFitManager& FreeFitManager::SetPOIsToFloat(const vector <std::string>& list) {
    if (ws_ == nullptr) {
        EFT_PROF_CRITICAL("Set WS before calling to SetPOIsToFloat[strings]");
        throw std::runtime_error("");
    }
    pois_to_float = new RooArgSet();
    for (const auto& poi : list) {
        EFT_PROF_DEBUG("Add {:10} to the list of the POIs to float", poi);
        pois_to_float->add(*ws_->GetVar(poi));
    }
    return *this;
}

void FreeFitManager::RunFit() {
    EFT_PROFILE_FN();

    // TODO: add asimov data
    auto data = fitSettings_.data;

    ws_->FixValConst(all_pois);
    ws_->FloatVals(pois_to_float);
    ws_->SetVarVal(fitSettings_.globalObs, 0.);

    EFT_PROF_INFO("RunFreeFit: pois before free fit:");
    for (auto poi : *all_pois) {
        auto ptr = dynamic_cast<RooRealVar*>(poi);

        string is_const_str = "F";
        if (ptr->isConstant())
            is_const_str = "C";
        EFT_PROF_DEBUG("{:60} [{:10} +- {:10}] {}",
                       ptr->GetName(),
                       ptr->getVal(),
                       ptr->getError(),
                       is_const_str);
    }
    EFT_PROF_DEBUG("RunFreeFit: NPS before free fit");
    fitSettings_.nps->Print("v");
    EFT_PROF_DEBUG("RunFreeFit: globs before free fit");
    fitSettings_.globalObs->Print("v");

    fit::Fitter fitter;
    auto nll_free_fit = fitter.CreatNll(fitSettings_);
    fitter.Minimize(fitSettings_, nll_free_fit);
    EFT_PROF_INFO("RunFreeFit: pois after free fit:");
    for (auto poi : *all_pois) {
        auto ptr = dynamic_cast<RooRealVar*>(poi);

        string is_const_str = "F";
        if (ptr->isConstant())
            is_const_str = "C";
        EFT_PROF_DEBUG("{:60} [{:10} +- {:10}] {}",
                       ptr->GetName(),
                       ptr->getVal(),
                       ptr->getError(),
                       is_const_str);
    }
    EFT_PROF_DEBUG("RunFreeFit: NPS after free fit");
    fitSettings_.nps->Print("v");
    EFT_PROF_DEBUG("RunFreeFit: globs after free fit");
    fitSettings_.globalObs->Print("v");
}

} //  eft::stats::freefit