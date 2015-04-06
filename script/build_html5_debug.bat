@echo off

set CONFIG=html5_debug

pushd ..
bash "script\build_%CONFIG%.sh"
popd
pause
