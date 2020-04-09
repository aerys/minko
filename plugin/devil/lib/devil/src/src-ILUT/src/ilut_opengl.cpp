//-----------------------------------------------------------------------------
//
// ImageLib Utility Toolkit Sources
// Copyright (C) 2000-2017 by Denton Woods
// Last modified: 05/25/2002
//
// Filename: src-ILUT/src/ilut_opengl.cpp
//
// Description: OpenGL functions for images
//
//-----------------------------------------------------------------------------


#include "ilut_opengl.h"

#ifdef ILUT_USE_OPENGL

#include <stdio.h>
#include <string.h>

#ifdef __APPLE__
#include <OpenGL/glext.h>
#include <dlfcn.h>
void *aglGetProcAddress( const GLubyte *name ) {
	// deprecated code! and wasn't working
	/*NSSymbol symbol;
 	char* symbolName;
 	// prepend a '_' for the Unix C symbol mangling convention
 	int len = strlen((const char*)name);
 	symbolName = malloc(len + 2);
 	memcpy(symbolName+1, (const char*)name, len );
 	symbolName[0] = '_';
 	symbol = NULL;
 	if (NSIsSymbolNameDefined(symbolName))
   		symbol = NSLookupAndBindSymbol(symbolName);
 	free(symbolName);
 	return symbol ? NSAddressOfSymbol(symbol) : NULL;
 	*/
 	// not deprecated code! and isn't working :( 
 	// now the address is directly known with glext.h include
 	const int len = strlen((const char*)name);
 	char *symbolName = calloc(len + 2,1);
 	
 	memcpy(symbolName+1, (const char*)name, len );
 	symbolName[0] = '_';
 	printf("searching %s as %s ",name,symbolName);
 	void *image = dlopen(NULL,RTLD_LAZY); // brutal solution
 	if( image == NULL ) {
		return NULL;
 	}
 	return dlsym(image,symbolName);
}
#endif

#ifdef _MSC_VER
	#pragma comment(lib, "opengl32.lib")
	#pragma comment(lib, "Glu32.lib")
//	#pragma comment(lib, "freeglut.lib")
#endif

#ifdef linux
	// fix for glXGetProcAddressARB
	#define GLX_GLXEXT_PROTOTYPES
	#include <GL/glx.h>
#endif

//used for automatic texture target detection
#define ILGL_TEXTURE_CUBE_MAP				0x8513
#define ILGL_TEXTURE_BINDING_CUBE_MAP		0x8514
#define ILGL_TEXTURE_CUBE_MAP_POSITIVE_X	0x8515
#define ILGL_TEXTURE_CUBE_MAP_NEGATIVE_X	0x8516
#define ILGL_TEXTURE_CUBE_MAP_POSITIVE_Y	0x8517
#define ILGL_TEXTURE_CUBE_MAP_NEGATIVE_Y	0x8518
#define ILGL_TEXTURE_CUBE_MAP_POSITIVE_Z	0x8519
#define ILGL_TEXTURE_CUBE_MAP_NEGATIVE_Z	0x851A
#define ILGL_CLAMP_TO_EDGE					0x812F
#define ILGL_TEXTURE_WRAP_R					0x8072

// Not defined in OpenGL 1.1 headers.
#define ILGL_TEXTURE_DEPTH					0x8071
#define ILGL_TEXTURE_3D						0x806F
#define ILGL_MAX_3D_TEXTURE_SIZE			0x8073


static ILboolean HasCubemapHardware = IL_FALSE;
static ILboolean HasNonPowerOfTwoHardware = IL_FALSE;
#if defined(_WIN32) || defined(_WIN64) || defined(linux) || defined(__APPLE__)
	ILGLTEXIMAGE3DARBPROC			ilGLTexImage3D = NULL;
	ILGLTEXSUBIMAGE3DARBPROC		ilGLTexSubImage3D = NULL;
	ILGLCOMPRESSEDTEXIMAGE2DARBPROC	ilGLCompressed2D = NULL;
	ILGLCOMPRESSEDTEXIMAGE3DARBPROC	ilGLCompressed3D = NULL;
#endif


