@echo off

chdir ..\..\..
tool\win\bin\premake5.exe --no-tutorial --with-offscreen vs2013
pause
