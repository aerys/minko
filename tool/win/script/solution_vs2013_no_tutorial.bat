@echo off

chdir ..\..\..
tool\win\bin\premake5.exe --no-tutorial --no-test vs2013
pause
