@echo off

chdir ..\..\..
tool\win\bin\premake5.exe --no-test gmake
pause
