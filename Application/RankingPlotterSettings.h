//
// Created by Aleksei Lukianchuk on 22-Sep-22.
//

#ifndef EFTPROFILER_RANKINGPLOTTERSETTINGS_H
#define EFTPROFILER_RANKINGPLOTTERSETTINGS_H

namespace eft::plot {

struct RankingPlotterSettings {
    size_t                      top;
    std::string                 poi;
    std::string                 input;
    std::vector<std::string>    fileformat ;
    std::vector<std::string>    ignore_name {};
    std::vector<std::string>    match_names {};
    std::string                 out_dir {};
    std::string                 output  {};
    bool                        vertical;     // <== to add
    std::string                 color_prefit;   // <== to add
    std::string                 color_postfit;  // <== to add
    float                       rmargin;      // <== to add
    float                       lmargin;      // <== to add
    float                       tmargin;      // <== to add
    float                       bmargin;      // <== to add
    std::vector<size_t>         plt_size; // <== to add
    float                       rmul;
    float                       rmuh;
    float                       np_scale;
    float                       label_size;
    std::vector<std::string>    remove_prefix;
    std::vector<std::string>    remove_suffix;

    using Replacement = std::pair<std::string, std::string>;
    std::vector<Replacement>    replacements;

    std::string                 ds_title;
    float                       energy;
    std::string                 lumi;
    std::string                 experiment;
    std::string                 res_status;
    float                       np_offset;
    float                       mu_offset;
    std::string                 mu_latex;
    std::vector<std::string>    np_names;
};

} // plot

#endif //EFTPROFILER_RANKINGPLOTTERSETTINGS_H
