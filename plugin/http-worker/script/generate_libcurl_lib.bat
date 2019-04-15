@echo off

pushd "%~dp0..\lib\curl\lib\%1"

if "%1"=="windows64" (
    set filename=libcurl-x64
    set machine=x64
) else (
    set filename=libcurl
    set machine=x86
)

dumpbin /exports "%filename%.dll" > %filename%-exports.txt
echo LIBRARY %filename% > %filename%.def
echo EXPORTS >> %filename%.def
for /f "skip=19 tokens=4" %%A in (%filename%-exports.txt) do echo %%A >> %filename%.def
lib /def:%filename%.def /out:%filename%.lib /machine:%machine%

popd
