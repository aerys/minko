@echo off

chdir ..\..\..
tool\win\bin\premake5.exe --no-test --with-writer vs2013
pause
