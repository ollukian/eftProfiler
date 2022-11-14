#!/bin/bash
submit_one_worker()
{
  current_script=/sps/atlas/o/ollukian/scratch/script_ranking_${poi}_${worker_id}.sh
  echo "#!/bin/sh" > "${current_script}"
  echo "# script automatically created by run_remote version @ 14Nov22 / Lukianchuk Aleksei lukianchuk.aleksei@gmail.com" >> "${current_script}"
  echo "# POI: ${poi}, worker_id: ${worker_id}" >> "${current_script}"
  echo "cd /pbs/home/o/ollukian/public/EFT/git/eftProfiler" >> "${current_script}"
  echo "sh job_script.sh --task compute_ranking --errors Hesse --poi_init_val 0 --worker_id ${worker_id} --poi ${poi} --res_path /pbs/home/o/ollukian/public/EFT/git/eftProfiler/res_${poi}/ --no_gamma" >> "${current_script}"

  echo "source ${current_script}"
  sbatch --mail-user lukianchuk@lal.in2p3.fr --mail-type=END,FAIL -L sps --mem 20G --export=ALL --job-name "${poi}_${worker_id}" --output "/sps/atlas/o/ollukian/scratch/Log_${poi}_${worker_id}.OU" --error "/sps/atlas/o/ollukian/scratch/Log_${poi}_${worker_id}.ER" "${current_script}"

  sleep 10
}


poi=eHRe33
for worker_id in {0..20}
do
  submit_one_worker
done