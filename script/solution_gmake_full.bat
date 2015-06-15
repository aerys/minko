@echo off

pushd ..
bash "%MINKO_HOME%\script\premake5.sh" %* gmake
popd
pause
