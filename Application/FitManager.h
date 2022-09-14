//
// Created by Aleksei Lukianchuk on 14-Sep-22.
//

#ifndef EFTPROFILER_FITMANAGER_H
#define EFTPROFILER_FITMANAGER_H

#include "IFitManager.h"
#include "WorkspaceWrapper.h"


namespace eft::stats {

class FitManager : public IFitManager {
public:
    FitManager() = default;
    ~FitManager() override= default;

    inline void SetNpNames(std::string name) const noexcept override;
    inline void SetObsNames(std::string name) const noexcept override;
    inline void SetGlobObsNames(std::string name) const noexcept override;
    inline void SetCatsNames(std::string name) const noexcept override;

    inline void SetWsWrapper() noexcept override;

    inline void ExtractNP()      noexcept override;
    inline void ExtractObs()     noexcept override;
    inline void ExtractGlobObs() noexcept override;
    inline void ExtractCats()    noexcept override;

    //TODO: imlement: create asimov data
    inline void CreateAsimovData() noexcept override {
        std::cerr << "TO IMPLEMENT~" << std::endl;
    };

    [[nodiscard]]
    inline const DataClosure& GetDataClosure() const noexcept override {return data_;}
    [[nodiscard]]
    inline const FuncClosure& GetFuncClosure() const noexcept override {return funcs_;}

    [[nodiscard]]
    inline const RooAbsData* GetData(std::string&& name) const override {return data_.at(name);}
    [[nodiscard]]
    inline const RooAbsPdf*  GetPdf (std::string&& name) const override {return funcs_.at(name);}
private:
    DataClosure data_;
    FuncClosure funcs_;
    IWorkspaceWrapper* ws_;

    mutable std::string np_names;
    mutable std::string obs_names;
    mutable std::string glob_obs_names;
    mutable std::string cat_names;
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
    data_["np"] = (RooAbsData *) ws_->GetNp();
}
inline void FitManager::ExtractObs() noexcept
{
    data_["np"] = (RooAbsData *) ws_->GetObs();
}
inline void FitManager::ExtractGlobObs()     noexcept
{
    data_["np"] = (RooAbsData *) ws_->GetGlobObs();
}
inline void FitManager::ExtractCats() noexcept
{
    data_["np"] = (RooAbsData *) ws_->GetNp();
}

inline void FitManager::SetWsWrapper() const noexcept
{
    ws_ = new WorkspaceWrapper();
}

} // eft::stats

#endif //EFTPROFILER_FITMANAGER_H
