; DevIL.nsi

Name "Developer's Image Library 1.7.1 Run-time Files"

; The file to write
OutFile "DevIL-1.7.1.exe"

; Request application privileges for Windows Vista
RequestExecutionLevel admin

;--------------------------------

; Pages

Page components
Page instfiles

;--------------------------------

; The stuff to install
Section "Developer's Image Library (required)"

  SectionIn RO
  
  ; Set output path to the installation directory.
  SetOutPath $SYSDIR
  
  ; Put file there
  File "..\devil\lib\devil.dll"
  File "..\devil\lib\ilu.dll"
  File "..\devil\lib\ilut.dll"
  
SectionEnd

