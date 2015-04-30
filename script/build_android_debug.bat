@echo off

set CONFIG=android_debug

pushd ..
bash "script\build_%CONFIG%.sh"
popd
pause
