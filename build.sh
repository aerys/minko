#!/bin/bash

TYPE="Debug"

if [[ "$1" != "linux" && "$1" != "android" && "$1" != "asmjs" && "$1" != "wasm" ]]; then
    echo "target error!"
    exit 2;
fi

if [[ "$2" != "debug" && "$2" != "release" ]]; then
    echo "buid type error!"
    exit 2;
fi

if [[ "$3" != "" && "$3" != "-j4" && "$3" != "-j8" ]]; then
    echo "invalid '-j' number!"
    exit 2;
fi

if [[ "$4" != "reset" ]]; then
    sudo rm -rf build
fi

mkdir -p build
cd build

if [[ "$1" == "linux" ]]; then
    cmake .. -DCMAKE_BUILD_TYPE="$2" -DWITH_GLSL_OPTIMIZER=ON && make "$3" VERBOSE=1
fi

if [[ "$1" == "asmjs" ]]; then
    cmake .. -DCMAKE_BUILD_TYPE="$2" -DWITH_GLSL_OPTIMIZER=ON -DCMAKE_TOOLCHAIN_FILE=/home/thomas/Aerys/emsdk/emscripten/1.38.8/cmake/Modules/Platform/Emscripten.cmake && make "$3" VERBOSE=1
fi

if [[ "$1" == "wasm" ]]; then
    cmake .. -DCMAKE_BUILD_TYPE="$2" -DWITH_GLSL_OPTIMIZER=ON -DWASM=ON -DCMAKE_TOOLCHAIN_FILE=/home/thomas/Aerys/emsdk/emscripten/1.38.8/cmake/Modules/Platform/Emscripten.cmake && make "$3" VERBOSE=1
fi

if [[ "$1" == "android" ]]; then
    cd ..
    docker run --rm -it -v $PWD:/srv/smartshape-engine android-latest bash -c "cd /srv/smartshape-engine && mkdir -p build && cd build && cmake .. -DCMAKE_BUILD_TYPE=$2 -DWITH_GLSL_OPTIMIZER=ON -DCMAKE_TOOLCHAIN_FILE=/opt/android-ndk-linux/build/cmake/android.toolchain.cmake && make $3 VERBOSE=1"
fi