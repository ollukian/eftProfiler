echo "[INFO] run script..."
echo "[INFO] run executable.."

# to shield atlasLocalSetup.sh from the arguments passed to a job
#args=$@
sh run_from_singularity.sh run.sh $@ || echo "[ERROR] running job script"; exit

echo "[INFO] successfully finished running job"
#cd /pbs/home/o/ollukian/ || echo "[ERROR] cannot cd to /pbs/home/o/ollukian/ - quit"; exit
#source ./setupATLAS && asetup StatAnalysis,0.0.4
#cd -

#sh run.sh "$args" || echo "[ERROR] running code with arguments: ${args} quit"; exit

#singularity exec --bind /cvmfs -H "$PWD" /cvmfs/unpacked.cern.ch/gitlab-registry.cern.ch/atlas_higgs_combination/software/hcomb-docker/analyzer:2-2 sh run.sh $@
###singularity exec --bind /cvmfs -H "$PWD" /cvmfs/unpacked.cern.ch/gitlab-registry.cern.ch/atlas_higgs_combination/software/hcomb-docker/analyzer\:latest sh run.sh $@
#singularity exec --bind /cvmfs -H "$PWD" /cvmfs/unpacked.cern.ch/gitlab-registry.cern.ch/atlas_higgs_combination/software/hcomb-docker/analyzer\:paper2021-6 sh build.sh $@