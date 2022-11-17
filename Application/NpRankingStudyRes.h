//
// Created by Aleksei Lukianchuk on 14-Sep-22.
//

#ifndef EFTPROFILER_NPRANKINGSTUDYRES_H
#define EFTPROFILER_NPRANKINGSTUDYRES_H

#include "nlohmann/json.hpp"
#include "ErrorsReEvaluation.h"
// fwd declaration
class IWorkspaceWrapper;

#include <string>

namespace eft::stats {

enum class StudyType  : uint8_t;
enum class StatType   : uint8_t;
enum class PrePostFit : uint8_t;

struct NpRankingStudyRes;  // result of running fit
struct NpRankingStudySettings; // settings for plotting
struct NpInfoForPlot;

//void to_json(nlohmann::json& j, const NpRankingStudyRes& np_res);
//void from_json(const nlohmann::json& j, NpRankingStudyRes& np_res);

enum class StudyType : uint8_t
{
    NOTDEF = 0,
    EXPECTED = 1,
    OBSERVED = 2
    //ASIMOV = 1,
    //REAL   = 2
};

enum class StatType : uint8_t
{
    NOTDEF     = 0,
    STAT       = 1,
    FULL       = 2,
    NP_RANKING = 3
};

enum class PrePostFit : uint8_t
{
   OBSERVED = 0,
   PREFIT   = 1,
   POSTFIT  = 2
};

void to_json(nlohmann::json& j, const StudyType& s);
void from_json(const nlohmann::json& j, StudyType& s);
void to_json(nlohmann::json& j, const StatType& s);
void from_json(const nlohmann::json& j, StatType& s);
void to_json(nlohmann::json& j, const PrePostFit& s);
void from_json(const nlohmann::json& j, PrePostFit& s);


struct NpRankingStudyRes {
    //using json = nlohmann::json;

    StudyType studyType { StudyType::NOTDEF };
    StatType  statType  { StatType::NOTDEF };
    PrePostFit prePostFit {PrePostFit::PREFIT};

    std::string poi_name {"none"};
    std::string np_name {"none"};

    double poi_free_fit_val {0};
    double poi_free_fit_err {0};

    double poi_fixed_np_val {0};
    double poi_fixed_np_err {0};

    double poi_plus_sigma_variation_val {0};
    double poi_plus_sigma_variation_err {0};

    double poi_minus_sigma_variation_val {0};
    double poi_minus_sigma_variation_err {0};

    double poi_minus_one_variation_val {0};
    double poi_minus_one_variation_err {0};

    double poi_plus_one_variation_val {0};
    double poi_plus_one_variation_err {0};

    double np_val {0};
    double np_err {0};

    double nll {0};

    void ExtractPoiValErr(IWorkspaceWrapper* ws, const std::string& poi);
    void ExtractNPValErr(IWorkspaceWrapper* ws, const std::string& np);

}; // NpRankingStudyRes

struct NpRankingStudySettings
{
    std::string poi;
    std::string path_to_save_res {"/pbs/home/o/ollukian/public/EFT/git/eftProfiler/"};
    StatType  statType  {StatType::NP_RANKING};
    StudyType studyType {StudyType::NOTDEF};
    PrePostFit prePostFit {PrePostFit::OBSERVED};
    fit::Errors errors {fit::Errors::DEFAULT};
    bool no_gamma {false};
    double poi_init_val;
    double eps;
    size_t retry;
    size_t strategy;
};

// Only information relevant for plotting
struct NpInfoForPlot
{
    std::string name;
    std::string poi;
    /*float post_fit_value;
    float post_fit_error;
    float pre_fit_value;
    float pre_fit_error;*/
    float obs_value;
    float obs_error;

    double impact;
    double impact_plus_sigma_var;
    double impact_minus_sigma_var;
    double impact_plus_one_var;
    double impact_minus_one_var;

};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(NpRankingStudyRes,
                                   poi_name, np_name,
                                   poi_free_fit_val,              poi_free_fit_err,
                                   poi_fixed_np_val,              poi_fixed_np_err,
                                   poi_plus_sigma_variation_val,  poi_plus_sigma_variation_err,
                                   poi_minus_sigma_variation_val, poi_minus_sigma_variation_err,
                                   poi_minus_one_variation_val,   poi_minus_one_variation_err,
                                   poi_plus_one_variation_val,    poi_plus_one_variation_err,
                                   np_val,                        np_err,
                                   nll,
                                   studyType,
                                   statType,
                                   prePostFit);




} // stats

#endif //EFTPROFILER_NPRANKINGSTUDYRES_H
