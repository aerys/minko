@echo off

@echo off

setlocal

rem Read the Git for Windows installation path from the Registry.
:REG_QUERY
for /f "skip=2 delims=: tokens=1*" %%a in ('reg query "HKLM\SOFTWARE%WOW%\Microsoft\Windows\CurrentVersion\Uninstall\Git_is1" /v InstallLocation 2^> nul') do (
    for /f "tokens=3" %%z in ("%%a") do (
        set GIT=%%z:%%b
    )
)
if "%GIT%"=="" (
    if "%WOW%"=="" (
        rem Assume we are on 64-bit Windows, so explicitly read the 32-bit Registry.
        set WOW=\Wow6432Node
        goto REG_QUERY
    ) else (
    	echo Git must be installed to properly clean the solution.
    	pause
    	exit /b 1
    )
)

rem Make sure Git is in our temporary PATH.
set PATH=%GIT%bin;%PATH%

chdir ..\..\..
tool\win\bin\premake5.exe clean
pause
