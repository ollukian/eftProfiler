//
// Created by Aleksei Lukianchuk on 16-Jan-23.
//
// @ lukianchuk.aleksei@gmail.com

#ifndef EFTPROFILER_NLLSCANPLOTTERSETTINGS_H
#define EFTPROFILER_NLLSCANPLOTTERSETTINGS_H

#include "../NpRankingStudyRes.h"
#include "../../Utils/ColourUtils.h"

namespace eft::stats::scan {

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
    //StudyType studyType; // expected or observed
};



} // eft::stats::scan

#endif //EFTPROFILER_NLLSCANPLOTTERSETTINGS_H
