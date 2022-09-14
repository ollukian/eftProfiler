//
// Created by Aleksei Lukianchuk on 12-Sep-22.
//

#ifndef EFTPROFILER_TOREFACTOR_MUTYPES_H
#define EFTPROFILER_TOREFACTOR_MUTYPES_H

#pragma once

#include "ToRefactor_Catgories.h"

#include <string>
#include <vector>
#include <map>
#include <utility>

namespace stats {

    enum class POIs {
        GLOBAL = 1,
        PROD_MODE_4_POI = 4,
        PROD_MODE_5_POI = 5,
        PROD_MODE_6_POI = 6,
        STXS = 27,
        CATEGORIES = 88,
        GAMGAM = 0,
    };

    std::ostream& operator<< (std::ostream& os, const POIs poi) {
        os << static_cast<int>(poi);
        return os;
    }

    /*operator string(const POIs poi) {

      }*/

    std::map<POIs, std::vector<std::string> > mapPOIsGivenScheme = {
            {POIs::GLOBAL,          {"mu"} },
            {POIs::GAMGAM,          {"mu_yy"} },
            {POIs::PROD_MODE_4_POI, {"mu_ggF", "mu_VBF", "mu_VH", "mu_top"}},
            {POIs::PROD_MODE_5_POI, {"mu_ggF", "mu_VBF", "mu_WH", "mu_ZH", "mu_top"}},
            {POIs::PROD_MODE_6_POI, {"mu_ggF", "mu_VBF", "mu_WH", "mu_ZH", "mu_ttH", "mu_tH"}},
            {POIs::STXS,            {"mu_gg2H_0J_ptH_0_10",
                                               "mu_gg2H_0J_ptH_gt10",
                                                         "mu_gg2H_1J_ptH_0_60",
                                                                  "mu_gg2H_1J_ptH_60_120",
                                                                           "mu_gg2H_1J_ptH_120_200",
                                                                                     "mu_gg2H_ge2J_mJJ_0_350_ptH_0_60",
                                            "mu_gg2H_ge2J_mJJ_0_350_ptH_60_120",
                                            "mu_gg2H_ge2J_mJJ_0_350_ptH_120_200",
                                            "mu_gg2H_ge2J_mJJ_350_700_ptH_0_200",
                                            "mu_gg2H_ge2J_mJJ_700_1000_ptH_0_200",
                                            "mu_gg2H_ge2J_mJJ_gt1000_ptH_0_200",
                                            "mu_gg2H_ptH_200_300",
                                            "mu_gg2H_ptH_300_450",
                                            "mu_gg2H_ptH_gt450",
                                            "mu_qq2Hqq_le1J",
                                            "mu_qq2Hqq_VHhad",
                                            "mu_qq2Hqq_VHveto",
                                            "mu_qq2Hqq_ge2J_mJJ_350_700_ptH_0_200",
                                            "mu_qq2Hqq_ge2J_mJJ_700_1000_ptH_0_200",
                                            "mu_qq2Hqq_ge2J_mJJ_gt1000_ptH_0_200",
                                            "mu_qq2Hqq_ge2J_mJJ_350_700_ptH_gt200",
                                            "mu_qq2Hqq_ge2J_mJJ_700_1000_ptH_gt200",
                                            "mu_qq2Hqq_ge2J_mJJ_gt1000_ptH_gt200",
                                            "mu_qq2Hlnu_ptV_0_150",
                                            "mu_qq2Hlnu_ptV_gt150",
                                            "mu_Hll_ptV_0_150",
                                            "mu_Hll_ptV_gt150",
                                            "mu_ttH_ptH_0_60",
                                            "mu_ttH_ptH_60_120",
                                            "mu_ttH_ptH_120_200",
                                            "mu_ttH_ptH_200_300",
                                            "mu_ttH_ptH_gt300",
                                            "mu_tH"}},
            {POIs::CATEGORIES,      {"mu_GG2H_0J_PTH_0_10__0",
                                            //"mu_GG2H_0J_PTH_0_10__1",
                                               "mu_GG2H_0J_PTH_GT10__0",
                                                         "mu_GG2H_1J_PTH_0_60__0",
                                                                  "mu_GG2H_1J_PTH_60_120__0",
                                                                           "mu_GG2H_1J_PTH_120_200__0",
                                                                                     "mu_GG2H_1J_PTH_120_200__1",
                                            "mu_GG2H_GE2J_MJJ_0_350_PTH_0_60__0",
                                            "mu_GG2H_GE2J_MJJ_0_350_PTH_0_60__1",
                                            "mu_GG2H_GE2J_MJJ_0_350_PTH_0_60__2",
                                            "mu_GG2H_GE2J_MJJ_0_350_PTH_60_120__0",
                                            "mu_GG2H_GE2J_MJJ_0_350_PTH_60_120__1",
                                            "mu_GG2H_GE2J_MJJ_0_350_PTH_60_120__2",
                                            "mu_GG2H_GE2J_MJJ_0_350_PTH_120_200__0",
                                            "mu_GG2H_GE2J_MJJ_0_350_PTH_120_200__1",
                                            "mu_GG2H_GE2J_MJJ_350_700_PTH_0_200_PTHJJ_0_25__0",
                                            "mu_GG2H_GE2J_MJJ_350_700_PTH_0_200_PTHJJ_0_25__1",
                                            "mu_GG2H_GE2J_MJJ_350_700_PTH_0_200_PTHJJ_0_25__2",
                                            "mu_GG2H_GE2J_MJJ_350_700_PTH_0_200_PTHJJ_GT25__0",
                                            "mu_GG2H_GE2J_MJJ_350_700_PTH_0_200_PTHJJ_GT25__1",
                                            "mu_GG2H_GE2J_MJJ_350_700_PTH_0_200_PTHJJ_GT25__2",
                                            "mu_GG2H_GE2J_MJJ_GT700_PTH_0_200_PTHJJ_0_25__0",
                                            "mu_GG2H_GE2J_MJJ_GT700_PTH_0_200_PTHJJ_0_25__1",
                                            "mu_GG2H_GE2J_MJJ_GT700_PTH_0_200_PTHJJ_0_25__2",
                                            "mu_GG2H_GE2J_MJJ_GT700_PTH_0_200_PTHJJ_GT25__0",
                                            "mu_GG2H_GE2J_MJJ_GT700_PTH_0_200_PTHJJ_GT25__1",
                                            "mu_GG2H_GE2J_MJJ_GT700_PTH_0_200_PTHJJ_GT25__2",
                                            "mu_GG2H_PTH_200_300__0",
                                            "mu_GG2H_PTH_200_300__1",
                                            "mu_GG2H_PTH_200_300__2",
                                            "mu_GG2H_PTH_300_450__0",
                                            "mu_GG2H_PTH_300_450__1",
                                            "mu_GG2H_PTH_300_450__2",
                                            "mu_GG2H_PTH_450_650__0",
                                            "mu_GG2H_PTH_450_650__1",
                                            "mu_GG2H_PTH_GT650__0",
                                            "mu_GG2H_PTH_GT650__1",
                                            "mu_QQ2HQQ_0J__0",
                                            "mu_QQ2HQQ_0J__1",
                                            "mu_QQ2HQQ_1J__0",
                                            "mu_QQ2HQQ_1J__1",
                                            "mu_QQ2HQQ_1J__2",
                                            "mu_QQ2HQQ_GE2J_MJJ_0_60__0",
                                            "mu_QQ2HQQ_GE2J_MJJ_0_60__1",
                                            "mu_QQ2HQQ_GE2J_MJJ_0_60__2",
                                            "mu_QQ2HQQ_GE2J_MJJ_60_120__0",
                                            "mu_QQ2HQQ_GE2J_MJJ_60_120__1",
                                            "mu_QQ2HQQ_GE2J_MJJ_120_350__0",
                                            "mu_QQ2HQQ_GE2J_MJJ_120_350__1",
                                            "mu_QQ2HQQ_GE2J_MJJ_120_350__2",
                                            "mu_QQ2HQQ_GE2J_MJJ_350_700_PTH_0_200_PTHJJ_0_25__0",
                                            "mu_QQ2HQQ_GE2J_MJJ_350_700_PTH_0_200_PTHJJ_0_25__1",
                                            "mu_QQ2HQQ_GE2J_MJJ_350_700_PTH_0_200_PTHJJ_GT25__0",
                                            "mu_QQ2HQQ_GE2J_MJJ_350_700_PTH_0_200_PTHJJ_GT25__1",
                                            "mu_QQ2HQQ_GE2J_MJJ_GT700_PTH_0_200_PTHJJ_0_25__0",
                                            "mu_QQ2HQQ_GE2J_MJJ_GT700_PTH_0_200_PTHJJ_0_25__1",
                                            "mu_QQ2HQQ_GE2J_MJJ_GT700_PTH_0_200_PTHJJ_GT25__0",
                                            "mu_QQ2HQQ_GE2J_MJJ_GT700_PTH_0_200_PTHJJ_GT25__1",
                                            "mu_QQ2HQQ_GE2J_MJJ_GT700_PTH_0_200_PTHJJ_GT25__2",
                                            "mu_QQ2HQQ_GE2J_MJJ_350_700_PTH_GT200__0",
                                            "mu_QQ2HQQ_GE2J_MJJ_350_700_PTH_GT200__1",
                                            "mu_QQ2HQQ_GE2J_MJJ_GT700_PTH_GT200__0",
                                            "mu_QQ2HQQ_GE2J_MJJ_GT700_PTH_GT200__1",
                                            "mu_UNSELECTED_WH",
                                            "mu_QQ2HLNU_PTV_0_75__0",
                                            "mu_QQ2HLNU_PTV_0_75__1",
                                            "mu_QQ2HLNU_PTV_75_150__0",
                                            "mu_QQ2HLNU_PTV_75_150__1",
                                            "mu_QQ2HLNU_PTV_150_250_0J__0",
                                            "mu_QQ2HLNU_PTV_150_250_GE1J__0",
                                            "mu_QQ2HLNU_PTV_GT250__0",
                                            "mu_UNSELECTED_ZH",
                                            "mu_HLL_PTV_0_75__0",
                                            "mu_HLL_PTV_75_150__0",
                                            "mu_HLL_PTV_75_150__1",
                                            "mu_HLL_PTV_150_250_0J__0",
                                            "mu_HLL_PTV_150_250_GE1J__0",
                                            "mu_HLL_PTV_GT250__0",
                                            "mu_UNSELECTED_TOP",
                                            "mu_TTH_PTH_0_60__0",
                                            "mu_TTH_PTH_0_60__1",
                                            "mu_TTH_PTH_60_120__0",
                                            "mu_TTH_PTH_60_120__1",
                                            "mu_TTH_PTH_120_200__0",
                                            "mu_TTH_PTH_120_200__1",
                                            "mu_TTH_PTH_200_300__0",
                                            "mu_TTH_PTH_GT300__0",
                                            "mu_THJB__0",
                                            "mu_TWH__0"}}//listCategories}
    };

