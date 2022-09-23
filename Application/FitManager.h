//
// Created by Aleksei Lukianchuk on 14-Sep-22.
//

#ifndef EFTPROFILER_FITMANAGER_H
#define EFTPROFILER_FITMANAGER_H

#include <CommandLineArgs.h>
#include "IFitManager.h"
#include "FitManagerConfig.h"
#include "WorkspaceWrapper.h"

#include "../Core/Logger.h"
#include "../Core/CommandLineArgs.h"


namespace eft::stats {

class FitManager : public IFitManager {
public:
    FitManager() = default;
    //FitManager(FitManagerConfig&& config) noexcept;
    ~FitManager() noexcept override = default;

    void Init(FitManagerConfig&& config);
    static void ReadConfigFromCommandLine(CommandLineArgs& commandLineArgs, FitManagerConfig& config) noexcept;

    void DoGlobalFit() override;
    void ComputeNpRankingOneWorker(NpRankingStudySettings settings, size_t workerId) override;
    void DoFitAllNpFloat(NpRankingStudySettings settings) override;

    inline void SetNpNames(std::string name) const noexcept override;
    inline void SetObsNames(std::string name) const noexcept override;
    inline void SetGlobObsNames(std::string name) const noexcept override;
    inline void SetCatsNames(std::string name) const noexcept override;

    inline void SetWsWrapper() noexcept override;
    inline void SetWS(std::string path, std::string name) override;
    inline void SetModelConfig(std::string name) override;

    inline void ExtractNP()      noexcept override;
    inline void ExtractObs()     noexcept override;
    inline void ExtractGlobObs() noexcept override;
           void ExtractPOIs()    noexcept override;
    inline void ExtractCats()    noexcept override;

    inline void ExtractDataTotal(std::string name) override;
    inline void ExtractPdfTotal(std::string name)  override;

    // TODO: to implement from the prev code
    void SetAllNuisanceParamsConst() noexcept override;
    void SetAllNuisanceParamsFloat() noexcept override;
    void SetAllNuisanceParamsToValue(float val) noexcept override;
    void SetGlobalObservablesToValueFoundInFit() noexcept override{};

    void SetAllPOIsConst() noexcept override;
    void SetAllPOIsFloat() noexcept override;

    inline void SetAllGlobObsConst() noexcept override;
    inline void SetAllGlobObsFloat() noexcept override;
    inline void SetAllGlobObsTo(float val) noexcept override;

    void CreateAsimovData(PrePostFit studyType) noexcept override;

    inline RooAbsData& GetData(PrePostFit studyType) noexcept override;
    inline void        SetUpGlobObs(PrePostFit studyType) noexcept override;

    [[nodiscard]]
    inline const DataClosure& GetDataClosure() const noexcept override {return data_;}
    [[nodiscard]]
    inline const FuncClosure& GetFuncClosure() const noexcept override {return funcs_;}

    [[nodiscard]]
    inline const ArgsClosure & GetArgsClosure() const noexcept override {return args_;}

    [[nodiscard]]
    inline const RooAbsData* GetData(std::string&& name) const override {return data_.at(name);}
    [[nodiscard]]
    inline const RooAbsPdf*  GetPdf (std::string&& name) const override {return funcs_.at(name);}

    inline const std::vector<std::string>& GetListPOIs() const noexcept override;

    inline IWorkspaceWrapper* ws() override { return ws_; }
private:
    DataClosure data_{};
    FuncClosure funcs_{};
    ArgsClosure args_{};

    std::vector<std::string> pois_;

    IWorkspaceWrapper* ws_ = nullptr;

