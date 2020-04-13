# Microsoft Developer Studio Generated NMAKE File, Based on WindowsTest.dsp
!IF "$(CFG)" == ""
CFG=WindowsTest - Win32 Debug
!MESSAGE No configuration specified. Defaulting to WindowsTest - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "WindowsTest - Win32 Release" && "$(CFG)" != "WindowsTest - Win32 Debug" && "$(CFG)" != "WindowsTest - Win32 Dynamic"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "WindowsTest.mak" CFG="WindowsTest - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "WindowsTest - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "WindowsTest - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE "WindowsTest - Win32 Dynamic" (based on "Win32 (x86) Application")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "WindowsTest - Win32 Release"

OUTDIR=.\../bin
INTDIR=.\obj
# Begin Custom Macros
OutDir=.\../bin
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\WindowsTest.exe"

!ELSE 

ALL : "ILU - Win32 Release" "IL - Win32 Release" "ILUT - Win32 Release" "$(OUTDIR)\WindowsTest.exe"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :"ILUT - Win32 ReleaseCLEAN" "IL - Win32 ReleaseCLEAN" "ILU - Win32 ReleaseCLEAN" 
!ELSE 
CLEAN :
!ENDIF 
	-@erase "$(INTDIR)\BatchConv.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\WindowsTest.obj"
	-@erase "$(INTDIR)\WindowsTest.res"
	-@erase "$(OUTDIR)\WindowsTest.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

CPP_PROJ=/nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\WindowsTest.res" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\WindowsTest.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=user32.lib gdi32.lib comdlg32.lib shell32.lib /nologo /subsystem:windows /incremental:no /pdb:"$(OUTDIR)\WindowsTest.pdb" /machine:I386 /out:"$(OUTDIR)\WindowsTest.exe" 
LINK32_OBJS= \
	"$(INTDIR)\BatchConv.obj" \
	"$(INTDIR)\WindowsTest.obj" \
	"$(INTDIR)\WindowsTest.res" \
	"..\..\lib\ILUT.lib" \
	"..\..\lib\DevIL.lib" \
	"..\..\lib\ILU.lib"

"$(OUTDIR)\WindowsTest.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "WindowsTest - Win32 Debug"

OUTDIR=.\../bin/debug
INTDIR=.\obj/debug
# Begin Custom Macros
OutDir=.\../bin/debug
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\WindowsTest.exe"

!ELSE 

ALL : "ILU - Win32 Debug" "IL - Win32 Debug" "ILUT - Win32 Debug" "$(OUTDIR)\WindowsTest.exe"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :"ILUT - Win32 DebugCLEAN" "IL - Win32 DebugCLEAN" "ILU - Win32 DebugCLEAN" 
!ELSE 
CLEAN :
!ENDIF 
	-@erase "$(INTDIR)\BatchConv.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(INTDIR)\WindowsTest.obj"
	-@erase "$(INTDIR)\WindowsTest.res"
	-@erase "$(OUTDIR)\WindowsTest.exe"
	-@erase "$(OUTDIR)\WindowsTest.ilk"
	-@erase "$(OUTDIR)\WindowsTest.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

CPP_PROJ=/nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\WindowsTest.res" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\WindowsTest.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=user32.lib gdi32.lib comdlg32.lib shell32.lib /nologo /subsystem:windows /incremental:yes /pdb:"$(OUTDIR)\WindowsTest.pdb" /debug /machine:I386 /out:"$(OUTDIR)\WindowsTest.exe" /pdbtype:sept 
LINK32_OBJS= \
	"$(INTDIR)\BatchConv.obj" \
	"$(INTDIR)\WindowsTest.obj" \
	"$(INTDIR)\WindowsTest.res" \
	"..\..\lib\debug\ilut-d.lib" \
	"..\..\lib\debug\DevIL-d.lib" \
	"..\..\lib\debug\ilu-d.lib"

"$(OUTDIR)\WindowsTest.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "WindowsTest - Win32 Dynamic"

OUTDIR=.\WindowsTest___Win32_Dynamic
INTDIR=.\WindowsTest___Win32_Dynamic
# Begin Custom Macros
OutDir=.\WindowsTest___Win32_Dynamic
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\WindowsTest.exe"

