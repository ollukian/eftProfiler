echo "[INFO] run script..."
echo "[INFO] build executable.."
singularity exec --bind /cvmfs -H "$PWD" /cvmfs/unpacked.cern.ch/gitlab-registry.cern.ch/atlas_higgs_combination/software/hcomb-docker/analyzer\:latest sh build.sh
echo "[INFO] run it"