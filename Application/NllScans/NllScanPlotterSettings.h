//
// Created by Aleksei Lukianchuk on 16-Jan-23.
//
// @ lukianchuk.aleksei@gmail.com

#ifndef EFTPROFILER_NLLSCANPLOTTERSETTINGS_H
#define EFTPROFILER_NLLSCANPLOTTERSETTINGS_H

#include "../NpRankingStudyRes.h"
#include "../../Utils/ColourUtils.h"
#include <map>
#include "TGraph.h"

#include <memory>
class CommandLineArgs;
// Old style to pass to ROOT...
inline double Parabola2(double* xs, double* pars) {
    double x = xs[0];

    double mean  = pars[0];
    double sigma = pars[1];

    return ((x - mean) * (x - mean)) / (sigma * sigma);
};

namespace eft::stats::scans {

struct NllCurveSettings;
struct NllScanPlotterSettings;

struct NllCurveSettings {
    size_t          colour      {utils::ColourUtils::GetColourFromString("kBlue")};
    size_t          thickness   {2u};
    bool            to_draw     {false};
    std::string     title       ;
    std::string     poi_name    {"mu"};
    double          min_poi   {0.f};
    double          max_poi   {0.f};

    std::vector<std::pair<double, double>> mu_nll_values;
    std::vector<double>  mu_values;
    std::vector<double>  nll_values;

    double central_value {0.f};
    double central_error {0.f};

    std::shared_ptr<TGraph> graph {std::make_shared<TGraph>()};
    bool is_graph_ready {false};
    bool is_central_value_computed {false};


    [[nodiscard]] size_t NbPoints() const noexcept { return mu_nll_values.size(); }
    void AddPoint(double mu, double nll) {mu_nll_values.emplace_back(mu, nll);}
    void PrepareMuNllValues();
    std::shared_ptr<TGraph> GetGraph();
    double GetCentralValue() { if (!is_central_value_computed) ComputeCentralValueAndError(); return central_value;}
    double GetCentralError() { if (!is_central_value_computed) ComputeCentralValueAndError(); return central_error;}
    void ComputeCentralValueAndError();
};

struct NllScanPlotterSettings {
    //NllCurveSettings()
    double range_mu_l       {0.f};
    double range_mu_h       {0.f};
    double range_2dnll_l    {0.f};
    double range_2dnll_h    {0.f};
    bool draw_obs           {false};
    bool draw_exp           {false};
    bool draw_stat          {false};
    bool draw_full          {false};
    std::map<std::string, NllCurveSettings> curves;

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
    std::string                 ds_title            {"H#rightarrow #gamma#gamma & H#rightarrow 4l STXS combination"};
    float                       energy              {13};
    std::string                 lumi                {"36.1-139"};
    std::string                 experiment          {"ATLAS"};
    std::string                 res_status          {"Internal"};
    std::string                 obs_expected        {"Observed"};
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
