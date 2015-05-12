@echo off

if %PROCESSOR_ARCHITECTURE%==x86 (
    "%MINKO_HOME%\bin\windows32\premake5.exe" %*
) else (
    "%MINKO_HOME%\bin\windows64\premake5.exe" %*
)
