@echo off

chdir ..\..\..
tool\win\bin\premake5.exe --no-test --no-example vs2013
pause
