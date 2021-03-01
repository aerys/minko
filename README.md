# SmartShape Engine

<!-- TOC depthFrom:2 -->

- [1. Build the engine](#1-build-the-engine)
    - [1.1. Prerequisites](#11-prerequisites)
    - [1.2. Build starting from **smartshape-engine@10.2.0**](#12-build-starting-from-smartshape-engine1020)
        - [1.2.1. Android](#121-android)
        - [1.2.2. HTML5](#122-html5)
        - [1.2.3. Linux64](#123-linux64)
	- [1.2.4. Windows64](#124-windows64)
        - [1.2.5. More options](#124-more-options)
    - [1.3. Build before **smartshape-engine@10.2.0**](#13-build-before-smartshape-engine1020)
        - [1.3.1. Android](#131-android)
        - [1.3.2. HTML5](#132-html5)
        - [1.3.3. Linux64](#133-linux64)
- [2. Run](#2-run)
- [3. Develop](#3-develop)
- [4. Contents](#4-contents)
    - [4.1 Framework](#41-framework)
    - [4.2 Example](#42-example)
    - [4.3 Template](#43-template)
    - [4.4 Plugin](#44-plugin)

<!-- /TOC -->

## 1. Build the engine

### 1.1. Prerequisites

- The following software are installed:
  - Docker CE 17+
- Being logged on to GitLab's Container Registry to be able to pull the docker images (see this [README](https://git.aerys.in/aerys/smartshape-docker/-/blob/master/README.md))
- Clone the repository [**smartshape-engine**](https://git.aerys.in/aerys/smartshape-engine)
- Go into the root directory of **smartshape-engine**.

### 1.2. Build starting from **smartshape-engine@10.2.0**

#### 1.2.1. Android

```bash
./script/build.sh android release
```

#### 1.2.2. HTML5

```bash
./script/build.sh html5 release
```

#### 1.2.3. Linux64

```bash
./script/build.sh linux64 release
```

#### 1.2.4. Windows64

```bash
bash plugin\serializer\script\download_dependencies.sh .
mkdir build-windows64-release
cd build-windows64-release
cmake -G "Visual Studio 15 2017 Win64" -DCMAKE_BUILD_TYPE=Release -DWITH_EXAMPLES=OFF -DWITH_PLUGINS=ON ..
msbuild Project.sln /property:Configuration=Release /property:Platform=x64 /m:4
```

If the final command fails, you might have to change the VCTargetsPath and add the path of msbuild.exe (for example "C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\MSBuild\15.0\Bin") to the PATH environment variable.
To change the VCTargetsPath (for example):
```bash
$ SET VCTargetsPath=C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\Common7\IDE\VC\VCTargets
```

#### 1.2.5. More options

There are more ways to build the engine. Here is the detailed usage of the building script

```bash
Usage: ./script/build.sh <target> <build-type> [--cmake '<cmake-args>']

<target>      The target platform of the build. Available targets are:
                  * android
                  * html5
                  * linux64

<build-type>  The type of build to perform. Available types are:
                  * debug
                  * release

ENVIRONMENT VARIABLES
              The following environment variables can customize the build:

              MAKE_ARGS
                  Arguments to pass to the make program. Default value: -j8.
```

### 1.3. Build before **smartshape-engine@10.2.0**

#### 1.3.1. Android

```bash
docker run -it --rm \
   -v ${PWD}:${PWD} -w ${PWD} \
    -v /etc/group:/etc/group:ro -v /etc/passwd:/etc/passwd:ro -u $(id -u $USER):$(id -g $USER) \
   registry.aerys.in/aerys/smartshape-docker/android:{TAG} \
   bash -c "
        mkdir -p build && cd build
        cmake .. \
            -DCMAKE_BUILD_TYPE=Release \
            -DWITH_EXAMPLES=OFF \
            -DWITH_PLUGINS=ON \
            -DWITH_NODEJS_WORKER=ON \
            -DCMAKE_TOOLCHAIN_FILE=/opt/android-ndk-linux/build/cmake/android.toolchain.cmake
        make
    "
```

#### 1.3.2. HTML5

```bash
docker run -it --rm \
   -v ${PWD}:${PWD} -w ${PWD} \
   -v /etc/group:/etc/group:ro -v /etc/passwd:/etc/passwd:ro -u $(id -u $USER):$(id -g $USER) \
   registry.aerys.in/aerys/smartshape-docker/html5:{TAG} \
    bash -c "
        mkdir -p build && cd build
        cmake .. \
            -DCMAKE_BUILD_TYPE=Release \
            -DWITH_EXAMPLES=OFF \
            -DWITH_PLUGINS=ON \
            -DCMAKE_TOOLCHAIN_FILE=/emsdk_portable/sdk/cmake/Modules/Platform/Emscripten.cmake
        make
    "
```

#### 1.3.3. Linux64

```bash
docker run -it --rm \
   -v ${PWD}:${PWD} -w ${PWD} \
   -v /etc/group:/etc/group:ro -v /etc/passwd:/etc/passwd:ro -u $(id -u $USER):$(id -g $USER) \
   registry.aerys.in/aerys/smartshape-docker/linux64:{TAG} \
   bash -c "
        mkdir -p build && cd build
        cmake .. \
            -DCMAKE_BUILD_TYPE=Release \
            -DWITH_EXAMPLES=OFF \
            -DWITH_PLUGINS=ON && \
        make
   "
```

## 2. Run

**smartshape-engine** doesn't have to be run. It is used by the other parts of SmartShape as a base.

## 3. Develop

When modifying the **smartshape-engine** code, you have to build the engine again once your modifications are done.


## 4. Contents

### 4.1 Framework
---------

Sources for the Minko framework.

### 4.2 Example

Example applications created with the Minko framework and its plugins.

### 4.3 Template

This folder contains templates you can use to ease the development of the Minko framework, plugins
or applications.

### 4.4 Plugin

Sources for Minko's plugins.

Available plugins are:
* angle
* assimp
* bullet
* debug
* devil
* fx
* html-overlay
* http-loader
* http-worker
* jpeg
* leap
* lua
* oculus
* offscreen
* particles
* png
* sdl
* sensors
* serializer
* video-camera
* vr
* zlib
