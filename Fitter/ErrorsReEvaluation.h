//
// Created by Aleksei Lukianchuk on 20-Oct-22.
//

#ifndef EFTPROFILER_ERRORSREEVALUATION_H
#define EFTPROFILER_ERRORSREEVALUATION_H


#include <cstdint>

namespace eft::stats::fit {

enum class Errors : uint8_t {
    DEFAULT = 0,
    HESSE = 1,
    MINOS_NPS,
    MINOS_POIS,
    MINOS_ALL
};


}

#endif //EFTPROFILER_ERRORSREEVALUATION_H
