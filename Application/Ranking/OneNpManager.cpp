//
// Created by Aleksei Lukianchuk on 11-Oct-22.
//

#include "../Core/Logger.h"

#include "OneNpManager.h"
#include "IWorkspaceWrapper.h"
//#include "WorkspaceWrapper.h"
#include "../Fitter/Fitter.h"
#include "FitSettings.h"

namespace eft::stats::ranking {

void OneNpManager::ResetNp() {
    EFT_PROF_TRACE("OneNpManager::ResetNp");
    ws_->FixValConst(np_);
    ws_->SetVarVal(np_, np_found_in_data_value);
    ws_->SetVarErr(np_, np_found_in_data_error);
}

void OneNpManager::ResetPoi()
{
    EFT_PROF_TRACE("OneNpManager::ResetPoi");
    ws_->FloatVal(poi_);
    ws_->SetVarVal(poi_, poi_init_value);
    ws_->SetVarErr(poi_, poi_init_error);
}
void OneNpManager::LoadSnapshot(const std::string& name)
{
    EFT_PROF_INFO("OneNpManager::LoadSnapshot {}", name);
    ws_->raw()->loadSnapshot("tmp_nps");
}
void OneNpManager::VaryNpPrefit(char sign) noexcept
{
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
    EFT_PROF_DEBUG("OneNpManager::VaryNpPostfit vary {} on {}sigma", np_, sign);
    if (sign == '+')
        ws_->VaryParNbSigmas(np_, +1.f);
    else if (sign == '-')
        ws_->VaryParNbSigmas(np_, -1.f);
    else
        EFT_PROF_CRITICAL("[OneNpManager::VaryNpPostfit] Called with {}. use only '+' or '-' as an argument",
                          sign);
}

void OneNpManager::RunPreFit(char sign)
{
    EFT_PROF_INFO("[OneNpManager] run pre-fit for {}1", sign);
    ResetToInitState();
    VaryNpPrefit(sign);

    RunFit();

    SavePreFit(sign);
}

void OneNpManager::RunPostFit(char sign)
{
    EFT_PROF_INFO("[OneNpManager] run post-fit for {}sigma", sign);
    ResetToInitState();
    VaryNpPostfit(sign);

    RunFit();

    SavePostFit(sign);
}
void OneNpManager::RunFitFixingNpAtCentralValue()
{
    EFT_PROF_INFO("[OneNpManager] run fit fixing np at its central value");
    ResetToInitState();

    RunFit();

    SaveResAs("fixed_np_fit");
}

void OneNpManager::RunFit()
{
    fit::Fitter fitter;

    fitter.SetNps(nps_);
    fitter.SetGlobs(globs_);

    fit::FitSettings fitSettings;
    fitSettings.pdf = pdf_;
    fitSettings.data = data_;
    fitSettings.pois = pois_;
    fitSettings.errors = errors_;
    fitSettings.nps = nps_;

    fitSettings.nll = fitter.CreatNll(fitSettings);
    auto fitRes = fitter.Minimize(fitSettings);
}

void OneNpManager::SaveResAs(std::string key)
{
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
    EFT_PROF_INFO("OneNpManagerBuilder the settings are correct. Create OneNpManager");
    return true;
}

} // ranking