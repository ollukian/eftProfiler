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
    bool                        vertical;       // <== to add
    size_t                      color_prefit_plus  ; // <= not read direclty, but processed
    size_t                      color_prefit_minus ; // using ColourUtils::RegisterColour
    size_t                      color_postfit_plus ;
    size_t                      color_postfit_minus;
    std::string                 color_np;       // <== to add
    float                       rmargin;
    float                       lmargin;
    float                       tmargin;
    float                       bmargin;
    std::vector<size_t>         plt_size;
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
    float                       text_size;

    float                       text_font;
    float                       dy;
    std::vector<std::string>    add_text;
    std::vector<std::string>    add_text_ndc;
    std::vector<std::string>    h_draw_options;
    size_t                      empty_bins;
    float                       dx_legend;
    float                       dy_legend;
    size_t                      max_digits;
    bool                        draw_impact;
};

} // plot

#endif //EFTPROFILER_RANKINGPLOTTERSETTINGS_H
