echo "[INFO] create build directory if it doesn't exist and cd there.. "
mkdir -p build && cd build || exit
echo "[INFO] Current path: $PWD "
echo "[INFO] cmake $PWD.."
cmake .. || echo "[ERROR] in cmake " exit
echo "[INFO] make -j"
make -j ||  echo "[ERROR] in make " exit
echo "[INFO] binary has been successfully built"
