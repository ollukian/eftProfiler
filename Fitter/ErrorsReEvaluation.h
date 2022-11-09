//
// Created by Aleksei Lukianchuk on 20-Oct-22.
//

#ifndef EFTPROFILER_ERRORSREEVALUATION_H
#define EFTPROFILER_ERRORSREEVALUATION_H


#include <cstdint>

#include "../Vendors/spdlog/fmt/ostr.h"

namespace eft::stats::fit {

enum class Errors : uint8_t {
    DEFAULT = 0,
    HESSE = 1,
    MINOS_NPS,
    MINOS_POIS,
    MINOS_ALL
};

template<typename OStream>
OStream& operator<<(OStream& os, const Errors& c) {
    switch (c) {
        case Errors::DEFAULT:
            os << "Default";
            break;
        case Errors::HESSE:
            os << "HESSE";
            break;
        case Errors::MINOS_NPS:
            os << "MINOS_NPS";
            break;
        case Errors::MINOS_POIS:
            os << "MINOS_POIS";
            break;
        case Errors::MINOS_ALL:
            os << "MINOS_ALL";
            break;
    }
    return os;
}


}

#endif //EFTPROFILER_ERRORSREEVALUATION_H
