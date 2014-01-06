call premake_documentation.bat

chdir ..\..\..
tools\win\bin\premake4.exe dist
cd tools\win\scripts

timeout /T 30