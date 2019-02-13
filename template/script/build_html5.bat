@echo off
@setlocal enabledelayedexpansion

chdir ..

if not defined EMSCRIPTEN (
	echo Environment variable EMSCRIPTEN is not defined.
	pause
	exit /b 1
)

if not defined MINKO_HOME (
	echo Environment variable MINKO_HOME is not defined.
	pause
	exit /b 1
)

call "%EMSCRIPTEN%\..\..\emsdk_add_path.bat"
"%MINKO_HOME%\tool\win\bin\premake5.exe" gmake
mingw32-make SHELL=cmd.exe config=html5_release verbose=1
pause
