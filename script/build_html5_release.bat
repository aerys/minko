@echo off

set CONFIG=html5_release

pushd ..
bash "script\build_%CONFIG%.sh"
popd
pause
