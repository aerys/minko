@echo off
pushd %~dp0
echo Compiling shader and packing into header: %~2
setlocal

set PATH=%PATH%;"%DXSDK_DIR%Utilities\bin\x86\"
fxc.exe  /nologo /E main /T vs_4_0 /Fo "%1" %2
bin2header.exe "%1"

echo Generating shader reflection data for %1
ShaderReflector "%1" "%1_refl.h"

del "%1"
endlocal
popd
