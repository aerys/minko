; DevIL.nsi

Name "Developer's Image Library 1.7.1 Run-time Files"

; The file to write
OutFile "DevIL-1.7.1.exe"

; The default installation directory
InstallDir "$PROGRAMFILES\Developer's Image Library"

; Request application privileges for Windows Vista
RequestExecutionLevel admin

;--------------------------------

; Pages

Page components
Page directory
Page instfiles

UninstPage uninstConfirm
UninstPage instfiles

;--------------------------------

; The stuff to install
Section "Developer's Image Library (required)"

  SectionIn RO
  
  ; Set output path to the installation directory.
  SetOutPath $SYSDIR
  
  ; Put files in the System folder
  File "..\devil\lib\DevIL.dll"
  File "..\devil\lib\ILU.dll"
  File "..\devil\lib\ILUT.dll"


  ; Set output path to the installation directory.
  SetOutPath $INSTDIR

  ; Write the installation path into the registry
  WriteRegStr HKLM SOFTWARE\DevIL "Install_Dir" "$INSTDIR"
  
  ; Write the uninstall keys for Windows
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\DevIL" "DisplayName" "Developer's Image Library"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\DevIL" "UninstallString" '"$INSTDIR\uninstall.exe"'
  WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\DevIL" "NoModify" 1
  WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\DevIL" "NoRepair" 1
  WriteUninstaller "uninstall.exe"
  
SectionEnd



;--------------------------------
; Uninstaller

Section "Uninstall"
  
  ; Remove registry keys
  DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\DevIL"
  DeleteRegKey HKLM SOFTWARE\DevIL

  ; Remove files and uninstaller
  Delete $SYSDIR\DevIL.dll
  Delete $SYSDIR\ILU.dll
  Delete $SYSDIR\ILUT.dll
  Delete $INSTDIR\uninstall.exe

  ; Remove directories used
  RMDir "$INSTDIR"

SectionEnd
