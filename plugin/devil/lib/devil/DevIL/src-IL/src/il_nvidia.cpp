//-----------------------------------------------------------------------------
//
// ImageLib Sources
// Copyright (C) 2000-2017 by Denton Woods
// Last modified: 01/19/2009
//
// Filename: src-IL/src/il_nvidia.cpp
//
// Description: Implements access to the nVidia Texture Tools library.
//
//-----------------------------------------------------------------------------


#include "il_internal.h"
#include "il_dds.h"
#include <limits.h>


#ifdef IL_USE_DXTC_NVIDIA
#include <nvtt/nvtt.h>
#include <nvcore/Memory.h>

using namespace nvtt;

#if defined(_WIN32) && defined(IL_USE_PRAGMA_LIBS)
	#if defined(_MSC_VER) || defined(__BORLANDC__)
		#ifndef _DEBUG
			#pragma comment(lib, "nvcore.lib")
			#pragma comment(lib, "nvtt.lib")
		#else
			#pragma comment(lib, "nvcore-d.lib")
			#pragma comment(lib, "nvtt-d.lib")
		#endif
	#endif
#endif


struct ilOutputHandlerMem : public nvtt::OutputHandler
{
	ilOutputHandlerMem(ILuint Width, ILuint Height, ILenum DxtType)
	{
		Width = Width + (4 - (Width % 4)) % 4;    // Operates on 4x4 blocks,
		Height = Height + (4 - (Height % 4)) % 4; //  so gives extra room.
		
		switch (DxtType)
		{
			case IL_DXT1:
			case IL_DXT1A:
				Size = Width * Height / 2;
				break;
			case IL_DXT3:
			case IL_DXT5:
				Size = Width * Height;
				break;

			default:  // NVTT does not accept DXT2 or DXT4.
				// Should error somehow...
				break;
		}
		NewData = (ILubyte*)ialloc(Size);
		if (NewData == NULL)
			return;
		Temp = NewData;
	}

	virtual void beginImage(int size, int width, int height, int depth, int face, int miplevel)
	{
		// ignore.
	}
	virtual bool writeData(const void *data, int size)
	{
		memcpy(Temp, data, size);
		Temp += size;
		return true;
	}

	ILubyte	*NewData, *Temp;
	ILuint	Size;
};


//! Compresses data to a DXT format using nVidia's Texture Tools library.
//  The data must be in unsigned byte RGBA format.  The alpha channel will be ignored if DxtType is IL_DXT1.
//  DxtSize is used to return the size in bytes of the DXTC data returned.
ILAPI ILubyte* ILAPIENTRY ilNVidiaCompressDXT(ILubyte *Data, ILuint Width, ILuint Height, ILuint Depth, ILenum DxtFormat, ILuint *DxtSize)
{
	if (Data == NULL) {  // We cannot operate on a null pointer.
		ilSetError(IL_INVALID_PARAM);
		return NULL;
	}

	// The nVidia Texture Tools library does not support volume textures yet.
	if (Depth != 1) {
		ilSetError(IL_INVALID_PARAM);
		return NULL;
	}

	InputOptions inputOptions;
	inputOptions.setTextureLayout(TextureType_2D, Width, Height);
	inputOptions.setMipmapData(Data, Width, Height);
	inputOptions.setMipmapGeneration(false, -1);  //@TODO: Use this in certain cases.

	OutputOptions outputOptions;
	ilOutputHandlerMem outputHandler(Width, Height, DxtFormat);
	outputOptions.setOutputHeader(false);
	outputOptions.setOutputHandler(&outputHandler);

	if (outputHandler.NewData == NULL)
		return NULL;

	CompressionOptions compressionOptions;
	switch (DxtFormat)
	{
		case IL_DXT1:
			compressionOptions.setFormat(Format_DXT1);
			break;
		case IL_DXT1A:
			compressionOptions.setFormat(Format_DXT1a);
			break;
		case IL_DXT3:
			compressionOptions.setFormat(Format_DXT1);
			break;
		case IL_DXT5:
			compressionOptions.setFormat(Format_DXT5);
			break;
		default:  // Does not support DXT2 or DXT4.
			ilSetError(IL_INVALID_PARAM);
			break;
	}

	Compressor compressor;
	compressor.process(inputOptions, compressionOptions, outputOptions);

	*DxtSize = outputHandler.Size;
	return outputHandler.NewData;
}



//
//
// The following is just a repeat of above, but it works generically on file streams or lumps.
//  @TODO: Merge these two together.
//
//


struct ilOutputHandlerFile : public nvtt::OutputHandler
{
	ilOutputHandlerFile(ILuint Width, ILuint Height, ILenum DxtType)
	{
		return;
	}

	virtual void beginImage(int size, int width, int height, int depth, int face, int miplevel)
	{
		// ignore.
	}
	virtual bool writeData(const void *data, int size)
	{
		if (iwrite(data, 1, size) == size)
			return true;
		return false;
	}

};


//! Compresses data to a DXT format using nVidia's Texture Tools library.
//  This version is supposed to be completely internal to DevIL.
//  The data must be in unsigned byte RGBA format.  The alpha channel will be ignored if DxtType is IL_DXT1.
ILuint ilNVidiaCompressDXTFile(ILubyte *Data, ILuint Width, ILuint Height, ILuint Depth, ILenum DxtFormat)
{
	ILuint FilePos = itellw();

	// The nVidia Texture Tools library does not support volume textures yet.
	if (Depth != 1) {
		ilSetError(IL_INVALID_PARAM);
		return 0;
	}

	InputOptions inputOptions;
	inputOptions.setTextureLayout(TextureType_2D, Width, Height);
	inputOptions.setMipmapData(Data, Width, Height);
	inputOptions.setMipmapGeneration(false, -1);  //@TODO: Use this in certain cases.

	OutputOptions outputOptions;
	ilOutputHandlerFile outputHandler(Width, Height, DxtFormat);
	outputOptions.setOutputHeader(false);
	outputOptions.setOutputHandler(&outputHandler);

	CompressionOptions compressionOptions;
	switch (DxtFormat)
	{
		case IL_DXT1:
			compressionOptions.setFormat(Format_DXT1);
			break;
		case IL_DXT1A:
			compressionOptions.setFormat(Format_DXT1a);
			break;
		case IL_DXT3:
			compressionOptions.setFormat(Format_DXT1);
			break;
		case IL_DXT5:
			compressionOptions.setFormat(Format_DXT5);
			break;
		default:  // Does not support DXT2 or DXT4.
			ilSetError(IL_INVALID_PARAM);
			break;
	}

	Compressor compressor;
	compressor.process(inputOptions, compressionOptions, outputOptions);

	return itellw() - FilePos;  // Return the number of characters written.
}

#else
// Let's have this so that the function is always created and exported, even if it does nothing.
ILAPI ILubyte* ILAPIENTRY ilNVidiaCompressDXT(ILubyte *Data, ILuint Width, ILuint Height, ILuint Depth, ILenum DxtFormat, ILuint *DxtSize)
{
	//@TODO: Do we need to set an error message?
	return NULL;
}

#endif//IL_USE_DXTC_NVIDIA
