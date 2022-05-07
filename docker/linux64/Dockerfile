FROM ubuntu:18.04

RUN apt-get update && apt-get install -y wget

RUN cd / && \
    wget -q https://cmake.org/files/v3.9/cmake-3.9.6-Linux-x86_64.sh -O cmake.sh && \
    chmod +x ./cmake.sh && \
    ./cmake.sh --skip-license && \
    rm -rf cmake.sh

RUN apt-get update && apt-get install -y \
    g++-7 \
    libgl1-mesa-dev \
    libglu1-mesa-dev \
    libosmesa6-dev \
    libglew-dev \
    libsdl2-dev \
    libgtk2.0-dev \
    libgtk-3-dev \
    libgtkglext1-dev \
    libssl-dev \
    libcurl4-openssl-dev \
    libfreetype6-dev \
    libnss3-dev \
    curl

RUN apt-get install -y jq
