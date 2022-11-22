echo "[INFO] update script: build & test"
#echo "[INFO] update..."
#git pull || echo "[ERROR] in git pull"; exit;

echo "[INFO] build..."
sh build.sh || echo "[ERROR] in building" exit #1;
echo "[INFO] test..."
sh run.sh --test || echo "[ERROR] running tests" exit #1;
echo "[INFO] successfully updated"
exit #0;