echo "[INFO] build from singularity"
sh wrap_by_singularity.sh sh build.sh "$*" || echo "[ERROR] building from singularity" exit
echo "[INFO] successfully built from singularity"