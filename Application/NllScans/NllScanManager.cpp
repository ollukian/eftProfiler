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

#include "StringUtils.h"

#include "RooStats/AsymptoticCalculator.h"

#include "TCanvas.h"


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
        EFT_PROF_DEBUG("\tDealing with: {}...", poi.Name());
        if (poi.IsGridPointKnown()) {
            EFT_PROF_INFO("IdentifyScanPointCoordinateAllPois:: Poi: {} value is already set: {}", poi.Name(), poi.Value());
        }
        else {
            if(gridType_ != GridType::USER_DEFINED) {
                poi.ToTestAt(GetPointAtGrid(poi));
            }
        }
        EFT_PROF_INFO("IdentifyScanPointCoordinateAllPois:: Poi: {} is to be tested at: {}", poi.Name(), poi.Value());
    }
}

/// If STAT_ONLY, then Global Observables need to be put to the value found in data
/// And NPs should be fixed const at the same position
void NllScanManager::SetGlobsToNpsIfNeeded() {
    EFT_PROFILE_FN();
    if (statType_ == StatType::FULL) {
        EFT_PROF_INFO("NllScanManager::SetGlobsToNpsIfNeeded ==> FULL study,  need to fit nps");
        return;
    }
    EFT_PROF_INFO("NllScanManager::SetGlobsToNpsIfNeeded ==> STAT ONLY study, no need to fit nps");

    auto nps = fitSettings_.nps;
    auto globs = fitSettings_.globalObs;

    const size_t nb_nps = nps->size();
    for (size_t idx_np {0}; idx_np < nb_nps; ++idx_np)
    {
        auto np = dynamic_cast<RooRealVar*>(nps->operator[](idx_np));
        for (size_t idx_glob {0}; idx_glob < nb_nps; ++idx_glob)
        {
            auto glob = dynamic_cast<RooRealVar*>( globs->operator[](idx_glob) );
            if (glob->dependsOn(*np))
            {
                std::string np_name   = np->GetName();
                std::string glob_name = glob->GetName();
                EFT_PROF_DEBUG("Set glob: {:60} to the value of \n\t np: {:60} => {:10}",
                               std::move(glob_name),
                               std::move(np_name),
                               np->getVal()
                );
                glob->setVal(np->getVal());
                np->setConstant(true);
            }
        }
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

RooAbsData* NllScanManager::GetData(PrePostFit prePostFit) {
    EFT_PROFILE_FN();
    if (prePostFit == PrePostFit::OBSERVED) {
        EFT_PROF_INFO("NllScanManager::GetData[OBSERVED] => return real data");
        return fitSettings_.data;
    }
    if (prePostFit == PrePostFit::PREFIT) {
        EFT_PROF_INFO("NllScanManager::GetData[{}] => create asimov data", "PREFIT");
    }
    else if (prePostFit == PrePostFit::POSTFIT) {
        EFT_PROF_INFO("NllScanManager::GetData[{}] => create asimov data", "POSTFIT");
    }


    EFT_PROF_DEBUG("* use {} pois to asimov data:", pois_to_float->size());
    for (auto poi : *pois_to_float) {
        EFT_PROF_DEBUG("\t |{}|", poi->GetName());
    }

    EFT_PROF_DEBUG(" * use {} globs to asimov data:", fitSettings_.globalObs->size());
    for (auto globs : *fitSettings_.globalObs) {
        EFT_PROF_DEBUG("\t |{}|", globs->GetName());
    }

    auto ds = (RooDataSet*) RooStats::AsymptoticCalculator::MakeAsimovData(*fitSettings_.data,
                                                                      ws_->GetModelConfig(),
                                                                      *pois_to_float,
                                                                      *fitSettings_.globalObs
    );

    ws_->raw()->saveSnapshot("condGlobObs", *fitSettings_.globalObs, kTRUE);
    ws_->raw()->loadSnapshot("condGlobObs");
    return ds;
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

void NllScanManager::RunFreeFit() {
    EFT_PROF_INFO("RunFreeFit: Run free fit to get required values of nps");
    if (one_at_time) {
        // TODO: allows 2D/3D cases
        EFT_PROF_INFO("RunFreeFit: option one_at_time ==> allow to float only: {}", pois_[0].Name());
        //EFT_PROF_INFO("RunFreeFit: Make all pois const and then float allowed ones..");
        ws_->FixValConst(all_pois);
        //EFT_PROF_INFO("RunFreeFit: float allowed POIs..");
        ws_->FloatVal(pois_[0].Name());
    }
    else if (fit_all_pois) {
        EFT_PROF_INFO("RunFreeFit: option fit_all_pois ==> allow to float all POIs: {}");
        EFT_PROF_INFO("RunFreeFit: option fit_all_pois ==> allow to float all POIs: {}");
        ws_->FloatVals(all_pois);
    }

    if (prePostFit_ == PrePostFit::POSTFIT) {
        EFT_PROF_INFO("RunFreeFit: option POSTFIT ==> allow globs to be not at zero");
    }
    else if (prePostFit_ == PrePostFit::PREFIT) {
        EFT_PROF_INFO("RunFreeFit: option PREFIT ==> force globs to zero");
        ws_->SetVarVal(fitSettings_.globalObs, 0.);
    }
    else if (prePostFit_ == PrePostFit::OBSERVED) {
        EFT_PROF_INFO("RunFreeFit: option OBSERVED ==> force globs to zero");
        ws_->SetVarVal(fitSettings_.globalObs, 0.);
    }

    EFT_PROF_DEBUG("RunFreeFit: pois before free fit:");
    for (auto poi : *all_pois) {
        auto ptr = dynamic_cast<RooRealVar*>(poi);
//    for (const auto& poi : pois_) {
//        auto ptr = ws_->GetVar(poi.Name());
        string is_const_str = "F";
        if (ptr->isConstant())
            is_const_str = "C";
        EFT_PROF_DEBUG("{:60} [{:10} +- {:10}] {}",
                       ptr->GetName(),
                       ptr->getVal(),
                       ptr->getError(),
                       is_const_str);
    }


    EFT_PROF_DEBUG("NPS before nll creation for free fit");
    fitSettings_.nps->Print("v");

    EFT_PROF_DEBUG("Globs before nll creation for free fit");
    fitSettings_.globalObs->Print("v");
    //ws_->FloatVals(all_pois);

    fit::Fitter fitter;
    auto nll_free_fit = fitter.CreatNll(fitSettings_);
    EFT_PROF_INFO("Run free fit to get required values of nps ==> nll is created");
    fitter.Minimize(fitSettings_, nll_free_fit);
    EFT_PROF_INFO("RunFreeFit: pois after free fit:");
    for (auto poi : *all_pois) {
        auto ptr = dynamic_cast<RooRealVar*>(poi);

        string is_const_str = "F";
        if (ptr->isConstant())
            is_const_str = "C";
        EFT_PROF_DEBUG("{:60} [{:10} +- {:10}] {}",
                       ptr->GetName(),
                       ptr->getVal(),
                       ptr->getError(),
                       is_const_str);
    }
    EFT_PROF_DEBUG("RunFreeFit: NPS after free fit");
    fitSettings_.nps->Print("v");
    EFT_PROF_DEBUG("RunFreeFit: globs after free fit");
    fitSettings_.globalObs->Print("v");
    //SetPOIsToTheRequiredGridPosition();
//            EFT_PROF_INFO("pois after setting the grid back after free fit:");
//            all_pois->Print("v");
    //EFT_PROF_INFO("Run free fit to get required values of nps ==> return back POIs");
//    EFT_PROFILE_FN();
//
//    if (all_pois != nullptr) {
//        EFT_PROF_INFO("FixNllScanManager::RunFreeFit:: all POIs (later on, needed ones will be float)");
//        ws_->FixValConst(all_pois);
//    }
//
//    EFT_PROF_INFO("FixNllScanManager::RunFreeFit:: Float required POIs (which are allowed to by a research)");
//    ws_->FloatVals(pois_to_float);
//    auto data = GetData(prePostFit_);
//    fitSettings_.data = data;
//    EFT_PROF_INFO("FixNllScanManager::RunFreeFit:: Set globs to zero...");
//    ws_->SetVarVal(fitSettings_.globalObs, 0.);
//
//    fit::Fitter fitter;
//    auto nll = fitter.CreatNll(fitSettings_);
//
//    EFT_PROF_INFO("Minimise Nll");
//    fitter.Minimize(fitSettings_, nll);
}

void NllScanManager::RunScan() {
    EFT_PROFILE_FN();

    if ( ! snapshot_.empty() ) {
        EFT_PROF_INFO("Load snapshot: {}", snapshot_);
        ws_->raw()->loadSnapshot(snapshot_.c_str());
    }

    RooAbsData* data;

    if (force_data) {
        EFT_PROF_INFO("Force loading dataset: {}", data_name);
        data = ws_->raw()->data(data_name.c_str());
        if (!data) {
            EFT_PROF_CRITICAL("Data: {} is not present in the ws", data_name);
            throw std::runtime_error("Data name is not correct (not present in the WS)");
        }
    }
    else {
        EFT_PROF_CRITICAL("Get dataset from the manager");
        data = GetData(prePostFit_);
    }
    fitSettings_.data = data;


    //EFT_PROF_INFO("Create Nll");
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

    if (statType_ == StatType::STAT) {
        EFT_PROF_INFO("Study type: Stan only ==> need to fix all nps to const after a free fit");
        RunFreeFit();
        ws_->FixValConst(fitSettings_.nps);
    }
    else {
        EFT_PROF_INFO("Study type: Full ==> no need to fix nps");
    }

    if (one_at_time) {
        // TODO: allows 2D/3D cases
        EFT_PROF_INFO("RunScan: option one_at_time ==> allow to float only: {}", pois_[0].Name());
        //EFT_PROF_INFO("RunFreeFit: Make all pois const and then float allowed ones..");
        ws_->FixValConst(all_pois);
        //EFT_PROF_INFO("RunFreeFit: float allowed POIs..");
        ws_->FloatVal(pois_[0].Name());
    }
    else if (fit_all_pois) {
        EFT_PROF_INFO("RunScan: option fit_all_pois ==> allow to float all {} POIs", all_pois->size());
        ws_->FloatVals(all_pois);
    }
    else if (user_defined) {
        EFT_PROF_INFO("RunScan: option user_defined ==> allow to float all {} mentioned POIs", pois_to_float->size());
        ws_->FixValConst(all_pois);
        ws_->FloatVals(pois_to_float);
    }

//    if (all_pois != nullptr) {
//        EFT_PROF_INFO("Fix all POIs (later on, needed ones will be float)");
//        ws_->FixValConst(all_pois);
//    }
//    EFT_PROF_INFO("Float required POIs (which are allowed to by a research)");
//    ws_->FloatVals(pois_to_float);

    EFT_PROF_DEBUG("Identify grid points to put the pois to");
    IdentifyScanPointCoordinateAllPois();

    EFT_PROF_DEBUG("Set up the grid by forcing all POIs to bet at the required values");
    SetPOIsToTheRequiredGridPosition();

    EFT_PROF_DEBUG("Fix GRID POIs to be const");
    FixGridPOIs();

    // TODO: to handle post-fit things
    if (prePostFit_ != PrePostFit::POSTFIT) {
        EFT_PROF_INFO("Not postfit => set globs to zero");
        ws_->SetVarVal(fitSettings_.globalObs, 0.);
    }

    //EFT_PROF_DEBUG("Globs before nll creation ....");
    //fitSettings_.globalObs->Print("v");
    //EFT_PROF_DEBUG("NPS before nll creation....");
    //fitSettings_.nps->Print("v");

    EFT_PROF_DEBUG("pois before final fit:");
    for (auto poi : *all_pois) {
        auto ptr = dynamic_cast<RooRealVar*>(poi);
        string is_const_str = "F";
        if (ptr->isConstant())
            is_const_str = "C";
        EFT_PROF_DEBUG("{:60} [{:10} +- {:10}] {}",
                       ptr->GetName(),
                       ptr->getVal(),
                       ptr->getError(),
                       is_const_str);
    }

    auto nll = fitter.CreatNll(fitSettings_);

    EFT_PROF_INFO("Minimise Nll");
    fitter.Minimize(fitSettings_, nll);

    EFT_PROF_INFO("pois after final fit:");
    for (auto poi : *all_pois) {
        auto ptr = dynamic_cast<RooRealVar*>(poi);
        string is_const_str = "F";
        if (ptr->isConstant())
            is_const_str = "C";
        EFT_PROF_DEBUG("{:60} [{:10} +- {:10}] {}",
                       ptr->GetName(),
                       ptr->getVal(),
                       ptr->getError(),
                       is_const_str);
    }

    //EFT_PROF_DEBUG("Globs after final fit creation ....");
    //fitSettings_.globalObs->Print("v");
    //EFT_PROF_DEBUG("NPS after final fit creation....");
    //fitSettings_.nps->Print("v");

    auto found_nll = nll->getVal();
    EFT_PROF_INFO("nll value: {}", found_nll);

    res_ = NllScanResult{};
    res_.nll_val = found_nll;
    res_.poi_configs = pois_;
    res_.fit_status = fitter.GetLastFitStatus();
    res_.statType   = statType_;
    res_.prePostFit = prePostFit_;
    res_.studyType  = studyType_;
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

    eft::stats::FitManager::ExtractConfigFromFile(config);
    eft::stats::FitManager::ReadConfigFromCommandLine(*cmdLineArgs, config);
    manager->Init(std::move(config));

    string study_type_str;
    PrePostFit  prePostFit  {PrePostFit::OBSERVED}; // pre- post-fit
    StudyType   studyType   {StudyType::OBSERVED}; // expected
    StatType    statType    {StatType::FULL}; // stat

    //cmdLineArgs->SetValIfArgExists("study_type", study_type_str);
    //StringUtils::ToLowCase(study_type_str);

    if (cmdLineArgs->HasKey("prefit")) {
    //if (study_type_str == "prefit" || study_type_str == "pre") {
        studyType   = StudyType::EXPECTED;
        prePostFit  = PrePostFit::PREFIT;
    }
    //else if (study_type_str == "postfit" || study_type_str == "post") {
    else if (cmdLineArgs->HasKey("postfit")) {
        studyType   = StudyType::EXPECTED;
        prePostFit  = PrePostFit::POSTFIT;
    }
    if (cmdLineArgs->HasKey("stat_only")) {
        statType = StatType::STAT;
    }

    NllScanManager scanManager;

    auto globObs = (RooArgSet*) manager->GetListAsArgSet("paired_globs")->clone("globs");
    auto nps = (RooArgSet*) manager->GetListAsArgSet("paired_nps")->clone("nps"); // TODO: refactor to get nps
    auto pdf = (RooAbsPdf*) manager->GetPdf("pdf_total")->clone("pdf");
    //RooAbsData* data = ws_-
    //auto data* =  manager->GetData(prePostFit);

    if (nps == nullptr) {
        EFT_PROF_CRITICAL("NllScanManager::InitFromCommandLine nps are nullptr");
    }
    if (globObs == nullptr) {
        EFT_PROF_CRITICAL("NllScanManager::InitFromCommandLine globs are nullptr");
    }
    if (pdf == nullptr) {
        EFT_PROF_CRITICAL("NllScanManager::InitFromCommandLine pdf is nullptr");
    }

    bool user_defined_pois_to_float {false};

    vector<string> pois_to_float;
    if (cmdLineArgs->HasKey("pois_float")) {
        cmdLineArgs->SetValIfArgExists("pois_float", pois_to_float);
        user_defined_pois_to_float = true;
    }

    bool  force_data {false};
    string snapshot_name;

    cmdLineArgs->SetValIfArgExists("snapshot", snapshot_name);
    if (cmdLineArgs->HasKey("force_data"))
        force_data = true;


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
            .SetGridType(GridType::EQUIDISTANT)
            .SetPrePostFit(prePostFit)
            .SetStudyType(studyType)
            .SetStatType(statType);

    scanManager.user_defined = user_defined_pois_to_float;

    if (cmdLineArgs->HasKey("one_at_time"))
        scanManager.one_at_time = true;
    if (cmdLineArgs->HasKey("fit_all_pois"))
        scanManager.fit_all_pois = true;
    if (scanManager.one_at_time && scanManager.fit_all_pois) {
        EFT_PROF_CRITICAL("Cannot use one_at_time and fit_all_pois at the same time. Choose one");
        throw std::logic_error("inconsistent options: one_at_time and fit_all_pois");
    }
    if (scanManager.one_at_time && scanManager.user_defined) {
        EFT_PROF_CRITICAL("Cannot use one_at_time and user_defined at the same time. Choose one");
        throw std::logic_error("inconsistent options: one_at_time and user_defined");
    }
    if (scanManager.fit_all_pois && scanManager.user_defined) {
        EFT_PROF_CRITICAL("Cannot use fit_all_pois and user_defined at the same time. Choose one");
        throw std::logic_error("inconsistent options: fit_all_pois and user_defined");
    }

    EFT_PROF_INFO("Create a list of all pois to keep them const");
    auto pois = new RooArgSet{};

    for (const auto& poi : manager->GetListPOIs()) {
        EFT_PROF_INFO("\t add: {}", poi);
        pois->add(*manager->ws()->GetVar(poi));
    }

    scanManager.force_data = force_data;
    scanManager.snapshot_  = std::move(snapshot_name);
    scanManager.all_pois   = pois;

    if ( scanManager.fitSettings_.nps == nullptr) {
        EFT_PROF_CRITICAL("fitsettiings.nps == nullptr in the init");
    }
    if ( scanManager.fitSettings_.globalObs == nullptr) {
        EFT_PROF_CRITICAL("fitsettiings.globs == nullptr in the init");
    }
    if ( scanManager.fitSettings_.pdf == nullptr) {
        EFT_PROF_CRITICAL("fitsettiings.pdf == nullptr in the init");
    }

    if (cmdLineArgs->HasKey("retry")) {
        cmdLineArgs->SetValIfArgExists("retry", scanManager.fitSettings_.retry);
    }
    if (cmdLineArgs->HasKey("eps")) {
        cmdLineArgs->SetValIfArgExists("eps", scanManager.fitSettings_.eps);
    }
    if (cmdLineArgs->HasKey("strategy")) {
        cmdLineArgs->SetValIfArgExists("strategy", scanManager.fitSettings_.strategy);
    }

    return scanManager;
}

 NllScanManager& NllScanManager::SetPOIsToFloat(const std::vector<std::string>& list) {
    if (ws_ == nullptr) {
        EFT_PROF_CRITICAL("Set WS before calling to SetPOIsToFloat[strings]");
        throw std::runtime_error("");
    }
    pois_to_float = new RooArgSet();
    for (const auto& poi : list) {
        EFT_PROF_DEBUG("Add {:10} to the list of the POIs to float", poi);
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

void NllScanManager::SaveRes(const std::string& path) const {
    EFT_PROFILE_FN();

    EFT_PROF_INFO("NllScanManager save results to: {}", path);
    //std::filesystem::path path_res = settings.path_to_save_res;
    //std::filesystem::path path_res = "NllScans";
    size_t scan_dimension = pois_.size();

    if ( !std::filesystem::exists(path) ) {
        EFT_PROF_INFO("Required path directory {} needs to be created", path);
        std::filesystem::create_directories(path);
    }

    // TODO: refactor to "string FormName()"
    string expected_type {"observed"};
    if (res_.prePostFit == PrePostFit::PREFIT) {
        expected_type = "prefit";
    }
    else if (res_.prePostFit == PrePostFit::POSTFIT) {
        expected_type = "postfit";
    }

    string stat_type ;
    if (res_.statType == StatType::STAT) {
        stat_type = "stat_only";
    }

    const string name = fmt::format("{}/res__{}D__{}__{}__worker_{}__{}_at_{}.json",
                                    path,
                                    scan_dimension,
                                    std::move(expected_type),
                                    std::move(stat_type),
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