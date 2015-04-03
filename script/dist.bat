@echo off

pushd ..\..\..
tool\win\bin\premake5.exe dist
popd
pause
