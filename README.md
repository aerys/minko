# SmartShape Engine

[[_TOC_]]

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

#### 1.2.4. More options

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

The **smartshape-engine** doesn't have to be run. It is used by the other part of smartshape as a base.

### 2.1. Develop

When modifying the **smartshape-engine** code, you have to build the engine again once your modifications are done.
