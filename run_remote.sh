#!/bin/bash
submit_one_worker()
{
  current_script=/sps/atlas/o/ollukian/scratch/script_ranking_${worker_id}.sh
  echo "#!/bin/sh" > "${current_script}"
  echo "cd /pbs/home/o/ollukian/public/EFT/git/eftProfiler" >> "${current_script}"
  echo "sh job_script.sh --task compute_ranking --worker_id ${worker_id} --poi cG --ws_name combWS --model_config ModelConfig" >> "${current_script}"


  echo "source ${current_script}"
  sbatch --mail-user lukianchuk@lal.in2p3.fr --mail-type=END,FAIL -L sps --mem 16G --export=ALL --job-name j_np_${worker_id} --output /sps/atlas/o/ollukian/scratch/Log_np_${worker_id}.OU --error /sps/atlas/o/ollukian/scratch/Log_np_${worker_id}.ER ${current_script}

  #qsub -P P_atlas -m e -M lukianchuk@lal.in2p3.fr -l h_rss=4G -l h_cpu=8:00:00 -l vmem=8G,fsize=8G,sps=1 -N j_np_.${worker_id} -o /sps/atlas/o/ollukian/scratch/Log_np_${worker_id}.OU -e /sps/atlas/o/ollukian/scratch/Log_np_${worker_id}.ER ${current_script}
  sleep 20
}

for worker_id in {0..99}
do
  submit_one_worker
done