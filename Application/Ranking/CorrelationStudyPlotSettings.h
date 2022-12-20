//
// Created by Aleksei Lukianchuk on 20-Dec-22.
//

#ifndef EFTPROFILER_CORRELATIONSTUDYPLOTSETTINGS_H
#define EFTPROFILER_CORRELATIONSTUDYPLOTSETTINGS_H

#include <vector>
#include <string>


struct CorrelationStudyPlotSettings {
    std::vector<std::pair<std::string, double>> correlations1;
    std::vector<std::pair<std::string, double>> correlations2;
    std::string label1;
    std::string label2;
};

#endif //EFTPROFILER_CORRELATIONSTUDYPLOTSETTINGS_H
