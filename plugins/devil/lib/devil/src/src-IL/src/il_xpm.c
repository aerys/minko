//-----------------------------------------------------------------------------
//
// ImageLib Sources
// Copyright (C) 2000-2009 by Denton Woods
// Last modified: 01/04/2009
//
// Filename: src-IL/src/il_xpm.c
//
// Description: Reads from an .xpm file.
//
//-----------------------------------------------------------------------------


#include "il_internal.h"
#ifndef IL_NO_XPM
#include <ctype.h>


//If this is defined, only xpm files with 1 char/pixel
//can be loaded. They load somewhat faster then, though
//(not much).
//#define XPM_DONT_USE_HASHTABLE

ILboolean iIsValidXpm(void);
ILboolean iLoadXpmInternal(void);
ILint XpmGetsInternal(ILubyte *Buffer, ILint MaxLen);

//! Checks if the file specified in FileName is a valid XPM file.
ILboolean ilIsValidXpm(ILconst_string FileName)
{
	ILHANDLE	XpmFile;
	ILboolean	bXpm = IL_FALSE;
	
	if (!iCheckExtension(FileName, IL_TEXT("xpm"))) {
		ilSetError(IL_INVALID_EXTENSION);
		return bXpm;
	}
	
	XpmFile = iopenr(FileName);
	if (XpmFile == NULL) {
		ilSetError(IL_COULD_NOT_OPEN_FILE);
		return bXpm;
	}
	
	bXpm = ilIsValidXpmF(XpmFile);
	icloser(XpmFile);
	
	return bXpm;
}


//! Checks if the ILHANDLE contains a valid XPM file at the current position.
ILboolean ilIsValidXpmF(ILHANDLE File)
{
	ILuint		FirstPos;
	ILboolean	bRet;
	
	iSetInputFile(File);
	FirstPos = itell();
	bRet = iIsValidXpm();
	iseek(FirstPos, IL_SEEK_SET);
	
	return bRet;
}


//! Checks if Lump is a valid XPM lump.
ILboolean ilIsValidXpmL(const void *Lump, ILuint Size)
{
	iSetInputLump(Lump, Size);
	return iIsValidXpm();
}


// Internal function to get the header and check it.
ILboolean iIsValidXpm(void)
{
	ILubyte	Buffer[10];
	ILuint	Pos = itell();

	XpmGetsInternal(Buffer, 10);
	iseek(Pos, IL_SEEK_SET);  // Restore position

	if (strncmp("/* XPM */", (char*)Buffer, strlen("/* XPM */")))
		return IL_FALSE;
	return IL_TRUE;
}


// Reads an .xpm file
ILboolean ilLoadXpm(ILconst_string FileName)
{
	ILHANDLE	XpmFile;
	ILboolean	bXpm = IL_FALSE;

	XpmFile = iopenr(FileName);
	if (XpmFile == NULL) {
		ilSetError(IL_COULD_NOT_OPEN_FILE);
		return bXpm;
	}

	iSetInputFile(XpmFile);
	bXpm = ilLoadXpmF(XpmFile);
	icloser(XpmFile);

	return bXpm;
}


//! Reads an already-opened .xpm file
ILboolean ilLoadXpmF(ILHANDLE File)
{
	ILuint		FirstPos;
	ILboolean	bRet;

	iSetInputFile(File);
	FirstPos = itell();
	bRet = iLoadXpmInternal();
	iseek(FirstPos, IL_SEEK_SET);

	return bRet;
}


//! Reads from a memory "lump" that contains an .xpm
ILboolean ilLoadXpmL(const void *Lump, ILuint Size)
 {
	iSetInputLump(Lump, Size);
	return iLoadXpmInternal();
}


typedef ILubyte XpmPixel[4];

#define XPM_MAX_CHAR_PER_PIXEL 2


#ifndef XPM_DONT_USE_HASHTABLE

//The following hash table code was inspired by the xpm
//loading code of xv, one of the best image viewers of X11

