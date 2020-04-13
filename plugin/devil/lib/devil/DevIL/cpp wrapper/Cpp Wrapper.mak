# Microsoft Developer Studio Generated NMAKE File, Based on Cpp Wrapper.dsp
!IF "$(CFG)" == ""
CFG=Cpp Wrapper - Win32 Debug
!MESSAGE No configuration specified. Defaulting to Cpp Wrapper - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "Cpp Wrapper - Win32 Release" && "$(CFG)" != "Cpp Wrapper - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Cpp Wrapper.mak" CFG="Cpp Wrapper - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Cpp Wrapper - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "Cpp Wrapper - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Cpp Wrapper - Win32 Release"

OUTDIR=.\../lib
INTDIR=.\obj
# Begin Custom Macros
OutDir=.\../lib
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\il_wrap.lib"

!ELSE 

ALL : "OpenILUT - Win32 Release" "OpenILU - Win32 Release" "OpenIL - Win32 Release" "$(OUTDIR)\il_wrap.lib"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :"OpenIL - Win32 ReleaseCLEAN" "OpenILU - Win32 ReleaseCLEAN" "OpenILUT - Win32 ReleaseCLEAN" 
!ELSE 
CLEAN :
!ENDIF 
	-@erase "$(INTDIR)\il_wrap.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\il_wrap.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

CPP_PROJ=/nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\Cpp Wrapper.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"$(OUTDIR)\il_wrap.lib" 
LIB32_OBJS= \
	"$(INTDIR)\il_wrap.obj" \
	"$(OUTDIR)\DevIL.lib" \
	"$(OUTDIR)\ilu.lib" \
	"$(OUTDIR)\ilut.lib"

"$(OUTDIR)\il_wrap.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ELSEIF  "$(CFG)" == "Cpp Wrapper - Win32 Debug"

OUTDIR=.\../lib/debug
INTDIR=.\obj/debug
# Begin Custom Macros
OutDir=.\../lib/debug
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\il_wrap.lib"

!ELSE 

ALL : "OpenILUT - Win32 Debug" "OpenILU - Win32 Debug" "OpenIL - Win32 Debug" "$(OUTDIR)\il_wrap.lib"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :"OpenIL - Win32 DebugCLEAN" "OpenILU - Win32 DebugCLEAN" "OpenILUT - Win32 DebugCLEAN" 
!ELSE 
CLEAN :
!ENDIF 
	-@erase "$(INTDIR)\il_wrap.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\il_wrap.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

CPP_PROJ=/nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /Fp"$(INTDIR)\Cpp Wrapper.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\Cpp Wrapper.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"$(OUTDIR)\il_wrap.lib" 
LIB32_OBJS= \
	"$(INTDIR)\il_wrap.obj" \
	"$(OUTDIR)\DevIL-d.lib" \
	"$(OUTDIR)\ilu-d.lib" \
	"$(OUTDIR)\ilut-d.lib"

"$(OUTDIR)\il_wrap.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
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
!IF EXISTS("Cpp Wrapper.dep")
!INCLUDE "Cpp Wrapper.dep"
!ELSE 
!MESSAGE Warning: cannot find "Cpp Wrapper.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "Cpp Wrapper - Win32 Release" || "$(CFG)" == "Cpp Wrapper - Win32 Debug"
SOURCE=.\il_wrap.cpp

"$(INTDIR)\il_wrap.obj" : $(SOURCE) "$(INTDIR)"


!IF  "$(CFG)" == "Cpp Wrapper - Win32 Release"

"OpenIL - Win32 Release" : 
   cd "\ImageLib\OpenIL"
   $(MAKE) /$(MAKEFLAGS) /F .\OpenIL.mak CFG="OpenIL - Win32 Release" 
   cd "..\Cpp Wrapper"

"OpenIL - Win32 ReleaseCLEAN" : 
   cd "\ImageLib\OpenIL"
   $(MAKE) /$(MAKEFLAGS) /F .\OpenIL.mak CFG="OpenIL - Win32 Release" RECURSE=1 CLEAN 
   cd "..\Cpp Wrapper"

!ELSEIF  "$(CFG)" == "Cpp Wrapper - Win32 Debug"

"OpenIL - Win32 Debug" : 
   cd "\ImageLib\OpenIL"
   $(MAKE) /$(MAKEFLAGS) /F .\OpenIL.mak CFG="OpenIL - Win32 Debug" 
   cd "..\Cpp Wrapper"

"OpenIL - Win32 DebugCLEAN" : 
   cd "\ImageLib\OpenIL"
   $(MAKE) /$(MAKEFLAGS) /F .\OpenIL.mak CFG="OpenIL - Win32 Debug" RECURSE=1 CLEAN 
   cd "..\Cpp Wrapper"

!ENDIF 

!IF  "$(CFG)" == "Cpp Wrapper - Win32 Release"

"OpenILU - Win32 Release" : 
   cd "\ImageLib\OpenILU"
   $(MAKE) /$(MAKEFLAGS) /F .\OpenILU.mak CFG="OpenILU - Win32 Release" 
   cd "..\Cpp Wrapper"

"OpenILU - Win32 ReleaseCLEAN" : 
   cd "\ImageLib\OpenILU"
   $(MAKE) /$(MAKEFLAGS) /F .\OpenILU.mak CFG="OpenILU - Win32 Release" RECURSE=1 CLEAN 
   cd "..\Cpp Wrapper"

!ELSEIF  "$(CFG)" == "Cpp Wrapper - Win32 Debug"

"OpenILU - Win32 Debug" : 
   cd "\ImageLib\OpenILU"
   $(MAKE) /$(MAKEFLAGS) /F .\OpenILU.mak CFG="OpenILU - Win32 Debug" 
   cd "..\Cpp Wrapper"

"OpenILU - Win32 DebugCLEAN" : 
   cd "\ImageLib\OpenILU"
   $(MAKE) /$(MAKEFLAGS) /F .\OpenILU.mak CFG="OpenILU - Win32 Debug" RECURSE=1 CLEAN 
   cd "..\Cpp Wrapper"

!ENDIF 

!IF  "$(CFG)" == "Cpp Wrapper - Win32 Release"

"OpenILUT - Win32 Release" : 
   cd "\ImageLib\OpenILUT"
   $(MAKE) /$(MAKEFLAGS) /F .\OpenILUT.mak CFG="OpenILUT - Win32 Release" 
   cd "..\Cpp Wrapper"

"OpenILUT - Win32 ReleaseCLEAN" : 
   cd "\ImageLib\OpenILUT"
   $(MAKE) /$(MAKEFLAGS) /F .\OpenILUT.mak CFG="OpenILUT - Win32 Release" RECURSE=1 CLEAN 
   cd "..\Cpp Wrapper"

!ELSEIF  "$(CFG)" == "Cpp Wrapper - Win32 Debug"

"OpenILUT - Win32 Debug" : 
   cd "\ImageLib\OpenILUT"
   $(MAKE) /$(MAKEFLAGS) /F .\OpenILUT.mak CFG="OpenILUT - Win32 Debug" 
   cd "..\Cpp Wrapper"

"OpenILUT - Win32 DebugCLEAN" : 
   cd "\ImageLib\OpenILUT"
   $(MAKE) /$(MAKEFLAGS) /F .\OpenILUT.mak CFG="OpenILUT - Win32 Debug" RECURSE=1 CLEAN 
   cd "..\Cpp Wrapper"

!ENDIF 


!ENDIF 

