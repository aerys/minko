# Known limitations

* Not supported on OSX

# Dependencies

* Android: libcurl 7.54.1 downloaded from [here](https://github.com/leenjewel/openssl_for_ios_and_android)
* iOS: libcurl 7.51.0 downloaded from [here](https://github.com/leenjewel/openssl_for_ios_and_android)
* Windows:
    * libcurl 7.61.1 downloaded from [here](https://curl.haxx.se/download.html);
    * each `*.lib` is generated from the corresponding DLL using `script/generate_libcurl_lib.bat windows(32|64)`.
* Linux: `apt-get install libcurl4-openssl-dev`
