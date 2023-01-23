//
// Created by Aleksei Lukianchuk on 09-Nov-22.
//

#ifndef EFTPROFILER_STUDYRES_H
#define EFTPROFILER_STUDYRES_H

#include <string>

namespace eft::stats::ranking {

struct StudyRes {
    std::string poi_name;
    std::string np_name;
    double poi_val {0.};
    double poi_err {0.};
    double nll {0.};
};

} // eft::stats::ranking

#endif //EFTPROFILER_STUDYRES_H
