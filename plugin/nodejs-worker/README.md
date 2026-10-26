# NodeJS Worker

## How to upgrade `lib/node/android/<abi>/<ndk>/lib/libnode.so`

Prebuilt packages come from [aerys/smartshape/vendor/node](https://git.aerys.in/aerys/smartshape/vendor/node), which
builds Node.js for Android from source (based on the [nodejs-mobile project](https://github.com/nodejs-mobile/nodejs-mobile)).
Download the `node-<version>-android-<abi>-<ndk>.tar.gz` generic packages of that project and extract each one in
`lib/node/android/<abi>/<ndk>/`: `lib/libnode.so`, licences and `build-info.json`. Leave out `include/`: nothing
compiles against the Node.js headers.

`<ndk>` must be the NDK the engine builds with, `MINKO_ANDROID_NDK_MAJOR` in `cmake/minko.cmake`: apps build with that
NDK or a later one and ship its `libc++_shared.so`, which `libnode.so` links against. When the NDK of the packages
changes, update `MINKO_ANDROID_NDK_MAJOR` (and the Android build image) with it.

`NodeJSWorker.cpp` declares the Node.js functions it calls instead of including `node.h`; the Android build fails if
`libnode.so` does not export one of them (`cmake/check-node-symbols.cmake`).
