//
// Created by Aleksei Lukianchuk on 12-Jan-23.
//
// @ lukianchuk.aleksei@gmail.com


#ifndef EFTPROFILER_GRID_H
#define EFTPROFILER_GRID_H

#include "nlohmann/json_fwd.hpp"

namespace eft::stats::scans {

enum class GridType : uint8_t ;
void to_json(nlohmann::json& j, const GridType& gridType);
void from_json(const nlohmann::json& j, GridType& gridType);


enum class GridType : uint8_t {
    USER_DEFINED = 0,
    EQUIDISTANT  = 1,
    HERMITE,
};


} // eft::stats::scans

#endif //EFTPROFILER_GRID_H
