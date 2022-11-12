//
// Created by Aleksei Lukianchuk on 16-Sep-22.
//

#ifndef EFTPROFILER_FITMANAGERCONFIG_H
#define EFTPROFILER_FITMANAGERCONFIG_H

#include <string>

namespace eft::stats {

struct FitManagerConfig {
    std::string ws_path;
    std::string ws_name {"combWS"};
    std::string model_config {"ModelConfig"};
    std::string comb_pdf {"combPdf"};
    std::string comb_data {"combData"};
    std::string res_path {"."};
    std::string poi {"cHG"};
    std::string color_prefit;   // <== to add
    std::string color_postfit;  // <== to add
    std::vector<std::string> errors;
    size_t worker_id {0};
    size_t top {20}; // np pois to plot
    double fit_precision {1E-3};
    double poi_init_val {0.};
    std::string study_type;
    std::string snapshot;
    bool no_gamma {false};
    bool fit_all_pois {false};
    bool fit_single_poi {true};
    bool vertical {false}; // <== to add
    bool reuse_nll {true}; // <== to add
    float rmargin {0.10}; // <== to add
    float lmargin {0.10}; // <== to add
    float tmargin {0.05}; // <== to add
    float bmargin {0.40}; // <== to add
    std::vector<size_t> plt_size {1200, 800}; // <== to add
};

} // stats

#endif //EFTPROFILER_FITMANAGERCONFIG_H
