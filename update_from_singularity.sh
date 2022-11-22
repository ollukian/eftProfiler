echo "[INFO] update script: update from git & test"
echo "[INFO] check if update is required..."

changed=0
git remote update && git status -uno | grep -q 'Your branch is behind' && changed=1
if [ $changed = 1 ]; then
    echo "[INFO] update is required, update..."
    git pull
    echo "[INFO] build...";
    sh wrap_by_singularity.sh sh build_and_test.sh || echo "[ERROR] in build&test"; exit #1;
else
    echo "[INFO] update is NOT required"
fi