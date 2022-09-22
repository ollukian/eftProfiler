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
    void SetAllNuisanceParamsToValue(float val) noexcept override{};
    void SetGlobalObservablesToValueFoundInFit() noexcept override{};

    void SetAllPOIsConst() noexcept override;
    void SetAllPOIsFloat() noexcept override;



    //TODO: imlement: create asimov data
    inline void CreateAsimovData() noexcept override {
        std::cerr << "TO IMPLEMENT~" << std::endl;
    };

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
    //std::cout << fmt::format("[FitManager] Extracted {} np      to args[np_all]", args_["np_all"]->size()) << std::endl;
    //std::cout << fmt::format("[FitManager] Extracted {} real np to args[np]",     args_["np"]->size()) << std::endl;
}
inline void FitManager::ExtractObs() noexcept
{
    assert(ws_ != nullptr);
    args_["obs"] = (RooArgSet *) ws_->GetObs();
    EFT_PROF_INFO("[FitManager] Extracted {} obs to args[obs]", args_["obs"]->size());
    //std::cout << fmt::format("[FitManager] Extracted {} obs to args[obs]", args_["obs"]->size()) << std::endl;
}
inline void FitManager::ExtractGlobObs()     noexcept
{
    assert(ws_ != nullptr);
    args_["globObs"] = (RooArgSet *) ws_->GetGlobObs();
    EFT_PROF_INFO("[FitManager] Extracted {} globObs to args[globObs]", args_["globObs"]->size());
    //std::cout << fmt::format("[FitManager] Extracted {} globObs to args[globObs]", args_["globObs"]->size()) << std::endl;
}
inline void FitManager::ExtractCats() noexcept
{
    assert(ws_ != nullptr);
    std::cout << "ERROR NOT IMPLEMENTED" << std::endl;
    //data_["np"] = (RooAbsData *) ws_->GetNp();
}

inline void FitManager::SetWsWrapper() noexcept
{
    //std::cout << "set ws wrapper" << std::endl;
    ws_ = new WorkspaceWrapper();
    //std::cout << "set ws wrapper DONE" << std::endl;
}

inline void FitManager::SetWS(std::string path, std::string name)
{
    EFT_PROF_INFO("[FitManager] set ws: {} from {}", name, path);
    //std::cout << fmt::format("[FitManager] set ws: {} from {}", name, path) << std::endl;
    if (ws_->SetWS(std::move(path), std::move(name)))
        EFT_PROF_INFO("[FitManager] successfully set ws: {} from {}", name, path);
        //std::cout << fmt::format("[FitManager] successfully set ws: {} from {}", name, path) << std::endl;
    else
        EFT_PROF_INFO("[FitManager] ERROR setting ws: {} from {}", name, path);
        //std::cout << fmt::format("[FitManager] ERROR setting ws: {} from {}", name, path) << std::endl;
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
    }
}
inline void FitManager::SetAllPOIsFloat() noexcept
{
    EFT_PROF_INFO("[FitManager] Set All POIs float");
    if (pois_.empty())
        ExtractPOIs();
    for (const auto& poi : pois_) {
        ws_->FloatVal(poi);
    }
}

} // eft::stats

#endif //EFTPROFILER_FITMANAGER_H
