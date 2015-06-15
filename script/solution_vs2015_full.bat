@echo off

pushd ..
call script\premake5.bat %* vs2015ctp
popd
pause
