echo "[INFO] run script..."
echo "[INFO] run executable.."
singularity exec --bind /cvmfs -H "$PWD" /cvmfs/unpacked.cern.ch/gitlab-registry.cern.ch/atlas_higgs_combination/software/hcomb-docker/analyzer:2-2 sh build.sh $@
#singularity exec --bind /cvmfs -H "$PWD" /cvmfs/unpacked.cern.ch/gitlab-registry.cern.ch/atlas_higgs_combination/software/hcomb-docker/analyzer\:latest sh build.sh $@