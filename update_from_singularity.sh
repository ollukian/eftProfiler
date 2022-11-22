echo "[INFO] update script: update from git & test"
echo "[INFO] update..."
git pull || echo "[ERROR] in git pull" exit;

sh wrap_by_singularity.sh sh update.sh || echo "[ERROR] updating"; exit #1;

