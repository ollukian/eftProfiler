//
// Created by Aleksei Lukianchuk on 15-Sep-22.
//

#ifdef _MSVC
#pragma once
#endif

#ifndef EFTPROFILER_NPRANKINGPLOTTER_H
#define EFTPROFILER_NPRANKINGPLOTTER_H

#include "../Application/NpRankingStudyRes.h"
#include "../Application/RankingPlotterSettins.h"

#include <vector>
#include <unordered_set>
#include <unordered_map>

//#include <nlohmann/json.hpp>

#include <TCanvas.h>
#include <TAxis.h>
#include "TH1D.h"
#include "THStack.h"
#include "TLegend.h"
#include "TLatex.h"

#include <filesystem>

namespace eft::plot {

using namespace eft::stats;

class NpRankingPlotter
{
public:
    void ReadValues(const std::filesystem::path& path);
    void Plot(const std::shared_ptr<RankingPlotterSettins>& settings) noexcept;
private:
    void ReadValuesOneFile(const std::filesystem::path& path);
    void RegisterRes(const NpRankingStudyRes& res) noexcept;
private:
    std::unordered_map<std::string, NpRankingStudyRes> np_study_res_;
    std::vector<NpInfoForPlot>                         res_for_plot_;
};

}


#endif //EFTPROFILER_NPRANKINGPLOTTER_H
