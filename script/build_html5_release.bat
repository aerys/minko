@echo off
@setlocal enabledelayedexpansion

pushd ..

if not defined MINKO_HOME (
	echo Environment variable MINKO_HOME is not defined.
	pause
	exit /b 1
)

call %MINKO_HOME%\script\premake5.bat --no-test gmake
mingw32-make SHELL=cmd.exe config=html5_release verbose=1 -j4

popd
pause
