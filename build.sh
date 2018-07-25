# build script / run in linux64 please

VERBO="0"
OPTIMIZER="OFF"
TYPE="Debug"

for i in "$@"
do
    if [[ "$i" == "VERBOSE"]]; then
        VERBO="1"
    elif [["$i" == "OPTIMIZER"]]; then
        OPTIMIZER="ON"
    elif [["$i" == "RELEASE"]]; then
        TYPE="Release"
    fi
done;

mkdir -p build
cd build

# build for linux64
rm CMakeCache.txt
cmake .. -DCMAKE_BUILD_TYPE=Release -DWITH_GLSL_OPTIMIZER="${OPTIMIZER}" && make -j4 VERBOSE="${VERBO}"

# build for WASM
rm CMakeCache.txt
cmake .. -DCMAKE_BUILD_TYPE=Release -DWITH_GLSL_OPTIMIZER="${OPTIMIZER}" -DWASM=ON -DCMAKE_TOOLCHAIN_FILE=/home/thomas/Aerys/emsdk/emscripten/1.38.8/cmake/Modules/Platform/Emscripten.cmake && make -j4 VERBOSE="${VERBO}"

# build for Android
rm CMakeCache.txt
cd ..
docker run --rm -it -v $PWD:/srv/smartshape-engine android-latest bash -c "cd /srv/smartshape-engine && mkdir -p build && cd build && cmake .. -DCMAKE_BUILD_TYPE=Release -DWITH_GLSL_OPTIMIZER=${OPTIMIZER} -DCMAKE_TOOLCHAIN_FILE=/opt/android-ndk-linux/build/cmake/android.toolchain.cmake && make -j4 VERBOSE=${VERBO}"
