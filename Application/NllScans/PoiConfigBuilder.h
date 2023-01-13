//
// Created by Aleksei Lukianchuk on 13-Jan-23.
//

#ifndef EFTPROFILER_POICONFIGBUILDER_H
#define EFTPROFILER_POICONFIGBUILDER_H

#include "PoiConfig.h"
#include <string>

namespace eft::stats::scans {

class PoiConfigBuilder {
public:
    PoiConfig buildFromString(const std::string& line);
    double ParseValueToken(std::string token);
    double ParseRangeToken(std::string token)
private:

};

} // eft::stats::scans

#endif //EFTPROFILER_POICONFIGBUILDER_H