// Absolutely *have* to call this if planning on using the image library with OpenGL.
//	Call this after OpenGL has initialized.
ILboolean ilutGLInit()
{
	ILint MaxTexW, MaxTexH, MaxTexD = 1;

	// Should we really be setting all this ourselves?  Seems too much like a glu(t) approach...
	glEnable(GL_TEXTURE_2D);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glPixelStorei(GL_UNPACK_SKIP_ROWS, 0);
	glPixelStorei(GL_UNPACK_SKIP_PIXELS, 0);
	glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
	glPixelStorei(GL_UNPACK_SWAP_BYTES, GL_FALSE);
	glPixelStorei(GL_PACK_ALIGNMENT, 1);

//#ifndef GL_VERSION_1_3
	#if (defined (_WIN32) || defined(_WIN64))
		if (IsExtensionSupported("GL_ARB_texture_compression") &&
			IsExtensionSupported("GL_EXT_texture_compression_s3tc")) {
				ilGLCompressed2D = (ILGLCOMPRESSEDTEXIMAGE2DARBPROC)wglGetProcAddress("glCompressedTexImage2DARB");
		}
		if (IsExtensionSupported("GL_EXT_texture3D")) {
			ilGLTexImage3D = (ILGLTEXIMAGE3DARBPROC)wglGetProcAddress("glTexImage3D");
			ilGLTexImage3D = (ILGLTEXIMAGE3DARBPROC)wglGetProcAddress("glTexSubImage3D");
		}
		if (IsExtensionSupported("GL_ARB_texture_compression") &&
			IsExtensionSupported("GL_EXT_texture_compression_s3tc") &&
			IsExtensionSupported("GL_EXT_texture3D")) {
				ilGLCompressed3D = (ILGLCOMPRESSEDTEXIMAGE3DARBPROC)wglGetProcAddress("glCompressedTexImage3DARB");
		}
	#elif linux
		if (IsExtensionSupported("GL_ARB_texture_compression") &&
			IsExtensionSupported("GL_EXT_texture_compression_s3tc")) {
				ilGLCompressed2D = (ILGLCOMPRESSEDTEXIMAGE2DARBPROC)
					glXGetProcAddressARB((const GLubyte*)"glCompressedTexImage2DARB");
		}
		if (IsExtensionSupported("GL_EXT_texture3D")) {
			ilGLTexImage3D = (ILGLTEXIMAGE3DARBPROC)glXGetProcAddressARB((const GLubyte*)"glTexImage3D");
		}
		if (IsExtensionSupported("GL_ARB_texture_compression") &&
			IsExtensionSupported("GL_EXT_texture_compression_s3tc") &&
			IsExtensionSupported("GL_EXT_texture3D")) {
				ilGLCompressed3D = (ILGLCOMPRESSEDTEXIMAGE3DARBPROC)glXGetProcAddressARB((const GLubyte*)"glCompressedTexImage3DARB");
		}
	#elif defined(__APPLE__)
		// Mac OS X headers are OpenGL 1.4 compliant already.
		ilGLCompressed2D = glCompressedTexImage2DARB;//(ILGLCOMPRESSEDTEXIMAGE2DARBPROC)aglGetProcAddress((const GLubyte *)"glCompressedTexImage2DARB");
		ilGLTexImage3D = glTexImage3D;
		ilGLCompressed3D = glCompressedTexImage3DARB;
	#else
		return IL_FALSE;  // @TODO: Find any other systems that we could be on.
	#endif
//#else
//#endif//GL_VERSION_1_3

	// Use PROXY_TEXTURE_2D/3D with glTexImage2D/3D() to test more accurately...
	glGetIntegerv(GL_MAX_TEXTURE_SIZE, (GLint*)&MaxTexW);
	glGetIntegerv(GL_MAX_TEXTURE_SIZE, (GLint*)&MaxTexH);
	if (ilGLTexImage3D != NULL)
		glGetIntegerv(ILGL_MAX_3D_TEXTURE_SIZE, (GLint*)&MaxTexD);
	if (MaxTexW <= 0 || MaxTexH <= 0 || MaxTexD <= 0) {
		MaxTexW = MaxTexH = 256;  // Trying this because of the VooDoo series of cards.
		MaxTexD = 1;
	}

	ilutSetInteger(ILUT_MAXTEX_WIDTH, MaxTexW);
	ilutSetInteger(ILUT_MAXTEX_HEIGHT, MaxTexH);
	ilutSetInteger(ILUT_MAXTEX_DEPTH, MaxTexD);

	if (IsExtensionSupported("GL_ARB_texture_cube_map"))
		HasCubemapHardware = IL_TRUE;
	if (IsExtensionSupported("GL_ARB_texture_non_power_of_two"))
		HasNonPowerOfTwoHardware = IL_TRUE;
	
	return IL_TRUE;
}


