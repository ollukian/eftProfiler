echo "[INFO] build from singularity"
sh run_from_singularity build.sh || echo "[ERROR] building from singularity"; exit

echo "[INFO] successfully built from singularity"