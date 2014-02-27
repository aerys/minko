@echo off

set target=%1
set error=%errorlevel%

echo Post-build command failed for project "%target%"

del %target:/=\%

if %error% neq 0 exit /b %error%
