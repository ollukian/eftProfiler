//
// Created by Aleksei Lukianchuk on 12-Jan-23.
//

#include "Grid.h"
#include "Core.h"
#include "nlohmann/json.hpp"

namespace eft::stats::scans {

    using std::string;

void to_json(nlohmann::json& j, const GridType& gridType) {

    string line;

    switch (gridType) {
        case GridType::USER_DEFINED: line = "USER_DEFINED";
            break;
        case GridType::EQUIDISTANT: line = "EQUIDISTANT";
            break;
        case GridType::HERMITE:     line = "HERMITE";
            break;
    }

    j = nlohmann::json {
            {"grid_type", std::move(line)}
    };

}
void from_json(const nlohmann::json& j, GridType& gridType) {

    const string type_text = j.at("grid_type");

   if (type_text == "USER_DEFINED") {
       gridType = GridType::USER_DEFINED;
       return;
   }
    if (type_text == "EQUIDISTANT") {
        gridType = GridType::EQUIDISTANT;
        return;
    }
    if (type_text == "HERMITE") {
        gridType = GridType::HERMITE;
        return;
    }
    throw std::runtime_error(fmt::format("from_json[GridType] grid type: [{}] is not supported. Use: {}",
                                         type_text,
                                         "USER_DEFINED, EQUIDISTANT, HERMITE"));

}

} // eft::stats::scans
