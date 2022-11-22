echo "[INFO] update script: update from git & test"
echo "[INFO] update..."
#git pull  && echo "updated then what?"|| echo "[ERROR] in git pull"; exit;

sh wrap_by_singularity.sh sh update.sh || echo "[ERROR] updating"; exit #1;
#exit 0;

#echo "[INFO] build..."
#sh wrap_by_singularity.sh sh build.sh || echo "[ERROR] in building"; exit;
#echo "[INFO] test..."
#sh wrap_by_singularity.sh sh run.sh --test || echo "[ERROR] running tests"; exit;
#echo "[INFO] successfully updated"