    mutable std::string np_names{};
    mutable std::string obs_names{};
    mutable std::string glob_obs_names{};
    mutable std::string cat_names{};
};

inline void FitManager::SetNpNames(std::string name) const noexcept
{
    np_names = std::move(name);
};
inline void FitManager::SetObsNames(std::string name) const noexcept
{
    obs_names = std::move(name);
};
inline void FitManager::SetGlobObsNames(std::string name) const noexcept
{
    glob_obs_names = std::move(name);
};
inline void FitManager::SetCatsNames(std::string name) const noexcept
{
    cat_names = std::move(name);
};
inline void FitManager::ExtractNP()      noexcept
{

    assert(ws_ != nullptr);
    args_["np_all"] = (RooArgSet *) ws_->GetNp();
    args_["np"]     = (RooArgSet *) ws_->GetNp();
    EFT_PROF_DEBUG("Extracted NP:");
    for (const auto& np : *args_["np"]) {
        np->Print();
    }
//    assert(ws_ != nullptr);
//    args_["np_all"] = (RooArgSet *) ws_->GetNp();
//    auto* real_np = new RooArgSet();
//    //std::cout << fmt::format("[ExtractNp] check for real np") << std::endl;
//    for (const auto& np : *args_["np_all"]) {
//        const std::string name = {np->GetTitle()};
//        //std::cout << fmt::format("[ExtractNp]dealing with: {:40} ...", name) << std::endl;
//        if (name.substr(0, 5) == "ATLAS") {
//            //std::cout << fmt::format("[ExtractNp] dealing with: {:40} OK Add it", name) << std::endl;
//            real_np->add(*np);
//            //dynamic_cast<RooRealVar *>(np)->setConstant(false);
//        }
//        else {
//            //std::cout << fmt::format("dealing with: {:40} DO NOT add it", name) << std::endl;
//        }
//    }
    //std::cout << fmt::format("[ExtractNp] real np list:") << std::endl;
    //real_np->Print("v");
    //args_["np"] = real_np;

    EFT_PROF_INFO("[FitManager] Extracted {} np      to args[np_all]", args_["np_all"]->size());
    EFT_PROF_INFO("[FitManager] Extracted {} real_np to args[np]",     args_["np"]->size());
}
inline void FitManager::ExtractObs() noexcept
{
    assert(ws_ != nullptr);
    args_["obs"] = (RooArgSet *) ws_->GetObs();
    EFT_PROF_INFO("[FitManager] Extracted {} obs to args[obs]", args_["obs"]->size());
}
inline void FitManager::ExtractGlobObs()     noexcept
{
    assert(ws_ != nullptr);
    args_["globObs"] = (RooArgSet *) ws_->GetGlobObs();
    EFT_PROF_INFO("[FitManager] Extracted {} globObs to args[globObs]", args_["globObs"]->size());
}
inline void FitManager::ExtractCats() noexcept
{
    assert(ws_ != nullptr);
    std::cout << "ERROR NOT IMPLEMENTED" << std::endl;
}

inline void FitManager::SetWsWrapper() noexcept
{
    ws_ = new WorkspaceWrapper();
}

inline void FitManager::SetWS(std::string path, std::string name)
{
    EFT_PROF_INFO("[FitManager] set ws: {} from {}", name, path);
    if (ws_->SetWS(std::move(path), std::move(name)))
        EFT_PROF_INFO("[FitManager] successfully set ws: {} from {}", name, path);
    else
        EFT_PROF_INFO("[FitManager] ERROR setting ws: {} from {}", name, path);
}
inline void FitManager::SetModelConfig(std::string name)
{
    std::cout << "set model config" << std::endl;
    if (ws_->SetModelConfig(std::move(name)))
        std::cout << "set model config DONE" << std::endl;
    else
        std::cout << fmt::format("[FitManager] error setting model config") << std::endl;
}

inline void FitManager::ExtractDataTotal(std::string name)
{
    assert(ws_ != nullptr);
    data_["ds_total"] = ws_->GetData(name);
}

inline void FitManager::ExtractPdfTotal(std::string name) {
    assert(ws_ != nullptr);
    funcs_["pdf_total"] = ws_->GetCombinedPdf(name);
}

inline const std::vector<std::string>& FitManager::GetListPOIs() const noexcept
{
    assert(ws_ != nullptr);
    return pois_;
}

inline void FitManager::SetAllPOIsConst() noexcept
{
    EFT_PROF_INFO("[FitManager] Set All POIs const");
    if (pois_.empty())
        ExtractPOIs();
    for (const auto& poi : pois_) {
        ws_->FixValConst(poi);
        EFT_PROF_DEBUG("Set poi: {:10} to const ==> {} +- {} (const? ==> {})",
                       poi,
                       ws_->GetParVal(poi),
                       ws_->GetParErr(poi),
                       ws_->raw()->var(poi.c_str())->isConstant()
                       );
    }
}
inline void FitManager::SetAllPOIsFloat() noexcept
{
    EFT_PROF_INFO("[FitManager] Set All POIs float");
    if (pois_.empty())
        ExtractPOIs();
    for (const auto& poi : pois_) {
        ws_->FloatVal(poi);
        EFT_PROF_DEBUG("Set poi: {:10} to float ==> {} +- {} (const? ==> {})",
                       poi,
                       ws_->GetParVal(poi),
                       ws_->GetParErr(poi),
                       ws_->raw()->var(poi.c_str())->isConstant()
        );
    }
}

inline void FitManager::SetAllGlobObsConst() noexcept
{
    EFT_PROF_TRACE("[FitManager]{SetAllGlobObsConst}");
    for (const auto& globObs : *args_["globObs"]) {
        const std::string name = {globObs->GetTitle()};
        dynamic_cast<RooRealVar *>(globObs)->setConstant(true);
        EFT_PROF_DEBUG("[FitManager] set {} const, status after: {} +- {}  (const? ==> {})", name,
                       dynamic_cast<RooRealVar *>(globObs)->getVal(),
                       dynamic_cast<RooRealVar *>(globObs)->getError(),
                       dynamic_cast<RooRealVar *>(globObs)->isConstant());
    }
}
inline void FitManager::SetAllGlobObsFloat() noexcept
{
    EFT_PROF_TRACE("[FitManager]{SetAllGlobObsFloat}");
    for (const auto& globObs : *args_["globObs"]) {
        const std::string name = {globObs->GetTitle()};
        dynamic_cast<RooRealVar *>(globObs)->setConstant(false);
        EFT_PROF_DEBUG("[FitManager] set {:30} float, status after: {} +- {}  (const? ==> {})", name,
                       dynamic_cast<RooRealVar *>(globObs)->getVal(),
                       dynamic_cast<RooRealVar *>(globObs)->getError(),
                       dynamic_cast<RooRealVar *>(globObs)->isConstant());
    }
}
inline void FitManager::SetAllGlobObsTo(float val) noexcept
{
    EFT_PROF_TRACE("[FitManager]SetAllGlobObsTo {}", val);
    for (const auto& globObs : *args_["globObs"]) {
        const std::string name = {globObs->GetTitle()};
        EFT_PROF_DEBUG("[FitManager][SetAllGlobObsTo] status of {:30} before: {} +- {}  (const? ==> {})",
                       name,
                       dynamic_cast<RooRealVar *>(globObs)->getVal(),
                       dynamic_cast<RooRealVar *>(globObs)->getError(),
                       dynamic_cast<RooRealVar *>(globObs)->isConstant());
        dynamic_cast<RooRealVar *>(globObs)->setVal(val);
        EFT_PROF_DEBUG("[FitManager][SetAllGlobObsTo] status of {:30} after: {} +- {}  (const? ==> {})",
                       name,
                       dynamic_cast<RooRealVar *>(globObs)->getVal(),
                       dynamic_cast<RooRealVar *>(globObs)->getError(),
                       dynamic_cast<RooRealVar *>(globObs)->isConstant());
    }
}

inline void FitManager::SetAllNuisanceParamsToValue(float val) noexcept
{
    EFT_PROF_TRACE("[FitManager] SetAllNPto {}", val);
    for (const auto& globObs : *args_["np"]) {
        const std::string name = {globObs->GetTitle()};
        EFT_PROF_DEBUG("[FitManager][SetAllNPto] status of {:30} after: {} +- {}  (const? ==> {}",
                       name,
                       dynamic_cast<RooRealVar *>(globObs)->getVal(),
                       dynamic_cast<RooRealVar *>(globObs)->getError(),
                       dynamic_cast<RooRealVar *>(globObs)->isConstant());
        dynamic_cast<RooRealVar *>(globObs)->setVal(val);
        EFT_PROF_DEBUG("[FitManager][SetAllNPto] status of {:30} after: {} +- {}  (const? ==> {})",
                       name,
                       dynamic_cast<RooRealVar *>(globObs)->getVal(),
                       dynamic_cast<RooRealVar *>(globObs)->getError(),
                       dynamic_cast<RooRealVar *>(globObs)->isConstant());
    }
}

inline RooAbsData& FitManager::GetData(PrePostFit studyType) noexcept
{
    if (studyType == PrePostFit::OBSERVED) {
        EFT_PROF_TRACE("[FitManager]{GetData} Observed, return data_[ \"ds_total\" ]");
        return *data_[ "ds_total" ];
    }
    CreateAsimovData(studyType);
    if (studyType == PrePostFit::PREFIT) {
        EFT_PROF_TRACE("[FitManager]{GetData} PREFIT, return data_[ \"asimov_prefit\" ]");
        return *data_[ "asimov_prefit" ];
    }
    EFT_PROF_TRACE("[FitManager]{GetData} POSTFIT, return data_[ \"asimov_postfit\" ]");
    return *data_[ "asimov_postfit" ];
}
inline void FitManager::SetUpGlobObs(PrePostFit studyType) noexcept
{
    EFT_PROF_TRACE("[FitManager]{SetUpGlobObs}");
    if (studyType == PrePostFit::POSTFIT) {
        EFT_PROF_INFO("[FitManager]{SetUpGlobObs} POSTFIT ==> fix glob obs to const, not change their values");
    }
    else if (studyType == PrePostFit::PREFIT) {
        EFT_PROF_INFO("[FitManager]{SetUpGlobObs} PREFIT ==> fix glob obs to 0 (const)");
        SetAllGlobObsTo(0);
    }
    else {
        EFT_PROF_INFO("[FitManager]{SetUpGlobObs} OBSERVED ==> fix glob obs to (const), not change their values");
        //SetAllGlobObsTo(0);
    }
    SetAllGlobObsConst();
}


} // eft::stats

#endif //EFTPROFILER_FITMANAGER_H
