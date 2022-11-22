//
// Created by Aleksei Lukianchuk on 12-Sep-22.
// alekseilukianchuk@gmail.com
//

#ifndef EFTPROFILER_WORKSPACEWRAPPER_H
#define EFTPROFILER_WORKSPACEWRAPPER_H

#include "IWorkspaceWrapper.h"
#include "../Core/Logger.h"

#include <string>
#include <vector>
#include <memory>

#include <spdlog/fmt/fmt.h>

#include <filesystem>

#include "TFile.h"
#include "RooStats/ModelConfig.h"
#include "RooWorkspace.h"
#include "RooAbsArg.h"
#include "RooRealVar.h"
#include "RooAbsPdf.h"
#include "RooDataSet.h"
#include "RooSimultaneous.h"



namespace eft::stats {

// Wraps the workspace, to simplify access to it
class WorkspaceWrapper : public IWorkspaceWrapper {
public:
    ~WorkspaceWrapper() override = default;
    WorkspaceWrapper() = default;

    inline bool SetWS(std::string path, std::string name) override;
    inline RooStats::ModelConfig* SetModelConfig(std::string name) override;
    inline RooStats::ModelConfig& GetModelConfig() override { return *modelConfig_;}

    inline RooWorkspace* raw() const noexcept override { return ws_.get();}


    inline void FixValConst(const std::string& poi) override;
    inline void FixValConst(const std::vector<std::string>& pois) override;
    //inline void FixValConst(std::initializer_list<std::vector<std::string>> pois) override;

    //inline void FixAllPois()   noexcept override;
    //inline void FloatAllPois() noexcept override;

    inline void FloatVal(const std::string& poi) override;
    inline void FloatVals(const std::vector<std::string>& pois) override;
    //inline void FloatVals(std::initializer_list<std::vector<std::string>> pois) override;

    inline void SetVarVal(const std::string& name, double val) override;
    inline void SetVarErr(const std::string& name, double err) override;

    inline void VaryParNbSigmas(const std::string& par, float nb_sigma) noexcept override;

    inline RooAbsPdf* GetPdfModelGivenCategory(const std::string& cat) noexcept override;
    inline RooAbsPdf* GetPdfSBGivenCategory(const std::string& cat)    noexcept override;
    inline RooAbsPdf* GetPdfBkgGivenCategory(const std::string& cat)   noexcept override;
    inline RooAbsPdf* GetPdfSigGivenCategory(const std::string& cat)   noexcept override;

    inline double GetParVal(const std::string& par)   const override;
    inline double GetParErr(const std::string& par)   const override;
    inline double GetParErrHi(const std::string& par) const override;
    inline double GetParErrLo(const std::string& par) const override;

    inline const RooArgSet* GetNp() const override;
    inline const RooArgSet* GetObs() const override;
    inline const RooArgSet* GetGlobObs() const override;
    inline const RooArgSet* GetPOIs() const override;
    inline const Categories& GetCats() const override;
    inline RooRealVar* GetVar(const std::string& name) override;

    inline RooDataSet*      GetData(const std::string& name) override;
    inline RooSimultaneous* GetCombinedPdf(const std::string& name) override;

    void SaveNPsSnapshot(const std::string& name) noexcept override;
    void SaveGlobsSnapshot(const std::string& name) noexcept override;
    void SaveNpsAndGlobsSnapshot(const std::string& name) noexcept override;

    void LoadNPsSnapshot(const std::string& name) noexcept override;
    void LoadGlobsSnapshot(const std::string& name) noexcept override;
    void LoadNpsAndGlobsSnapshot(const std::string& name) noexcept override;

#if 0
    inline RooDataSet* GetDataSetGivenCategory(const std::string& cat) override;
    inline RooRealVar* GetVar(const std::string& name) override;
    inline RooRealVar* GetObservable(const std::string& name) override;

    inline RooRealVar* GetNbkgGivenCategory(const std::string& name) const noexcept override;

    inline double GetParameterValue(const std::string& par)   const noexcept override;
    inline double GetParameterError(const std::string& par)   const noexcept override;
    inline double GetParameterErrorHi(const std::string& par) const noexcept override;
    inline double GetParameterErrorLo(const std::string& par) const noexcept override;

    inline void SetAllNuisanceParamsZero()  noexcept override;
    inline void SetAllNuisanceParamsConst()  noexcept override;
    inline void SetAllNuisanceParamsFloat()  noexcept override;
    inline void SetAllNuisanceParamsToValue(float val)  noexcept override;

