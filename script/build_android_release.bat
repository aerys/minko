@echo off

set CONFIG=android_release

pushd ..
bash "script\build_%CONFIG%.sh"
popd
pause
