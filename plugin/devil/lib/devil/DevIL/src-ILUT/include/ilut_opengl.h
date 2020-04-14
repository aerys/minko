//-----------------------------------------------------------------------------
//
// ImageLib Utility Toolkit Sources
// Copyright (C) 2000-2017 by Denton Woods
// Last modified: 01/09/2009
//
// Filename: src-ILUT/include/ilut_opengl.h
//
// Description: OpenGL functions for images
//
//-----------------------------------------------------------------------------

#ifndef ILUT_OPENGL_H
#define ILUT_OPENGL_H

#include "ilut_internal.h"

#ifndef min
#define min(a, b)	(((a) < (b)) ? (a) : (b))
#endif

#ifdef ILUT_USE_OPENGL
ILenum    ilutGLFormat(ILenum, ILubyte);
ILimage*  MakeGLCompliant2D(ILimage *Src);
ILimage*  MakeGLCompliant3D(ILimage *Src);
ILboolean IsExtensionSupported(const char *extension);


#ifdef _WIN32
	#include <windows.h>
	#include <GL/gl.h>
#elif linux
	#include <GL/gl.h>
	#include <GL/glx.h>  // patch #1504388. X86_64 Problems by Hans de Goede
#elif defined __APPLE__
	#include <mach-o/dyld.h>
	void* aglGetProcAddress (const GLubyte *name);
#endif

typedef void (ILAPIENTRY * ILGLTEXIMAGE3DARBPROC) (GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth, GLint border, GLenum format, GLenum type, const GLvoid *data);
typedef void (ILAPIENTRY * ILGLTEXSUBIMAGE3DARBPROC) (GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum type, const GLvoid *data);
typedef void (ILAPIENTRY * ILGLCOMPRESSEDTEXIMAGE2DARBPROC) (GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLint border, GLsizei imageSize, const GLvoid *data);
typedef void (ILAPIENTRY * ILGLCOMPRESSEDTEXIMAGE3DARBPROC) (GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth, GLint border, GLsizei imageSize, const GLvoid *data);


#endif //ILUT_USE_OPENGL
#endif //ILUT_OPENGL_H
