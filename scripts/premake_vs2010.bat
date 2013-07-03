call premake_clean.bat
call premake_templates.bat
cd ..
scripts\premake4.exe --os=windows --platform=x32 vs2010
scripts\premake4.exe --os=windows --platform=x32 copyDLLs
cd scripts
 