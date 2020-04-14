
#ifndef INTERNAL_H
#define INTERNAL_H

#include <string.h>

#ifdef _MSC_VER
	#if _MSC_VER > 1000
		#pragma once
		#pragma intrinsic(memcpy)
		#pragma intrinsic(memset)
		//pragma comment(linker, "/NODEFAULTLIB:libc")
		#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers

		#ifdef _DEBUG 
			#define _CRTDBG_MAP_ALLOC
			#include <stdlib.h>
			#ifndef _WIN32_WCE
				#include <crtdbg.h>
			#endif
		#endif
	#endif // _MSC_VER > 1000
#endif

#define _IL_BUILD_LIBRARY
#define _ILU_BUILD_LIBRARY

// Standard headers
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>

// Local headers
#define _IL_BUILD_LIBRARY
#define _ILU_BUILD_LIBRARY

#include <IL/ilu.h>
#include <IL/devil_internal_exports.h>


// From DevIL's internal.h:
#ifdef _WIN32_WCE
	#include <windows.h>
	#define IL_TEXT(s) ((char*)TEXT(s))
#elif _WIN32
	#include <windows.h>
	#define IL_TEXT(s) TEXT(s)
#else
	#define IL_TEXT(s) s
	#define TEXT(s) s
#endif

extern ILimage *iluCurImage;


// Useful global variables
extern const ILdouble	IL_PI;
extern const ILdouble	IL_DEGCONV;


#ifdef ILU_INTERNAL_C
#undef NOINLINE
#undef INLINE
#define INLINE
#endif

// Internal functions
ILfloat ilCos(ILfloat Angle);
ILfloat ilSin(ILfloat Angle);
ILint ilRound(ILfloat Num);

/*#ifndef NOINLINE
ILfloat ilCos(ILfloat Angle);
ILfloat ilSin(ILfloat Angle);
ILint ilRound(ILfloat Num);
#endif*/



ILuint	iluScaleAdvanced(ILuint Width, ILuint Height, ILenum Filter);
ILubyte	*iScanFill(void);


#endif//INTERNAL_H
