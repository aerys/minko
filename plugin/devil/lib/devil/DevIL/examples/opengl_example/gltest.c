//-----------------------------------------------------------------------------
//
// ImageLib GL Test Source
// Copyright (C) 2000-2002 by Denton Woods
// Last modified: 05/17/2002 <--Y2K Compliant! =]
//
// Filename: examples/gl example/gl example.c
//
// Description: Sample implementation of an OpenGL image viewer.
//
//-----------------------------------------------------------------------------

//
// We use FreeGlut in Windows, because it's more stable.
//
#ifdef _WIN32
#include <GL/freeglut.h>
#else
#include <GL/glut.h>
#endif

#ifdef  _DEBUG
#define IL_DEBUG
#endif//_DEBUG

#define ILUT_USE_OPENGL
#include <IL/il.h>
#include <IL/ilu.h>
#include <IL/ilut.h>
#include "gltest.h"
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

//
// Easier to do this than change the project's settings in MSVC++. (I'm lazy!)
//
#ifdef _MSC_VER

	#pragma comment(lib, "freeglut.lib")

// Prevent the console window from popping up.
//#pragma comment(linker, "/entry:mainCRTStartup")
//#pragma comment(linker, "/subsystem:windows")

#endif

char	*FileName;
ILuint	Width, Height, window;



void HandleDevILErrors ()
{
	ILenum error = ilGetError ();
	
	if (error != IL_NO_ERROR) {
		do {
			printf ("\n\n%s\n", iluErrorString (error));	
		} while ((error = ilGetError ()));

		exit (1);
	}
}

extern int main(int argc, char** argv);

int main(int argc, char** argv)
{
	// No filename is specified on the command-line.
	if (argc < 2) {
		printf ("Please run as:\n\nDevIL_testGL image_filename\n");
		return 1;
	}
	FileName = argv[1];  // Set filename equal to the first argument.

	//
	// Check if the shared lib's version matches the executable's version.
	//



//
// fixed to get the right numbers from the right library call...
//
	if (ilGetInteger(IL_VERSION_NUM) < IL_VERSION ||
		iluGetInteger(ILU_VERSION_NUM) < ILU_VERSION ||
		ilutGetInteger(ILUT_VERSION_NUM) < ILUT_VERSION) {
		printf ("DevIL library is out of date! Please upgrade\n");
		return 2;
	}

	// Needed to initialize DevIL.
	ilInit ();
	iluInit();

	// GL cannot use palettes anyway, so convert early.
	ilEnable (IL_CONV_PAL);

	// Gets rid of dithering on some nVidia-based cards.
	ilutEnable (ILUT_OPENGL_CONV);

	// Generate the main image name to use.
	ilGenImages (1, &ImgId);
	
	// Bind this image name.
	ilBindImage (ImgId);

	// Loads the image specified by File into the ImgId image.
	if (!ilLoadImage (FileName)) {
		HandleDevILErrors ();
	}

	// Make sure the window is in the same proportions as the image.
	//  Generate the appropriate width x height less than or equal to MAX_X x MAX_Y.
	//	Instead of just clipping Width x Height to MAX_X x MAX_Y, we scale to
	//	an appropriate size, so the image isn't stretched/squished.
	Width  = ilGetInteger (IL_IMAGE_WIDTH);
	Height = ilGetInteger (IL_IMAGE_HEIGHT);
	
	if (Width > 0) {  // Don't want a divide by 0...
		if (Width > MAX_X) {
			Width = MAX_X;
			Height = (ILuint)(MAX_X / (ILfloat)ilGetInteger(IL_IMAGE_WIDTH) * Height);
		}
	}
	if (Height > 0) {  // Don't want a divide by 0...
		if (Height > MAX_Y) {
			Height = MAX_Y;
			Width = (ILuint)(MAX_Y / (ILfloat)ilGetInteger(IL_IMAGE_HEIGHT) * Width);
		}
	}

	HandleDevILErrors ();

	// Standard glut initializations.
	glutInit               (&argc, argv);  // Standard glut initialization.
	glutInitDisplayMode    (GLUT_RGB | GLUT_DOUBLE);
	glutInitWindowPosition (100, 100);
	glutInitWindowSize     (Width, Height);
	
	window = glutCreateWindow("Developer's Image Library (DevIL) Test");

	ilutInit();

	glutDisplayFunc  (DisplayFunc);
	glutKeyboardFunc (KeyboardFunc);

	// Goes into our setup function.
	if (Setup() == IL_FALSE)
		return 1;

	// Enter the main (Free)GLUT processing loop
	glutMainLoop();

	// Clean up any loose ends.
	CleanUp();

	return 0;
}


