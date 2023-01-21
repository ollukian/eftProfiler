//
// Created by Aleksei Lukianchuk on 16-Jan-23.
// @ lukianchuk.aleksei@gmail.com
//
// * File to handle different versions of the NllScanResult file
// * Externally, namespace detail is exposed which points out on
//      the most recent version of the structure NllScanResult
// * In the main NllScanResult file one has
//      using detail::NllScanResult
// * Hence, just eft::stats::scans::NllScanResult will respresent
//      the most "recent" version of the file

#ifndef EFTPROFILER_NLLSCANRESULT_DETAIL_H
#define EFTPROFILER_NLLSCANRESULT_DETAIL_H

#include <string>
#include "PoiConfig.h"
#include "../NpRankingStudyRes.h"

// for printing of NllScanResults in logger
#include <spdlog/spdlog.h>
#include <spdlog/fmt/ostr.h>


namespace eft::stats::scans {

    //namespace detail_v1 {
        // v1
        struct NllScanResult_v1 {
            std::string             version         {"v1"};
            std::vector<PoiConfig>  poi_configs;
            double                  nll_val         {0.};
            int                     fit_status      {0};
        };
    //}

    //namespace detail_v2 {
        // v2
        struct NllScanResult_v2 {
            std::string             version     {"v2"};
            std::vector<PoiConfig>  poi_configs;
            double                  nll_val     {0.};
            int                     fit_status  {0};
            StatType                statType    {StatType::FULL};
            PrePostFit              prePostFit  {PrePostFit::OBSERVED};
            StudyType               studyType   {StudyType::OBSERVED};

            [[nodiscard]] inline std::string PrintAsString() const noexcept;
        };
    //}
    using NllScanResult = NllScanResult_v2;
    //namespace detail = detail_v2;

    // TODO: v3 => fit type (one-at-time / all float / user defined)


std::string NllScanResult::PrintAsString() const noexcept {
    EFT_PROFILE_FN();
    std::string res;
    if (poi_configs.size() == 1) {
        const auto& config = poi_configs[0];
        const auto& name = config.Name();
        const auto& value = config.Value();

        std::string stat_type_str {"full"};
        if (statType == StatType::STAT)
            stat_type_str = "stat";

        std::string prefit_str {"observed"};
        if (prePostFit == PrePostFit::PREFIT)
            prefit_str = "prefit";
        else if (prePostFit == PrePostFit::POSTFIT)
            prefit_str = "postfit";
        //                         name  val             nll      stat/full obs/exp
        res = fmt::format("1D {} at {:.4} with nll: {:.2} | {:4} {:8} | fit status: {} | version: {}",
                               name,
                               value,
                               nll_val,
                               stat_type_str,
                               prefit_str,
                               fit_status,
                               version);
    }
    else {
        EFT_PROF_CRITICAL("llScanResult::PrintAsString() only for 1D is supported");
        return fmt::format("{}D results are not supported", poi_configs.size());
    }
    return res;
}

template<typename OStream>
OStream& operator << (OStream& os, const NllScanResult& res) {
    EFT_PROFILE_FN();
    if (res.version != "v2")
        return os << fmt::format("print for res version {} is not supported", res.version);
    return os << res.PrintAsString();
}


} // namespace eft::stats::scans

#endif //EFTPROFILER_NLLSCANRESULT_DETAIL_H
