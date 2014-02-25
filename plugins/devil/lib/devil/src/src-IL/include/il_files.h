//-----------------------------------------------------------------------------
//
// ImageLib Sources
// Copyright (C) 2000-2009 by Denton Woods
// Last modified: 01/04/2009
//
// Filename: src-IL/include/il_files.h
//
// Description: File handling for DevIL
//
//-----------------------------------------------------------------------------

#ifndef FILES_H
#define FILES_H

#if defined (__FILES_C)
#define __FILES_EXTERN
#else
#define __FILES_EXTERN extern
#endif
#include <IL/il.h>


__FILES_EXTERN void ILAPIENTRY iPreserveReadFuncs(void);
__FILES_EXTERN void ILAPIENTRY iRestoreReadFuncs(void);
__FILES_EXTERN void ILAPIENTRY iPreserveWriteFuncs(void);
__FILES_EXTERN void ILAPIENTRY iRestoreWriteFuncs(void);

__FILES_EXTERN fEofProc		EofProc;
__FILES_EXTERN fGetcProc	GetcProc;
__FILES_EXTERN fReadProc	ReadProc;
__FILES_EXTERN fSeekRProc	SeekRProc;
__FILES_EXTERN fSeekWProc	SeekWProc;
__FILES_EXTERN fTellRProc	TellRProc;
__FILES_EXTERN fTellWProc	TellWProc;
__FILES_EXTERN fPutcProc	PutcProc;
__FILES_EXTERN fWriteProc	WriteProc;

__FILES_EXTERN ILHANDLE			ILAPIENTRY iDefaultOpen(ILconst_string FileName);
__FILES_EXTERN void		        ILAPIENTRY iDefaultClose(ILHANDLE Handle);
__FILES_EXTERN ILint			ILAPIENTRY iDefaultGetc(ILHANDLE Handle);
__FILES_EXTERN ILint			ILAPIENTRY iDefaultRead(void *Buffer, ILuint Size, ILuint Number, ILHANDLE Handle);
__FILES_EXTERN ILint			ILAPIENTRY iDefaultSeekR(ILHANDLE Handle, ILint Offset, ILint Mode);
__FILES_EXTERN ILint			ILAPIENTRY iDefaultSeekW(ILHANDLE Handle, ILint Offset, ILint Mode);
__FILES_EXTERN ILint			ILAPIENTRY iDefaultTellR(ILHANDLE Handle);
__FILES_EXTERN ILint			ILAPIENTRY iDefaultTellW(ILHANDLE Handle);
__FILES_EXTERN ILint			ILAPIENTRY iDefaultPutc(ILubyte Char, ILHANDLE Handle);
__FILES_EXTERN ILint			ILAPIENTRY iDefaultWrite(const void *Buffer, ILuint Size, ILuint Number, ILHANDLE Handle);

__FILES_EXTERN void				iSetInputFile(ILHANDLE File);
__FILES_EXTERN void				iSetInputLump(const void *Lump, ILuint Size);
__FILES_EXTERN ILboolean		(ILAPIENTRY *ieof)(void);
__FILES_EXTERN ILHANDLE			(ILAPIENTRY *iopenr)(ILconst_string);
__FILES_EXTERN void				(ILAPIENTRY *icloser)(ILHANDLE);
__FILES_EXTERN ILint			(ILAPIENTRY *igetc)(void);
__FILES_EXTERN ILuint			(ILAPIENTRY *iread)(void *Buffer, ILuint Size, ILuint Number);
__FILES_EXTERN ILint			(ILAPIENTRY *iseek)(ILint Offset, ILuint Mode);
__FILES_EXTERN ILuint			(ILAPIENTRY *itell)(void);

__FILES_EXTERN void				iSetOutputFile(ILHANDLE File);
__FILES_EXTERN void				iSetOutputLump(void *Lump, ILuint Size);
__FILES_EXTERN void				iSetOutputFake(void);
__FILES_EXTERN void				(ILAPIENTRY *iclosew)(ILHANDLE);
__FILES_EXTERN ILHANDLE			(ILAPIENTRY *iopenw)(ILconst_string);
__FILES_EXTERN ILint			(ILAPIENTRY *iputc)(ILubyte Char);
__FILES_EXTERN ILint			(ILAPIENTRY *iseekw)(ILint Offset, ILuint Mode);
__FILES_EXTERN ILuint			(ILAPIENTRY *itellw)(void);
__FILES_EXTERN ILint			(ILAPIENTRY *iwrite)(const void *Buffer, ILuint Size, ILuint Number);
 
__FILES_EXTERN ILHANDLE			ILAPIENTRY iGetFile(void);
__FILES_EXTERN const ILubyte*	ILAPIENTRY iGetLump(void);

__FILES_EXTERN ILuint			ILAPIENTRY ilprintf(const char *, ...);
__FILES_EXTERN void				ipad(ILuint NumZeros);

__FILES_EXTERN ILboolean		iPreCache(ILuint Size);
__FILES_EXTERN void				iUnCache(void);

#endif//FILES_H
