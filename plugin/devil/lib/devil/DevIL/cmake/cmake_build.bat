rem @echo off

rem BUILD 1 - 32-BIT ANSI
rem =====================

rm CMakeCache.txt
rm -rf cmakefiles

cmake . -G "Visual Studio 14 2015" -DUNICODE=0 -DJPEG_LIBRARY="D:\Coding\Libraries\jpeg-9b\libjpeg.lib" -DJPEG_INCLUDE_DIR="D:\Coding\Libraries\jpeg-9b" -DJPEG_INCLUDE_DIRS="D:\Coding\Libraries\jpeg-9b" -DPNG_LIBRARY="D:\Coding\Libraries\libpng-1.6.21\Release\libpng16_static.lib" -DPNG_PNG_INCLUDE_DIR="D:\Coding\Libraries\libpng-1.6.21" -DZLIB_LIBRARY="D:\Coding\Libraries\zlib-1.2.8\Release\zlibstatic.lib" -DZLIB_INCLUDE_DIR="D:\Coding\Libraries\zlib-1.2.8" -DTIFF_LIBRARY="D:\Coding\Libraries\tiff-4.0.6\libtiff\Release\tiff.lib" -DTIFF_INCLUDE_DIR="D:\Coding\Libraries\tiff-4.0.6\libtiff" -DJASPER_LIBRARIES="D:\Coding\Libraries\jasper-1.900.1\src\msvc\Win32_Release\libjasper.lib" -DJASPER_INCLUDE_DIR="D:\Coding\Libraries\jasper-1.900.1\src\libjasper\include" -DLIBSQUISH_LIBRARY="D:\Coding\Libraries\libsquish-1.14\Release\squish.lib" -DLIBSQUISH_LIBRARY_DEBUG="D:\Coding\Libraries\libsquish-1.14\Debug\squishd.lib" -DLIBSQUISH_INCLUDE_DIR="D:\Coding\Libraries\libsquish-1.14" -DLCMS2_LIBRARY="D:\Coding\Libraries\lcms2-2.7\Lib\MS\lcms2_static.lib" -DLCMS2_INCLUDE_DIR="D:\Coding\Libraries\lcms2-2.7\include" -DMNG_LIBRARY="D:\Coding\Libraries\libmng-2.0.3\bin\Release\libmng.lib" -DMNG_INCLUDE_DIR="D:\Coding\Libraries\libmng-2.0.3"

devenv ImageLib.sln /Rebuild Release
devenv ImageLib.sln /Rebuild Debug



rem BUILD 2 - 32-BIT UNICODE
rem ========================

rm CMakeCache.txt
rm -rf cmakefiles

cmake . -G "Visual Studio 14 2015" -DUNICODE=1 -DJPEG_LIBRARY="D:\Coding\Libraries\jpeg-9b\libjpeg.lib" -DJPEG_INCLUDE_DIR="D:\Coding\Libraries\jpeg-9b" -DJPEG_INCLUDE_DIRS="D:\Coding\Libraries\jpeg-9b" -DPNG_LIBRARY="D:\Coding\Libraries\libpng-1.6.21\Release\libpng16_static.lib" -DPNG_PNG_INCLUDE_DIR="D:\Coding\Libraries\libpng-1.6.21" -DZLIB_LIBRARY="D:\Coding\Libraries\zlib-1.2.8\Release\zlibstatic.lib" -DZLIB_INCLUDE_DIR="D:\Coding\Libraries\zlib-1.2.8" -DTIFF_LIBRARY="D:\Coding\Libraries\tiff-4.0.6\libtiff\Release\tiff.lib" -DTIFF_INCLUDE_DIR="D:\Coding\Libraries\tiff-4.0.6\libtiff" -DJASPER_LIBRARIES="D:\Coding\Libraries\jasper-1.900.1\src\msvc\Win32_Release\libjasper.lib" -DJASPER_INCLUDE_DIR="D:\Coding\Libraries\jasper-1.900.1\src\libjasper\include" -DLIBSQUISH_LIBRARY="D:\Coding\Libraries\libsquish-1.14\Release\squish.lib" -DLIBSQUISH_LIBRARY_DEBUG="D:\Coding\Libraries\libsquish-1.14\Debug\squishd.lib" -DLIBSQUISH_INCLUDE_DIR="D:\Coding\Libraries\libsquish-1.14" -DLCMS2_LIBRARY="D:\Coding\Libraries\lcms2-2.7\Lib\MS\lcms2_static.lib" -DLCMS2_INCLUDE_DIR="D:\Coding\Libraries\lcms2-2.7\include" -DMNG_LIBRARY="D:\Coding\Libraries\libmng-2.0.3\bin\Release\libmng.lib" -DMNG_INCLUDE_DIR="D:\Coding\Libraries\libmng-2.0.3"

devenv ImageLib.sln /Build Release
devenv ImageLib.sln /Build Debug



rem BUILD 3 - 64-BIT ANSI
rem =====================

rm CMakeCache.txt
rm -rf cmakefiles

