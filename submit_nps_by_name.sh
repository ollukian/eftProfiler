#!/bin/bash
submit_one_worker()
{
  current_script=/sps/atlas/o/ollukian/scratch/script_ranking_${POI}_${NP_NAME}.sh
  echo "#!/bin/sh" > "${current_script}"
  echo "####################################################################################################################" >> "${current_script}"
  echo "# script automatically created by run_remote version @ 16Dec22 / Lukianchuk Aleksei lukianchuk.aleksei@gmail.com  ##" >> "${current_script}"
  echo "# eftProfiler version 1.1.0 @ 16Dec22                                                                             ##" >> "${current_script}"
  echo "####################################################################################################################" >> "${current_script}"
  echo "# script is called from: ${PWD} with the following settings:                                                        " >> "${current_script}"
  echo "# save path to:    ${EFT_SAVE_RES_TO}                                                                               " >> "${current_script}"
  echo "# name of the ws:  ${WS_NAME}                                                                                       " >> "${current_script}"
  echo "# path to the ws:  ${WS_PATH}                                                                                       " >> "${current_script}"
  echo "# name of the poi: ${POI}                                                                                           " >> "${current_script}"
  echo "# poi init value:  ${POI_INIT_VAL}                                                                                  " >> "${current_script}"
  echo "# pdf name:        ${PDF_NAME}                                                                                      " >> "${current_script}"
  echo "# errors handling: ${EFT_ERRORS_HANDLING}                                                                           " >> "${current_script}"
  echo "# np name:         ${NP_NAME}                                                                                       " >> "${current_script}"
  echo "####################################################################################################################" >> "${current_script}"

  echo "cd ${EFT_PROFILER_PATH}" >> "${current_script}"
  echo "sh job_script.sh --task compute_ranking --np_name ${NP_NAME} --poi_init_val ${POI_INIT_VAL} --worker_id ${worker_id} --poi ${POI} --res_path ${EFT_SAVE_RES_TO} --ws_path ${WS_PATH} --comb_pdf ${PDF_NAME} --strategy ${STRATEGY} --errors ${EFT_ERRORS_HANDLING} ${OTHER_OPTIONS}" >> "${current_script}"

  echo "source ${current_script}"
  sbatch --mail-user lukianchuk@lal.in2p3.fr --mail-type=END,FAIL -L sps --mem 20G --export=ALL --job-name "${POI}_${NP_NAME}" --output "/sps/atlas/o/ollukian/scratch/Log_${POI}_${NP_NAME}.OU" --error "/sps/atlas/o/ollukian/scratch/Log_${POI}_${NP_NAME}.ER" "${current_script}"

  sleep 1
}

WS_NAME="WS-Comb-EFT-fitbasis_v3__rotated_my_side.root"
POI="HZZ_01"

POI_INIT_VAL=0
EFT_PROFILER_PATH="${PWD}"
WS_PATH=${EFT_PROFILER_PATH}/source/${WS_NAME}
EFT_SAVE_RES_TO=${EFT_PROFILER_PATH}/res__${POI}
EFT_ERRORS_HANDLING="Hesse"
PDF_NAME="combPdf"
STRATEGY="1"
OTHER_OPTIONS="--debug" # --no_gamma
worker_id=9999

for NP_NAME in # names TheorySig_QCDscale_ggF_mu
do
  submit_one_worker
done
