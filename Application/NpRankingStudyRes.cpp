//
// Created by Aleksei Lukianchuk on 14-Sep-22.
//

#include "NpRankingStudyRes.h"

namespace eft::stats {


void to_json(nlohmann::json& j, const StudyType& s)
{
    switch (s) {
        case StudyType::NOTDEF:
            j["StudyType"] = "NOTDEF";
            break;
        case StudyType::EXPECTED:
            j["StudyType"] = "EXPECTED";
            break;
        case StudyType::OBSERVED:
            j["StudyType"] = "OBSERVED";
            break;
    }
}

void to_json(nlohmann::json& j, const StatType& s)
{
    switch (s) {
        case StatType::NOTDEF:
            j["StatType"] = "NOTDEF";
            break;
        case StatType::STAT:
            j["StatType"] = "STAT";
            break;
        case StatType::FULL:
            j["StatType"] = "FULL";
            break;
        case StatType::NP_RANKING:
            j["StatType"] = "NP_RANKING";
            break;
    }
}

void to_json(nlohmann::json& j, const PrePostFit& s)
{
    switch (s) {
        case PrePostFit::PREFIT:
            j["PrePostFit"] = "PREFIT";
            break;
        case PrePostFit::POSTFIT:
            j["PrePostFit"] = "POSTFIT";
            break;
    }
}

void from_json(const nlohmann::json& j, StudyType& s)
{
    const std::string& s_type = j.at("StudyType");
    if      (s_type == "NOTDEF")   s = StudyType::NOTDEF;
    else if (s_type == "OBSERVED") s = StudyType::OBSERVED;
    else if (s_type == "EXPECTED") s = StudyType::EXPECTED;
    else {
        throw std::runtime_error("Impossible to parse Study Type: {" + s_type + "}");
    }
}

void from_json(const nlohmann::json& j, StatType& s)
{
    const std::string& s_type = j.at("StudyType");
    if      (s_type == "NOTDEF")     s = StatType::NOTDEF;
    else if (s_type == "STAT")       s = StatType::STAT;
    else if (s_type == "NP_RANKING") s = StatType::NP_RANKING;
    else if (s_type == "FULL")       s = StatType::FULL;
    else {
        throw std::runtime_error("Impossible to parse StatType: {" + s_type + "}");
    }
}


void from_json(const nlohmann::json& j, PrePostFit& s);

} // stats