echo "[INFO] create build directory if it doesn't exist and cd there.. "
mkdir -p build && cd build || echo "[ERROR] in cd build " exit
echo "[INFO] Current path: $PWD "
echo "[INFO] cmake $PWD.."
cmake "$*" .. || echo "[ERROR] in cmake " exit
echo "[INFO] make -j"
make -j ||  echo "[ERROR] in make " exit
exit_code=$?
if [ $exit_code -ne 0 ]; then
  echo "[ERROR] make ended with error code: ${exit_code}"; exit
fi
echo "[INFO] binary has been successfully built"
