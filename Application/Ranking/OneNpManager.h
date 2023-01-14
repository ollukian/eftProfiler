//
// Created by Aleksei Lukianchuk on 11-Oct-22.
//

#ifndef EFTPROFILER_ONENPMANAGER_H
#define EFTPROFILER_ONENPMANAGER_H

#include <string>
#include <map>
#include <memory>
#include <FitSettings.h>

#include "RooAbsReal.h"

#include "StudyRes.h"
#include "../Fitter/ErrorsReEvaluation.h"
#include "NpRankingStudyRes.h"

#include "Logger.h"
#include "Profiler.h"

class IWorkspaceWrapper;
class RooArgSet;
class RooAbsPdf;
class RooAbsData;

#include "RooRealVar.h"

//class RooAbsReal;

namespace eft::stats::ranking {

//using Nps   = std::shared_ptr<RooArgSet>;
//using Globs = std::shared_ptr<RooArgSet>;
//using Data  = std::shared_ptr<RooAbsData>;
//using Pdf   = std::shared_ptr<RooAbsPdf>;
using Nps   = RooArgSet*;
using Globs = RooArgSet*;
using Data  = RooAbsData*;
using Pdf   = RooAbsPdf*;
using Poi = std::string;
using NP  = std::string;

class OneNpManagerBuilder;
class OneNpManager;

// TODO: to add an option: float all POIs or only the required one
class OneNpManager {
    friend class OneNpManagerBuilder;
public:
    static inline OneNpManagerBuilder create();
    inline void SetNpPreferredValue(double val = 0., double err = 0.);
    inline void SetPoiPreferredValue(double val = 1., double err = 0.);
    inline void SetErrors(fit::Errors errors) noexcept;

    inline const StudyRes& GetResult(const std::string& type) const;
    void RunPreFit(char sign);
    void RunPostFit(char sign);
    void RunFitFixingNpAtCentralValue();
    void RunFreeFit();

    OneNpManager(const OneNpManager&) = default;
    OneNpManager(OneNpManager&&) = delete;
    OneNpManager operator=(const OneNpManager&) = delete;
    OneNpManager operator=(OneNpManager&&) = delete;
private:
    void ResetNp();
    void ResetPoi();
    inline void ResetToInitState();

    inline void SavePreFit(char sign);
    inline void SavePostFit(char sign);
    void SaveResAs(std::string key);

    void RunFit(); // to be moved
    void VaryNpPrefit(char sign) noexcept;
    void VaryNpPostfit(char sign) noexcept;

    void LoadSnapshot(const std::string& name);
private:
    std::map<std::string, StudyRes> results_;
    std::string                     snapshot_init_values_name_;
    //std::unique_ptr<RooAbsReal> nll_ {nullptr};
    std::shared_ptr<RooAbsReal>     nll_ {nullptr};

    double np_found_in_data_value   {0.};
    double np_found_in_data_error   {0.};
    double poi_init_value           {1.};
    double poi_init_error           {0.};

    mutable Poi   poi_;
    mutable NP    np_;
    mutable Nps   nps_       {nullptr};
    mutable Globs globs_     {nullptr};
    RooAbsData*  data_       {nullptr};
    RooAbsPdf*   pdf_        {nullptr};
    RooArgSet*  pois_        {nullptr};
    fit::Errors  errors_     {fit::Errors::DEFAULT};
    IWorkspaceWrapper* ws_   {nullptr};

    NpRankingStudySettings np_ranking_settings_;
    fit::FitSettings       fitSettings_;
    bool is_initiated_ = false;
    //std::shared_ptr<IWorkspaceWrapper> ws_;
private:
    OneNpManager() noexcept = default;
};


class OneNpManagerBuilder {
public:
    //inline OneNpManager build() const noexcept { return result_; }

    inline operator OneNpManager();

