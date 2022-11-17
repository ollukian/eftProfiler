echo "[INFO] run from singularity file: {$1} with args" ${@:2}

singularity exec --bind /cvmfs -H "$PWD" /cvmfs/unpacked.cern.ch/gitlab-registry.cern.ch/atlas_higgs_combination/software/hcomb-docker/analyzer:2-2 sh "$1" "${@:2}" ||