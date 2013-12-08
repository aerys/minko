call premake_clean.bat
call premake_templates.bat

chdir ..\..\..
tools\win\bin\premake4.exe --os=windows --platform=x32 --no-tests vs2012
cd tools\win\scripts

chdir ..\..\..\examples
..\tools\win\bin\premake4.exe --os=windows --platform=x32 vs2010
cd ..\tools\win\scripts

timeout /T 30