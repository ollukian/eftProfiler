//
// Created by Aleksei Lukianchuk on 11-Oct-22.
//

#include "../Core/Logger.h"

#include "OneNpManager.h"
#include "IWorkspaceWrapper.h"
//#include "WorkspaceWrapper.h"
#include "../Fitter/Fitter.h"
#include "FitSettings.h"

#include "RooRealVar.h"

namespace eft::stats::ranking {

void OneNpManager::ResetNp() {
    EFT_PROFILE_FN();
    EFT_PROF_TRACE("OneNpManager::ResetNp");
    ws_->SetVarVal(np_, np_found_in_data_value);
    ws_->SetVarErr(np_, np_found_in_data_error);
    ws_->FixValConst(np_);
}

void OneNpManager::ResetPoi()
{
    EFT_PROFILE_FN();
    EFT_PROF_TRACE("OneNpManager::ResetPoi");

    EFT_PROF_INFO("Fix all pois const and to their init values");
    for (auto poi : *pois_) {
        auto poi_cast = dynamic_cast<RooRealVar*>(poi);
        poi_cast->setVal(poi_init_value);
        poi_cast->setError(poi_init_error);
        poi_cast->setConstant(true);
        EFT_PROF_DEBUG("Is {:10} const ==> {}", poi_cast->GetName(), poi_cast->isConstant());
    }

    if (np_ranking_settings_.fit_all_pois) {
        EFT_PROF_DEBUG("Fit all pois => float all of them");
        for (auto poi : *pois_) {
            auto poi_cast = dynamic_cast<RooRealVar*>(poi);
            poi_cast->setConstant(false);
            EFT_PROF_DEBUG("Is {:10} const ==> {}", poi_cast->GetName(), poi_cast->isConstant());
        }
    }
    else {
        EFT_PROF_DEBUG("Fit one poi => float only {} and fix the rest", poi_);
        ws_->FloatVal(poi_);
        EFT_PROF_DEBUG("Status of all pois:");
        for (auto poi : *pois_) {
            auto poi_cast = dynamic_cast<RooRealVar*>(poi);
            EFT_PROF_DEBUG("using macro:", *poi_cast);
            EFT_PROF_DEBUG("Is {:10} const ==> {}", poi_cast->GetName(), poi_cast->isConstant());
        }
    }
    ws_->SetVarVal(poi_, poi_init_value);
    ws_->SetVarErr(poi_, poi_init_error);
}
void OneNpManager::LoadSnapshot(const std::string& name)
{
    EFT_PROFILE_FN();
    EFT_PROF_INFO("OneNpManager::LoadSnapshot {}", name);
    ws_->raw()->loadSnapshot("tmp_nps");
}
void OneNpManager::VaryNpPrefit(char sign) noexcept
{
    EFT_PROFILE_FN();
    EFT_PROF_DEBUG("OneNpManager::VaryNpPrefit vary {} on {}1", np_, sign);
    if (sign == '+')
        ws_->SetVarVal(np_, np_found_in_data_value + 1.);
    else if (sign == '-')
        ws_->SetVarVal(np_, np_found_in_data_value - 1.);
    else
        EFT_PROF_CRITICAL("[OneNpManager::VaryNpPrefit] Called with {}. use only '+' or '-' as an argument",
                          sign);
}
void OneNpManager::VaryNpPostfit(char sign) noexcept
{
    EFT_PROFILE_FN();
    EFT_PROF_DEBUG("OneNpManager::VaryNpPostfit vary {} on {}sigma", np_, sign);
    if (sign == '+')
        ws_->SetVarVal(np_, np_found_in_data_value + np_found_in_data_error);
        //ws_->VaryParNbSigmas(np_, +1.f);
    else if (sign == '-')
        ws_->SetVarVal(np_, np_found_in_data_value - np_found_in_data_error);
        //ws_->VaryParNbSigmas(np_, -1.f);
    else
        EFT_PROF_CRITICAL("[OneNpManager::VaryNpPostfit] Called with {}. use only '+' or '-' as an argument",
                          sign);
}

void OneNpManager::RunPreFit(char sign)
{
    EFT_PROFILE_FN();
    EFT_PROF_INFO("[OneNpManager] run pre-fit for {}1", sign);
    ResetToInitState();
    VaryNpPrefit(sign);

    RunFit();

    SavePreFit(sign);
}

void OneNpManager::RunPostFit(char sign)
{
    EFT_PROFILE_FN();
    EFT_PROF_INFO("[OneNpManager] run post-fit for {}sigma", sign);
    ResetToInitState();
    VaryNpPostfit(sign);

    RunFit();

    SavePostFit(sign);
}

void OneNpManager::RunFreeFit()
{
    EFT_PROFILE_FN();
    EFT_PROF_INFO("[OneNpManager] run free fit");
    ResetPoi(); // no reset np, since we know nothing about them yet
    RunFit();
    SaveResAs("free_fit");
}

void OneNpManager::RunFitFixingNpAtCentralValue()
{
    EFT_PROFILE_FN();
    EFT_PROF_INFO("[OneNpManager] run fit fixing np at its central value");
    ResetToInitState();

    RunFit();

    SaveResAs("fixed_np_fit");
}

void OneNpManager::RunFit()
{
    EFT_PROFILE_FN();
    EFT_PROF_DEBUG("Run Fit");
    fit::Fitter fitter;

    fitter.SetNps(nps_);
    fitter.SetGlobs(globs_);

    if ( ! is_initiated_ ) {
        fitSettings_.pdf        = pdf_;
        fitSettings_.data       = data_;
        fitSettings_.pois       = pois_;
        fitSettings_.errors     = errors_;
        fitSettings_.nps        = nps_;
        fitSettings_.retry      = np_ranking_settings_.retry;
        fitSettings_.strategy   = np_ranking_settings_.strategy;
        fitSettings_.eps = np_ranking_settings_.eps;
        is_initiated_ = true;
        EFT_PROF_INFO("Status of POIs before nll creation:");
        for (auto poi : *pois_) {
            auto poi_cast = dynamic_cast<RooRealVar*>(poi);
            EFT_PROF_DEBUG("Is {:10} const ==> {}", poi_cast->GetName(), poi_cast->isConstant());
        }
    }

    // errors may change from run to run
    fitSettings_.errors = errors_;

    if ( ! np_ranking_settings_.reuse_nll || nll_ == nullptr) {
        EFT_PROF_INFO("Need to create nll");
        nll_.reset(fitter.CreatNll(fitSettings_));
    }
    else {
        EFT_PROF_INFO("No need to re-create nll");
    }

    fitter.Minimize(fitSettings_, nll_.get());
}

void OneNpManager::SaveResAs(std::string key)
{
    EFT_PROFILE_FN();
    StudyRes res;
    res.poi_name = poi_;
    res.np_name = np_;
    res.poi_val = ws_->GetParVal(poi_);
    res.poi_err = ws_->GetParErr(poi_);
    EFT_PROF_DEBUG("OneNpManager::SaveResAs save res for poi: {}, np {}: {} +- {} as {}",
                   poi_,
                   np_,
                   res.poi_val,
                   res.poi_err,
                   key);
    results_[std::move(key)] = std::move(res);
}

bool OneNpManagerBuilder::CheckValidity()
{
    EFT_PROFILE_FN();
    if (result_.nps_ == nullptr) {
        EFT_PROF_CRITICAL("OneNpManagerBuilder no nps set");
        return false;
    }
    if (result_.globs_ == nullptr) {
        EFT_PROF_CRITICAL("OneNpManagerBuilder no global Observables set");
        return false;
    }
    if (result_.data_ == nullptr) {
        EFT_PROF_CRITICAL("OneNpManagerBuilder no data set");
        return false;
    }
    if (result_.pdf_ == nullptr) {
        EFT_PROF_CRITICAL("OneNpManagerBuilder no pdf set");
        return false;
    }
    if (result_.ws_ == nullptr) {
        EFT_PROF_CRITICAL("OneNpManagerBuilder no WorkSpace set");
        return false;
    }
    if (result_.pois_ == nullptr) {
        EFT_PROF_CRITICAL("OneNpManagerBuilder no pois set");
        return false;
    }
    if (result_.np_.empty()) {
        EFT_PROF_CRITICAL("OneNpManagerBuilder no np to check set");
        return false;
    }
    if (result_.snapshot_init_values_name_.empty()) {
        EFT_PROF_CRITICAL("OneNpManagerBuilder no snapshot to load is set");
        return false;
    }

    // do not check now
    // - POI
    // -
    EFT_PROF_DEBUG("OneNpManagerBuilder the settings are correct. Create OneNpManager");
    return true;
}

} // ranking