# Known limitations

* Not tested on:
 * iOS
 * macOS

# Dependencies

* [OpenSSL](https://www.openssl.org/)
  * Version being used is 1.1.0
  * Windows 32bit and 64bit pre-built release binaries are provided, other pre-build Windows binaries are available [here](http://p-nand-q.com/programming/windows/building_openssl_with_visual_studio_2013.html).
  * Android armeabi-v7a pre-built binaries are provided and taken from [here](https://github.com/leenjewel/openssl_for_ios_and_android). But it can also be built using `script/build_openssl_android.sh`.
  * Linux: install libssl with `apt-get install libssl-dev`.
  * iOS ARMv7, ARMv7s, ARM64 and i386 simulator prebuilt binaries are provided and taken from [here](https://github.com/leenjewel/openssl_for_ios_and_android).
