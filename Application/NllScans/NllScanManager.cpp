//
// Created by Aleksei Lukianchuk on 12-Jan-23.
//
// @ lukianchuk.aleksei@gmail.com


#include "NllScanManager.h"
#include "Core.h"

#include "IWorkspaceWrapper.h"
#include "../../Fitter/Fitter.h"

#include "CommandLineArgs.h"

#include "FitManager.h"
#include "FitManagerConfig.h"


namespace eft::stats::scans {

using namespace std;


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

    switch (gridType_) {
        case GridType::EQUIDISTANT:
            return GetPointAtGridEquidistant(low, high, config.GridSize(), worker_id);;
        case GridType::HERMITE:
            return GetPointAtGridHermite(low, high, config.GridSize(), worker_id);;
        case GridType::USER_DEFINED: // never used
            return 0;
    }
    return 0; // never reached
}

void NllScanManager::IdentifyScanPointCoordinateAllPois() noexcept {
    EFT_PROFILE_FN();
    EFT_PROF_INFO("NllScanManager::IdentifyScanPointCoordinateAllPois for {} pois", pois_.size());
    for (auto& poi : pois_) {
        EFT_PROF_INFO("Dealing with: {}...", poi.Name());
        if (gridType_ != GridType::USER_DEFINED) {
            poi.ToTestAt(GetPointAtGrid(poi));
        }
        EFT_PROF_INFO("Poi: {} is to be tested at: {}", poi.Name(), poi.Value());
    }
}

void NllScanManager::SetPOIsToTheRequiredGridPosition() {
    EFT_PROFILE_FN();
    for (auto& poi : pois_) {
        if (not poi.IsGridPointKnown()) {
            EFT_PROF_WARN("NllScanManager::SetPOIsToTheRequiredGridPosition grid point of {} is not known", poi.Name());
        }
        ws_->SetVarVal(poi.Name(), poi.Value());
    }
}

