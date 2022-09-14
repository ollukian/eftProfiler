//
// Created by Aleksei Lukianchuk on 12-Sep-22.
//

#ifndef EFTPROFILER_TOREFACTOR_CATGORIES_H
#define EFTPROFILER_TOREFACTOR_CATGORIES_H

#pragma once

#include <string>
#include <vector>

/*const std::vector<std::string> listCategories = {
  "mu_GG2H_0J_PTH_0_10__0",
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
  "mu_TWH__0"
  };*/

/*const std::vector<std::string> listCategories = {

  };*/


/*const std::vector<std::string> listSTXSnames = {
  "mu_tHqb"
  "mu_tHW"
  "mu_gg2H"
  "mu_qq2Hqq"
  "mu_qq2Hlnu"
  "mu_qq2Hll"
  "mu_gg2Hll"
  "mu_Hll"
  "mu_ttH"
  "mu_bbH"
  "mu_tH"
  "mu_ggF_gg2H_0J_ptH_gt10"
  "mu_ggF_gg2H_1J_ptH_0_60"
  "mu_ggF_gg2H_ge2J_mJJ_0_350_ptH_0_60"
  "mu_ggF_gg2H_ge2J_mJJ_350_700_ptH_0_200_ptHJJ_0_25",
  "mu_ggF_gg2H_ge2J_mJJ_350_700_ptH_0_200_ptHJJ_gt25",
  "mu_ggF_gg2H_ge2J_mJJ_gt700_ptH_0_200_ptHJJ_0_25",
  "mu_ggF_gg2H_ge2J_mJJ_gt700_ptH_0_200_ptHJJ_gt25",
  "mu_VBF_qq2Hqq_0J"
  "mu_VBF_qq2Hqq_1J"
  "mu_VBF_qq2Hqq_ge2J_mJJ_60_120"
  "mu_VBF_qq2Hqq_ge2J_mJJ_120_350"
  "mu_VBF_qq2Hqq_ge2J_mJJ_350_700_ptH_0_200_ptHJJ_0_25",
  "mu_VBF_qq2Hqq_ge2J_mJJ_350_700_ptH_0_200_ptHJJ_gt25",
  "mu_VBF_qq2Hqq_ge2J_mJJ_gt700_ptH_0_200_ptHJJ_0_25",
  "mu_VBF_qq2Hqq_ge2J_mJJ_gt700_ptH_0_200_ptHJJ_gt25",
  "mu_WH_qq2Hqq_0J"
  "mu_WH_qq2Hqq_1J"
  "mu_WH_qq2Hqq_ge2J_mJJ_0_60"
  "mu_WH_qq2Hqq_ge2J_mJJ_60_120"
  "mu_WH_qq2Hqq_ge2J_mJJ_120_350"
  "mu_qq2Hlnu_ptV_0_75_0J"
  "mu_qq2Hlnu_ptV_0_75_1J"
  "mu_qq2Hlnu_ptV_75_150_1J"
  "mu_ZH_qq2Hqq_0J"
  "mu_ZH_qq2Hqq_1J"
  "mu_ZH_qq2Hqq_ge2J_mJJ_60_120"
  "mu_ZH_qq2Hqq_ge2J_mJJ_120_350"
  "mu_ZH_qq2Hqq_ge2J_mJJ_350_700_ptH_0_200_ptHJJ_gt25",
  "mu_qq2Hll_ptV_0_75_0J"
  "mu_qq2Hll_ptV_0_75_1J"
  "mu_qq2Hll_ptV_75_150_1J"
  "mu_qq2Hll_ptV_150_250_1J"
  "mu_qq2Hll_ptV_0_75_ge2J"
  "mu_ggZH_gg2H_1J_ptH_0_60"
  "mu_ggZH_gg2H_ge2J_mJJ_0_350_ptH_0_60"
  "mu_ggZH_gg2H_ge2J_mJJ_0_350_ptH_120_200"
  "mu_gg2Hll_ptV_0_75_0J"
  "mu_gg2Hll_ptV_0_75_1J"
  "mu_ttH_ptH_0_60"
  "mu_tHqb_tH"
  "mu_ggF_gg2H_1J_ptH_60_120"
  "mu_ggF_gg2H_ge2J_mJJ_0_350_ptH_60_120"
  "mu_VBF_qq2Hqq_ge2J_mJJ_0_60"
  "mu_WH_qq2Hqq_ge2J_mJJ_350_700_ptH_0_200_ptHJJ_gt25",
  "mu_qq2Hlnu_ptV_75_150_0J"
  "mu_qq2Hlnu_ptV_0_75_ge2J"
  "mu_qq2Hlnu_ptV_75_150_ge2J"
  "mu_qq2Hlnu_ptV_150_250_ge2J"
  "mu_ZH_qq2Hqq_ge2J_mJJ_0_60"
  "mu_ZH_qq2Hqq_ge2J_mJJ_350_700_ptH_0_200_ptHJJ_0_25"
  "mu_qq2Hll_ptV_75_150_0J"
  "mu_qq2Hll_ptV_75_150_ge2J"
  "mu_qq2Hll_ptV_150_250_ge2J"
  "mu_ggZH_gg2H_0J_ptH_gt10"
  "mu_gg2Hll_ptV_75_150_0J"
  "mu_gg2Hll_ptV_75_150_1J"
  "mu_ttH_ptH_60_120"
  "mu_WH_qq2Hqq_ge2J_mJJ_350_700_ptH_0_200_ptHJJ_0_25"
  "mu_ZH_qq2Hqq_ge2J_mJJ_gt700_ptH_0_200_ptHJJ_0_25"
  "mu_ZH_qq2Hqq_ge2J_mJJ_gt700_ptH_0_200_ptHJJ_gt25"
  "mu_ggZH_gg2H_1J_ptH_60_120"
  "mu_ggZH_gg2H_ge2J_mJJ_350_700_ptH_0_200_ptHJJ_0_25"
  "mu_ggZH_gg2H_ge2J_mJJ_350_700_ptH_0_200_ptHJJ_gt25"
  "mu_ggF_gg2H_1J_ptH_120_200"
  "mu_ggF_gg2H_ge2J_mJJ_0_350_ptH_120_200"
  "mu_WH_qq2Hqq_ge2J_mJJ_gt700_ptH_0_200_ptHJJ_0_25"
  "mu_WH_qq2Hqq_ge2J_mJJ_gt700_ptH_0_200_ptHJJ_gt25"
  "mu_qq2Hlnu_ptV_150_250_1J"
  "mu_ggZH_gg2H_ge2J_mJJ_0_350_ptH_60_120"
  "mu_ggZH_gg2H_ge2J_mJJ_gt700_ptH_0_200_ptHJJ_0_25"
  "mu_ggZH_gg2H_ge2J_mJJ_gt700_ptH_0_200_ptHJJ_gt25"
  "mu_gg2Hll_ptV_0_75_ge2J"
  "mu_gg2Hll_ptV_150_250_ge2J"
  "mu_ttH_ptH_120_200"
  "mu_ggF_gg2H_ptH_200_300"
  "mu_qq2Hlnu_ptV_150_250_0J"
  "mu_qq2Hll_ptV_150_250_0J"
  "mu_ggZH_gg2H_1J_ptH_120_200"
  "mu_ggZH_gg2H_ptH_200_300"
  "mu_gg2Hll_ptV_150_250_1J"
  "mu_VBF_qq2Hqq_ge2J_mJJ_350_700_ptH_gt200"
  "mu_VBF_qq2Hqq_ge2J_mJJ_gt700_ptH_gt200"
  "mu_WH_qq2Hqq_ge2J_mJJ_gt700_ptH_gt200"
  "mu_qq2Hlnu_ptV_250_400_0J"
  "mu_gg2Hll_ptV_150_250_0J"
  "mu_gg2Hll_ptV_75_150_ge2J"
  "mu_ttH_ptH_200_300"
  "mu_qq2Hlnu_ptV_250_400_1J"
  "mu_gg2Hll_ptV_250_400_ge2J"
  "mu_ttH_ptH_300_450"
  "mu_qq2Hlnu_ptV_250_400_ge2J"
  "mu_qq2Hll_ptV_250_400_1J"
  "mu_gg2Hll_ptV_250_400_1J"

  "mu_ZH_qq2Hqq_ge2J_mJJ_350_700_ptH_gt200"

  "mu_WH_qq2Hqq_ge2J_mJJ_350_700_ptH_gt200"


  "mu_qq2Hll_ptV_250_400_ge2J"

  "mu_gg2Hll_ptV_gt400_1J"
  "mu_ttH_ptH_gt450"

  "mu_ggF_gg2H_ptH_300_450"
  "mu_qq2Hll_ptV_250_400_0J"

  "mu_ZH_qq2Hqq_ge2J_mJJ_gt700_ptH_gt200"
  "mu_ggZH_gg2H_ptH_300_450"

  "mu_ggF_gg2H_ptH_450_650"
  "mu_qq2Hll_ptV_gt400_ge2J"

  "mu_qq2Hlnu_ptV_gt400_0J"
  "mu_qq2Hlnu_ptV_gt400_1J"
  "mu_qq2Hll_ptV_gt400_0J"
  "mu_qq2Hll_ptV_gt400_1J"
  "mu_ggZH_gg2H_ptH_450_650"
  "mu_gg2Hll_ptV_250_400_0J"

  "mu_ggF_gg2H_ptH_gt650"

  "mu_ggZH_gg2H_ptH_gt650"

  "mu_qq2Hlnu_ptV_gt400_ge2J"
  "mu_gg2Hll_ptV_gt400_ge2J"
      "mu_gg2Hll_ptV_gt400_0J"
      }*/


#endif //EFTPROFILER_TOREFACTOR_CATGORIES_H