//For xpm files with more than one character/pixel, it is
//impractical to use a simple lookup table for the
//character-to-color mapping (because the table requires
//2^(chars/pixel) entries, this is quite big).
//Because of that, a hash table is used for the mapping.
//The hash table has 257 entries, and collisions are
//resolved by chaining.

//257 is the smallest prime > 256
#define XPM_HASH_LEN 257

typedef struct XPMHASHENTRY
{
	ILubyte ColourName[XPM_MAX_CHAR_PER_PIXEL];
	XpmPixel ColourValue;
	struct XPMHASHENTRY *Next;
} XPMHASHENTRY;


static ILuint XpmHash(const ILubyte* name, int len)
{
	ILint i, sum;
	for (sum = i = 0; i < len; ++i)
		sum += name[i];
	return sum % XPM_HASH_LEN;
}


XPMHASHENTRY** XpmCreateHashTable()
{
	XPMHASHENTRY** Table =
		(XPMHASHENTRY**)ialloc(XPM_HASH_LEN*sizeof(XPMHASHENTRY*));
	if (Table != NULL)
		memset(Table, 0, XPM_HASH_LEN*sizeof(XPMHASHENTRY*));
	return Table;
}


void XpmDestroyHashTable(XPMHASHENTRY **Table)
{
	ILint i;
	XPMHASHENTRY* Entry;

	for (i = 0; i < XPM_HASH_LEN; ++i) {
		while (Table[i] != NULL) {
			Entry = Table[i]->Next;
			ifree(Table[i]);
			Table[i] = Entry;
		}
	}

	ifree(Table);
}


void XpmInsertEntry(XPMHASHENTRY **Table, const ILubyte* Name, int Len, XpmPixel Colour)
{
	XPMHASHENTRY* NewEntry;
	ILuint Index;
	Index = XpmHash(Name, Len);

	NewEntry = (XPMHASHENTRY*)ialloc(sizeof(XPMHASHENTRY));
	if (NewEntry != NULL) {
		NewEntry->Next = Table[Index];
		memcpy(NewEntry->ColourName, Name, Len);
		memcpy(NewEntry->ColourValue, Colour, sizeof(Colour));
		Table[Index] = NewEntry;
	}
}


void XpmGetEntry(XPMHASHENTRY **Table, const ILubyte* Name, int Len, XpmPixel Colour)
{
	XPMHASHENTRY* Entry;
	ILuint Index;

	Index = XpmHash(Name, Len);
	Entry = Table[Index];
	while (Entry != NULL && strncmp((char*)(Entry->ColourName), (char*)Name, Len) != 0)
		Entry = Entry->Next;

	if (Entry != NULL)
		memcpy(Colour, Entry->ColourValue, sizeof(Colour));
}

#endif //XPM_DONT_USE_HASHTABLE


ILint XpmGetsInternal(ILubyte *Buffer, ILint MaxLen)
{
	ILint	i = 0, Current;

	if (ieof())
		return IL_EOF;

	while ((Current = igetc()) != IL_EOF && i < MaxLen - 1) {
		if (Current == IL_EOF)
			return 0;
		if (Current == '\n') //unix line ending
			break;

		if (Current == '\r') { //dos/mac line ending
			Current = igetc();
			if (Current == '\n') //dos line ending
				break;

			if (Current == IL_EOF)
				break;

			Buffer[i++] = (ILubyte)Current;
			continue;
		}
		Buffer[i++] = (ILubyte)Current;
	}

	Buffer[i++] = 0;

	return i;
}


