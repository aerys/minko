@echo off

pushd ..
bash "%MINKO_HOME%\script\premake5.sh" --no-test --no-example --no-tutorial %* gmake
popd
pause
