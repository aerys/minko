//-----------------------------------------------------------------------------
//
// ImageLib Sources
// Copyright (C) 2000-2017 by Denton Woods
// Last modified: 02/16/2002
//
// Filename: src-IL/src/il_main.cpp
//
// Description: Startup function
//
//-----------------------------------------------------------------------------


#include "il_internal.h"

/* Only needed for MSVC++ unless extended to actually do something =) */
#if defined(_WIN32) && defined(_MSC_VER)
BOOL APIENTRY DllMain(HANDLE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
	hModule;  ul_reason_for_call;  lpReserved;

	if (ul_reason_for_call == DLL_PROCESS_ATTACH) {
		//ilInit();
	}

	return TRUE;
}
#endif


