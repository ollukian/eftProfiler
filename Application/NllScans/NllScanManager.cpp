//
// Created by Aleksei Lukianchuk on 12-Jan-23.
//

#include "NllScanManager.h"
#include "Core.h"

#include "IWorkspaceWrapper.h"
#include "../../Fitter/Fitter.h"


namespace eft::stats::scans {


void NllScanManager::ComputeScanPointAtGrid() {
    EFT_PROFILE_FN();

    EFT_PROF_INFO("Compute scan point (value of each poi) to compute the Nll at");
    IdentifyScanPointCoordinateAllPois();
}

double NllScanManager::GetPointAtGrid(PoiConfig& config) const {
    EFT_PROFILE_FN();
    EFT_PROF_DEBUG("Get point at grid for a Poi Config");

    bool use_sigmas = false;
    double low;
    double high;

    if (config.range_scan_low == 0 && config.range_scan_high == 0)
        use_sigmas = true;

    if (use_sigmas) {
        double val = config.central_val;
        double err = config.central_err;
        double sig = config.range_scan_sigmas;

        low  = val - sig * err;
        high = val + sig * err;
        EFT_PROF_DEBUG("compute from sigmas. nb sigma: {} => low = {}, high = {}", sig, low, high);
    }
    else {
        low  = config.range_scan_low;
        high = config.range_scan_high;
        EFT_PROF_DEBUG("compute from range. low = {}, high = {}", low, high);
    }
    return GetPointAtGridHermite(low, high, config.grid_size, worker_id);
}

void NllScanManager::IdentifyScanPointCoordinateAllPois() noexcept {
    EFT_PROFILE_FN();
    for (auto& poi : pois_) {
        EFT_PROF_INFO("Dealing with: {}", poi.name);
        if (gridType_ != GridType::USER_DEFINED) {
            poi.val = GetPointAtGrid(poi);
        }
    }
}

void NllScanManager::SetPOIsToTheRequiredGridPosition() {
    EFT_PROFILE_FN();
    for (const auto& poi : pois_) {
        ws_->SetVarVal(poi.name, poi.val);
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


}

} // eft::stats::scans