//
// Created by Aleksei Lukianchuk on 23-Jan-23.
//

#include "FreeFitManager.h"
#include "FitManager.h"
#include "StringUtils.h"
#include "Fitter.h"
#include "RooStats/AsymptoticCalculator.h"

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

    if (cmdLineArgs->HasKey("prefit")) {
        fitManager.prePostFit = PrePostFit::PREFIT;
    }
    else if (cmdLineArgs->HasKey("postfit")) {
        fitManager.prePostFit = PrePostFit::POSTFIT;
    }
    else {
        fitManager.prePostFit = PrePostFit::OBSERVED;
    }

    string error_str;
    if (cmdLineArgs->HasKey("errors")) {
        cmdLineArgs->SetValIfArgExists("errors", error_str);
        eft::StringUtils::ToLowCase(error_str);
        if (error_str == "hesse") {
            EFT_PROF_INFO("Errors estimation method: Hesse");
            fitManager.SetErorsEvaluation(fit::Errors::HESSE);
        }
        else if (error_str == "minos") {
            EFT_PROF_INFO("Errors estimation method: Minos");
            fitManager.SetErorsEvaluation(fit::Errors::MINOS_POIS);
            fitManager.fitSettings_.pois_to_estimate_errors = new RooArgSet{};
            for (const auto& poi : pois_to_float_) {
                EFT_PROF_INFO("Add MINOS estimation for: {:6}", poi);
                fitManager.fitSettings_.pois_to_estimate_errors->add(*manager->GetWs()->GetVar(poi));
            }
        }
        else if (error_str == "user") {
            EFT_PROF_INFO("Errors estimation method: User-defined for: ");
            fitManager.SetErorsEvaluation(fit::Errors::USER_DEFINED);
            vector<string> pois_to_estimate_errors;
            cmdLineArgs->SetValIfArgExists("errors_for", pois_to_estimate_errors);
            fitManager.fitSettings_.pois_to_estimate_errors = new RooArgSet{};
            for (const auto& poi : pois_to_estimate_errors) {
                EFT_PROF_INFO("Add MINOS estimation for:", poi);
                fitManager.fitSettings_.pois_to_estimate_errors->add(*manager->GetWs()->GetVar(poi));
            }
        }
        else {
            EFT_PROF_CRITICAL("FreeFit:: errors: {} are not supported - only hesse and MINOS now", error_str);
            throw std::logic_error("inconsistent settings for errors");
        }
    }

    if (cmdLineArgs->HasKey("eps"))
        cmdLineArgs->SetValIfArgExists("eps", fitManager.fitSettings_.eps);
    if (cmdLineArgs->HasKey("retry"))
        cmdLineArgs->SetValIfArgExists("retry", fitManager.fitSettings_.retry);
    if (cmdLineArgs->HasKey("strategy"))
        cmdLineArgs->SetValIfArgExists("strategy", fitManager.fitSettings_.strategy);


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
    if (prePostFit != PrePostFit::OBSERVED) {
        if (prePostFit == PrePostFit::PREFIT) {
            EFT_PROF_INFO("RunFreeFit: prefit => create asimov data and use it for free fit");
            EFT_PROF_INFO("RunFreeFit: reset global observables to 0.");
            ws_->SetVarVal(fitSettings_.globalObs, 0.);

        } else {
            EFT_PROF_INFO("RunFreeFit: postfit => create asimov data and use it for free fit");
        }
        EFT_PROF_INFO("RunFreeFit: create asimov data...");
        data = (RooDataSet *) RooStats::AsymptoticCalculator::MakeAsimovData(*fitSettings_.data,
                                                                             ws_->GetModelConfig(),
                                                                             *pois_to_float,
                                                                             *fitSettings_.globalObs
        );
        EFT_PROF_INFO("RunFreeFit: asimov data created, load it to the workspace...");
        ws_->raw()->saveSnapshot("condGlobObs", *fitSettings_.globalObs, kTRUE);
        ws_->raw()->loadSnapshot("condGlobObs");
        EFT_PROF_INFO("RunFreeFit: asimov data is loaded it to the workspace");
    } else {
        EFT_PROF_INFO("RunFreeFit: observed data => use it for free fit");
        ws_->SetVarVal(fitSettings_.globalObs, 0.);
    }

    fitSettings_.data = data;

    if (stat_only) {
        stat_only = false;

    }

    if (errors_type == fit::Errors::MINOS_POIS || errors_type == fit::Errors::USER_DEFINED) {
        fitSettings_.pois_to_estimate_errors = pois_to_float;
        fitSettings_.pois = pois_to_float;
    }
    //if (fitma)
    //RunFreeFit();
    //ws_->FixValConst(fitSettings_.nps);

    ws_->FixValConst(all_pois);
    ws_->FloatVals(pois_to_float);


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
    fitSettings_.errors = errors_type;

    fitSettings_.save_res = true;
    RooArgList list_pois(*pois_to_float);
    //list_pois.add(*pois_to_float);

    fit::Fitter fitter;
    auto nll_free_fit = fitter.CreatNll(fitSettings_);
    auto res = fitter.Minimize(fitSettings_, nll_free_fit);
    auto cov = res->reducedCovarianceMatrix(list_pois);

    EFT_PROF_DEBUG("RunFreeFit: NPS after free fit");
    fitSettings_.nps->Print("v");
    EFT_PROF_DEBUG("RunFreeFit: globs after free fit");
    fitSettings_.globalObs->Print("v");

    EFT_PROF_INFO("RunFreeFit: pois after free fit:");
    for (auto poi : *fitSettings_.pois_to_estimate_errors) {
        auto ptr = dynamic_cast<RooRealVar*>(poi);

        string is_const_str = "F";
        if (ptr->isConstant())
            is_const_str = "C";

        if (fitSettings_.errors != fit::Errors::USER_DEFINED) {
            EFT_PROF_DEBUG("{:60} [{:10} +- {:10}] {}",
                           ptr->GetName(),
                           ptr->getVal(),
                           ptr->getError(),
                           is_const_str);
        } else {
            EFT_PROF_DEBUG("{:60} [{:10} + {:10} - {:10}] {}",
                           ptr->GetName(),
                           ptr->getVal(),
                           ptr->getErrorHi(),
                           ptr->getErrorLo(),
                           is_const_str);
        }
    }

    EFT_PROF_INFO("RunFreeFit: central values and errors:");
    for (size_t idx_poi_1 {0}; idx_poi_1 < list_pois.size(); ++idx_poi_1) {
        auto poi_1 = dynamic_cast<RooRealVar *>(list_pois.at(idx_poi_1));
        auto val   = poi_1->getVal();
        auto errLo = poi_1->getErrorLo();
        auto errHi = poi_1->getErrorLo();
        EFT_PROF_INFO("{:6}: {:6} + {:6} - {:6}", poi_1->GetName(), val, errLo, errHi);
        poi_1->getVal();
    }


    for (size_t idx_poi_1 {0}; idx_poi_1 < list_pois.size(); ++idx_poi_1) {
        for (size_t idx_poi_2 {idx_poi_1}; idx_poi_2 < list_pois.size(); ++idx_poi_2) {
            auto poi_1 = dynamic_cast<RooRealVar *>(list_pois.at(idx_poi_1));
            auto poi_2 = dynamic_cast<RooRealVar *>(list_pois.at(idx_poi_2));
            //auto corr = cov.operator()(idx_poi_1, idx_poi_2);
            string poi_name_1 = poi_1->GetName();
            string poi_name_2 = poi_2->GetName();
            auto corr = res->correlation(poi_name_1.c_str(), poi_name_2.c_str());
            EFT_PROF_INFO("Correlation: [{:6}][{:6}] = {}", poi_name_1, poi_name_2, corr);
        }
    }

    EFT_PROF_INFO("Save free fit result to a file: {}", "free_fit_res.root");
    TFile fres("free_fit_res.root", "RECREATE");
    res->Write();
    fres.Close();

    // TODO: to save result to a file... via SaveRes()
}

} //  eft::stats::freefit