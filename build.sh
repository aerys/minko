#!/bin/bash

OPTIMIZER="OFF"
TYPE="Debug"

for i in "$@"
do
    if [[ "$i" == "optimize" ]]; then
        OPTIMIZER="ON"
    elif [[ "$i" == "release" ]]; then
        TYPE="Release"
    fi
done;

mkdir -p build
cd build

# build for linux64
rm CMakeCache.txt
cmake .. -DCMAKE_BUILD_TYPE="${TYPE}" -DWITH_GLSL_OPTIMIZER="${OPTIMIZER}" && make -j4 VERBOSE=1

# build for WASM
rm CMakeCache.txt
cmake .. -DCMAKE_BUILD_TYPE="${TYPE}" -DWITH_GLSL_OPTIMIZER="${OPTIMIZER}" -DWASM=ON -DCMAKE_TOOLCHAIN_FILE=/home/thomas/Aerys/emsdk/emscripten/1.38.8/cmake/Modules/Platform/Emscripten.cmake && make -j4 VERBOSE=1

# build for Android
rm CMakeCache.txt
cd ..
docker run --rm -it -v $PWD:/srv/smartshape-engine android-latest bash -c "cd /srv/smartshape-engine && mkdir -p build && cd build && cmake .. -DCMAKE_BUILD_TYPE=Release -DWITH_GLSL_OPTIMIZER=ON -DCMAKE_TOOLCHAIN_FILE=/opt/android-ndk-linux/build/cmake/android.toolchain.cmake && make -j4 VERBOSE=1"
