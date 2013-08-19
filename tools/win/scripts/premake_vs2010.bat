call premake_clean.bat
call premake_templates.bat
chdir ..\..\..
tools\win\bin\premake4.exe --os=windows --no-glsl-optimizer=true --platform=x32 vs2010
cd tools\win\scripts