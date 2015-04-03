@echo off

pushd ..\..\..
tool\win\bin\premake5.exe --no-test gmake
popd
pause
