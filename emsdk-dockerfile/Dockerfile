FROM trzeci/emscripten:sdk-tag-1.37.22-64bit

RUN apt-get update && apt-get install make

# Install CMake
RUN cd / && \
    wget -q https://cmake.org/files/v3.9/cmake-3.9.6-Linux-x86_64.sh -O cmake.sh && \
   chmod +x ./cmake.sh && \
   ./cmake.sh --skip-license && \
   rm -rf cmake.sh
