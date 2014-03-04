@echo off

chdir ..\..\..
tool\win\bin\premake5.exe --no-test --no-example --no-tutorial vs2013
pause
