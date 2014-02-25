//-----------------------------------------------------------------------------
//
// ImageLib Sources
// Copyright (C) 2000-2009 by Denton Woods
// Last modified: 01/04/2009
//
// Filename: src-IL/src/il_files.c
//
// Description: File handling for DevIL
//
//-----------------------------------------------------------------------------


#define __FILES_C
#include "il_internal.h"
#include <stdarg.h>


// All specific to the next set of functions
ILboolean	ILAPIENTRY iEofFile(void);
ILboolean	ILAPIENTRY iEofLump(void);
ILint		ILAPIENTRY iGetcFile(void);
ILint		ILAPIENTRY iGetcLump(void);
ILuint		ILAPIENTRY iReadFile(void *Buffer, ILuint Size, ILuint Number);
ILuint		ILAPIENTRY iReadLump(void *Buffer, const ILuint Size, const ILuint Number);
ILint		ILAPIENTRY iSeekRFile(ILint Offset, ILuint Mode);
ILint		ILAPIENTRY iSeekRLump(ILint Offset, ILuint Mode);
ILint		ILAPIENTRY iSeekWFile(ILint Offset, ILuint Mode);
ILint		ILAPIENTRY iSeekWLump(ILint Offset, ILuint Mode);
ILuint		ILAPIENTRY iTellRFile(void);
ILuint		ILAPIENTRY iTellRLump(void);
ILuint		ILAPIENTRY iTellWFile(void);
ILuint		ILAPIENTRY iTellWLump(void);
ILint		ILAPIENTRY iPutcFile(ILubyte Char);
ILint		ILAPIENTRY iPutcLump(ILubyte Char);
ILint		ILAPIENTRY iWriteFile(const void *Buffer, ILuint Size, ILuint Number);
ILint		ILAPIENTRY iWriteLump(const void *Buffer, ILuint Size, ILuint Number);
ILHANDLE	FileRead = NULL, FileWrite = NULL;
const void *ReadLump = NULL;
void 		*WriteLump = NULL;
ILuint		ReadLumpPos = 0, ReadLumpSize = 0, ReadFileStart = 0, WriteFileStart = 0;
ILuint		WriteLumpPos = 0, WriteLumpSize = 0;

fGetcProc	GetcProcCopy;
fReadProc	ReadProcCopy;
fSeekRProc	SeekProcCopy;
fTellRProc	TellProcCopy;
ILHANDLE	(ILAPIENTRY *iopenCopy)(ILconst_string);
void		(ILAPIENTRY *icloseCopy)(ILHANDLE);

fPutcProc	PutcProcCopy;
fSeekWProc	SeekWProcCopy;
fTellWProc	TellWProcCopy;
fWriteProc	WriteProcCopy;
ILHANDLE	(ILAPIENTRY *iopenwCopy)(ILconst_string);
void		(ILAPIENTRY *iclosewCopy)(ILHANDLE);

ILboolean	UseCache = IL_FALSE;
ILubyte		*Cache = NULL;
ILuint		CacheSize, CachePos, CacheStartPos, CacheBytesRead;

// "Fake" size functions
//  Definitions are in il_size.c.
ILint		ILAPIENTRY iSizeSeek(ILint Offset, ILuint Mode);
ILuint		ILAPIENTRY iSizeTell(void);
ILint		ILAPIENTRY iSizePutc(ILubyte Char);
ILint		ILAPIENTRY iSizeWrite(const void *Buffer, ILuint Size, ILuint Number);

// Just preserves the current read functions and replaces
//	the current read functions with the default read funcs.
void ILAPIENTRY iPreserveReadFuncs()
{
	// Create backups
	GetcProcCopy = GetcProc;
	ReadProcCopy = ReadProc;
	SeekProcCopy = SeekRProc;
	TellProcCopy = TellRProc;
	iopenCopy = iopenr;
	icloseCopy = icloser;

	// Set the standard procs to read
	ilResetRead();

	return;
}


// Restores the read functions - must be used after iPreserveReadFuncs().
void ILAPIENTRY iRestoreReadFuncs()
{
	GetcProc = GetcProcCopy;
	ReadProc = ReadProcCopy;
	SeekRProc = SeekProcCopy;
	TellRProc = TellProcCopy;
	iopenr = iopenCopy;
	icloser = icloseCopy;

	return;
}


