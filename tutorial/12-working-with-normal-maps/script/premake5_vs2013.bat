@echo off

chdir ..
"%MINKO_HOME%"\tools\win\bin\premake5.exe --os=windows vs2013
PAUSE
