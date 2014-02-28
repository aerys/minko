chdir ..\..\..
call "%EMSCRIPTEN%\..\..\emsdk_add_path.bat"
tool\win\bin\premake5.exe --no-test gmake
mingw32-make SHELL=cmd.exe config=html5_release verbose=1
pause
