//-----------------------------------------------------------------------------
//
// ImageLib Sources
// Copyright (C) 2000-2008 by Denton Woods
// Last modified: 11/08/2008
//
// Filename: src-IL/src/il_internal.c
//
// Description: Internal stuff for DevIL
//
//-----------------------------------------------------------------------------


#include "il_internal.h"
#include <string.h>
#include <stdlib.h>


ILimage *iCurImage = NULL;


/* Siigron: added this for Linux... a #define should work, but for some reason
	it doesn't (anyone who knows why?) */
#if !_WIN32 || (_WIN32 && __GNUC__) // Cygwin
	int stricmp(const char *src1, const char *src2)
	{
		return strcasecmp(src1, src2);
	}
	int strnicmp(const char *src1, const char *src2, size_t max)
	{
		return strncasecmp(src1, src2, max);
	}
#elif _WIN32_WCE
	int stricmp(const char *src1, const char *src2)
	{
		return _stricmp(src1, src2);
	}
	int strnicmp(const char *src1, const char *src2, size_t max)
	{
		return _strnicmp(src1, src2, max);
	}
#endif /* _WIN32 */

#ifdef _WIN32_WCE
	char *strdup(const char *src)
	{
		return _strdup(src);
	}
#endif//_WIN32_WCE


#ifdef _UNICODE
	int iStrCmp(ILconst_string src1, ILconst_string src2)
	{
		return wcsicmp(src1, src2);
	}
#else
	int iStrCmp(ILconst_string src1, ILconst_string src2)
	{
		return stricmp(src1, src2);
	}
#endif


//! Glut's portability.txt says to use this...
ILstring ilStrDup(ILconst_string Str)
{
	ILstring copy;

	copy = (ILstring)ialloc((ilStrLen(Str) + 1) * sizeof(ILchar));
	if (copy == NULL)
		return NULL;
	iStrCpy(copy, Str);
	return copy;
}


// Because MSVC++'s version is too stupid to check for NULL...
ILuint ilStrLen(ILconst_string Str)
{
	ILconst_string eos = Str;

	if (Str == NULL)
		return 0;

	while (*eos++);

	return((int)(eos - Str - 1));
}


// Because MSVC++'s version is too stupid to check for NULL...
//  Passing NULL to strlen will definitely cause a crash.
ILuint ilCharStrLen(const char *Str)
{
	const char *eos = Str;

	if (Str == NULL)
		return 0;

	while (*eos++);

	return((int)(eos - Str - 1));
}


// Simple function to test if a filename has a given extension, disregarding case
ILboolean iCheckExtension(ILconst_string Arg, ILconst_string Ext)
{
	ILboolean PeriodFound = IL_FALSE;
	ILint i, Len;
	ILstring Argu = (ILstring)Arg;

	if (Arg == NULL || Ext == NULL || !ilStrLen(Arg) || !ilStrLen(Ext))  // if not a good filename/extension, exit early
		return IL_FALSE;

	Len = ilStrLen(Arg);
	Argu += Len;  // start at the end

	for (i = Len; i >= 0; i--) {
		if (*Argu == '.') {  // try to find a period 
			PeriodFound = IL_TRUE;
			break;
		}
		Argu--;
	}

	if (!PeriodFound)  // if no period, no extension
		return IL_FALSE;

	if (!iStrCmp(Argu+1, Ext))  // extension and ext match?
		return IL_TRUE;

	return IL_FALSE;  // if all else fails, return IL_FALSE
}


ILstring iGetExtension(ILconst_string FileName)
{
	ILboolean PeriodFound = IL_FALSE;
	ILstring Ext = (ILstring)FileName;
	ILint i, Len = ilStrLen(FileName);

	if (FileName == NULL || !Len)  // if not a good filename/extension, exit early
		return NULL;

	Ext += Len;  // start at the end

	for (i = Len; i >= 0; i--) {
		if (*Ext == '.') {  // try to find a period 
			PeriodFound = IL_TRUE;
			break;
		}
		Ext--;
	}

	if (!PeriodFound)  // if no period, no extension
		return NULL;

	return Ext+1;
}


