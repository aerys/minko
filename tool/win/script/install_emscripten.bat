echo off
@setlocal enabledelayedexpansion

set EMSDK_WIKI=https://github.com/kripken/emscripten/wiki/Emscripten-SDK#wiki-downloads

if "%PROCESSOR_ARCHITECTURE%" == "AMD64" (
	set EMSDK_MINGW=mingw-4.6.2-32bit
	set EMSDK_JAVA=java-7.45-64bit
) else (
	set EMSDK_MINGW=mingw-4.6.2-32bit
	set EMSDK_JAVA=java-7.45-32bit
)

if not defined EMSCRIPTEN (
	echo Environment variable EMSCRIPTEN is not defined.
	echo Press any key to open your browser to the Emscripten SDK download page...
	echo Once installed, please run this script again.
	pause
	start %EMSDK_WIKI%
	exit /b 1
)

call "%EMSCRIPTEN%\..\..\emsdk_env.bat"

call emsdk list | findstr %EMSDK_MINGW% | findstr INSTALLED | findstr *

if "%errorlevel%" == "1" (
	call emsdk install %EMSDK_MINGW%
	call emsdk activate %EMSDK_MINGW%
)

call emsdk list | findstr %EMSDK_JAVA% | findstr INSTALLED | findstr *

if "%errorlevel%" == "1" (
	call emsdk install %EMSDK_JAVA%
	call emsdk activate %EMSDK_JAVA%
)