// Just preserves the current read functions and replaces
//	the current read functions with the default read funcs.
void ILAPIENTRY iPreserveWriteFuncs()
{
	// Create backups
	PutcProcCopy = PutcProc;
	SeekWProcCopy = SeekWProc;
	TellWProcCopy = TellWProc;
	WriteProcCopy = WriteProc;
	iopenwCopy = iopenw;
	iclosewCopy = iclosew;

	// Set the standard procs to write
	ilResetWrite();

	return;
}


// Restores the read functions - must be used after iPreserveReadFuncs().
void ILAPIENTRY iRestoreWriteFuncs()
{
	PutcProc = PutcProcCopy;
	SeekWProc = SeekWProcCopy;
	TellWProc = TellWProcCopy;
	WriteProc = WriteProcCopy;
	iopenw = iopenwCopy;
	iclosew = iclosewCopy;

	return;
}


// Next 7 functions are the default read functions

ILHANDLE ILAPIENTRY iDefaultOpenR(ILconst_string FileName)
{
#ifndef _UNICODE
	return (ILHANDLE)fopen((char*)FileName, "rb");
#else
	// Windows has a different function, _wfopen, to open UTF16 files,
	//  whereas Linux just uses fopen for its UTF8 files.
	#ifdef _WIN32
		return (ILHANDLE)_wfopen(FileName, L"rb");
	#else
		return (ILHANDLE)fopen((char*)FileName, "rb");
	#endif
#endif//UNICODE
}


void ILAPIENTRY iDefaultCloseR(ILHANDLE Handle)
{
	fclose((FILE*)Handle);
	return;
}


ILboolean ILAPIENTRY iDefaultEof(ILHANDLE Handle)
{
	ILuint OrigPos, FileSize;

	// Find out the filesize for checking for the end of file
	OrigPos = itell();
	iseek(0, IL_SEEK_END);
	FileSize = itell();
	iseek(OrigPos, IL_SEEK_SET);

	if (itell() >= FileSize)
		return IL_TRUE;
	return IL_FALSE;
}


ILint ILAPIENTRY iDefaultGetc(ILHANDLE Handle)
{
	ILint Val;

	if (!UseCache) {
		Val = fgetc((FILE*)Handle);
		if (Val == IL_EOF)
			ilSetError(IL_FILE_READ_ERROR);
	}
	else {
		Val = 0;
		if (iread(&Val, 1, 1) != 1)
			return IL_EOF;
	}
	return Val;
}


ILint ILAPIENTRY iDefaultRead(void *Buffer, ILuint Size, ILuint Number, ILHANDLE Handle)
{
	return (ILint)fread(Buffer, Size, Number, (FILE*)Handle);
}


ILint ILAPIENTRY iDefaultRSeek(ILHANDLE Handle, ILint Offset, ILint Mode)
{
	return fseek((FILE*)Handle, Offset, Mode);
}


ILint ILAPIENTRY iDefaultWSeek(ILHANDLE Handle, ILint Offset, ILint Mode)
{
	return fseek((FILE*)Handle, Offset, Mode);
}


ILint ILAPIENTRY iDefaultRTell(ILHANDLE Handle)
{
	return ftell((FILE*)Handle);
}


ILint ILAPIENTRY iDefaultWTell(ILHANDLE Handle)
{
	return ftell((FILE*)Handle);
}


ILHANDLE ILAPIENTRY iDefaultOpenW(ILconst_string FileName)
{
#ifndef _UNICODE
	return (ILHANDLE)fopen((char*)FileName, "wb");
#else
	// Windows has a different function, _wfopen, to open UTF16 files,
	//  whereas Linux just uses fopen.
	#ifdef _WIN32
		return (ILHANDLE)_wfopen(FileName, L"wb");
	#else
		return (ILHANDLE)fopen((char*)FileName, "wb");
	#endif
#endif//UNICODE
}


void ILAPIENTRY iDefaultCloseW(ILHANDLE Handle)
{
	fclose((FILE*)Handle);
	return;
}


ILint ILAPIENTRY iDefaultPutc(ILubyte Char, ILHANDLE Handle)
{
	return fputc(Char, (FILE*)Handle);
}


ILint ILAPIENTRY iDefaultWrite(const void *Buffer, ILuint Size, ILuint Number, ILHANDLE Handle)
{
	return (ILint)fwrite(Buffer, Size, Number, (FILE*)Handle);
}


void ILAPIENTRY ilResetRead()
{
	ilSetRead(iDefaultOpenR, iDefaultCloseR, iDefaultEof, iDefaultGetc, 
				iDefaultRead, iDefaultRSeek, iDefaultRTell);
	return;
}


