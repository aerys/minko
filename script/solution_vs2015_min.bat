@echo off

pushd ..
call script\premake5.bat --no-test --no-example --no-tutorial %* vs2015ctp
popd
pause