void NllScanManager::FixGridPOIs() {
    EFT_PROFILE_FN();
    for (const auto& poi : pois_) {
        ws_->FixValConst(poi.Name());
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
    return 0;
}

double NllScanManager::GetPointAtGridEquidistant(double low, double high, size_t size_grid, size_t nb_point) {
    EFT_PROFILE_FN();
    const double length = high - low;
    const double step   = length / (size_grid - 1);
    double res = low + (nb_point) * step;

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

    if (all_pois != nullptr) {
        EFT_PROF_INFO("Fix all POIs (later on, needed ones will be float)");
        ws_->FixValConst(all_pois);
    }

    EFT_PROF_INFO("Float required POIs (which are allowed to by a research)");
    ws_->FloatVals(pois_to_float);

    EFT_PROF_INFO("Identify grid points to put the pois to");
    IdentifyScanPointCoordinateAllPois();

    EFT_PROF_INFO("Set up the grid by forcing all POIs to bet at the required values");
    SetPOIsToTheRequiredGridPosition();

    EFT_PROF_INFO("Fix GRID POIs to be const");
    FixGridPOIs();

    EFT_PROF_INFO("Create Nll");
    fit::Fitter fitter;
    if (fitSettings_.nps == nullptr) {
        EFT_PROF_CRITICAL("nps are nullptr before create nll");
    }
    if (fitSettings_.globalObs == nullptr) {
        EFT_PROF_CRITICAL("globalObs are nullptr before create nll");
    }
    if (fitSettings_.pdf == nullptr) {
        EFT_PROF_CRITICAL("pdf are nullptr before create nll");
    }
    auto nll = fitter.CreatNll(fitSettings_);

    EFT_PROF_INFO("Minimise Nll");
    fitter.Minimize(fitSettings_, nll);

    auto found_nll = nll->getVal();
    EFT_PROF_INFO("nll value: {}", found_nll);

    res_ = NllScanResult{};
    res_.nll_val = found_nll;
    res_.poi_configs = pois_;
    res_.fit_status = fitter.GetLastFitStatus();

    //if ()
}

bool NllScanManager::ParseConfig(std::string path, std::string format) {
    EFT_PROFILE_FN();
    return true;
}

double NllScanManager::GetPointAtGridHermite(double low, double high, size_t size_grid, size_t nb_point) {
    EFT_PROFILE_FN();

    throw std::runtime_error("GetPointAtGridHermite is not implemented");

    return 0;
}

NllScanManager NllScanManager::InitFromCommandLine(const std::shared_ptr<CommandLineArgs>& cmdLineArgs) {
    EFT_PROFILE_FN();

    eft::stats::FitManagerConfig config;
    //auto manager = make_unique<eft::stats::FitManager>();
    auto manager = new FitManager{};
    eft::stats::FitManager::ReadConfigFromCommandLine(*cmdLineArgs, config);
    manager->Init(std::move(config));

    NllScanManager scanManager;

    auto globObs = (RooArgSet*) manager->GetListAsArgSet("paired_globs")->clone("globs");
    auto nps = (RooArgSet*) manager->GetListAsArgSet("paired_nps")->clone("nps"); // TODO: refactor to get nps
    auto pdf = (RooAbsPdf*) manager->GetPdf("pdf_total")->clone("pdf");

    if (nps == nullptr) {
        EFT_PROF_CRITICAL("NllScanManager::InitFromCommandLine nps are nullptr");
    }
    if (globObs == nullptr) {
        EFT_PROF_CRITICAL("NllScanManager::InitFromCommandLine globs are nullptr");
    }
    if (pdf == nullptr) {
        EFT_PROF_CRITICAL("NllScanManager::InitFromCommandLine pdf is nullptr");
    }

    vector<string> pois_to_float;
    EFT_PROF_CRITICAL("try to set up pois_to_float");
    cmdLineArgs->SetValIfArgExists("pois_float", pois_to_float);
    EFT_PROF_CRITICAL("pois_to_float are set");



    auto pdf_to_use = dynamic_cast<RooAbsPdf*>(pdf->clone());
    //auto nps_to_use = dynamic_cast<RooArgSet*>(nps->clone());
    //auto globs_to_use = dynamic_cast<RooArgSet*>(globObs->clone());

    auto nps_to_use = new RooArgSet{};
    nps_to_use->add(*nps);

    auto globs_to_use = new RooArgSet{};
    globs_to_use->add(*globObs);

    scanManager
            .SetWorkerId(1)
            .SetWS(manager->GetWs())
            .SetPOIsToFloat(pois_to_float)
            .SetGlobs(globs_to_use)
            .SetNPs(nps_to_use)
            .SetData(&manager->GetData(PrePostFit::OBSERVED))
            .SetPDF(pdf_to_use)
            .SetGridType(GridType::EQUIDISTANT);

    EFT_PROF_CRITICAL("before leaving init function");

    if ( scanManager.fitSettings_.nps == nullptr) {
        EFT_PROF_CRITICAL("fitsettiings.nps == nullptr in the init");
    }
    if ( scanManager.fitSettings_.globalObs == nullptr) {
        EFT_PROF_CRITICAL("fitsettiings.globs == nullptr in the init");
    }
    if ( scanManager.fitSettings_.pdf == nullptr) {
        EFT_PROF_CRITICAL("fitsettiings.pdf == nullptr in the init");
    }

    return scanManager;
}

 NllScanManager& NllScanManager::SetPOIsToFloat(const std::vector<std::string>& list) {
    if (ws_ == nullptr) {
        EFT_PROF_CRITICAL("Seet WS before calling to SetPOIsToFloat[strings]");
        throw std::runtime_error("");
    }
    pois_to_float = new RooArgSet();
    for (const auto& poi : list) {
        pois_to_float->add(*ws_->GetVar(poi));
    }
    return *this;
}
 NllScanManager& NllScanManager::SetPOIsFullList(const std::vector<std::string>& list) {
    if (ws_ == nullptr) {
        EFT_PROF_CRITICAL("Seet WS before calling to SetPOIsFullList[strings]");
        throw std::runtime_error("");
    }
    all_pois = new RooArgSet();
    for (const auto& poi : list) {
        all_pois->add(*ws_->GetVar(poi));
    }
    return *this;
}

void NllScanManager::SaveRes() const {
    EFT_PROFILE_FN();


    //std::filesystem::path path_res = settings.path_to_save_res;
    std::filesystem::path path_res = "NllScans";
    size_t scan_dimension = pois_.size();

    if ( !std::filesystem::exists(path_res) ) {
        EFT_PROF_INFO("Required path directory {} needs to be created", path_res);
        std::filesystem::create_directories(path_res);
    }

    const string name = fmt::format("{}/res__{}D__worker_{}__{}_at_{}.json",
                                    path_res.string(),
                                    scan_dimension,
                                    worker_id,
                                    pois_[0].Name(),
                                    pois_[0].Value()
    );

    nlohmann::json j;
    j = res_;
    ofstream f_res;
    f_res.exceptions(ofstream::failbit | ofstream::badbit);

    try {
        f_res.open(name);
        f_res << setw(4) << j << endl;
        cout << "duplicate to the console:" << endl;
        cout << setw(4) << j << endl;
    }
    catch (...) {
        cout << "impossible to open: " << name << endl;
        cout << "print to console:" << endl;
        cout << setw(4) << j << endl;
    }
}

} // eft::stats::scans