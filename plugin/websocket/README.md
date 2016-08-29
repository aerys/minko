# Known limitations

* Only one WebSocket object per app when building with Emscripten/targeting HTML5.
* Not tested/working on:
 * iOS
 * OSX

# Dependencies

* [OpenSSL](https://www.openssl.org/) 1.0.2
  * Windows 32bit and 64bit pre-built binaries are provided, other pre-build Windows binaries are available [here](http://p-nand-q.com/programming/windows/building_openssl_with_visual_studio_2013.html).
  * Android armeabi-v7a pre-built binaries are provided, other binaries can be built using `script/build_openssl_android.sh`.
  * Linux: install libssl with `apt-get install libssl-dev`.
  * iOS ARMv7, ARMv7s, ARM64 and i386 simulator prebuilt binaries are provided and taken from [here](https://github.com/noloader/openssl-1.0.1i-ios/).
* [WebSocket++](https://github.com/zaphoyd/websocketpp) 0.7.0
* [ASIO](http://think-async.com/Asio/) 1.10.6
