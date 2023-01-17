submit_one_worker()
{
  current_script=/sps/atlas/o/ollukian/scratch/script_nll_scan_1d_${POI}_${STAT_TYPE}_${PRE_POSTFIT}_${worker_id}.sh
  echo "#!/bin/sh" > "${current_script}"
  echo "####################################################################################################################" >> "${current_script}"
  echo "# script automatically created by run_remote version @ 15Jan23 / Lukianchuk Aleksei lukianchuk.aleksei@gmail.com  ##" >> "${current_script}"
  echo "# eftProfiler version 1.2.0 @ 15Jan23                                                                             ##" >> "${current_script}"
  echo "####################################################################################################################" >> "${current_script}"
  echo "# script is called from: ${PWD} with the following settings:"                                                         >> "${current_script}"
  #echo "# save path to:    ${EFT_SAVE_RES_TO}                                                                                " >> "${current_script}"
  echo "# name of the ws:  ${WS_NAME}"                                                                                        >> "${current_script}"
  echo "# path to the ws:  ${WS_PATH}"                                                                                        >> "${current_script}"
  echo "# name of the poi: ${POI}"                                                                                            >> "${current_script}"
  echo "# grid size:       ${EFT_GRID_SIZE}"                                                                                  >> "${current_script}"
  #echo "# poi init value:  ${POI_INIT_VAL}                                                                                  " >> "${current_script}"
  echo "# pdf name:        ${PDF_NAME}"                                                                                       >> "${current_script}"
  #echo "# errors handling: ${EFT_ERRORS_HANDLING}                                                                           " >> "${current_script}"
  echo "####################################################################################################################" >> "${current_script}"

  echo "cd ${EFT_PROFILER_PATH}" >> "${current_script}"
  echo "sh job_script.sh --task nll_scan --pois \"${POI}(val ${EFT_POI_VAL} ${EFT_POI_ERR} : grid ${EFT_GRID_SIZE} : range ${EFT_POI_RANGE})\" --ws_path ${WS_PATH} --comb_pdf ${PDF_NAME} --worker_id ${worker_id} ${OTHER_OPTIONS} ${EFT_STAT_TYPE} ${EFT_PRE_POST_FIT}" >> "${current_script}"

  echo "source ${current_script}"
  sbatch --mail-user lukianchuk@lal.in2p3.fr --mail-type=END,FAIL -L sps --mem 30G --export=ALL --job-name "nll_scan_1D_${POI}_${EFT_STAT_TYPE}_${EFT_PRE_POST_FIT}_${worker_id}" --output "/sps/atlas/o/ollukian/scratch/Log_nll_scan_${POI}_${EFT_STAT_TYPE}_${EFT_PRE_POST_FIT}_${worker_id}.OU" --error "/sps/atlas/o/ollukian/scratch/Log_nll_scan_${POI}_${EFT_STAT_TYPE}_${EFT_PRE_POST_FIT}_${worker_id}.ER" "${current_script}"

  sleep 1
}

#WS_NAME="WS-Comb-EFT-fitbasis_v3__rotated_my_side.root"
WS_NAME="CouplingWS_xs_6XS.root"
#POI="HZZ_01"
#POI="mu_ZH"
EFT_GRID_SIZE=10
EFT_POI_VAL=0
EFT_POI_ERR=0.1
EFT_POI_RANGE="0 2"
#POI_INIT_VAL=0
EFT_PROFILER_PATH="${PWD}"
WS_PATH=${EFT_PROFILER_PATH}/source/${WS_NAME}
#EFT_SAVE_RES_TO=${EFT_PROFILER_PATH}/res__${POI}
#PDF_NAME="combPdf"
PDF_NAME="CombinedPdf"
EFT_STAT_TYPE=""     # {--stat_only}
EFT_PRE_POST_FIT=""  # {--postfit --prefit}
#STRATEGY="1"
#OTHER_OPTIONS="--debug" # --no_gamma
#worker_id=9999

for EFT_STAT_TYPE in "--stat_only" ""
do
    for EFT_PRE_POST_FIT in "--postfit" ""
    do
	for POI in "mu_ggF" "mu_VBF" "mu_ZH" "mu_ttH" "mu_tH" #cHG ctGRe ctHRe
	do
	    for worker_id in {0..10}
	    do
		    submit_one_worker
	    done
	done    # pois
    done  # pre post fit: postfit or observed
done      # stat type: full stat_only
