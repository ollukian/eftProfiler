//
// Created by Aleksei Lukianchuk on 12-Sep-22.
//

#ifndef EFTPROFILER_IFITMANAGER_H
#define EFTPROFILER_IFITMANAGER_H

#include "IWorkspaceWrapper.h"
#include <memory>
#include <unordered_map>

//#include "TFitResult.h"
//#include "RooFitResult.h"
#include "RooAbsData.h"
#include "RooAbsPdf.h"
#include "RooAbsReal.h"

#include "IWorkspaceWrapper.h"
#include "NpRankingStudyRes.h"
//#include "FitManagerConfig.h"
//#include "NpRankingStudyRes.h"

namespace eft::stats {

struct NpRankingStudySettings;
struct HesseStudyResult;


// Interface to the fit engine
// apart, allows extracting variables
// to the global closure
class IFitManager {
public:
    using DataClosure = std::unordered_map<std::string, RooAbsData*> ;
    using ArgsClosure = std::unordered_map<std::string, RooArgSet*> ;
    using FuncClosure = std::unordered_map<std::string, RooAbsPdf*>  ;

    // global tasks
    virtual void DoGlobalFit() = 0;
    virtual void ComputeNpRankingOneWorker(const NpRankingStudySettings& settings, size_t workerId) = 0;
    virtual void DoFitAllNpFloat(const NpRankingStudySettings& settings) = 0;
    //virtual HesseStudyResult ComputeHesseNps(const NpRankingStudySettings& settings) = 0;

    //IFitManager() = default;
    virtual ~IFitManager() noexcept = default;

    virtual void SetWsWrapper() noexcept = 0;

    virtual void SetNpNames(std::string name) const noexcept = 0;
    virtual void SetObsNames(std::string name) const noexcept = 0;
    virtual void SetGlobObsNames(std::string name) const noexcept = 0;
    virtual void SetCatsNames(std::string name) const noexcept = 0;

    virtual void SetWS(std::string path, std::string name)  = 0;
    virtual void SetModelConfig(std::string name)  = 0;

    virtual void ExtractNP()      noexcept = 0;
    virtual void ExtractObs()     noexcept = 0;
    virtual void ExtractGlobObs() noexcept = 0;
    virtual void ExtractPOIs()    noexcept = 0;
    virtual void ExtractCats()    noexcept = 0;

    virtual void ExtractDataTotal(std::string name) = 0;
    virtual void ExtractPdfTotal(std::string name)  = 0;

    virtual RooAbsData& GetData(PrePostFit studyType) = 0;
    //virtual RooAbsData* GetDataPtr(PrePostFit studyType) = 0;
    virtual void        SetUpGlobObs(PrePostFit studyType) = 0;

    virtual void CreateAsimovData(PrePostFit studyType) noexcept = 0;

    virtual const DataClosure& GetDataClosure() const noexcept = 0;
    virtual const ArgsClosure& GetArgsClosure() const noexcept = 0;
    virtual const FuncClosure& GetFuncClosure() const noexcept = 0;

    virtual const RooAbsData* GetData(std::string name) const = 0;
    virtual RooAbsPdf * GetPdf (std::string name) const = 0;

    virtual const std::vector<std::string>& GetListPOIs() const noexcept= 0;

    virtual IWorkspaceWrapper* ws() = 0;

    virtual void SetAllNuisanceParamsConst() noexcept = 0;
    virtual void SetAllNuisanceParamsFloat() noexcept = 0;
    virtual void SetAllNuisanceParamsToValue(float val) noexcept = 0;
    virtual void SetGlobalObservablesToValueFoundInFit() noexcept = 0;

    virtual void SetAllPOIsConst() noexcept = 0;
    virtual void SetAllPOIsFloat() noexcept = 0;

    virtual void SetAllGlobObsConst() noexcept = 0;
    virtual void SetAllGlobObsFloat() noexcept = 0;
    virtual void SetAllGlobObsTo(float val) noexcept = 0;


//private:
    //std::unique_ptr<IWorkspaceWrapper> ws_;
};

} // stats

#endif //EFTPROFILER_IFITMANAGER_H
