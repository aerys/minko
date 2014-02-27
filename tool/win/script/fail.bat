@echo off

:: If the script is called from another location
:: => we go directly in the real script location
set exdir=%~dp0
cd /%exdir:~0,1% %exdir%

set target=%1
set error=%errorlevel%

echo Post-build command failed for project "%target%"

del %target:/=\%

if %error% neq 0 exit /b %error%
