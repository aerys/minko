@echo off

chdir ..\..\..
tool\win\bin\premake5.exe --no-test --no-tutorial vs2013
pause
