@echo off
@setlocal enabledelayedexpansion

pushd ..\..\..

if not defined MINKO_HOME (
	echo Environment variable MINKO_HOME is not defined.
	pause
	exit /b 1
)

call "%MINKO_HOME%\tool\win\script\install_emscripten.bat"

tool\win\bin\premake5.exe gmake && mingw32-make SHELL=cmd.exe config=html5_release minko-example-fog verbose=1

popd
pause
