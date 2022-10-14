//
// Created by Aleksei Lukianchuk on 22-Sep-22.
//

#ifndef EFTPROFILER_RANKINGPLOTTERSETTINGS_H
#define EFTPROFILER_RANKINGPLOTTERSETTINGS_H

namespace eft::plot {

struct RankingPlotterSettings {
    size_t top {10};
    std::string fileformat {"pdf"};
    std::string ignore_name {""};
};

} // plot

#endif //EFTPROFILER_RANKINGPLOTTERSETTINGS_H
