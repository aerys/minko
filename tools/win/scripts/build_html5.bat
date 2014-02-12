chdir ..\..\..
call "%EMSCRIPTEN%\..\..\emsdk_add_path.bat"
tools\win\bin\premake5.exe --no-tests gmake
mingw32-make config=html5_release verbose=1
pause
