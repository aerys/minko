@echo off
chdir ..
"%MINKO_HOME%"\tools\win\bin\premake5.exe --no-tests vs2013
pause