ILint XpmGets(ILubyte *Buffer, ILint MaxLen)
{
	ILint		Size, i, j;
	ILboolean	NotComment = IL_FALSE, InsideComment = IL_FALSE;

	do {
		Size = XpmGetsInternal(Buffer, MaxLen);
		if (Size == IL_EOF)
			return IL_EOF;

		//skip leading whitespace (sometimes there's whitespace
		//before a comment or before the pixel data)

		for(i = 0; i < Size && isspace(Buffer[i]); ++i) ;
		Size = Size - i;
		for(j = 0; j < Size; ++j)
			Buffer[j] = Buffer[j + i];

		if (Size == 0)
			continue;

		if (Buffer[0] == '/' && Buffer[1] == '*') {
			for (i = 2; i < Size; i++) {
				if (Buffer[i] == '*' && Buffer[i+1] == '/') {
					break;
				}
			}
			if (i >= Size)
				InsideComment = IL_TRUE;
		}
		else if (InsideComment) {
			for (i = 0; i < Size; i++) {
				if (Buffer[i] == '*' && Buffer[i+1] == '/') {
					break;
				}
			}
			if (i < Size)
				InsideComment = IL_FALSE;
		}
		else {
			NotComment = IL_TRUE;
		}
	} while (!NotComment);

	return Size;
}


ILint XpmGetInt(ILubyte *Buffer, ILint Size, ILint *Position)
{
	char		Buff[1024];
	ILint		i, j;
	ILboolean	IsInNum = IL_FALSE;

	for (i = *Position, j = 0; i < Size; i++) {
		if (isdigit(Buffer[i])) {
			IsInNum = IL_TRUE;
			Buff[j++] = Buffer[i];
		}
		else {
			if (IsInNum) {
				Buff[j] = 0;
				*Position = i;
				return atoi(Buff);
			}
		}
	}

	return -1;
}


ILboolean XpmPredefCol(char *Buff, XpmPixel *Colour)
{
	ILint len;
	ILint val = 128;

	if (!stricmp(Buff, "none")) {
		(*Colour)[0] = 0;
		(*Colour)[1] = 0;
		(*Colour)[2] = 0;
		(*Colour)[3] = 0;
		return IL_TRUE;
	}

	(*Colour)[3] = 255;

	if (!stricmp(Buff, "black")) {
		(*Colour)[0] = 0;
		(*Colour)[1] = 0;
		(*Colour)[2] = 0;
		return IL_TRUE;
	}
	if (!stricmp(Buff, "white")) {
		(*Colour)[0] = 255;
		(*Colour)[1] = 255;
		(*Colour)[2] = 255;
		return IL_TRUE;
	}
	if (!stricmp(Buff, "red")) {
		(*Colour)[0] = 255;
		(*Colour)[1] = 0;
		(*Colour)[2] = 0;
		return IL_TRUE;
	}
	if (!stricmp(Buff, "green")) {
		(*Colour)[0] = 0;
		(*Colour)[1] = 255;
		(*Colour)[2] = 0;
		return IL_TRUE;
	}
	if (!stricmp(Buff, "blue")) {
		(*Colour)[0] = 0;
		(*Colour)[1] = 0;
		(*Colour)[2] = 255;
		return IL_TRUE;
	}
	if (!stricmp(Buff, "yellow")) {
		(*Colour)[0] = 255;
		(*Colour)[1] = 255;
		(*Colour)[2] = 0;
		return IL_TRUE;
	}
	if (!stricmp(Buff, "cyan")) {
		(*Colour)[0] = 0;
		(*Colour)[1] = 255;
		(*Colour)[2] = 255;
		return IL_TRUE;
	}
	if (!stricmp(Buff, "gray")) {
		(*Colour)[0] = 128;
		(*Colour)[1] = 128;
		(*Colour)[2] = 128;
		return IL_TRUE;
	}

	//check for grayXXX codes (added 20040218)
	len = ilCharStrLen(Buff);
	if (len >= 4) {
		if (Buff[0] == 'g' || Buff[0] == 'G'
			|| Buff[1] == 'r' || Buff[1] == 'R'
			|| Buff[2] == 'a' || Buff[2] == 'A'
			|| Buff[3] == 'y' || Buff[3] == 'Y') {
			if (isdigit(Buff[4])) { // isdigit returns false on '\0'
				val = Buff[4] - '0';
				if (isdigit(Buff[5])) {
					val = val*10 + Buff[5] - '0';
					if (isdigit(Buff[6]))
						val = val*10 + Buff[6] - '0';
				}
				val = (255*val)/100;
			}
			(*Colour)[0] = (ILubyte)val;
			(*Colour)[1] = (ILubyte)val;
			(*Colour)[2] = (ILubyte)val;
			return IL_TRUE;
		}
	}


	// Unknown colour string, so use black
	// (changed 20040218)
	(*Colour)[0] = 0;
	(*Colour)[1] = 0;
	(*Colour)[2] = 0;

	return IL_FALSE;
}


