#!/bin/bash
submit_one_worker()
{
  current_script=/sps/atlas/o/ollukian/scratch/script_ranking_${POI}_${worker_id}.sh
  echo "#!/bin/sh" > "${current_script}"
  echo "####################################################################################################################" >> "${current_script}"
  echo "# script automatically created by run_remote version @ 28Nov22 / Lukianchuk Aleksei lukianchuk.aleksei@gmail.com  ##" >> "${current_script}"
  echo "# eftProfiler version 1.0.0 @ 25Nov22                                                                              ##" >> "${current_script}"
  echo "####################################################################################################################" >> "${current_script}"
  echo "# script is called from: ${PWD} with the following settings:" >> "${current_script}"
  echo "# save path to:    ${EFT_SAVE_RES_TO}"                        >> "${current_script}"
  echo "# name of the ws:  ${WS_NAME}"                                >> "${current_script}"
  echo "# path to the ws:  ${WS_PATH}"                                >> "${current_script}"
  echo "# name of the poi: ${POI}"                                    >> "${current_script}"
  echo "# errors handling: ${EFT_ERRORS_HANDLING}"                    >> "${current_script}"
  echo "# worker id:       ${worker_id}"                              >> "${current_script}"
  echo "##################################################################################################################" >> "${current_script}"

  echo "cd ${EFT_PROFILER_PATH}" >> "${current_script}"
  echo "sh job_script.sh --task compute_ranking --errors ${EFT_ERRORS_HANDLING} --poi_init_val ${POI_INIT_VAL} --worker_id ${worker_id} --poi ${POI} --res_path ${EFT_SAVE_RES_TO} --no_gamma --ws_path ${WS_PATH} --comb_pdf ${PDF_NAME} --strategy ${STRATEGY}" >> "${current_script}"

  echo "source ${current_script}"
  sbatch --mail-user lukianchuk@lal.in2p3.fr --mail-type=END,FAIL -L sps --mem 20G --export=ALL --job-name "${worker_id}${POI}" --output "/sps/atlas/o/ollukian/scratch/Log_${POI}_${worker_id}.OU" --error "/sps/atlas/o/ollukian/scratch/Log_${POI}_${worker_id}.ER" "${current_script}"

  sleep 1
}

WS_NAME="WS-Comb-Higgs_topU3l_obs.root"
POI=cHG

POI_INIT_VAL=0
EFT_PROFILER_PATH="${PWD}"
WS_PATH=${EFT_PROFILER_PATH}/source/${WS_NAME}
EFT_SAVE_RES_TO=${EFT_PROFILER_PATH}/res__${POI}
EFT_ERRORS_HANDLING="Hesse"
PDF_NAME="combPdf"
STRATEGY="1"
for worker_id in {0..20}
do
  submit_one_worker
done