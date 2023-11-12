export VCPKG_HOME=~/workspace/cpp/vcpkg
mkdir -p build
cd build
cmake .. -DCMAKE_TOOLCHAIN_FILE=$VCPKG_HOME/scripts/buildsystems/vcpkg.cmake
make -j4