    inline OneNpManagerBuilder& UsingNPs(const Nps& nps) noexcept;
    inline OneNpManagerBuilder& UsingGlobalObservables(const Globs& globs) noexcept;
    inline OneNpManagerBuilder& UsingData(RooAbsData *data) noexcept;
    inline OneNpManagerBuilder& UsingPdf(RooAbsPdf *pdf) noexcept;
    inline OneNpManagerBuilder& ForPOI(Poi poi) noexcept;
    inline OneNpManagerBuilder& ForNP(NP np) noexcept;
    inline OneNpManagerBuilder& UsingWS(IWorkspaceWrapper* ws) noexcept;
    inline OneNpManagerBuilder& UsingSnapshotWithInitVals(std::string name) noexcept;
    inline OneNpManagerBuilder& UsingPOIs(RooArgSet* pois) noexcept;
    inline OneNpManagerBuilder& UsingErrors(fit::Errors errors) noexcept;
    inline OneNpManagerBuilder& UsingFitSettings(NpRankingStudySettings settings) noexcept;
private:
    OneNpManager result_;
private:
    bool CheckValidity();
};

inline void OneNpManager::SetErrors(fit::Errors errors) noexcept
{
    EFT_PROFILE_FN();
    EFT_PROF_INFO("OneNpManagerBuilder::SetErrors evaluation regime to {}", errors);
    errors_ = errors;
}

OneNpManagerBuilder::operator OneNpManager() {
    EFT_PROFILE_FN();
    if (CheckValidity())
        return result_;
    EFT_PROF_CRITICAL("OneNpManagerBuilder cannot create OneNpManager. See the messages above");
    throw std::logic_error("");
}

inline void OneNpManager::ResetToInitState()
{
    EFT_PROFILE_FN();
    EFT_PROF_TRACE("OneNpManagerBuilder::ResetToInitState");
//    EFT_PROF_DEBUG("Nps before loading snapshot:");
//    for (const auto np : *nps_) {
//        auto np_var = dynamic_cast<RooRealVar*>(np);
//        EFT_PROF_DEBUG("{:40}, {} +- {}, const => {}",
//                       np_var->GetName(),
//                       np_var->getVal(),
//                       np_var->getError(),
//                       np_var->isConstant());
//    }
    LoadSnapshot(snapshot_init_values_name_);
//    EFT_PROF_DEBUG("Nps after loading snapshot:");
//    for (const auto np : *nps_) {
//        auto np_var = dynamic_cast<RooRealVar*>(np);
//        EFT_PROF_DEBUG("{:40}, {} +- {}, const => {}",
//                       np_var->GetName(),
//                       np_var->getVal(),
//                       np_var->getError(),
//                       np_var->isConstant());
//    }
    ResetNp();
    ResetPoi();
}

inline void OneNpManager::SavePreFit(char sign)
{
    EFT_PROFILE_FN();
    std::string name {"prefit_"};
    name += sign;
    SaveResAs(std::move(name));
}
inline void OneNpManager::SavePostFit(char sign)
{
    EFT_PROFILE_FN();
    std::string name {"postfit_"};
    name += sign;
    SaveResAs(std::move(name));
}

inline const StudyRes& OneNpManager::GetResult(const std::string& type) const
{
    EFT_PROFILE_FN();
    if (results_.find(type) != results_.end())
        return results_.at(type);
    return {};
}
inline void OneNpManager::SetNpPreferredValue(double val, double err)
{
    EFT_PROFILE_FN();
    EFT_PROF_INFO("OneNpManagerBuilder::SetNpPreferredValue {} +- {}", val, err);
    np_found_in_data_value = val;
    np_found_in_data_error = err;
}
inline void OneNpManager::SetPoiPreferredValue(double val, double err) {
    EFT_PROFILE_FN();
    EFT_PROF_INFO("OneNpManagerBuilder::SetPoiPreferredValue {} +- {}", val, err);
    poi_init_value = val;
    poi_init_error = err;
}
inline OneNpManagerBuilder OneNpManager::create() {
    EFT_PROFILE_FN();
    return {};
}
inline OneNpManagerBuilder& OneNpManagerBuilder::UsingNPs(const Nps& nps) noexcept {
    EFT_PROFILE_FN();
    EFT_PROF_TRACE("OneNpManagerBuilder::UsingNPs");
    result_.nps_ = nps;
    return *this;
}
inline OneNpManagerBuilder& OneNpManagerBuilder::UsingWS(IWorkspaceWrapper* ws) noexcept
{
    EFT_PROFILE_FN();
    EFT_PROF_TRACE("OneNpManagerBuilder::UsingWS");
    //result_.ws_ = std::make_shared<IWorkspaceWrapper>(ws);
    result_.ws_ = ws;
    return *this;
}
inline OneNpManagerBuilder& OneNpManagerBuilder::UsingSnapshotWithInitVals(std::string name) noexcept
{
    EFT_PROFILE_FN();
    EFT_PROF_TRACE("OneNpManagerBuilder::UsingSnapshotWithInitVals {}", name);
    result_.snapshot_init_values_name_ = std::move(name);
    return *this;
}

inline OneNpManagerBuilder& OneNpManagerBuilder::UsingGlobalObservables(const Globs& globs) noexcept
{
    EFT_PROFILE_FN();
    result_.globs_ = globs;
    return *this;
}

inline OneNpManagerBuilder& OneNpManagerBuilder::UsingData(RooAbsData *data) noexcept
{
    EFT_PROFILE_FN();
    result_.data_ = data;
    return *this;
}

inline OneNpManagerBuilder& OneNpManagerBuilder::UsingPdf(RooAbsPdf *pdf) noexcept
{
    EFT_PROFILE_FN();
    EFT_PROF_TRACE("OneNpManagerBuilder::UsingPdf");
    result_.pdf_ = pdf;
    return *this;
}

inline OneNpManagerBuilder& OneNpManagerBuilder::ForPOI(Poi poi) noexcept
{
    EFT_PROFILE_FN();
    EFT_PROF_INFO("OneNpManagerBuilder::ForPOI [{}]", poi);
    result_.poi_ = std::move(poi);
    return *this;
}

inline OneNpManagerBuilder& OneNpManagerBuilder::ForNP(NP np) noexcept
{
    EFT_PROFILE_FN();
    EFT_PROF_INFO("OneNpManagerBuilder::ForNP [{}]", np);
    result_.np_ = std::move(np);
    return *this;
}
inline OneNpManagerBuilder& OneNpManagerBuilder::UsingPOIs(RooArgSet* pois) noexcept
{
    EFT_PROFILE_FN();
    EFT_PROF_TRACE("OneNpManagerBuilder::UsingPOIs");
    result_.pois_ = pois;
    return *this;
}
inline OneNpManagerBuilder& OneNpManagerBuilder::UsingErrors(fit::Errors errors) noexcept
{
    EFT_PROFILE_FN();
    EFT_PROF_INFO("OneNpManagerBuilder::UsingErrors [{}]", errors);
    result_.errors_ = errors;
    return *this;
}
inline OneNpManagerBuilder& OneNpManagerBuilder::UsingFitSettings(NpRankingStudySettings settings) noexcept
{
    EFT_PROFILE_FN();
    result_.np_ranking_settings_ = std::move(settings);
    return *this;
}

} // eft::stats::ranking

#endif //EFTPROFILER_ONENPMANAGER_H