void ILAPIENTRY ilResetWrite()
{
	ilSetWrite(iDefaultOpenW, iDefaultCloseW, iDefaultPutc,
				iDefaultWSeek, iDefaultWTell, iDefaultWrite);
	return;
}


//! Allows you to override the default file-reading functions.
void ILAPIENTRY ilSetRead(fOpenRProc Open, fCloseRProc Close, fEofProc Eof, fGetcProc Getc, fReadProc Read, fSeekRProc Seek, fTellRProc Tell)
{
	iopenr    = Open;
	icloser   = Close;
	EofProc   = Eof;
	GetcProc  = Getc;
	ReadProc  = Read;
	SeekRProc = Seek;
	TellRProc = Tell;

	return;
}


//! Allows you to override the default file-writing functions.
void ILAPIENTRY ilSetWrite(fOpenRProc Open, fCloseRProc Close, fPutcProc Putc, fSeekWProc Seek, fTellWProc Tell, fWriteProc Write)
{
	iopenw    = Open;
	iclosew   = Close;
	PutcProc  = Putc;
	WriteProc = Write;
	SeekWProc = Seek;
	TellWProc = Tell;

	return;
}


// Tells DevIL that we're reading from a file, not a lump
void iSetInputFile(ILHANDLE File)
{
	ieof  = iEofFile;
	igetc = iGetcFile;
	iread = iReadFile;
	iseek = iSeekRFile;
	itell = iTellRFile;
	FileRead = File;
	ReadFileStart = itell();
}


// Tells DevIL that we're reading from a lump, not a file
void iSetInputLump(const void *Lump, ILuint Size)
{
	ieof  = iEofLump;
	igetc = iGetcLump;
	iread = iReadLump;
	iseek = iSeekRLump;
	itell = iTellRLump;
	ReadLump = Lump;
	ReadLumpPos = 0;
	ReadLumpSize = Size;
}


// Tells DevIL that we're writing to a file, not a lump
void iSetOutputFile(ILHANDLE File)
{
	// Helps with ilGetLumpPos().
	WriteLump = NULL;
	WriteLumpPos = 0;
	WriteLumpSize = 0;

	iputc  = iPutcFile;
	iseekw = iSeekWFile;
	itellw = iTellWFile;
	iwrite = iWriteFile;
	FileWrite = File;
}


// This is only called by ilDetermineSize.  Associates iputc, etc. with
//  "fake" writing functions in il_size.c.
void iSetOutputFake(void)
{
	iputc  = iSizePutc;
	iseekw = iSizeSeek;
	itellw = iSizeTell;
	iwrite = iSizeWrite;
	return;
}


// Tells DevIL that we're writing to a lump, not a file
void iSetOutputLump(void *Lump, ILuint Size)
{
	// In this case, ilDetermineSize is currently trying to determine the
	//  output buffer size.  It already has the write functions it needs.
	if (Lump == NULL)
		return;

	iputc  = iPutcLump;
	iseekw = iSeekWLump;
	itellw = iTellWLump;
	iwrite = iWriteLump;
	WriteLump = Lump;
	WriteLumpPos = 0;
	WriteLumpSize = Size;
}


ILuint ILAPIENTRY ilGetLumpPos()
{
	if (WriteLump)
		return WriteLumpPos;
	return 0;
}


ILuint ILAPIENTRY ilprintf(const char *Line, ...)
{
	char	Buffer[2048];  // Hope this is large enough
	va_list	VaLine;
	ILuint	i;

	va_start(VaLine, Line);
	vsprintf(Buffer, Line, VaLine);
	va_end(VaLine);

	i = ilCharStrLen(Buffer);
	iwrite(Buffer, 1, i);

	return i;
}


// To pad zeros where needed...
void ipad(ILuint NumZeros)
{
	ILuint i = 0;
	for (; i < NumZeros; i++)
		iputc(0);
	return;
}


//
// The rest of the functions following in this file are quite
//	self-explanatory, except where commented.
//

// Next 12 functions are the default write functions

ILboolean ILAPIENTRY iEofFile(void)
{
	return EofProc((FILE*)FileRead);
}


ILboolean ILAPIENTRY iEofLump(void)
{
	if (ReadLumpSize)
		return (ReadLumpPos >= ReadLumpSize);
	return IL_FALSE;
}


ILint ILAPIENTRY iGetcFile(void)
{
	if (!UseCache) {
		return GetcProc(FileRead);
	}
	if (CachePos >= CacheSize) {
		iPreCache(CacheSize);
	}

	CacheBytesRead++;
	return Cache[CachePos++];
}


