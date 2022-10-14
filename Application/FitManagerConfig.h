//
// Created by Aleksei Lukianchuk on 16-Sep-22.
//

#ifndef EFTPROFILER_FITMANAGERCONFIG_H
#define EFTPROFILER_FITMANAGERCONFIG_H

#include <string>

namespace eft::stats {

struct FitManagerConfig {
    std::string ws_path;
    std::string ws_name {"CombWS"};
    std::string model_config {"ModelConfig"};
    std::string comb_pdf {"combPdf"};
    std::string comb_data {"combData"};
    std::string res_path {"."};
    std::string poi {"cHG"};
    size_t worker_id {0};
    size_t top {20}; // np pois to plot
    double fit_precision {1E-3};
    std::string study_type;
    bool no_gamma = false;
};

} // stats

#endif //EFTPROFILER_FITMANAGERCONFIG_H
