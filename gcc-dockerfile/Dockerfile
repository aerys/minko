FROM gcc:4.9

# Intall make
RUN apt-get update && apt-get install make

# Install CMake
RUN cd / && \
    wget -q https://cmake.org/files/v3.9/cmake-3.9.6-Linux-x86_64.sh -O cmake.sh && \
   chmod +x ./cmake.sh && \
   ./cmake.sh --skip-license && \
   rm -rf cmake.sh

RUN apt-get -y install software-properties-common && \
    apt-get update

# SDL2 
RUN apt-get -y install libgl1-mesa-dev libglu1-mesa-dev libglew-dev libsdl2-dev

# HTTP (optional) 
RUN apt-get -y install libcurl4-openssl-dev

# HTML overlay (optional) 
RUN apt-get -y install libgtk2.0-dev libgtkglextmm-x11-1.2-dev

# EMSDK
RUN git clone https://github.com/juj/emsdk.git && cd emsdk && \
    ./emsdk install latest && ./emsdk activate latest && ./emsdk_env.sh