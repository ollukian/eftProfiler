//
// Created by Aleksei Lukianchuk on 12-Sep-22.
//

#ifndef EFTPROFILER_WSPLOTTER_H
#define EFTPROFILER_WSPLOTTER_H

#include "TCanvas.h"
#include "RooAbsPdf.h"
#include "RooAbsData.h"

namespace eft {

class WsPlotter {
public:
    static bool plotDS(RooAbsData* data, RooAbsPdf* pdf) noexcept;

};

} // eft

#endif //EFTPROFILER_WSPLOTTER_H
