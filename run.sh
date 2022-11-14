echo "[INFO] run code with arguments: {$@} "
cd build  || echo "[ERROR] cannot cd to the build directory, we're at: {$PWD}" exit
./eftProfiler $@
echo "clean up: delete dump of the linux kernel (core.* files)"
# cd build || echo "[ERROR] cannot cd to the build directory, we're at: {$PWD}" exit
rm core.*
cd ..
echo "[INFO] run has successfully finished {$@} "