//
// Standard glut resize function.
//
void ResizeFunc(int NewWidth, int NewHeight)
{
	glMatrixMode   (GL_PROJECTION);
	glLoadIdentity ();

	glViewport (0, 0, NewWidth, NewHeight);
	glOrtho    (0, Width, Height, 0, -1, 1);
}


//
// Standard glut display function.
//
void DisplayFunc()
{
	glClear(GL_COLOR_BUFFER_BIT);  // Clear the colour buffer.

	// Texture a quad with our image that fills the entire window.
	glBindTexture (GL_TEXTURE_2D, TexID);
	
	glBegin (GL_QUADS);
	/*glTexCoord2f (0.0f, 0.0f); glVertex3i (0,     0,      0);
	glTexCoord2f (1.0f, 0.0f); glVertex3i (Width, 0,      0);
	glTexCoord2f (1.0f, 1.0f); glVertex3i (Width, Height, 0);
	glTexCoord2f (0.0f, 1.0f); glVertex3i (0,     Height, 0);*/

	glTexCoord2f (0.0f, 1.0f); glVertex3i (0,     0,      0);
	glTexCoord2f (1.0f, 1.0f); glVertex3i (Width, 0,      0);
	glTexCoord2f (1.0f, 0.0f); glVertex3i (Width, Height, 0);
	glTexCoord2f (0.0f, 0.0f); glVertex3i (0,     Height, 0);
	glEnd();

	glutSwapBuffers();  // We use double buffering, so swap the buffers.
}


//
// Standard glut idle function
//
void IdleFunc()
{
	glutShowWindow    ();
	glutPostRedisplay ();
}


//
// Any keypress closes the window - standard glut keypress function.
//
void KeyboardFunc(unsigned char cChar, int nMouseX, int nMouseY)
{
	CleanUp();  // Clean up OpenGL.
	glutDestroyWindow(window);  // Destroy the window.
#ifndef _WIN32
	// Must use this with regular glut, since it never returns control to main().
	exit(0);
#endif
}


//
// Setup OpenGL to use our image.
//
ILboolean Setup()
{
	glEnable       (GL_TEXTURE_2D);  // Enable texturing.
	glMatrixMode   (GL_PROJECTION);  // We want to use the projection matrix.
	glLoadIdentity ();  // Loads the identity matrix into the current matrix.

	// Lets ILUT know to use its OpenGL functions.
	ilutRenderer (ILUT_OPENGL);
	
	// Goes through all steps of sending the image to OpenGL.
	TexID = ilutGLBindTexImage();
	
	// We're done with our image, so we go ahead and delete it.
	ilDeleteImages(1, &ImgId);

	glOrtho(0, Width, Height, 0, -1, 1);  // Set up an orthographic projection with OpenGL.

	return IL_TRUE;
}


//
// Cleans up any loose ends.
//
void CleanUp()
{
	if (!bCleaned)  // Can only delete the texture once.
		glDeleteTextures (1, &TexID);  // Delete our OpenGL texture.
	bCleaned = IL_TRUE;  // Want to make sure we only delete it once.
}
