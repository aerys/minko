@echo off
@setlocal enabledelayedexpansion

pushd ..

if not defined MINKO_HOME (
    echo Environment variable MINKO_HOME is not defined.
    pause
    exit /b 1
)

mingw32-make %* SHELL=cmd.exe config=html5_release verbose=1

popd
pause
