echo "[INFO] run from singularity with args" ${@}

sh wrap_by_singularity.sh sh run.sh $@ || echo "[ERROR] running form singularity"; exit;
echo "[INFO] successfully ran from singularity with args: " ${@}