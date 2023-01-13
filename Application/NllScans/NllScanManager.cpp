//
// Created by Aleksei Lukianchuk on 12-Jan-23.
//

#include "NllScanManager.h"
#include "Core.h"

#include "IWorkspaceWrapper.h"
#include "../../Fitter/Fitter.h"


namespace eft::stats::scans {


//void NllScanManager::ComputeScanPointAtGrid() {
//    EFT_PROFILE_FN();
//
//    EFT_PROF_INFO("Compute scan point (value of each poi) to compute the Nll at");
//    IdentifyScanPointCoordinateAllPois();
//}

double NllScanManager::GetPointAtGrid(PoiConfig& config) const {
    EFT_PROFILE_FN();
    EFT_PROF_DEBUG("Get point at grid for a Poi Config");

    double low  = config.ScanRangeLow();
    double high = config.ScanRangeHigh();
    EFT_PROF_DEBUG("compute from range. low = {}, high = {}", low, high);

    return GetPointAtGridHermite(low, high, config.GridSize(), worker_id);
}

void NllScanManager::IdentifyScanPointCoordinateAllPois() noexcept {
    EFT_PROFILE_FN();
    for (auto& poi : pois_) {
        EFT_PROF_INFO("Dealing with: {}", poi.Name());
        if (gridType_ != GridType::USER_DEFINED) {
            poi.ToTestAt(GetPointAtGrid(poi));
        }
    }
}

void NllScanManager::SetPOIsToTheRequiredGridPosition() {
    EFT_PROFILE_FN();
    for (const auto& poi : pois_) {
        ws_->SetVarVal(poi.Name(), poi.Value());
    }
}

double NllScanManager::GetPointAtGrid(double low, double  high, size_t size_grid, size_t nb_point, GridType gridType) {
    EFT_PROFILE_FN();

    switch (gridType) {
        case GridType::EQUIDISTANT: return GetPointAtGridEquidistant(low, high, size_grid, nb_point);
        case GridType::HERMITE:     return GetPointAtGridHermite(low, high, size_grid, nb_point);
        case GridType::USER_DEFINED:
            throw std::runtime_error("cannot set up grid for a user-defined grid");
    }
}

double NllScanManager::GetPointAtGridEquidistant(double low, double high, size_t size_grid, size_t nb_point) {
    EFT_PROFILE_FN();
    const double length = high - low;
    const double step   = length / (size_grid - 1);
    double res = low + (nb_point - 1) * step;

    EFT_PROF_DEBUG("low: {}, high: {}, np pts: {}, number to get: {}",
                    low, high, size_grid, nb_point);

    EFT_PROF_DEBUG("length: {}", length);
    EFT_PROF_DEBUG("step: {}", step);
    EFT_PROF_DEBUG("res: {}", res);

    EFT_PROF_INFO("Get point # {} using equidistant grid in range: [{}, {}] over {} points ==> {}",
                  nb_point,
                  low,
                  high,
                  size_grid,
                  res);
    return res;
}

void NllScanManager::RunScan() {
    EFT_PROFILE_FN();

    EFT_PROF_INFO("Fix all POIs (later on, needed ones will be float)");
    ws_->FixValConst(all_pois);

    EFT_PROF_INFO("Float required POIs (which are allowed to by a research)");
    ws_->FloatVals(pois_to_float);

    EFT_PROF_INFO("Set up the grid by forcing all POIs to bet at the required values");
    SetPOIsToTheRequiredGridPosition();

    //if ()
}

bool NllScanManager::ParseConfig(std::string path, std::string format) {
    EFT_PROFILE_FN();
}

double NllScanManager::GetPointAtGridHermite(double low, double high, size_t size_grid, size_t nb_point) {
    EFT_PROFILE_FN();

    throw std::runtime_error("GetPointAtGridHermite is not implemented");

    return 0;
}

} // eft::stats::scans