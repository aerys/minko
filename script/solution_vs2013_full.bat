@echo off

pushd ..
call script\premake5.bat %* vs2013
popd
pause