ILint ILAPIENTRY iGetcLump(void)
{
	// If ReadLumpSize is 0, don't even check to see if we've gone past the bounds.
	if (ReadLumpSize > 0) {
		if (ReadLumpPos + 1 > ReadLumpSize) {
			ReadLumpPos--;
			ilSetError(IL_FILE_READ_ERROR);
			return IL_EOF;
		}
	}

	return *((ILubyte*)ReadLump + ReadLumpPos++);
}


ILuint ILAPIENTRY iReadFile(void *Buffer, ILuint Size, ILuint Number)
{
	ILuint	TotalBytes = 0, BytesCopied;
	ILuint	BuffSize = Size * Number;
	ILuint	NumRead;

	if (!UseCache) {
		NumRead = ReadProc(Buffer, Size, Number, FileRead);
		if (NumRead != Number)
			ilSetError(IL_FILE_READ_ERROR);
		return NumRead;
	}

	/*if (Cache == NULL || CacheSize == 0) {  // Shouldn't happen, but we check anyway.
		return ReadProc(Buffer, Size, Number, FileRead);
	}*/

	if (BuffSize < CacheSize - CachePos) {
		memcpy(Buffer, Cache + CachePos, BuffSize);
		CachePos += BuffSize;
		CacheBytesRead += BuffSize;
		if (Size != 0)
			BuffSize /= Size;
		return BuffSize;
	}
	else {
		while (TotalBytes < BuffSize) {
			// If loop through more than once, after first, CachePos is 0.
			if (TotalBytes + CacheSize - CachePos > BuffSize)
				BytesCopied = BuffSize - TotalBytes;
			else
				BytesCopied = CacheSize - CachePos;

			memcpy((ILubyte*)Buffer + TotalBytes, Cache + CachePos, BytesCopied);
			TotalBytes += BytesCopied;
			CachePos += BytesCopied;
			if (TotalBytes < BuffSize) {
				iPreCache(CacheSize);
			}
		}
	}

	// DW: Changed on 12-27-2008.  Was causing the position to go too far if the
	//     cache was smaller than the buffer.
	//CacheBytesRead += TotalBytes;
	CacheBytesRead = CachePos;
	if (Size != 0)
		TotalBytes /= Size;
	if (TotalBytes != Number)
		ilSetError(IL_FILE_READ_ERROR);
	return TotalBytes;
}


ILuint ILAPIENTRY iReadLump(void *Buffer, const ILuint Size, const ILuint Number)
{
	ILuint i, ByteSize = IL_MIN( Size*Number, ReadLumpSize-ReadLumpPos);

	for (i = 0; i < ByteSize; i++) {
		*((ILubyte*)Buffer + i) = *((ILubyte*)ReadLump + ReadLumpPos + i);
		if (ReadLumpSize > 0) {  // ReadLumpSize is too large to care about apparently
			if (ReadLumpPos + i > ReadLumpSize) {
				ReadLumpPos += i;
				if (i != Number)
					ilSetError(IL_FILE_READ_ERROR);
				return i;
			}
		}
	}

	ReadLumpPos += i;
	if (Size != 0)
		i /= Size;
	if (i != Number)
		ilSetError(IL_FILE_READ_ERROR);
	return i;
}


ILboolean iPreCache(ILuint Size)
{
	// Reading from a memory lump, so don't cache.
	if (iread == iReadLump) {
		//iUnCache();  // DW: Removed 06-10-2002.
		return IL_TRUE;
	}

	if (Cache) {
		ifree(Cache);
	}

	if (Size == 0) {
		Size = 1;
	}

	Cache = (ILubyte*)ialloc(Size);
	if (Cache == NULL) {
		return IL_FALSE;
	}

	UseCache = IL_FALSE;
	CacheStartPos = itell();
	CacheSize = iread(Cache, 1, Size);
	if (CacheSize != Size)
		ilGetError();  // Get rid of the IL_FILE_READ_ERROR.

	//2003-09-09: uncommented the following line to prevent
	//an infinite loop in ilPreCache()
	CacheSize = Size;
	CachePos = 0;
	UseCache = IL_TRUE;
	CacheBytesRead = 0;

	return IL_TRUE;
}


void iUnCache()
{
	//changed 2003-09-01:
	//make iUnCache smart enough to return if
	//no cache is used
	if (!UseCache)
		return;

	if (iread == iReadLump)
		return;

	CacheSize = 0;
	CachePos = 0;
	if (Cache) {
		ifree(Cache);
		Cache = NULL;
	}
	UseCache = IL_FALSE;

	iseek(CacheStartPos + CacheBytesRead, IL_SEEK_SET);

	return;
}


