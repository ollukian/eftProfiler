//
// Created by Aleksei Lukianchuk on 12-Sep-22.
// AlekseiLukianchuk@gmail.com
//

#ifndef EFTPROFILER_IWORKSPACEWRAPPER_H
#define EFTPROFILER_IWORKSPACEWRAPPER_H

#include <vector>
#include <string>

#include <RooAbsData.h>
#include <RooAbsPdf.h>
#include <RooStats/ModelConfig.h>
#include <RooWorkspace.h>
#include "RooCategory.h"

#include <filesystem>

class IWorkspaceWrapper {
public:
    using Categories = std::vector<std::string>;

    virtual ~IWorkspaceWrapper() noexcept = default;
    IWorkspaceWrapper() = default;

    IWorkspaceWrapper(const IWorkspaceWrapper&) = delete;
    IWorkspaceWrapper(IWorkspaceWrapper&&)      = delete;
    IWorkspaceWrapper& operator = (const IWorkspaceWrapper&) = delete;
    IWorkspaceWrapper& operator = (IWorkspaceWrapper&&) = delete;

    virtual RooWorkspace* raw() const noexcept = 0;

    // name & path to be moved
    virtual bool SetWS(std::string path, std::string name) = 0;
    virtual RooStats::ModelConfig* SetModelConfig(std::string name)          = 0;

    virtual void FixPoi(const std::string& poi)  = 0;
    virtual void FixPois(const std::vector<std::string>& pois) = 0;
    //virtual void FixPois(std::initializer_list<std::vector<std::string>> pois) = 0;

    //virtual void FixAllPois()   noexcept = 0;
    //virtual void FloatAllPois() noexcept = 0;

    virtual void FloatPoi(const std::string& poi)  = 0;
    virtual void FloatPois(const std::vector<std::string>& pois) = 0;
    //virtual void FloatPois(std::initializer_list<std::vector<std::string>> pois) = 0;

    virtual void SetVarVal(const std::string& name, double val) = 0;
    virtual void SetVarErr(const std::string& name, double err) = 0;

    virtual RooAbsPdf* GetPdfModelGivenCategory(const std::string& cat)  = 0;
    virtual RooAbsPdf* GetPdfSBGivenCategory(const std::string& cat)     = 0;
    virtual RooAbsPdf* GetPdfBkgGivenCategory(const std::string& cat)    = 0;
    virtual RooAbsPdf* GetPdfSigGivenCategory(const std::string& cat)    = 0;
    //virtual const Categories& GetCategories() const = 0;

    virtual const RooArgSet* GetNp() const = 0;
    virtual const RooArgSet* GetObs() const = 0;
    virtual const RooArgSet* GetGlobObs() const = 0;
    virtual const RooArgSet* GetPOIs() const = 0;
    virtual const Categories& GetCats() const = 0;

#if 0
    virtual RooDataSet* GetDataSetGivenCategory(const std::string& cat) = 0;
    virtual RooRealVar* GetVar(const std::string& name) = 0;
    virtual RooRealVar* GetObservable(const std::string& name) = 0;

    virtual RooRealVar* GetNbkgGivenCategory(const std::string& name) const noexcept = 0;

    virtual bool SaveSnapshot(std::string name, const RooArgSet& globalAbs, bool importValues) = 0;
    virtual bool LoadSnapshot(std::string name)                                                = 0;


    virtual double GetParameterValue(const std::string& par)   const noexcept = 0;
    virtual double GetParameterError(const std::string& par)   const noexcept = 0;
    virtual double GetParameterErrorHi(const std::string& par) const noexcept = 0;
    virtual double GetParameterErrorLo(const std::string& par) const noexcept = 0;

    virtual void SetAllNuisanceParamsZero()  noexcept = 0;
    virtual void SetAllNuisanceParamsConst()  noexcept = 0;
    virtual void SetAllNuisanceParamsFloat()  noexcept = 0;
    virtual void SetAllNuisanceParamsToValue(float val)  noexcept = 0;

    virtual RooArgSet* ExtractNuisanseParameters() const noexcept = 0;
    virtual RooArgSet* ExtractObservables()        const noexcept = 0;
    virtual RooArgSet* ExtractGlobalObservables()  const noexcept = 0;
    virtual RooArgSet* ExtractPOIs()               const noexcept = 0;
#endif

//    virtual void AddWSbyAddress(const std::string& path, const std::string& name);
//    virtual void AddModelConfig(const std::string& name);
//    virtual void SetUpData(const std::string& name)        { dataSetTotal_ = dynamic_cast<RooDataSet*>      ( ws_->data( name.c_str() ) ); };
//    virtual void SetUpCombinedPdf(const std::string& name) { combinedPdf_  = dynamic_cast<RooSimultaneous*> ( ws_->pdf(  name.c_str() ) ); };
//    virtual void SetUpCats(const std::vector<std::string>& cats);
//    virtual void SetUpCats();
//    virtual void SetUpPoisToFit(std::vector<std::string>& pois); // to add
//    virtual void SetUpPoisToFit(stats::POIs pois);
//    virtual void UseBinnedData(bool b);

};


#endif //EFTPROFILER_IWORKSPACEWRAPPER_H
