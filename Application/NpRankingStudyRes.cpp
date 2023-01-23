//
// Created by Aleksei Lukianchuk on 14-Sep-22.
//

#include "NpRankingStudyRes.h"
#include "IWorkspaceWrapper.h"
#include "FitManagerConfig.h"

#include "../Core/Logger.h"

using namespace std;

namespace eft::stats {


void to_json(nlohmann::json& j, const StudyType& s)
{
    switch (s) {
        case StudyType::NOTDEF:
            j["studyType"] = "NOTDEF";
            break;
        case StudyType::EXPECTED:
            j["studyType"] = "EXPECTED";
            break;
        case StudyType::OBSERVED:
            j["studyType"] = "OBSERVED";
            break;
    }
}

void to_json(nlohmann::json& j, const StatType& s)
{
    switch (s) {
        case StatType::NOTDEF:
            j["statType"] = "NOTDEF";
            break;
        case StatType::STAT:
            j["statType"] = "STAT";
            break;
        case StatType::FULL:
            j["statType"] = "FULL";
            break;
        case StatType::NP_RANKING:
            j["statType"] = "NP_RANKING";
            break;
    }
}

void to_json(nlohmann::json& j, const PrePostFit& s)
{
    switch (s) {
        case PrePostFit::PREFIT:
            j["prePostFit"] = "PREFIT";
            break;
        case PrePostFit::POSTFIT:
            j["prePostFit"] = "POSTFIT";
            break;
        case PrePostFit::OBSERVED:
            j["prePostFit"] = "OBSERVED";
            break;
    }
}

void from_json(const nlohmann::json& j, StudyType& s)
{
    const std::string& s_type = j.at("studyType");
    if      (s_type == "NOTDEF")   s = StudyType::NOTDEF;
    else if (s_type == "OBSERVED") s = StudyType::OBSERVED;
    else if (s_type == "EXPECTED") s = StudyType::EXPECTED;
    else {
        throw std::runtime_error("Impossible to parse Study Type: {" + s_type + "}");
    }
}

void from_json(const nlohmann::json& j, StatType& s)
{
    const std::string& s_type = j.at("statType");
    if      (s_type == "NOTDEF")     s = StatType::NOTDEF;
    else if (s_type == "STAT")       s = StatType::STAT;
    else if (s_type == "NP_RANKING") s = StatType::NP_RANKING;
    else if (s_type == "FULL")       s = StatType::FULL;
    else {
        throw std::runtime_error("Impossible to parse StatType: {" + s_type + "}");
    }
}

void from_json(const nlohmann::json& j, PrePostFit& s)
{
    const std::string& s_type = j.at("prePostFit");
    if      (s_type == "PREFIT")     s = PrePostFit::PREFIT;
    else if (s_type == "POSTFIT")    s = PrePostFit::POSTFIT;
    else if (s_type == "OBSERVED")    s = PrePostFit::OBSERVED;
    else {
        throw std::runtime_error("Impossible to parse PrePostFit: {" + s_type + "}");
    }
}

void NpRankingStudyRes::ExtractPoiValErr(IWorkspaceWrapper* ws, const std::string& poi)
{
    EFT_PROF_CRITICAL("do not use NpRankingStudyRes::ExtractPoiValErr!!");
    this->poi_fixed_np_err = ws->GetParErr(poi);
    this->poi_fixed_np_val = ws->GetParVal(poi);
}

void NpRankingStudyRes::ExtractNPValErr(IWorkspaceWrapper* ws, const std::string& np)
{
    this->np_err = ws->GetParErr(np);
    this->np_val = ws->GetParVal(np);
}

void NpRankingStudySettings::GetFromConfig(FitManagerConfig* config)
{
    poi                = config->poi;
    path_to_save_res   = config->res_path;
    poi_init_val       = config->poi_init_val;
    eps                = config->eps;
    retry              = config->retry;
    strategy           = config->strategy;
    reuse_nll          = config->reuse_nll;
    fit_all_pois       = config->fit_all_pois;
    fit_single_poi     = config->fit_single_poi;
    np_name            = config->np_name;

    string postFit = config->study_type;
    if (postFit == "prefit")
        prePostFit = eft::stats::PrePostFit::PREFIT;
    else if (postFit == "postfit")
        prePostFit = eft::stats::PrePostFit::POSTFIT;
    else if (postFit == "observed")
        prePostFit = eft::stats::PrePostFit::OBSERVED;
    else {
        EFT_PROF_WARN("Prepostfit: is not set known. Use OBSERVED by default (options: prefit, postfit, observed");
        prePostFit = eft::stats::PrePostFit::OBSERVED;
    }

    // construct Errors type from the input strings "errors" : vector<string>
    {
        bool isHesse {false};
        bool isMinos {false};
        bool refineNPs {false};
        bool refinePOIs {false};

        // todo: add stringutils: trim, tolowcase, tocapitalcase

        for (const string& arg : config->errors) {
            EFT_PROF_DEBUG("dispatch: {}", arg);
            if (arg == "Minos" || arg == "minos") {
                EFT_PROF_INFO("found Minos argument");
                isMinos = true;
            }
            else if (arg == "Hesse" || arg == "HESSE" || arg == "hesse") {
                EFT_PROF_INFO("found Hesse argument");
                isHesse = true;
            }
            else if (arg == "nps" || arg == "NPs" || arg == "Nps" || arg == "NP" || arg == "np") {
                EFT_PROF_INFO("found NP argument");
                refineNPs = true;
            }
            else if (arg == "POIs" || arg == "Pois" || arg == "pois" || arg == "poi") {
                EFT_PROF_INFO("found POI argument");
                refinePOIs = true;
            }
            else {
                EFT_PROF_CRITICAL("Command line, key -errors, got UNKNOWN argument: {}", arg);
                EFT_PROF_CRITICAL("Known: Minos, Hesse, nps, pois, np, poi");
            }
        } // args

        // if only Minos with no other option - refine everything
        if (config->errors.size() == 1) {
            if (isMinos) {
                EFT_PROF_WARN("Minos with no arguments, refine everything (nps and POIs)");
                refineNPs = true;
                refinePOIs = true;
            }
        }

        // check logic // TODO: to make an independent function to construct if from a string and to validate
        if (isHesse && isMinos) {
            EFT_PROF_CRITICAL("Command line, key -errors, cannot use MINOS and HESSE at the same time, use on of them");
            throw std::runtime_error("");
        }
        if (!isHesse && !isMinos) {
            EFT_PROF_WARN("Command line, key -errors, neither MINOS nor HESSE are set, use DEFAULT - no error re-evaluation");
            errors = eft::stats::fit::Errors::DEFAULT;
        }
        else if (isHesse) {
            EFT_PROF_INFO("Use HESSE");
            errors = eft::stats::fit::Errors::HESSE;
        }
        else { // MINOS
            if (refineNPs && refinePOIs) {
                EFT_PROF_INFO("Use MINOS for nps and pois");
                errors = eft::stats::fit::Errors::MINOS_ALL;
            }
            else if (refineNPs) {
                EFT_PROF_INFO("Use MINOS only for nps");
                errors = eft::stats::fit::Errors::MINOS_NPS;
            }
            else {
                EFT_PROF_INFO("Use MINOS only for pois");
                errors = eft::stats::fit::Errors::MINOS_POIS;
            }
        } // MINOS

    } // construction of errros type
}

} // stats