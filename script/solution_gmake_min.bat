@echo off

pushd ..
call script\premake5.bat --no-test --no-example --no-tutorial $* gmake
popd
pause
