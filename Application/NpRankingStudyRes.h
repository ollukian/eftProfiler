//
// Created by Aleksei Lukianchuk on 14-Sep-22.
//

#ifndef EFTPROFILER_NPRANKINGSTUDYRES_H
#define EFTPROFILER_NPRANKINGSTUDYRES_H

#include "nlohmann/json.hpp"

#include <string>

namespace eft::stats {

enum class StudyType  : uint8_t;
enum class StatType   : uint8_t;
enum class PrePostFit : uint8_t;

struct NpRankingStudyRes;
struct NpRankingStudySettings;

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
   PREFIT  = 0,
   POSTFIT = 1
};


struct
NpRankingStudyRes {
    using json = nlohmann::json;

    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(NpRankingStudyRes,
                                       poi_name, np_name,
                                       poi_val, poi_err,
                                       nll,
                                       studyType,
                                       statType,
                                       prePostFit);

    StudyType studyType { StudyType::NOTDEF };
    StatType  statType  { StatType::NOTDEF };
    PrePostFit prePostFit {PrePostFit::PREFIT};

    std::string poi_name;
    std::string np_name;

    double poi_val;
    double poi_err;

    double nll;

}; // NpRankingStudyRes

struct NpRankingStudySettings
{
    std::string poi;
    std::string path_to_save_res;
    StatType  statType  {StatType::NP_RANKING};
    StudyType studyType {StudyType::NOTDEF};
    PrePostFit prePostFit {PrePostFit::PREFIT};
};




} // stats

#endif //EFTPROFILER_NPRANKINGSTUDYRES_H
