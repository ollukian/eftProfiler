//
// Created by Aleksei Lukianchuk on 16-Jan-23.
//
// @ lukianchuk.aleksei@gmail.com

#ifndef EFTPROFILER_NLLSCANPLOTTERSETTINGS_H
#define EFTPROFILER_NLLSCANPLOTTERSETTINGS_H

#include "../NpRankingStudyRes.h"
#include "../../Utils/ColourUtils.h"

#include <memory>
class CommandLineArgs;

namespace eft::stats::scans {

struct NllCurveSettings;
struct NllScanPlotterSettings;

struct NllCurveSettings {
    utils::Colour   colour      {utils::Colour::CreateFromString("kBlue")};
    size_t          thickness   {2u};
    bool            to_draw     {true};
    std::string     title       ;
    std::string     poi_name    {"mu"};

    std::vector<float>  mu_values;
    std::vector<float>  dnll_values;
};

struct NllScanPlotterSettings {
    float range_mu_l    {0.f};
    float range_mu_h    {0.f};
    float range_2dnll_l {0.f};
    float range_2dnll_h {0.f};
    NllCurveSettings    full;
    NllCurveSettings    stat;

    float                       rmargin             {0.05};
    float                       lmargin             {0.10};
    float                       tmargin             {0.03};
    float                       bmargin             {0.40};
    std::vector<size_t>         plt_size            {1200, 800}; // X
    float                       label_size          {0.02}; // X
    std::vector<std::string>    fileformat          {"pdf"}; // X
    //std::vector<std::string>    ignore_name         {};
    //std::vector<std::string>    match_names         {};
    std::string                 out_dir             {"figures"}; // X
    std::string                 output              {};
    //std::vector<std::string>    remove_prefix       {};
    //std::vector<std::string>    remove_suffix       {};
    std::vector<std::string>    replace             {};
    std::string                 ds_title            {"Higgs Combination"}; // X
    float                       energy              {13}; // X
    std::string                 lumi                {"36.1-139"}; // X
    std::string                 experiment          {"ATLAS"}; // X
    std::string                 res_status          {"Internal"}; // X
    float                       mu_offset           {1.4}; // X
    std::string                 mu_latex            {}; // X
    float                       text_size           {0.030}; // X
    float                       text_font           {42};  // X
    //std::vector<std::string>    add_text            {};
    //std::vector<std::string>    add_text_ndc        {};

    void ReadSettingsFromCommandLine(std::shared_ptr<CommandLineArgs>& cmdLine);
    //StudyType studyType; // expected or observed
};



} // eft::stats::scan

#endif //EFTPROFILER_NLLSCANPLOTTERSETTINGS_H