// @TODO:  Check what dimensions an image has and use the appropriate IL_IMAGE_XD #define!

GLuint ILAPIENTRY ilutGLBindTexImage()
{
	GLuint	TexID = 0, Target = GL_TEXTURE_2D;
	ILimage *Image;

	Image = ilGetCurImage();
	if (Image == NULL)
		return 0;

	if (ilutGetBoolean(ILUT_GL_AUTODETECT_TEXTURE_TARGET)) {
		if (HasCubemapHardware && Image->CubeFlags != 0)
			Target = ILGL_TEXTURE_CUBE_MAP;
		
	}

	glGenTextures(1, &TexID);
	glBindTexture(Target, TexID);

	if (Target == GL_TEXTURE_2D) {
		glTexParameteri(Target, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(Target, GL_TEXTURE_WRAP_T, GL_REPEAT);
	}
	else if (Target == ILGL_TEXTURE_CUBE_MAP) {
		glTexParameteri(Target, GL_TEXTURE_WRAP_S, ILGL_CLAMP_TO_EDGE);
		glTexParameteri(Target, GL_TEXTURE_WRAP_T, ILGL_CLAMP_TO_EDGE);
		glTexParameteri(Target, ILGL_TEXTURE_WRAP_R, ILGL_CLAMP_TO_EDGE);
	}
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glTexParameteri(Target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(Target, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glPixelStorei(GL_UNPACK_SKIP_ROWS, 0);
	glPixelStorei(GL_UNPACK_SKIP_PIXELS, 0);
	glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
	glPixelStorei(GL_UNPACK_SWAP_BYTES, IL_FALSE);

	if (!ilutGLTexImage(0)) {
		glDeleteTextures(1, &TexID);
		return 0;
	}

	return TexID;
}


ILuint GLGetDXTCNum(ILenum DXTCFormat)
{
	switch (DXTCFormat)
	{
		// Constants from glext.h.
		case IL_DXT1:
			DXTCFormat = 0x83F1;
			break;
		case IL_DXT3:
			DXTCFormat = 0x83F2;
			break;
		case IL_DXT5:
			DXTCFormat = 0x83F3;
			break;
	}

	return DXTCFormat;
}


// We assume *all* states have been set by the user, including 2D texturing!
ILboolean ILAPIENTRY ilutGLTexImage_(GLuint Level, GLuint Target, ILimage *Image)
{
	ILimage	*ImageCopy, *OldImage;
#if defined (_MSC_VER) || defined (linux) || defined(__APPLE__)
	ILenum	DXTCFormat;
	ILuint	Size;
	ILubyte	*Buffer;
#endif

	if (Image == NULL) {
		ilSetError(ILUT_ILLEGAL_OPERATION);
		return IL_FALSE;
	}

	OldImage = ilGetCurImage();

#if defined (_MSC_VER) || defined (linux) || defined(__APPLE__)
	if (ilutGetBoolean(ILUT_GL_USE_S3TC) && ilGLCompressed2D != NULL) {
		if (Image->DxtcData != NULL && Image->DxtcSize != 0) {
			DXTCFormat = GLGetDXTCNum(Image->DxtcFormat);
			ilGLCompressed2D(Target, Level, DXTCFormat, Image->Width,
				Image->Height, 0, Image->DxtcSize, Image->DxtcData);
			return IL_TRUE;
		}

		if (ilutGetBoolean(ILUT_GL_GEN_S3TC)) {
			DXTCFormat = ilutGetInteger(ILUT_S3TC_FORMAT);

			ilSetCurImage(Image);
			Size = ilGetDXTCData(NULL, 0, DXTCFormat);
			if (Size != 0) {
				Buffer = (ILubyte*)ialloc(Size);
				if (Buffer == NULL) {
					ilSetCurImage(OldImage);
					return IL_FALSE;
				}

				Size = ilGetDXTCData(Buffer, Size, DXTCFormat);
				if (Size == 0) {
					ilSetCurImage(OldImage);
					ifree(Buffer);
					return IL_FALSE;
				}

				DXTCFormat = GLGetDXTCNum(DXTCFormat);
				ilGLCompressed2D(Target, Level, DXTCFormat, Image->Width,
					Image->Height, 0, Size, Buffer);
				ifree(Buffer);
				ilSetCurImage(OldImage);
				return IL_TRUE;
			}
			ilSetCurImage(OldImage);
		}
	}
#endif//_MSC_VER

	ImageCopy = MakeGLCompliant2D(Image);
	if (ImageCopy == NULL)
		return IL_FALSE;

	glTexImage2D(
			Target, 
			Level, 
			ilutGLFormat(ImageCopy->Format, ImageCopy->Bpp),
			ImageCopy->Width,
			ImageCopy->Height,
			0,
			ImageCopy->Format,
			ImageCopy->Type,
			ImageCopy->Data);	

	if (Image != ImageCopy)
		ilCloseImage(ImageCopy);

	return IL_TRUE;
}

GLuint iToGLCube(ILuint cube)
{
	switch (cube) {
		case IL_CUBEMAP_POSITIVEX:
			return ILGL_TEXTURE_CUBE_MAP_POSITIVE_X;
		case IL_CUBEMAP_POSITIVEY:
			return ILGL_TEXTURE_CUBE_MAP_POSITIVE_Y;
		case IL_CUBEMAP_POSITIVEZ:
			return ILGL_TEXTURE_CUBE_MAP_POSITIVE_Z;
		case IL_CUBEMAP_NEGATIVEX:
			return ILGL_TEXTURE_CUBE_MAP_NEGATIVE_X;
		case IL_CUBEMAP_NEGATIVEY:
			return ILGL_TEXTURE_CUBE_MAP_NEGATIVE_Y;
		case IL_CUBEMAP_NEGATIVEZ:
			return ILGL_TEXTURE_CUBE_MAP_NEGATIVE_Z;

		default:
			return ILGL_TEXTURE_CUBE_MAP_POSITIVE_X; //???
	}
}

ILboolean ILAPIENTRY ilutGLTexImage(GLuint Level)
{
	ILimage *Temp;

	ilutCurImage = ilGetCurImage();

	if (!ilutGetBoolean(ILUT_GL_AUTODETECT_TEXTURE_TARGET))
		return ilutGLTexImage_(Level, GL_TEXTURE_2D, ilGetCurImage());
	else {
		// Autodetect texture target

		// Cubemap
		if (ilutCurImage->CubeFlags != 0 && HasCubemapHardware) { //bind to cubemap
			Temp = ilutCurImage;
			while (Temp != NULL && Temp->CubeFlags != 0) {
				ilutGLTexImage_(Level, iToGLCube(Temp->CubeFlags), Temp);
				Temp = Temp->Next;
			}
			return IL_TRUE; //@TODO: check for errors??
		}
		else  // 2D texture
			return ilutGLTexImage_(Level, GL_TEXTURE_2D, ilGetCurImage());
	}
}

GLuint ILAPIENTRY ilutGLBindMipmaps()
{
	GLuint	TexID = 0;

//	glPushAttrib(GL_ALL_ATTRIB_BITS);

	glGenTextures(1, &TexID);
	glBindTexture(GL_TEXTURE_2D, TexID);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

	if (!ilutGLBuildMipmaps()) {
		glDeleteTextures(1, &TexID);
		return 0;
	}

//	glPopAttrib();

	return TexID;
}


ILboolean ILAPIENTRY ilutGLBuildMipmaps()
{
	ILimage	*Image;

	ilutCurImage = ilGetCurImage();
	if (ilutCurImage == NULL) {
		ilSetError(ILUT_ILLEGAL_OPERATION);
		return IL_FALSE;
	}

	Image = MakeGLCompliant2D(ilutCurImage);
	if (Image == NULL)
		return IL_FALSE;

	gluBuild2DMipmaps(GL_TEXTURE_2D, ilutGLFormat(Image->Format, Image->Bpp), Image->Width,
						Image->Height, Image->Format, Image->Type, Image->Data);

	if (Image != ilutCurImage)
		ilCloseImage(Image);
	
	return IL_TRUE;
}


ILboolean ILAPIENTRY ilutGLSubTex(GLuint TexID, ILuint XOff, ILuint YOff)
{
	return ilutGLSubTex2D(TexID, XOff, YOff);
}


ILboolean ILAPIENTRY ilutGLSubTex2D(GLuint TexID, ILuint XOff, ILuint YOff)
{
	ILimage	*Image;
	ILint Width, Height;

	ilutCurImage = ilGetCurImage();
	if (ilutCurImage == NULL) {
		ilSetError(ILUT_ILLEGAL_OPERATION);
		return IL_FALSE;
	}

	Image = MakeGLCompliant2D(ilutCurImage);
	if (Image == NULL)
		return IL_FALSE;

	glBindTexture(GL_TEXTURE_2D, TexID);

	glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH,  (GLint*)&Width);
	glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, (GLint*)&Height);

	if (Image->Width + XOff > (ILuint)Width || Image->Height + YOff > (ILuint)Height) {
		ilSetError(ILUT_BAD_DIMENSIONS);
		return IL_FALSE;
	}

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glPixelStorei(GL_UNPACK_SKIP_ROWS, 0);
	glPixelStorei(GL_UNPACK_SKIP_PIXELS, 0);
	glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
	glPixelStorei(GL_UNPACK_SWAP_BYTES, IL_FALSE);
	glTexSubImage2D(GL_TEXTURE_2D, 0, XOff, YOff, Image->Width, Image->Height, Image->Format,
			Image->Type, Image->Data);

	if (Image != ilutCurImage)
		ilCloseImage(Image);

	return IL_TRUE;
}


ILboolean ILAPIENTRY ilutGLSubTex3D(GLuint TexID, ILuint XOff, ILuint YOff, ILuint ZOff)
{
	ILimage	*Image;
	ILint Width, Height, Depth;

	if (ilGLTexSubImage3D == NULL) {
		ilSetError(ILUT_ILLEGAL_OPERATION);  // Set a different error?
		return IL_FALSE;
	}

	ilutCurImage = ilGetCurImage();
	if (ilutCurImage == NULL) {
		ilSetError(ILUT_ILLEGAL_OPERATION);
		return IL_FALSE;
	}

	Image = MakeGLCompliant3D(ilutCurImage);
	if (Image == NULL)
		return IL_FALSE;

	glBindTexture(ILGL_TEXTURE_3D, TexID);

	glGetTexLevelParameteriv(ILGL_TEXTURE_3D, 0, GL_TEXTURE_WIDTH,  (GLint*)&Width);
	glGetTexLevelParameteriv(ILGL_TEXTURE_3D, 0, GL_TEXTURE_HEIGHT, (GLint*)&Height);
	glGetTexLevelParameteriv(ILGL_TEXTURE_3D, 0, ILGL_TEXTURE_DEPTH, (GLint*)&Depth);

	if (Image->Width + XOff > (ILuint)Width || Image->Height + YOff > (ILuint)Height
		|| Image->Depth + ZOff > (ILuint)Depth) {
		ilSetError(ILUT_BAD_DIMENSIONS);
		return IL_FALSE;
	}

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glPixelStorei(GL_UNPACK_SKIP_ROWS, 0);
	glPixelStorei(GL_UNPACK_SKIP_PIXELS, 0);
	glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
	glPixelStorei(GL_UNPACK_SWAP_BYTES, IL_FALSE);
	ilGLTexSubImage3D(ILGL_TEXTURE_3D, 0, XOff, YOff, ZOff, Image->Width, Image->Height, Image->Depth, 
			Image->Format, Image->Type, Image->Data);

	if (Image != ilutCurImage)
		ilCloseImage(Image);

	return IL_TRUE;
}


ILimage* MakeGLCompliant2D(ILimage *Src)
{
	ILimage		*Dest = Src, *Temp;
	ILboolean	Created = IL_FALSE;
	ILenum		Filter;
	ILubyte		*Flipped;
	ILboolean   need_resize = IL_FALSE;
	ILint		MaxTexW, MaxTexH;

	MaxTexW = ilutGetInteger(ILUT_MAXTEX_WIDTH);
	MaxTexH = ilutGetInteger(ILUT_MAXTEX_HEIGHT);
	
	if (Src->Pal.Palette != NULL && Src->Pal.PalSize != 0 && Src->Pal.PalType != IL_PAL_NONE) {
		//ilSetCurImage(Src);
		Dest = iConvertImage(Src, ilGetPalBaseType(Src->Pal.PalType), IL_UNSIGNED_BYTE);
		//Dest = iConvertImage(IL_BGR);
		//ilSetCurImage(ilutCurImage);
		if (Dest == NULL)
			return NULL;

		Created = IL_TRUE;

		// Change here!


		// Set Dest's palette stuff here
		Dest->Pal.PalType = IL_PAL_NONE;
	}

	if (HasNonPowerOfTwoHardware == IL_FALSE && 
		  (Src->Width  != ilNextPower2(Src->Width)  ||
		   Src->Height != ilNextPower2(Src->Height)  )) {
				need_resize = IL_TRUE;
			}

	if ((ILint)Src->Width > MaxTexW || (ILint)Src->Height > MaxTexH)
		need_resize = IL_TRUE;

	if (need_resize == IL_TRUE) {
		if (!Created) {
			Dest = ilCopyImage_(Src);
			if (Dest == NULL) {
				return NULL;
			}
			Created = IL_TRUE;
		}

		Filter = iluGetInteger(ILU_FILTER);
		if (Src->Format == IL_COLOUR_INDEX) {
			iluImageParameter(ILU_FILTER, ILU_NEAREST);
			Temp = HasNonPowerOfTwoHardware == IL_TRUE ? 
				iluScale_(Dest, min((ILuint)MaxTexW, Dest->Width), min((ILuint)MaxTexH, Dest->Height), 1)
			  : iluScale_(Dest, min((ILuint)MaxTexW, ilNextPower2(Dest->Width)),
			  		min((ILuint)MaxTexH, ilNextPower2(Dest->Height)), 1);
			iluImageParameter(ILU_FILTER, Filter);
		} else {
			iluImageParameter(ILU_FILTER, ILU_BILINEAR);
			Temp = HasNonPowerOfTwoHardware == IL_TRUE ?
				iluScale_(Dest, min((ILuint)MaxTexW, Dest->Width), min((ILuint)MaxTexH, Dest->Height), 1)
			 :	iluScale_(Dest, min((ILuint)MaxTexW, (ILint)ilNextPower2(Dest->Width)),
			 		min(MaxTexH, (ILint)ilNextPower2(Dest->Height)), 1);
			iluImageParameter(ILU_FILTER, Filter);
		}

		ilCloseImage(Dest);
		if (!Temp) {
			return NULL;
		}
		Dest = Temp;
	}

	if (Dest->Origin != IL_ORIGIN_LOWER_LEFT) {
		Flipped = iGetFlipped(Dest);
		ifree(Dest->Data);
		Dest->Data = Flipped;
		Dest->Origin = IL_ORIGIN_LOWER_LEFT;
	}

	return Dest;
}


ILimage* MakeGLCompliant3D(ILimage *Src)
{
	ILimage		*Dest = Src, *Temp;
	ILboolean	Created = IL_FALSE;
	ILenum		Filter;
	ILubyte		*Flipped;
	ILboolean   need_resize = IL_FALSE;
	ILint		MaxTexW, MaxTexH, MaxTexD;

	MaxTexW = ilutGetInteger(ILUT_MAXTEX_WIDTH);
	MaxTexH = ilutGetInteger(ILUT_MAXTEX_HEIGHT);
	MaxTexD = ilutGetInteger(ILUT_MAXTEX_DEPTH);

	if (Src->Pal.Palette != NULL && Src->Pal.PalSize != 0 && Src->Pal.PalType != IL_PAL_NONE) {
		//ilSetCurImage(Src);
		Dest = iConvertImage(Src, ilGetPalBaseType(Src->Pal.PalType), IL_UNSIGNED_BYTE);
		//Dest = iConvertImage(IL_BGR);
		//ilSetCurImage(ilutCurImage);
		if (Dest == NULL)
			return NULL;

		Created = IL_TRUE;

		// Change here!


		// Set Dest's palette stuff here
		Dest->Pal.PalType = IL_PAL_NONE;
	}

	if (HasNonPowerOfTwoHardware == IL_FALSE && 
		  (Src->Width  != ilNextPower2(Src->Width)  ||
		   Src->Height != ilNextPower2(Src->Height) ||
		   Src->Depth  != ilNextPower2(Src->Depth) )) {
				need_resize = IL_TRUE;
			}

	if ((ILint)Src->Width > MaxTexW || (ILint)Src->Height > MaxTexH || (ILint)Src->Depth > MaxTexD)
		need_resize = IL_TRUE;

	if (need_resize == IL_TRUE) {
		if (!Created) {
			Dest = ilCopyImage_(Src);
			if (Dest == NULL) {
				return NULL;
			}
			Created = IL_TRUE;
		}

		Filter = iluGetInteger(ILU_FILTER);
		if (Src->Format == IL_COLOUR_INDEX) {
			iluImageParameter(ILU_FILTER, ILU_NEAREST);
			Temp = HasNonPowerOfTwoHardware == IL_TRUE ? 
				iluScale_(Dest, min((ILuint)MaxTexW, Dest->Width), min((ILuint)MaxTexH, Dest->Height), min((ILuint)MaxTexD, Dest->Depth))
			  : iluScale_(Dest, min((ILuint)MaxTexW, ilNextPower2(Dest->Width)),
			  		min((ILuint)MaxTexH, ilNextPower2(Dest->Height)),
					min((ILuint)MaxTexD, ilNextPower2(Dest->Height)));
			iluImageParameter(ILU_FILTER, Filter);
		} else {
			iluImageParameter(ILU_FILTER, ILU_BILINEAR);
			Temp = HasNonPowerOfTwoHardware == IL_TRUE ?
				iluScale_(Dest, min((ILuint)MaxTexW, Dest->Width), min((ILuint)MaxTexH, Dest->Height), min((ILuint)MaxTexD, Dest->Depth))
			 :	iluScale_(Dest, min((ILuint)MaxTexW, (ILint)ilNextPower2(Dest->Width)),
			 		min(MaxTexH, (ILint)ilNextPower2(Dest->Height)),
					min(MaxTexD, (ILint)ilNextPower2(Dest->Depth)));
			iluImageParameter(ILU_FILTER, Filter);
		}

		ilCloseImage(Dest);
		if (!Temp) {
			return NULL;
		}
		Dest = Temp;
	}

	if (Dest->Origin != IL_ORIGIN_LOWER_LEFT) {
		Flipped = iGetFlipped(Dest);
		ifree(Dest->Data);
		Dest->Data = Flipped;
		Dest->Origin = IL_ORIGIN_LOWER_LEFT;
	}

	return Dest;
}


//! Just a convenience function.
#ifndef _WIN32_WCE
GLuint ILAPIENTRY ilutGLLoadImage(ILstring FileName)
{
	GLuint	TexId;
	//ILuint	Id;

	iBindImageTemp();
	//ilGenImages(1, &Id);
	//ilBindImage(Id);

	if (!ilLoadImage(FileName))
		return 0;

	TexId = ilutGLBindTexImage();

	//ilDeleteImages(1, &Id);

	return TexId;
}
#endif//_WIN32_WCE


#ifndef _WIN32_WCE
ILboolean ILAPIENTRY ilutGLSaveImage(ILstring FileName, GLuint TexID)
{
	ILuint		CurName;
	ILboolean	Saved;
	
	CurName = ilGetCurName();

	iBindImageTemp();

	if (!ilutGLSetTex(TexID)) {
		ilBindImage(CurName);
		return IL_FALSE;
	}

	Saved = ilSaveImage(FileName);
	ilBindImage(CurName);

	return Saved;
}
#endif//_WIN32_WCE


//! Takes a screenshot of the current OpenGL window.
ILboolean ILAPIENTRY ilutGLScreen()
{
	ILuint	ViewPort[4];

	ilutCurImage = ilGetCurImage();
	if (ilutCurImage == NULL) {
		ilSetError(ILUT_ILLEGAL_OPERATION);
		return IL_FALSE;
	}

	glGetIntegerv(GL_VIEWPORT, (GLint*)ViewPort);

	if (!ilTexImage(ViewPort[2], ViewPort[3], 1, 3, IL_RGB, IL_UNSIGNED_BYTE, NULL))
		return IL_FALSE;  // Error already set.
	ilutCurImage->Origin = IL_ORIGIN_LOWER_LEFT;

	glPixelStorei(GL_PACK_ALIGNMENT, 1);
	glReadPixels(0, 0, ViewPort[2], ViewPort[3], GL_RGB, GL_UNSIGNED_BYTE, ilutCurImage->Data);

	return IL_TRUE;
}


#ifndef _WIN32_WCE
ILboolean ILAPIENTRY ilutGLScreenie()
{
	FILE		*File;
	ILchar		Buff[255];
	ILuint		i, CurName;
	ILboolean	ReturnVal = IL_TRUE;

	CurName = ilGetCurName();

	// Could go above 128 easily...
	for (i = 0; i < 128; i++) {
#ifndef _UNICODE
		sprintf(Buff, "screen%d.tga", i);
		File = fopen(Buff, "rb");
#else
		swprintf(Buff, 128, L"screen%d.tga", i);
		// Windows has a different function, _wfopen, to open UTF16 files,
		//  whereas Linux just uses fopen.
		#ifdef _WIN32
			File = _wfopen(Buff, L"rb");
		#else
			File = fopen((char*)Buff, "rb");
		#endif
#endif
		if (!File)
			break;
		fclose(File);
	}

	if (i == 127) {
		ilSetError(ILUT_COULD_NOT_OPEN_FILE);
		return IL_FALSE;
	}

	iBindImageTemp();
	if (!ilutGLScreen()) {
		ReturnVal = IL_FALSE;
	}

	if (ReturnVal)
		ilSave(IL_TGA, Buff);

	ilBindImage(CurName);

	return ReturnVal;
}
#endif//_WIN32_WCE


//! Deprecated - use ilutGLSetTex2D instead.
ILboolean ILAPIENTRY ilutGLSetTex(GLuint TexID)
{
	return ilutGLSetTex2D(TexID);
}


ILboolean ILAPIENTRY ilutGLSetTex2D(GLuint TexID)
{
	ILubyte *Data;
	ILuint Width, Height;

	glBindTexture(GL_TEXTURE_2D, TexID);

	glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH,  (GLint*)&Width);
	glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, (GLint*)&Height);

	Data = (ILubyte*)ialloc(Width * Height * 4);
	if (Data == NULL) {
		return IL_FALSE;
	}

	glGetTexImage(GL_TEXTURE_2D, 0, IL_BGRA, GL_UNSIGNED_BYTE, Data);

	if (!ilTexImage(Width, Height, 1, 4, IL_BGRA, IL_UNSIGNED_BYTE, Data)) {
		ifree(Data);
		return IL_FALSE;
	}
	ilutCurImage->Origin = IL_ORIGIN_LOWER_LEFT;

	ifree(Data);
	return IL_TRUE;
}


