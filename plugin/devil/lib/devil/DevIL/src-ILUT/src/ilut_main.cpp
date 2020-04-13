//-----------------------------------------------------------------------------
//
// ImageLib Utility Toolkit Sources
// Copyright (C) 2000-2017 by Denton Woods
// Last modified: 05/28/2001
//
// Filename: src-ILUT/src/ilut_main.cpp
//
// Description: Startup functions
//
//-----------------------------------------------------------------------------


#include "ilut_internal.h"

#ifdef _WIN32
#ifndef IL_STATIC_LIB
	//#define WIN32_LEAN_AND_MEAN
	#include <windows.h>


#ifdef _WIN32
	#if (defined(IL_USE_PRAGMA_LIBS))
		#if defined(_MSC_VER) || defined(__BORLANDC__)
			#pragma comment(lib, "ILU.lib")
		#endif
	#endif
#endif



BOOL APIENTRY DllMain(HANDLE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
	hModule;  lpReserved;

	// only initialize when attached to a new process. setup can cause errors in OpenIL
	// when called on a per thread basis
	if (ul_reason_for_call == DLL_PROCESS_ATTACH) {
		//ilutInit();
	}

	return TRUE;
}
#endif

#else  // Should check if gcc?

// Should be able to condense this...
static void GccMain() __attribute__((constructor));
static void GccMain()
{
	//ilutInit();
}

#endif


void ILAPIENTRY ilutInit()
{
	ilutDefaultStates();  // Set states to their defaults
	// Can cause crashes if DevIL is not initialized yet

#ifdef ILUT_USE_OPENGL
	ilutGLInit();  // default renderer is OpenGL
#endif

#ifdef ILUT_USE_DIRECTX8
	ilutD3D8Init();
#endif

#ifdef ILUT_USE_DIRECTX9
	ilutD3D9Init();
#endif

	return;
}
