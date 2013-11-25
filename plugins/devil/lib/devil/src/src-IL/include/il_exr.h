//-----------------------------------------------------------------------------
//
// ImageLib Sources
// Copyright (C) 2000-2008 by Denton Woods
// Last modified: 08/29/2008
//
// Filename: src-IL/include/il_exr.h
//
// Description: Reads from an OpenEXR (.exr) file.
//
//-----------------------------------------------------------------------------


#ifndef EXR_H
#define EXR_H

#include "il_internal.h"
#include <ImfIO.h>


//using namespace Imf;  // Using this leads to errors with Microsoft's IStream.
						//   So it is better to just specify the namespace explicitly.


typedef struct EXRHEAD
{
	ILuint		MagicNumber;		// File signature (0x76, 0x2f, 0x31, 0x01)
	ILuint		Version;			// Treated as two bitfields
} IL_PACKSTRUCT EXRHEAD;

//@TODO: Should I just do these as enums?
#define EXR_UINT 0
#define EXR_HALF 1
#define EXR_FLOAT 2

#define EXR_NO_COMPRESSION    0
#define EXR_RLE_COMPRESSION   1
#define EXR_ZIPS_COMPRESSION  2
#define EXR_ZIP_COMPRESSION   3
#define EXR_PIZ_COMPRESSION   4
#define EXR_PXR24_COMPRESSION 5
#define EXR_B44_COMPRESSION   6
#define EXR_B44A_COMPRESSION  7


#ifdef __cplusplus
extern "C" {
#endif

ILboolean iIsValidExr();
ILboolean iCheckExr(EXRHEAD *Header);
ILboolean iLoadExrInternal();
ILboolean iSaveExrInternal();

#ifdef __cplusplus
}
#endif

class ilIStream : public Imf::IStream
{
	public:
		ilIStream(/*ILHANDLE Handle*/);
		virtual bool	read (char c[/*n*/], int n);
		// I don't think I need this one, since we are taking care of the file handles ourselves.
		//virtual char *	readMemoryMapped (int n);
		virtual Imf::Int64	tellg ();
		virtual void	seekg (Imf::Int64 Pos);
		virtual void	clear ();

	protected:

	private:
};

class ilOStream : public Imf::OStream
{
	public:
		ilOStream(/*ILHANDLE Handle*/);
		virtual void	write (const char c[/*n*/], int n);
		// I don't think I need this one, since we are taking care of the file handles ourselves.
		//virtual char *	readMemoryMapped (int n);
		virtual Imf::Int64	tellp ();
		virtual void	seekp (Imf::Int64 Pos);

	protected:

	private:
};

#endif//EXR_H
