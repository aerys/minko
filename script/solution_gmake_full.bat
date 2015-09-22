@echo off

pushd ..
call script\premake5.bat %* gmake
popd
pause
