# Known limitations

* Only one WebSocket object per app when building with Emscripten/targeting HTML5.
* Not tested/working on:
 * iOS
 * OSX

# Dependencies

* OpenSSL 1.0.2 (https://www.openssl.org/)
  * [Pre-build Windows binaries](http://p-nand-q.com/programming/windows/building_openssl_with_visual_studio_2013.html)
  * Android binaries can be built using `script/build_openssl_android.sh`
* WebSocket++ 0.7.0 (https://github.com/zaphoyd/websocketpp)
* ASIO 1.10.6 (http://think-async.com/Asio/)
