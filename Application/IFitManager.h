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


namespace eft::stats {


// Interface to the fit engine
// apart, allows extracting variables
// to the global closure
class IFitManager {
public:

    using DataClosure = std::unordered_map<std::string, RooAbsData*> ;
    using FuncClosure = std::unordered_map<std::string, RooAbsPdf*>  ;

    IFitManager() = default;
    virtual ~IFitManager() = default;

    virtual void SetWsWrapper() const noexcept = 0;

    virtual void SetNpNames(std::string name) const noexcept = 0;
    virtual void SetObsNames(std::string name) const noexcept = 0;
    virtual void SetGlobObsNames(std::string name) const noexcept = 0;
    virtual void SetCatsNames(std::string name) const noexcept = 0;

    virtual void ExtractNP()      noexcept = 0;
    virtual void ExtractObs()     noexcept = 0;
    virtual void ExtractGlobObs() noexcept = 0;
    virtual void ExtractCats()    noexcept = 0;

    virtual void CreateAsimovData() noexcept = 0;

    virtual const DataClosure& GetDataClosure() const noexcept = 0;
    virtual const FuncClosure& GetFuncClosure() const noexcept = 0;

    virtual const RooAbsData* GetData(std::string&& name) const = 0;
    virtual const RooAbsPdf*  GetPdf (std::string&& name) const = 0;


//private:
    //std::unique_ptr<IWorkspaceWrapper> ws_;
};

} // stats

#endif //EFTPROFILER_IFITMANAGER_H
