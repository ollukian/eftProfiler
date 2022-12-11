#!/bin/bash
submit_one_worker()
{
  current_script=/sps/atlas/o/ollukian/scratch/script_suggest_ranking_${POI}.sh
  echo "#!/bin/sh" > "${current_script}"
  echo "####################################################################################################################" >> "${current_script}"
  echo "# script automatically created by run_remote version @ 11Dec22 / Lukianchuk Aleksei lukianchuk.aleksei@gmail.com  ##" >> "${current_script}"
  echo "# eftProfiler version 1.0.7 @ 11Dec22                                                                             ##" >> "${current_script}"
  echo "####################################################################################################################" >> "${current_script}"
  echo "# Script will make a prediction on the most important nps to start ranking from. It is based on the computing of  ##" >> "${current_script}"
  echo "# the correlation coefficients between the POI and all nps (via Hessian) in a free fit.                           ##" >> "${current_script}"
  echo "####################################################################################################################" >> "${current_script}"
  echo "# script is called from: ${PWD} with the following settings:                                                      ##" >> "${current_script}"
  echo "# save path to:    ${EFT_SAVE_RES_TO}                                                                             ##" >> "${current_script}"
  echo "# name of the ws:  ${WS_NAME}                                                                                     ##" >> "${current_script}"
  echo "# path to the ws:  ${WS_PATH}                                                                                     ##" >> "${current_script}"
  echo "# name of the poi: ${POI}                                                                                         ##" >> "${current_script}"
  echo "# poi init value:  ${POI_INIT_VAL}                                                                                ##" >> "${current_script}"
  echo "# pdf name:        ${PDF_NAME}                                                                                    ##" >> "${current_script}"
  echo "# errors handling: ${EFT_ERRORS_HANDLING}                                                                         ##" >> "${current_script}"
  echo "####################################################################################################################" >> "${current_script}"

  echo "cd ${EFT_PROFILER_PATH}" >> "${current_script}"

  echo "sh job_script.sh --task compute_hesse_nps --poi ${POI} --errors ${EFT_ERRORS_HANDLING} --poi_init_val ${POI_INIT_VAL} --ws_path ${WS_PATH} --comb_pdf ${PDF_NAME} --strategy ${STRATEGY} ${OTHER_OPTIONS}"  >> "${current_script}"

  echo "source ${current_script}"
  sbatch --mail-user lukianchuk@lal.in2p3.fr --mail-type=END,FAIL -L sps --mem 20G --export=ALL --job-name "sug_${POI}" --output "/sps/atlas/o/ollukian/scratch/Log_${POI}_suggest_ranking.OU" --error "/sps/atlas/o/ollukian/scratch/Log_${POI}_suggest_ranking.ER" "${current_script}"

  sleep 1
}


WS_NAME="WS-Comb-EFT-fitbasis_v3__rotated_my_side.root"
#WS_NAME="CouplingWS_xs_6XS.root"

POI_INIT_VAL=0
#POI_INIT_VAL=1
EFT_PROFILER_PATH="${PWD}"
WS_PATH=${EFT_PROFILER_PATH}/source/${WS_NAME}
EFT_SAVE_RES_TO=${EFT_PROFILER_PATH}/res__${POI}
EFT_ERRORS_HANDLING="Hesse"
PDF_NAME="combPdf"
#PDF_NAME="CombinedPdf" # for the old Hyy workspspace
STRATEGY="1"
OTHER_OPTIONS="--debug"

for POI in ceHRe33 cuHRe top_01 Hyy_01 HZZ_01 cZH_01 fermi_01
do
    submit_one_worker
done
