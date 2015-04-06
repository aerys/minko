@echo off

if %PROCESSOR_ARCHITECTURE%==x86 (
    bin\windows32\premake5.exe $@
) else (
    bin\windows64\premake5.exe $@
)
