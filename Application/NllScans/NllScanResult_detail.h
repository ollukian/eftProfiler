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


namespace eft::stats::scans {

    namespace detail_v1 {
        // v1
        struct NllScanResult {
            std::string             version         {"v1"};
            std::vector<PoiConfig>  poi_configs;
            double                  nll_val         {0.};
            int                     fit_status      {0};
        };
    }

    namespace detail_v2 {
        // v2
        struct NllScanResult {
            std::string             version     {"v2"};
            std::vector<PoiConfig>  poi_configs;
            double                  nll_val     {0.};
            int                     fit_status  {0};
            StatType                statType    {StatType::FULL};
            PrePostFit              prePostFit  {PrePostFit::OBSERVED};
            StudyType               studyType   {StudyType::OBSERVED};
        };
    }

    namespace detail = detail_v2;


}

#endif //EFTPROFILER_NLLSCANRESULT_DETAIL_H