ILint ILAPIENTRY iSeekRFile(ILint Offset, ILuint Mode)
{
	if (Mode == IL_SEEK_SET)
		Offset += ReadFileStart;  // This allows us to use IL_SEEK_SET in the middle of a file.
	return SeekRProc(FileRead, Offset, Mode);
}


// Returns 1 on error, 0 on success
ILint ILAPIENTRY iSeekRLump(ILint Offset, ILuint Mode)
{
	switch (Mode)
	{
		case IL_SEEK_SET:
			if (Offset > (ILint)ReadLumpSize)
				return 1;
			ReadLumpPos = Offset;
			break;

		case IL_SEEK_CUR:
			if (ReadLumpPos + Offset > ReadLumpSize)
				return 1;
			ReadLumpPos += Offset;
			break;

		case IL_SEEK_END:
			if (Offset > 0)
				return 1;
			// Should we use >= instead?
			if (abs(Offset) > (ILint)ReadLumpSize)  // If ReadLumpSize == 0, too bad
				return 1;
			ReadLumpPos = ReadLumpSize + Offset;
			break;

		default:
			return 1;
	}

	return 0;
}


ILuint ILAPIENTRY iTellRFile(void)
{
	return TellRProc(FileRead);
}


ILuint ILAPIENTRY iTellRLump(void)
{
	return ReadLumpPos;
}


ILHANDLE ILAPIENTRY iGetFile(void)
{
	return FileRead;
}


const ILubyte* ILAPIENTRY iGetLump(void) {
	return (ILubyte*)ReadLump;
}



// Next 4 functions are the default write functions

ILint ILAPIENTRY iPutcFile(ILubyte Char)
{
	return PutcProc(Char, FileWrite);
}


ILint ILAPIENTRY iPutcLump(ILubyte Char)
{
	if (WriteLumpPos >= WriteLumpSize)
		return IL_EOF;  // IL_EOF
	*((ILubyte*)(WriteLump) + WriteLumpPos++) = Char;
	return Char;
}


ILint ILAPIENTRY iWriteFile(const void *Buffer, ILuint Size, ILuint Number)
{
	ILuint NumWritten;
	NumWritten = WriteProc(Buffer, Size, Number, FileWrite);
	if (NumWritten != Number) {
		ilSetError(IL_FILE_WRITE_ERROR);
		return 0;
	}
	return NumWritten;
}


ILint ILAPIENTRY iWriteLump(const void *Buffer, ILuint Size, ILuint Number)
{
	ILuint SizeBytes = Size * Number;
	ILuint i = 0;

	for (; i < SizeBytes; i++) {
		if (WriteLumpSize > 0) {
			if (WriteLumpPos + i >= WriteLumpSize) {  // Should we use > instead?
				ilSetError(IL_FILE_WRITE_ERROR);
				WriteLumpPos += i;
				return i;
			}
		}

		*((ILubyte*)WriteLump + WriteLumpPos + i) = *((ILubyte*)Buffer + i);
	}

	WriteLumpPos += SizeBytes;
	
	return SizeBytes;
}


ILint ILAPIENTRY iSeekWFile(ILint Offset, ILuint Mode)
{
	if (Mode == IL_SEEK_SET)
		Offset += WriteFileStart;  // This allows us to use IL_SEEK_SET in the middle of a file.
	return SeekWProc(FileWrite, Offset, Mode);
}


// Returns 1 on error, 0 on success
ILint ILAPIENTRY iSeekWLump(ILint Offset, ILuint Mode)
{
	switch (Mode)
	{
		case IL_SEEK_SET:
			if (Offset > (ILint)WriteLumpSize)
				return 1;
			WriteLumpPos = Offset;
			break;

		case IL_SEEK_CUR:
			if (WriteLumpPos + Offset > WriteLumpSize)
				return 1;
			WriteLumpPos += Offset;
			break;

		case IL_SEEK_END:
			if (Offset > 0)
				return 1;
			// Should we use >= instead?
			if (abs(Offset) > (ILint)WriteLumpSize)  // If WriteLumpSize == 0, too bad
				return 1;
			WriteLumpPos = WriteLumpSize + Offset;
			break;

		default:
			return 1;
	}

	return 0;
}


ILuint ILAPIENTRY iTellWFile(void)
{
	return TellWProc(FileWrite);
}


ILuint ILAPIENTRY iTellWLump(void)
{
	return WriteLumpPos;
}
