@echo off

pushd ..\..\..
tool\win\bin\premake5.exe --no-test %* vs2013
popd
pause
