//-----------------------------------------------------------------------------
//
// ImageLib GL Test Source
// Copyright (C) 2000-2002 by Denton Woods
// Last modified: 10/13/2000 <--Y2K Compliant! =]
//
// Filename: examples/gl example/gl example.h
//
// Description: Sample implementation of an OpenGL image viewer.
//
//-----------------------------------------------------------------------------

#ifndef GLTEST_H
#define GLTEST_H

// We don't want a larger window than this.
#define MAX_X 640
#define MAX_Y 480

// Function prototypes.
void DisplayFunc(void);
void CleanUp(void);
void ResizeFunc(int NewWidth, int NewHeight);
void IdleFunc(void);
void KeyboardFunc(unsigned char cChar, int nMouseX, int nMouseY);
ILboolean Setup();

// Has the main GL texture been deleted?
ILboolean bCleaned = 0;

// GL texture ID
GLuint TexID = 0;
// IL image ID
ILuint ImgId = 0;

#endif//GLTEST_H