cmake . -G "Visual Studio 14 2015 Win64" -DUNICODE=0 -DPNG_LIBRARY="D:\Coding\Libraries\64-bit for DevIL\libpng-1.6.21\Release\libpng16_static.lib" -DPNG_PNG_INCLUDE_DIR="D:\Coding\Libraries\64-bit for DevIL\libpng-1.6.21" -DJASPER_LIBRARIES="D:\Coding\Libraries\64-bit for DevIL\jasper-1.900.1\src\msvc\Win64_Release\libjasper.lib" -DJASPER_INCLUDE_DIR="D:\Coding\Libraries\64-bit for DevIL\jasper-1.900.1\src\libjasper\include" -DZLIB_LIBRARY="D:\Coding\Libraries\64-bit for DevIL\zlib-1.2.8\Release\zlibstatic.lib" -DZLIB_INCLUDE_DIR="D:\Coding\Libraries\64-bit for DevIL\zlib-1.2.8" -DTIFF_LIBRARY="D:\Coding\Libraries\64-bit for DevIL\tiff-4.0.6\libtiff\Release\tiff.lib" -DTIFF_INCLUDE_DIR="D:\Coding\Libraries\64-bit for DevIL\tiff-4.0.6\libtiff" -DJPEG_LIBRARY="D:\Coding\Libraries\64-bit for DevIL\jpeg-9b\x64\Release\libjpeg.lib" -DJPEG_INCLUDE_DIR="D:\Coding\Libraries\64-bit for DevIL\jpeg-9b" -DJPEG_INCLUDE_DIRS="D:\Coding\Libraries\64-bit for DevIL\jpeg-9b" -DLIBSQUISH_LIBRARY="D:\Coding\Libraries\64-bit for DevIL\libsquish-1.14\Release\squish.lib" -DLIBSQUISH_LIBRARY_DEBUG="D:\Coding\Libraries\64-bit for DevIL\libsquish-1.14\Debug\squishd.lib" -DLIBSQUISH_INCLUDE_DIR="D:\Coding\Libraries\64-bit for DevIL\libsquish-1.14" -DLCMS2_LIBRARY="D:\Coding\Libraries\64-bit for DevIL\lcms2-2.7\Lib\MS\lcms2_static.lib" -DLCMS2_INCLUDE_DIR="D:\Coding\Libraries\64-bit for DevIL\lcms2-2.7\include" -DMNG_LIBRARY="D:\Coding\Libraries\64-bit for DevIL\libmng-2.0.3\bin\Release\libmng.lib" -DMNG_INCLUDE_DIR="D:\Coding\Libraries\64-bit for DevIL\libmng-2.0.3"

devenv ImageLib.sln /Build Release
devenv ImageLib.sln /Build Debug



rem BUILD 4 - 64-BIT UNICODE
rem ========================

rm CMakeCache.txt
rm -rf cmakefiles

cmake . -G "Visual Studio 14 2015 Win64" -DUNICODE=1 -DPNG_LIBRARY="D:\Coding\Libraries\64-bit for DevIL\libpng-1.6.21\Release\libpng16_static.lib" -DPNG_PNG_INCLUDE_DIR="D:\Coding\Libraries\64-bit for DevIL\libpng-1.6.21" -DJASPER_LIBRARIES="D:\Coding\Libraries\64-bit for DevIL\jasper-1.900.1\src\msvc\Win64_Release\libjasper.lib" -DJASPER_INCLUDE_DIR="D:\Coding\Libraries\64-bit for DevIL\jasper-1.900.1\src\libjasper\include" -DZLIB_LIBRARY="D:\Coding\Libraries\64-bit for DevIL\zlib-1.2.8\Release\zlibstatic.lib" -DZLIB_INCLUDE_DIR="D:\Coding\Libraries\64-bit for DevIL\zlib-1.2.8" -DTIFF_LIBRARY="D:\Coding\Libraries\64-bit for DevIL\tiff-4.0.6\libtiff\Release\tiff.lib" -DTIFF_INCLUDE_DIR="D:\Coding\Libraries\64-bit for DevIL\tiff-4.0.6\libtiff" -DJPEG_LIBRARY="D:\Coding\Libraries\64-bit for DevIL\jpeg-9b\x64\Release\libjpeg.lib" -DJPEG_INCLUDE_DIR="D:\Coding\Libraries\64-bit for DevIL\jpeg-9b" -DJPEG_INCLUDE_DIRS="D:\Coding\Libraries\64-bit for DevIL\jpeg-9b" -DLIBSQUISH_LIBRARY="D:\Coding\Libraries\64-bit for DevIL\libsquish-1.14\Release\squish.lib" -DLIBSQUISH_LIBRARY_DEBUG="D:\Coding\Libraries\64-bit for DevIL\libsquish-1.14\Debug\squishd.lib" -DLIBSQUISH_INCLUDE_DIR="D:\Coding\Libraries\64-bit for DevIL\libsquish-1.14" -DLCMS2_LIBRARY="D:\Coding\Libraries\64-bit for DevIL\lcms2-2.7\Lib\MS\lcms2_static.lib" -DLCMS2_INCLUDE_DIR="D:\Coding\Libraries\64-bit for DevIL\lcms2-2.7\include" -DMNG_LIBRARY="D:\Coding\Libraries\64-bit for DevIL\libmng-2.0.3\bin\Release\libmng.lib" -DMNG_INCLUDE_DIR="D:\Coding\Libraries\64-bit for DevIL\libmng-2.0.3"

devenv ImageLib.sln /Build Release
devenv ImageLib.sln /Build Debug
