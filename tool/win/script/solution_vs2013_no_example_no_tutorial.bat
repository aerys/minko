@echo off

pushd ..\..\..
tool\win\bin\premake5.exe --no-test --no-example --no-tutorial vs2013
popd
pause
