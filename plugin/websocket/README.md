# Known limitations

* Not tested/working on:
 * iOS
 * OSX

# Dependencies

* [OpenSSL](https://www.openssl.org/) 1.0.2
  * Windows 32bit and 64bit pre-build binaries are provided, other pre-build Windows binaries are available [here](http://p-nand-q.com/programming/windows/building_openssl_with_visual_studio_2013.html).
  * Android armeabi-v7a pre-built binaries are provided, other binaries can be built using `script/build_openssl_android.sh`.
  * On Linux, install libssl with `apt-get install libssl-dev`.
* [WebSocket++](https://github.com/zaphoyd/websocketpp) 0.7.0
* [ASIO](http://think-async.com/Asio/) 1.10.6