#ifndef XPM_DONT_USE_HASHTABLE
ILboolean XpmGetColour(ILubyte *Buffer, ILint Size, int Len, XPMHASHENTRY **Table)
#else
ILboolean XpmGetColour(ILubyte *Buffer, ILint Size, int Len, XpmPixel* Colours)
#endif
{
	ILint		i = 0, j, strLen = 0;
	ILubyte		ColBuff[3];
	char		Buff[1024];

	XpmPixel	Colour;
	ILubyte		Name[XPM_MAX_CHAR_PER_PIXEL];

	for ( ; i < Size; i++) {
		if (Buffer[i] == '\"')
			break;
	}
	i++;  // Skip the quotes.

	if (i >= Size)
		return IL_FALSE;

	// Get the characters.
	for (j = 0; j < Len; ++j) {
		Name[j] = Buffer[i++];
	}

	// Skip to the colour definition.
	for ( ; i < Size; i++) {
		if (Buffer[i] == 'c')
			break;
	}
	i++;  // Skip the 'c'.

	if (i >= Size || Buffer[i] != ' ') { // no 'c' found...assume black
#ifndef XPM_DONT_USE_HASHTABLE
		memset(Colour, 0, sizeof(Colour));
		Colour[3] = 255;
		XpmInsertEntry(Table, Name, Len, Colour);
#else
		memset(Colours[Name[0]], 0, sizeof(Colour));
		Colours[Name[0]][3] = 255;
#endif
		return IL_TRUE;
	}

	for ( ; i < Size; i++) {
		if (Buffer[i] != ' ')
			break;
	}

	if (i >= Size)
		return IL_FALSE;

	if (Buffer[i] == '#') {
		// colour string may 4 digits/color or 1 digit/color
		// (added 20040218) TODO: is isxdigit() ANSI???
		++i;
		while (i + strLen < Size && isxdigit(Buffer[i + strLen]))
			++strLen;

		for (j = 0; j < 3; j++) {
			if (strLen >= 10) { // 4 digits
				ColBuff[0] = Buffer[i + j*4];
				ColBuff[1] = Buffer[i + j*4 + 1];
			}
			else if (strLen >= 8) { // 3 digits
				ColBuff[0] = Buffer[i + j*3];
				ColBuff[1] = Buffer[i + j*3 + 1];
			}
			else if (strLen >= 6) { // 2 digits
				ColBuff[0] = Buffer[i + j*2];
				ColBuff[1] = Buffer[i + j*2 + 1];
			}
			else if(j < strLen) { // 1 digit, strLen >= 1
				ColBuff[0] = Buffer[i + j];
				ColBuff[1] = 0;
			}

			ColBuff[2] = 0; // add terminating '\0' char
			Colour[j] = (ILubyte)strtol((char*)ColBuff, NULL, 16);
		}
		Colour[3] = 255;  // Full alpha.
	}
	else {
		for (j = 0; i < Size; i++) {
			if (!isalnum(Buffer[i]))
				break;
			Buff[j++] = Buffer[i];
		}
		Buff[j] = 0;

		if (i >= Size)
			return IL_FALSE;

		if (!XpmPredefCol(Buff, &Colour))

			return IL_FALSE;
	}


#ifndef XPM_DONT_USE_HASHTABLE
	XpmInsertEntry(Table, Name, Len, Colour);
#else
	memcpy(Colours[Name[0]], Colour, sizeof(Colour));
#endif
	return IL_TRUE;
}