    inline RooArgSet* ExtractNuisanseParameters() const noexcept override;
    inline RooArgSet* ExtractObservables()        const noexcept override;
    inline RooArgSet* ExtractGlobalObservables()  const noexcept override;
    inline RooArgSet* ExtractPOIs()               const noexcept override;

    bool SaveSnapshot(std::string name,
                             const RooArgSet& globalAbs,
                             bool importValues) override;
    bool LoadSnapshot(std::string name)  override;
#endif // if 0 to screen
private:
    std::unique_ptr<RooWorkspace> ws_{};
    std::unique_ptr<RooStats::ModelConfig> modelConfig_{};
    std::unique_ptr<RooCategory> channelList_{};
    mutable Categories categories_{};

    mutable std::string pdf_model_sufix_{};
    mutable std::string pdf_model_prefix_{};
    mutable std::string pdf_sb_sufix_{};
    mutable std::string pdf_sb_prefix_{};
    mutable std::string pdf_bkg_sufix_{};
    mutable std::string pdf_bkg_prefix_{};
    mutable std::string pdf_sig_sufix_{};
    mutable std::string pdf_sig_prefix_{};


    //inline void FixValConst(const std::vector<std::string>& pois) { for (const std::string& poi : pois) { ws_->var( poi.c_str() )->setConstant(true) override; } } override;
};

inline RooRealVar* WorkspaceWrapper::GetVar(const std::string& name)
{
    return ws_->var(name.c_str());
}
inline bool WorkspaceWrapper::SetWS(std::string path, std::string name)
{
    if (! std::filesystem::exists(path) ) {
        EFT_PROF_CRITICAL("Ws under {} doesn't exist", path);
        return false;
    }

    TFile* f_ = TFile::Open(std::move(path).c_str());
    if (f_) {
        ws_ = std::make_unique<RooWorkspace>(
                *dynamic_cast<RooWorkspace*>( f_->Get( std::move(name).c_str() ) )
                );
        return true;
    }
    return false;
}

inline void WorkspaceWrapper::FixValConst(const std::string& poi)
{
    EFT_PROF_DEBUG("Set {:30} const", poi);
    if (ws_->var(poi.c_str()) == nullptr) {
        EFT_PROF_CRITICAL("WorkspaceWrapper::FixValConst variable {} is not present in the WS", poi);
        return;
    }
    ws_->var( poi.c_str() )->setConstant(true);
}

inline void WorkspaceWrapper::FixValConst(const std::vector<std::string>& pois)
{
    for (const auto& poi : pois) {
        FixValConst(poi);
    }
}

inline void WorkspaceWrapper::FloatVal(const std::string& poi)
{
    EFT_PROF_DEBUG("Set {:30} float", poi);
    if (ws_->var(poi.c_str()) == nullptr) {
        EFT_PROF_CRITICAL("WorkspaceWrapper::FloatVal variable {} is not present in the WS", poi);
        return;
    }
    ws_->var( poi.c_str() )->setConstant(false);
}

inline void WorkspaceWrapper::FloatVals(const std::vector<std::string>& pois)
{
    for (const auto& poi : pois) {
        FloatVal(poi);
    }
}

inline void WorkspaceWrapper::SetVarVal(const std::string& name, double val)
{
    EFT_PROF_TRACE("[WorkspaceWrapper] Set value of {:30} to {}", name, val);
    ws_->var( name.c_str() )->setVal(val);
}
inline void WorkspaceWrapper::SetVarErr(const std::string& name, double err)
{
    EFT_PROF_TRACE("[WorkspaceWrapper] Set error of {:30} to {}", name, err);
    ws_->var( name.c_str() )->setError(err);
}

inline RooAbsPdf* WorkspaceWrapper::GetPdfModelGivenCategory(const std::string& cat) noexcept
{
    return ws_->pdf(fmt::format("{}{}{}", cat, pdf_model_prefix_, pdf_model_sufix_).c_str());
    //return ws_->pdf(("_model_" + cat + "_HGam__addConstr").c_str()); };
}
inline RooAbsPdf* WorkspaceWrapper::GetPdfSBGivenCategory(const std::string& cat) noexcept
{
    return ws_->pdf(fmt::format("{}{}{}", cat, pdf_sb_prefix_, pdf_sb_sufix_).c_str());
}
inline RooAbsPdf* WorkspaceWrapper::GetPdfBkgGivenCategory(const std::string& cat) noexcept
{
    return ws_->pdf(fmt::format("{}{}{}", cat, pdf_bkg_prefix_, pdf_bkg_sufix_).c_str());
}
inline RooAbsPdf* WorkspaceWrapper::GetPdfSigGivenCategory(const std::string& cat) noexcept
{
    return ws_->pdf(fmt::format("{}{}{}", cat, pdf_sig_prefix_, pdf_sig_sufix_).c_str());
}

inline RooStats::ModelConfig* WorkspaceWrapper::SetModelConfig(std::string name)
{
    auto _mc = ws_->obj( name.c_str() );
    if (! _mc) {
        EFT_PROF_CRITICAL("Model Config with name {} is not present in the WS", name);
        throw std::logic_error("Wrong Model Config name");
    }
    modelConfig_.reset( dynamic_cast<RooStats::ModelConfig*> (
             _mc
            ))
            ;
    return modelConfig_.get();
}

inline const RooArgSet* WorkspaceWrapper::GetNp() const
{
    return modelConfig_->GetNuisanceParameters();
}
inline const RooArgSet* WorkspaceWrapper::GetObs() const
{
    return modelConfig_->GetObservables();
}
inline const RooArgSet* WorkspaceWrapper::GetGlobObs() const
{
    return modelConfig_->GetGlobalObservables();
}
inline const RooArgSet* WorkspaceWrapper::GetPOIs() const
{
    return modelConfig_->GetParametersOfInterest();
}

inline const WorkspaceWrapper::Categories& WorkspaceWrapper::GetCats() const
{
    if (categories_.empty())
    {
        Categories cats;
        cats.reserve(channelList_->size());
        for (const auto& cat : *channelList_) {
            cats.push_back( cat.first );
        }
    }
    return categories_;
}

inline RooDataSet*      WorkspaceWrapper::GetData(const std::string& name)
{
    if (ws_->data(  name.c_str() ) == nullptr)
    {
        EFT_PROF_CRITICAL("WorkspaceWrapper::GetData {} no such data is present", name);
        return nullptr;
    }
    return dynamic_cast<RooDataSet*> ( ws_->data( name.c_str() ) );
}
inline RooSimultaneous* WorkspaceWrapper::GetCombinedPdf(const std::string& name)
{
    if (ws_->pdf(  name.c_str() ) == nullptr)
    {
        EFT_PROF_CRITICAL("WorkspaceWrapper::GetCombinedPdf {} no such pdf is present", name);
        return nullptr;
    }
    return dynamic_cast<RooSimultaneous*> ( ws_->pdf(  name.c_str() ) );
}

inline double WorkspaceWrapper::GetParVal(const std::string& par)   const  { return ws_->var( par.c_str() )->getVal(); };
inline double WorkspaceWrapper::GetParErr(const std::string& par)   const  { return ws_->var( par.c_str() )->getError(); };
inline double WorkspaceWrapper::GetParErrHi(const std::string& par) const  { return ws_->var( par.c_str() )->getAsymErrorHi();}
inline double WorkspaceWrapper::GetParErrLo(const std::string& par) const  { return ws_->var( par.c_str() )->getAsymErrorLo();}


inline void WorkspaceWrapper::VaryParNbSigmas(const std::string& par, float nb_sigma) noexcept
{
    //EFT_PROF_TRACE("WorkspaceWrapper::VaryParNbSigmas vary {} on {} sigmas", par, nb_sigma);
    const auto val = GetParVal(par);
    const auto err = GetParErr(par);
    EFT_PROF_INFO("WorkspaceWrapper::VaryParNbSigmas set {} ({} +- {}) to {}",
        par, GetParVal(par), GetParErr(par), GetParVal(par) + err * nb_sigma );
    SetVarVal(par, val + err * nb_sigma);
}

template<typename OStream>
OStream& operator<<(OStream& os, const RooRealVar& var) {
    std::string constness = "const";
    if (! var.isConstant() )
        constness = "float";

    EFT_PROF_CRITICAL("operator<<(OStream& os, const RooRealVar& var): name: {}", var.GetName());
    EFT_PROF_CRITICAL("operator<<(OStream& os, const RooRealVar& var): val: {}", var.getVal());
    EFT_PROF_CRITICAL("operator<<(OStream& os, const RooRealVar& var): err: {}", var.getError());
    EFT_PROF_CRITICAL("operator<<(OStream& os, const RooRealVar& var): const: {}", constness);
    return os << fmt::format("{:30} [{:.4} +- {:.4} ({})]",
                             var.GetName(),
                             var.getVal(),
                             var.getError(),
                             std::move(constness));
}

//inline void WorkspaceWrapper::FixValConst(std::initializer_list<std::vector<std::string>> pois)
//{
//    const std::vector<std::string> pois_{pois};
//    for (const auto& poi : pois) {
//        FixValConst(poi);
//    }
//}

} // stats

#endif //EFTPROFILER_WORKSPACEWRAPPER_H
