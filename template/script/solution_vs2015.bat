@echo off

pushd ..
call "%MINKO_HOME%\script\premake5.bat" %* vs2015ctp
popd
pause
