//
// Created by Aleksei Lukianchuk on 12-Jan-23.
//

#ifndef EFTPROFILER_NLLSCANMANAGER_H
#define EFTPROFILER_NLLSCANMANAGER_H

#include "Core.h"

#include "NllScanResult.h"
#include "PoiConfig.h"
#include "Grid.h"

#include "../Fitter/FitSettings.h"


class IWorkspaceWrapper;
class IWorkspaceWrapper;
class RooArgSet;
class RooAbsPdf;
class RooAbsData;
class CommandLineArgs;

//#include "RooRealVar.h"

namespace eft::stats::scans {


class NllScanManager {
public:
    void RunScan();
    static NllScanManager InitFromCommandLine(const std::shared_ptr<CommandLineArgs>& cmdLineArgs);

    bool ParseConfig(std::string path, std::string format);

    inline NllScanManager& SetWorkerId(size_t id)           noexcept { worker_id = id; return *this; }
    inline NllScanManager& SetPOIsToFloat(RooArgSet* pois)  noexcept { pois_to_float = pois; return *this; }
    inline NllScanManager& SetPOIsFullList(RooArgSet* pois) noexcept { all_pois = pois; return *this; }
    inline NllScanManager& SetPOIsToFloat(const std::vector<std::string>& list);
    inline NllScanManager& SetPOIsFullList(const std::vector<std::string>& list);
    inline NllScanManager& SetWS(IWorkspaceWrapper* ws)     noexcept { ws_ = ws; return *this; }
    inline NllScanManager& SetGlobs(RooArgSet* globs)       noexcept { fitSettings_.globalObs = globs; return *this; }
    inline NllScanManager& SetNPs(RooArgSet* nps)           noexcept { fitSettings_.nps = nps; return *this; }
    inline NllScanManager& SetGridType(GridType grid)       noexcept { gridType_ = grid; return *this; }

    inline PoiConfig& AddPoi(std::string name) { pois_.emplace_back(std::move(name)); return pois_.back();}
    inline PoiConfig& AddPoi(PoiConfig poi) { pois_.emplace_back(std::move(poi)); return pois_.back();}

    [[nodiscard]] const inline NllScanResult& GetRes() const noexcept { return res_; }

    void SaveRes(const std::string& path) const;
    void SaveRes(std::ostream& os) const;

    //void inline SetGridSize1D(size_t sz) noexcept { size_grid_1d = sz; }

    void SetPOIsToTheRequiredGridPosition();
    void FixGridPOIs();
private:
    void IdentifyScanPointCoordinateAllPois()  noexcept;

    double GetPointAtGrid(PoiConfig& config) const;
    static double GetPointAtGrid(double low, double  high, size_t size_grid, size_t nb_point, GridType gridType);

    static double GetPointAtGridEquidistant(double low, double  high, size_t size_grid, size_t nb_point);
    static double GetPointAtGridHermite(double low, double  high, size_t size_grid, size_t nb_point);

    //void PreparePOIs();
private:
    std::vector<PoiConfig>  pois_;
    NllScanResult           res_;
    size_t                  worker_id   {0};
    //size_t                  size_grid_1d {0};
    GridType                gridType_    {GridType::EQUIDISTANT};
    fit::FitSettings        fitSettings_;
    IWorkspaceWrapper* ws_   {nullptr};
    RooArgSet*              pois_to_float {nullptr};
    RooArgSet*              all_pois {nullptr};
};

inline NllScanManager& NllScanManager::SetPOIsToFloat(const std::vector<std::string>& list) {
    if (ws_ == nullptr) {
        EFT_PROF_CRITICAL("Seet WS before calling to SetPOIsToFloat[strings]");
        throw std::runtime_error("");
    }
    pois_to_float = new RooArgSet();
    for (const auto& poi : list) {
        pois_to_float->add(*ws_->GetVar(poi));
    }
}
inline NllScanManager& NllScanManager::SetPOIsFullList(const std::vector<std::string>& list) {
    if (ws_ == nullptr) {
        EFT_PROF_CRITICAL("Seet WS before calling to SetPOIsFullList[strings]");
        throw std::runtime_error("");
    }
    all_pois = new RooArgSet();
    for (const auto& poi : list) {
        all_pois->add(*ws_->GetVar(poi));
    }
}


} // eft::stats::scans

#endif //EFTPROFILER_NLLSCANMANAGER_H