ILboolean ILAPIENTRY ilutGLSetTex3D(GLuint TexID)
{
	ILubyte *Data;
	ILuint Width, Height, Depth;

	if (ilGLTexImage3D == NULL) {
		ilSetError(ILUT_ILLEGAL_OPERATION);  // Set a different error?
		return IL_FALSE;
	}

	glBindTexture(ILGL_TEXTURE_3D, TexID);

	glGetTexLevelParameteriv(ILGL_TEXTURE_3D, 0, GL_TEXTURE_WIDTH,  (GLint*)&Width);
	glGetTexLevelParameteriv(ILGL_TEXTURE_3D, 0, GL_TEXTURE_HEIGHT, (GLint*)&Height);
	glGetTexLevelParameteriv(ILGL_TEXTURE_3D, 0, ILGL_TEXTURE_DEPTH, (GLint*)&Depth);

	Data = (ILubyte*)ialloc(Width * Height * Depth * 4);
	if (Data == NULL) {
		return IL_FALSE;
	}

	glGetTexImage(ILGL_TEXTURE_3D, 0, IL_BGRA, GL_UNSIGNED_BYTE, Data);

	if (!ilTexImage(Width, Height, Depth, 4, IL_BGRA, IL_UNSIGNED_BYTE, Data)) {
		ifree(Data);
		return IL_FALSE;
	}
	ilutCurImage->Origin = IL_ORIGIN_LOWER_LEFT;

	ifree(Data);
	return IL_TRUE;
}



