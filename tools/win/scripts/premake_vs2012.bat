call premake_clean.bat
call premake_templates.bat

chdir ..\..\..
tools\win\bin\premake5.exe --no-tests vs2012
cd tools\win\scripts

timeout /T 30