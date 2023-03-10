//
// Created by Aleksei Lukianchuk on 20-Dec-22.
//

#ifndef EFTPROFILER_CORRELATIONSTUDYPLOTSETTINGS_H
#define EFTPROFILER_CORRELATIONSTUDYPLOTSETTINGS_H

#include <vector>
#include <string>

//namespace eft::drawing {

struct CorrelationStudyPlotSettings {
    std::vector<std::pair<std::string, double>> correlations1;
    std::vector<std::pair<std::string, double>> correlations2;
    std::string label1;
    std::string label2;

    std::vector<std::string> sorted_names_1;
    std::vector<std::string> sorted_names_2;

    std::string name_to_save {"Comparison.pdf"};
    std::string draw_options {"colz"};

    size_t np_nps_plot;

    float                       rmargin             {0.10};
    float                       lmargin             {0.25};
    float                       tmargin             {0.05};
    float                       bmargin             {0.25};

    std::vector<size_t> plt_size {4000, 4000};
    float label_size {0.20};
    bool weighted {false};

    void FormSortedNames();
};

//}

#endif //EFTPROFILER_CORRELATIONSTUDYPLOTSETTINGS_H