// Checks if the file exists
ILboolean iFileExists(ILconst_string FileName)
{
#if (!defined(_UNICODE) || !defined(_WIN32))
	FILE *CheckFile = fopen(FileName, "rb");
#else // Windows uses _wfopen instead.
	FILE *CheckFile = _wfopen(FileName, L"rb");
#endif//_UNICODE

	if (CheckFile) {
		fclose(CheckFile);
		return IL_TRUE;
	}
	return IL_FALSE;
}


// Last time I tried, MSVC++'s fgets() was really really screwy
ILbyte *iFgets(char *buffer, ILuint maxlen)
{
	ILuint	counter = 0;
	ILint	temp = '\0';

	while ((temp = igetc()) && temp != '\n' && temp != IL_EOF && counter < maxlen) {
		buffer[counter] = temp;
		counter++;
	}
	buffer[counter] = '\0';
	
	if (temp == IL_EOF && counter == 0)  // Only return NULL if no data was "got".
		return NULL;

	return (ILbyte*)buffer;
}


// A fast integer squareroot, completely accurate for x < 289.
// Taken from http://atoms.org.uk/sqrt/
// There is also a version that is accurate for all integers
// < 2^31, if we should need it

static int table[] = {
	0,    16,  22,  27,  32,  35,  39,  42,  45,  48,  50,  53,  55,  57,
	59,   61,  64,  65,  67,  69,  71,  73,  75,  76,  78,  80,  81,  83,
	84,   86,  87,  89,  90,  91,  93,  94,  96,  97,  98,  99, 101, 102,
	103, 104, 106, 107, 108, 109, 110, 112, 113, 114, 115, 116, 117, 118,
	119, 120, 121, 122, 123, 124, 125, 126, 128, 128, 129, 130, 131, 132,
	133, 134, 135, 136, 137, 138, 139, 140, 141, 142, 143, 144, 144, 145,
	146, 147, 148, 149, 150, 150, 151, 152, 153, 154, 155, 155, 156, 157,
	158, 159, 160, 160, 161, 162, 163, 163, 164, 165, 166, 167, 167, 168,
	169, 170, 170, 171, 172, 173, 173, 174, 175, 176, 176, 177, 178, 178,
	179, 180, 181, 181, 182, 183, 183, 184, 185, 185, 186, 187, 187, 188,
	189, 189, 190, 191, 192, 192, 193, 193, 194, 195, 195, 196, 197, 197,
	198, 199, 199, 200, 201, 201, 202, 203, 203, 204, 204, 205, 206, 206,
	207, 208, 208, 209, 209, 210, 211, 211, 212, 212, 213, 214, 214, 215,
	215, 216, 217, 217, 218, 218, 219, 219, 220, 221, 221, 222, 222, 223,
	224, 224, 225, 225, 226, 226, 227, 227, 228, 229, 229, 230, 230, 231,
	231, 232, 232, 233, 234, 234, 235, 235, 236, 236, 237, 237, 238, 238,
	239, 240, 240, 241, 241, 242, 242, 243, 243, 244, 244, 245, 245, 246,
	246, 247, 247, 248, 248, 249, 249, 250, 250, 251, 251, 252, 252, 253,
	253, 254, 254, 255
};

int iSqrt(int x) {
	if (x >= 0x10000) {
		if (x >= 0x1000000) {
			if (x >= 0x10000000) {
				if (x >= 0x40000000) {
					return (table[x >> 24] << 8);
				} else {
					return (table[x >> 22] << 7);
				}
			} else if (x >= 0x4000000) {
				return (table[x >> 20] << 6);
			} else {
				return (table[x >> 18] << 5);
			}
		} else if (x >= 0x100000) {
			if (x >= 0x400000) {
				return (table[x >> 16] << 4);
			} else {
				return (table[x >> 14] << 3);
			}
		} else if (x >= 0x40000) {
			return (table[x >> 12] << 2);
		} else {
			return (table[x >> 10] << 1);
		}
	} else if (x >= 0x100) {
		if (x >= 0x1000) {
			if (x >= 0x4000) {
				return (table[x >> 8]);
			} else {
				return (table[x >> 6] >> 1);
			}
		} else if (x >= 0x400) {
			return (table[x >> 4] >> 2);
		} else {
			return (table[x >> 2] >> 3);
		}
	} else if (x >= 0) {
		return table[x] >> 4;
	}

	//hm, x was negative....
	return -1;
}

