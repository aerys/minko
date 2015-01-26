@echo off

pushd ..\..\..
tool\win\bin\premake5.exe %* vs2013
popd
pause
