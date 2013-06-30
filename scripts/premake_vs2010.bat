cd ..
call premake_clean.bat
call premake_templates.bat
scripts\premake4.exe --os=windows --platform=x32 vs2010
scripts\premake4.exe --os=windows --platform=x32 copyDLLs
cd scripts
 