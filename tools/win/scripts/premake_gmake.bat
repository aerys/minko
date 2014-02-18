:: If the script is called from another location
:: => we go directly in the real script location
set exdir=%~dp0
cd /%exdir:~0,1% %exdir%

chdir ..\..\..
tools\win\bin\premake5.exe --no-tests gmake
cd tools\win\scripts

timeout /T 30