!ELSE 

ALL : "ILU - Win32 Dynamic" "IL - Win32 Dynamic" "ILUT - Win32 Dynamic" "$(OUTDIR)\WindowsTest.exe"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :"ILUT - Win32 DynamicCLEAN" "IL - Win32 DynamicCLEAN" "ILU - Win32 DynamicCLEAN" 
!ELSE 
CLEAN :
!ENDIF 
	-@erase "$(INTDIR)\BatchConv.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\WindowsTest.obj"
	-@erase "$(INTDIR)\WindowsTest.res"
	-@erase "$(OUTDIR)\WindowsTest.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\WindowsTest.res" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\WindowsTest.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=user32.lib gdi32.lib comdlg32.lib shell32.lib /nologo /subsystem:windows /incremental:no /pdb:"$(OUTDIR)\WindowsTest.pdb" /machine:I386 /out:"$(OUTDIR)\WindowsTest.exe" 
LINK32_OBJS= \
	"$(INTDIR)\BatchConv.obj" \
	"$(INTDIR)\WindowsTest.obj" \
	"$(INTDIR)\WindowsTest.res" \
	"..\..\lib\ilut-l.lib" \
	"..\..\lib\DevIL-l.lib" \
	"..\..\lib\ilu-l.lib"

"$(OUTDIR)\WindowsTest.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 

