//
// Created by Aleksei Lukianchuk on 12-Sep-22.
//

#ifndef EFTPROFILER_IFITMANAGER_H
#define EFTPROFILER_IFITMANAGER_H

#include "IWorksSpaceWrapper.h"
#include <memory>
#include <unordered_map>

//#include "TFitResult.h"
//#include "RooFitResult.h"
#include "RooAbsData.h"
#include "RooAbsPdf.h"
#include "RooAbsReal.h"


namespace eft {
namespace stats {


// Interface to the fit engine
// apart, allows extracting variables
// to the global closure
class IFitManager {
public:
    using FitResPtr = std::unique_ptr<RooFitResult>;

    using DataClosure = std::unordered_map<std::string, RooAbsData*> ;
    using FuncClosure = std::unordered_map<std::string, RooAbsPdf*>  ;

    IFitManager() = default;
    virtual ~IFitManager() = default;

    virtual void ExtractNP()      noexcept = 0;
    virtual void ExtractObs()     noexcept = 0;
    virtual void ExtractGlobObs() noexcept = 0;
    virtual void ExtractCats()    noexcept = 0;

    virtual RooAbsReal* CreatNll(RooAbsData* data, RooAbsPdf* pdf)      = 0;
    virtual RooAbsReal* CreatNll(std::string&& data, std::string&& pdf) = 0;
    virtual FitResPtr   Minimize(RooAbsReal* nll)                       = 0;
    virtual FitResPtr   Fit(RooAbsData* data, RooAbsPdf* pdf)           = 0;
    virtual FitResPtr   Fit(std::string&& data, std::string&& pdf)      = 0;

    virtual void CreateAsimovData() noexcept = 0;

    virtual const DataClosure& GetDataClosure() const noexcept = 0;
    virtual const FuncClosure& GetFuncClosure() const noexcept = 0;

    virtual const RooAbsData* GetData(std::string&& name) const = 0;
    virtual const RooAbsPdf*  GetPdf (std::string&& name) const = 0;


//private:
    //std::unique_ptr<IWorksSpaceWrapper> ws_;
};

} // eft
} // stats

#endif //EFTPROFILER_IFITMANAGER_H
