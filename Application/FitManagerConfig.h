//
// Created by Aleksei Lukianchuk on 16-Sep-22.
//

#ifndef EFTPROFILER_FITMANAGERCONFIG_H
#define EFTPROFILER_FITMANAGERCONFIG_H

#include <string>

namespace eft::stats {

struct FitManagerConfig {
    std::string ws_path;
    std::string ws_name;
    std::string model_configi_name;
};

} // stats

#endif //EFTPROFILER_FITMANAGERCONFIG_H