.c{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.c{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("WindowsTest.dep")
!INCLUDE "WindowsTest.dep"
!ELSE 
!MESSAGE Warning: cannot find "WindowsTest.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "WindowsTest - Win32 Release" || "$(CFG)" == "WindowsTest - Win32 Debug" || "$(CFG)" == "WindowsTest - Win32 Dynamic"
SOURCE=.\BatchConv.cpp

"$(INTDIR)\BatchConv.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\WindowsTest.cpp

"$(INTDIR)\WindowsTest.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\WindowsTest.rc

"$(INTDIR)\WindowsTest.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)


!IF  "$(CFG)" == "WindowsTest - Win32 Release"

"ILUT - Win32 Release" : 
   cd "\DevIL\src-ILUT\msvc"
   $(MAKE) /$(MAKEFLAGS) /F ".\ILUT.mak" CFG="ILUT - Win32 Release" 
   cd "..\..\Examples\WindowsTest"

"ILUT - Win32 ReleaseCLEAN" : 
   cd "\DevIL\src-ILUT\msvc"
   $(MAKE) /$(MAKEFLAGS) /F ".\ILUT.mak" CFG="ILUT - Win32 Release" RECURSE=1 CLEAN 
   cd "..\..\Examples\WindowsTest"

!ELSEIF  "$(CFG)" == "WindowsTest - Win32 Debug"

"ILUT - Win32 Debug" : 
   cd "\DevIL\src-ILUT\msvc"
   $(MAKE) /$(MAKEFLAGS) /F ".\ILUT.mak" CFG="ILUT - Win32 Debug" 
   cd "..\..\Examples\WindowsTest"

"ILUT - Win32 DebugCLEAN" : 
   cd "\DevIL\src-ILUT\msvc"
   $(MAKE) /$(MAKEFLAGS) /F ".\ILUT.mak" CFG="ILUT - Win32 Debug" RECURSE=1 CLEAN 
   cd "..\..\Examples\WindowsTest"

!ELSEIF  "$(CFG)" == "WindowsTest - Win32 Dynamic"

"ILUT - Win32 Dynamic" : 
   cd "\DevIL\src-ILUT\msvc"
   $(MAKE) /$(MAKEFLAGS) /F ".\ILUT.mak" CFG="ILUT - Win32 Dynamic" 
   cd "..\..\Examples\WindowsTest"

"ILUT - Win32 DynamicCLEAN" : 
   cd "\DevIL\src-ILUT\msvc"
   $(MAKE) /$(MAKEFLAGS) /F ".\ILUT.mak" CFG="ILUT - Win32 Dynamic" RECURSE=1 CLEAN 
   cd "..\..\Examples\WindowsTest"

!ENDIF 

!IF  "$(CFG)" == "WindowsTest - Win32 Release"

"IL - Win32 Release" : 
   cd "\DevIL\src-IL\msvc"
   $(MAKE) /$(MAKEFLAGS) /F ".\IL.mak" CFG="IL - Win32 Release" 
   cd "..\..\Examples\WindowsTest"

"IL - Win32 ReleaseCLEAN" : 
   cd "\DevIL\src-IL\msvc"
   $(MAKE) /$(MAKEFLAGS) /F ".\IL.mak" CFG="IL - Win32 Release" RECURSE=1 CLEAN 
   cd "..\..\Examples\WindowsTest"

!ELSEIF  "$(CFG)" == "WindowsTest - Win32 Debug"

"IL - Win32 Debug" : 
   cd "\DevIL\src-IL\msvc"
   $(MAKE) /$(MAKEFLAGS) /F ".\IL.mak" CFG="IL - Win32 Debug" 
   cd "..\..\Examples\WindowsTest"

"IL - Win32 DebugCLEAN" : 
   cd "\DevIL\src-IL\msvc"
   $(MAKE) /$(MAKEFLAGS) /F ".\IL.mak" CFG="IL - Win32 Debug" RECURSE=1 CLEAN 
   cd "..\..\Examples\WindowsTest"

!ELSEIF  "$(CFG)" == "WindowsTest - Win32 Dynamic"

"IL - Win32 Dynamic" : 
   cd "\DevIL\src-IL\msvc"
   $(MAKE) /$(MAKEFLAGS) /F ".\IL.mak" CFG="IL - Win32 Dynamic" 
   cd "..\..\Examples\WindowsTest"

"IL - Win32 DynamicCLEAN" : 
   cd "\DevIL\src-IL\msvc"
   $(MAKE) /$(MAKEFLAGS) /F ".\IL.mak" CFG="IL - Win32 Dynamic" RECURSE=1 CLEAN 
   cd "..\..\Examples\WindowsTest"

!ENDIF 

!IF  "$(CFG)" == "WindowsTest - Win32 Release"

"ILU - Win32 Release" : 
   cd "\DevIL\src-ILU\msvc"
   $(MAKE) /$(MAKEFLAGS) /F ".\ILU.mak" CFG="ILU - Win32 Release" 
   cd "..\..\Examples\WindowsTest"

"ILU - Win32 ReleaseCLEAN" : 
   cd "\DevIL\src-ILU\msvc"
   $(MAKE) /$(MAKEFLAGS) /F ".\ILU.mak" CFG="ILU - Win32 Release" RECURSE=1 CLEAN 
   cd "..\..\Examples\WindowsTest"

!ELSEIF  "$(CFG)" == "WindowsTest - Win32 Debug"

"ILU - Win32 Debug" : 
   cd "\DevIL\src-ILU\msvc"
   $(MAKE) /$(MAKEFLAGS) /F ".\ILU.mak" CFG="ILU - Win32 Debug" 
   cd "..\..\Examples\WindowsTest"

"ILU - Win32 DebugCLEAN" : 
   cd "\DevIL\src-ILU\msvc"
   $(MAKE) /$(MAKEFLAGS) /F ".\ILU.mak" CFG="ILU - Win32 Debug" RECURSE=1 CLEAN 
   cd "..\..\Examples\WindowsTest"

!ELSEIF  "$(CFG)" == "WindowsTest - Win32 Dynamic"

"ILU - Win32 Dynamic" : 
   cd "\DevIL\src-ILU\msvc"
   $(MAKE) /$(MAKEFLAGS) /F ".\ILU.mak" CFG="ILU - Win32 Dynamic" 
   cd "..\..\Examples\WindowsTest"

"ILU - Win32 DynamicCLEAN" : 
   cd "\DevIL\src-ILU\msvc"
   $(MAKE) /$(MAKEFLAGS) /F ".\ILU.mak" CFG="ILU - Win32 Dynamic" RECURSE=1 CLEAN 
   cd "..\..\Examples\WindowsTest"

!ENDIF 


!ENDIF 