ILboolean iLoadXpmInternal()
{
#define BUFFER_SIZE 2000
	ILubyte			Buffer[BUFFER_SIZE], *Data;
	ILint			Size, Pos, Width, Height, NumColours, i, x, y;

	ILint			CharsPerPixel;

#ifndef XPM_DONT_USE_HASHTABLE
	XPMHASHENTRY	**HashTable;
#else
	XpmPixel	*Colours;
	ILint		Offset;
#endif

	Size = XpmGetsInternal(Buffer, BUFFER_SIZE);
	if (strncmp("/* XPM */", (char*)Buffer, strlen("/* XPM */"))) {
		ilSetError(IL_INVALID_FILE_HEADER);
		return IL_FALSE;
	}

	Size = XpmGets(Buffer, BUFFER_SIZE);
	// @TODO:  Actually check the variable name here.

	Size = XpmGets(Buffer, BUFFER_SIZE);
	Pos = 0;
	Width = XpmGetInt(Buffer, Size, &Pos);
	Height = XpmGetInt(Buffer, Size, &Pos);
	NumColours = XpmGetInt(Buffer, Size, &Pos);

	CharsPerPixel = XpmGetInt(Buffer, Size, &Pos);

#ifdef XPM_DONT_USE_HASHTABLE
	if (CharsPerPixel != 1) {
		ilSetError(IL_FORMAT_NOT_SUPPORTED);
		return IL_FALSE;
	}
#endif

	if (CharsPerPixel > XPM_MAX_CHAR_PER_PIXEL
		|| Width*CharsPerPixel > BUFFER_SIZE) {
		ilSetError(IL_FORMAT_NOT_SUPPORTED);
		return IL_FALSE;
	}

#ifndef XPM_DONT_USE_HASHTABLE
	HashTable = XpmCreateHashTable();
	if (HashTable == NULL)
		return IL_FALSE;
#else
	Colours = ialloc(256 * sizeof(XpmPixel));
	if (Colours == NULL)
		return IL_FALSE;
#endif

	for (i = 0; i < NumColours; i++) {
		Size = XpmGets(Buffer, BUFFER_SIZE);
#ifndef XPM_DONT_USE_HASHTABLE
		if (!XpmGetColour(Buffer, Size, CharsPerPixel, HashTable)) {
			XpmDestroyHashTable(HashTable);
#else
		if (!XpmGetColour(Buffer, Size, CharsPerPixel, Colours)) {
			ifree(Colours);
#endif
			return IL_FALSE;
		}
	}
	
	if (!ilTexImage(Width, Height, 1, 4, IL_RGBA, IL_UNSIGNED_BYTE, NULL)) {
#ifndef XPM_DONT_USE_HASHTABLE
		XpmDestroyHashTable(HashTable);
#else
		ifree(Colours);
#endif
		return IL_FALSE;
	}

	Data = iCurImage->Data;

	for (y = 0; y < Height; y++) {
		Size = XpmGets(Buffer, BUFFER_SIZE);
		for (x = 0; x < Width; x++) {
#ifndef XPM_DONT_USE_HASHTABLE
			XpmGetEntry(HashTable, &Buffer[1 + x*CharsPerPixel], CharsPerPixel, &Data[(x << 2)]);
#else
			Offset = (x << 2);
			Data[Offset + 0] = Colours[Buffer[x + 1]][0];
			Data[Offset + 1] = Colours[Buffer[x + 1]][1];
			Data[Offset + 2] = Colours[Buffer[x + 1]][2];
			Data[Offset + 3] = Colours[Buffer[x + 1]][3];
#endif
		}

		Data += iCurImage->Bps;
	}

	//added 20040218
	iCurImage->Origin = IL_ORIGIN_UPPER_LEFT;


#ifndef XPM_DONT_USE_HASHTABLE
	XpmDestroyHashTable(HashTable);
#else
	ifree(Colours);
#endif
	return IL_TRUE;

#undef BUFFER_SIZE
}

#endif//IL_NO_XPM