    struct ValueError {
        double value;
        double error_stat;
        double error_syst;
        double error_total;

        ValueError() {
            value = 0;
            error_stat  = 0;
            error_syst  = 0;
            error_total = 0;
        }

        inline void ComputeSystError() {
            if (error_total != 0 and error_stat != 0 && error_syst == 0)
                error_syst = sqrt( error_total * error_total - error_stat * error_stat );
        };

        inline void ComputeTotalError() {
            //std::cout << " error_total = " << error_total << std::endl;
            //std::cout << " error_syst  = " << error_syst  << std::endl;
            //std::cout << " error_stat  = " << error_stat  << std::endl;
            if (error_total == 0 and (error_stat != 0 or error_syst !=0)) {
                error_total = sqrt(error_stat * error_stat + error_syst * error_syst);
                //cout << "inside" << endl;
            }
        }

        inline void FinaliseComputation() {
            ComputeSystError();
            ComputeTotalError();
        }


        /*inline void ComputeStatError() {
          if (error_total != 0 and error_syst != 0 and error_stat == 0)
            error_stat = sqrt( error_total * error_total - error_syst * syst );
            };*/



        //inline void Update

    };

    /*struct Uncertainty {
      ValueError stat;
      ValueError sys;
      ValueError total;

      void Compute
      };*/

    struct MuResult {
        /*Uncertainty exp;
          Uncertainty obs;*/

        ValueError exp;
        ValueError obs;

        /*ValueError exp_stat;
        ValueError obs_stat;

        ValueError exp_total;
        ValueError obs_total;

        ValueError exp_sys;
        ValueError obs_sys;*/
    };

    enum class ResultType {
        EXPECTED,
        OBSERVED
    };

    enum class ErrorType {
        STAT,
        SYS,
        TOTAL
    };


    struct MuSensitivityConfig {
        //double mu1;
        //double mu2;
        std::pair<std::string, double> mu1;
        std::pair<std::string, double> mu2;

        RooAbsPdf*  pdf;
        RooAbsData* dataset;
    };

    struct MuConfig {
        std::string name;
        double      value;
        double      error;
        double      min;
        double      max;
        size_t      bins;

        MuConfig(std::string name)
                : name(name)
        {
        }

        MuConfig(std::string name, double val)
                : name(name)
                , value(val)
        {
        }
    };


} //namespace stats


#endif //EFTPROFILER_TOREFACTOR_MUTYPES_H
