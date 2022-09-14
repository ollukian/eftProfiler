echo " * create build directory if it doesn't exist and cd there.. "
mkdir -p build && cd build || exit
echo " * Current path: $PWD "
echo " * cmake $PWD.."
cmake ..
echo " * make -j"
make -j
echo " * binary has been successfully built, execute it.."
eftProfiler