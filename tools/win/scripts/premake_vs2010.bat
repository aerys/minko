call premake_clean.bat
call premake_templates.bat
chdir ..\..\..
tools\win\bin\premake4.exe --os=windows --platform=x32 vs2010
tools\win\bin\premake4.exe --os=windows --platform=x32 copyDLLs
 cd tools\win\scripts