//
// Created by Aleksei Lukianchuk on 16-Sep-22.
//

#ifndef EFTPROFILER_FITMANAGERCONFIG_H
#define EFTPROFILER_FITMANAGERCONFIG_H

#include "../Vendors/spdlog/fmt/ostr.h"



#include <string>

namespace eft::stats {

struct FitManagerConfig {
    std::string                 ws_path;
    std::string                 ws_name             {"combWS"};
    std::string                 model_config        {"ModelConfig"};
    std::string                 comb_pdf            {"combPdf"};
    std::string                 comb_data           {"combData"};
    std::string                 res_path;
    std::string                 poi                 {"cHG"};
    std::vector<std::string>    errors;
    size_t                      worker_id           {0};
    double                      eps                 {1E-3}; // to be divided on 1000 later on
    size_t                      strategy            {1};
    size_t                      retry               {0};
    double                      poi_init_val        {0.};
    std::string                 study_type;
    std::string                 snapshot;
    bool                        no_gamma            {false};
    bool                        fit_all_pois        {false};
    bool                        fit_single_poi      {true};
    bool                        reuse_nll           {true};      // <== to add
    bool                        save_prelim         {false};
    bool                        silent              {false};
    std::string                 get                 {""};
    // PLOTTING
    bool                        vertical            {false};     // <== to add
    std::string                 color_prefit_plus   {"kBlue"};
    std::string                 color_prefit_minus  {"kGreen"};
    std::string                 color_postfit_plus  {"kBlue"};
    std::string                 color_postfit_minus {"kGreen"};

    std::string                 color_np;  // <== to add
    float                       rmargin             {0.10};
    float                       lmargin             {0.10};
    float                       tmargin             {0.05};
    float                       bmargin             {0.40};
    std::vector<size_t>         plt_size            {1200, 800};
    float                       rmul                {-0.002};
    float                       rmuh                {0.002};
    float                       np_scale            {1E-9};
    float                       label_size          {0.02};
    std::vector<std::string>    fileformat          {"pdf"};
    std::vector<std::string>    ignore_name         {};
    std::vector<std::string>    match_names         {};
    size_t                      top                 {20};       // np pois to plot
    std::string                 out_dir             {"figures"};
    std::string                 output              {};
    std::string                 input               {};
    std::vector<std::string>    remove_prefix       {};
    std::vector<std::string>    remove_suffix       {};
    std::vector<std::string>    replace             {};
    std::string                 ds_title            {"Higgs Combination"};
    float                       energy              {13};
    std::string                 lumi                {"36.1-139"};
    std::string                 experiment          {"ATLAS"};
    std::string                 res_status          {"Internal"};
    float                       mu_offset           {1.4};
    float                       np_offset           {1.0};
    std::string                 mu_latex            {};
    std::vector<std::string>    np_names            {};
    float                       text_size           {0.030};
    float                       text_font           {42};
    float                       dy                  {0.03};
    std::vector<std::string>    add_text            {};
    std::vector<std::string>    add_text_ndc        {};
};

template<typename OStream>
OStream& operator<<(OStream& os, const std::vector<std::basic_string<char>>& vec)
{
    os << '{';
    for (size_t idx {0}; idx < vec.size(); ++idx)
    {
        os << vec.at(idx) << ", ";
    }
    return os << vec.back() << '}';
}

} // stats

#endif //EFTPROFILER_FITMANAGERCONFIG_H