ILenum ilutGLFormat(ILenum Format, ILubyte Bpp)
{
	if (Format == IL_RGB || Format == IL_BGR) {
		if (ilutIsEnabled(ILUT_OPENGL_CONV)) {
			return GL_RGB8;
		}
	}
	else if (Format == IL_RGBA || Format == IL_BGRA) {
		if (ilutIsEnabled(ILUT_OPENGL_CONV)) {
			return GL_RGBA8;
		}
	}
	else if (Format == IL_ALPHA) {
		if (ilutIsEnabled(ILUT_OPENGL_CONV)) {
			return GL_ALPHA;
		}
	}

	return Bpp;
}


// From http://www.opengl.org/News/Special/OGLextensions/OGLextensions.html
//	Should we make this accessible outside the lib?
ILboolean IsExtensionSupported(const char *extension)
{
	const GLubyte *extensions;// = NULL;
	const GLubyte *start;
	GLubyte *where, *terminator;

	// Extension names should not have spaces.
	where = (GLubyte *) strchr(extension, ' ');
	if (where || *extension == '\0')
		return IL_FALSE;
	extensions = glGetString(GL_EXTENSIONS);
	if (!extensions)
		return IL_FALSE;
	// It takes a bit of care to be fool-proof about parsing the
	// OpenGL extensions string. Don't be fooled by sub-strings, etc.
	start = extensions;
	for (;;) {
		where = (GLubyte *)strstr((const char *) start, extension);
		if (!where)
			break;
		terminator = where + strlen(extension);
		if (where == start || *(where - 1) == ' ')
		if (*terminator == ' ' || *terminator == '\0')
			return IL_TRUE;
		start = terminator;
	}
	return IL_FALSE;
}


#endif//ILUT_USE_OPENGL
