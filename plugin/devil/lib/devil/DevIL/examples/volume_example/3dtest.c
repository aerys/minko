#ifdef _WIN32
#include <GL/freeglut.h>
#else
#include <GL/glut.h>
#endif
#include <IL/il.h>
#include <IL/ilu.h>
//#define ILUT_USE_OPENGL
#include <IL/ilut.h>
#include "3dtest.h"
#include <math.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>

#ifdef _MSC_VER
	#pragma comment(lib, "opengl32.lib")
	#pragma comment(lib, "freeglut.lib")

	// Prevent the console window from popping up.
	#pragma comment(linker, "/entry:mainCRTStartup")
	#pragma comment(linker, "/subsystem:windows")
#endif

char	*File;
ILint	Width, Height, Depth, Window;
ILuint	ActiveImage = 0;

int main(int argc, char** argv)
{
	//char Test[6] = { 0, 0, 0, 0, 0, 0 };

	if (argc < 2) {
		//cout << "Please specify a filename." << endl;
		return 1;
	}
	File = argv[1];

	if (argc > 2) {
		TransFactor = atoi(argv[2]) != 0 ? -atoi(argv[2]) : TransFactor;
	}

	if (ilGetInteger(IL_VERSION_NUM) < IL_VERSION ||
		ilGetInteger(ILU_VERSION_NUM) < ILU_VERSION ||
		ilGetInteger(ILUT_VERSION_NUM) < ILUT_VERSION) {
		//cout << "OpenIL version is different...exiting!" << endl;
		return 2;
	}

	ilInit();
	//ilEnable(IL_CONV_PAL);
	ilutEnable(ILUT_OPENGL_CONV);

	glutInit(&argc, argv);

	ilGenImages(1, &ImgId);
	ilBindImage(ImgId);
	ilLoadImage(File);

	// Generate the appropriate width x height less than or equal to MAX_X x MAX_Y.
	//	Instead of just clipping Width x Height to MAX_X x MAX_Y, we scale to
	//	an appropriate size, so the image isn't stretched/squished.
	Width  = ilGetInteger(IL_IMAGE_WIDTH);
	Height = ilGetInteger(IL_IMAGE_HEIGHT);
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

    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(Width, Height);

	Window = glutCreateWindow("Open Image Library (OpenIL) Test");
	glutDisplayFunc(DisplayFunc);
	glutKeyboardFunc(KeyboardFunc);
	glutSpecialFunc(KeySpecialFunc);
	if (Setup() == IL_FALSE)
		return 1;

    // Enter the main (Free)GLUT processing loop
    glutMainLoop();

	CleanUp();

    return 0;
}


void ResizeFunc(int NewWidth, int NewHeight)
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

    glViewport(0, 0, NewWidth, NewHeight);
	//glOrtho(0, Width, 0, Height, -100, 1);
	SetPerspective(50.0f);
}


void DisplayFunc()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glLoadIdentity();
	SetPerspective(50.0f);
	glTranslatef(0.0f, 0.0f, TransFactor);
	glRotatef(Angle, 0.0f, 1.0f, 0.0f);

	glBindTexture(GL_TEXTURE_2D, TexID1);
	glBegin(GL_QUADS);
		glTexCoord2f(0.0f, 0.0f);
		glVertex3i(-Width, -Height, Depth);
		glTexCoord2f(1.0f, 0.0f);
		glVertex3i(Width, -Height, Depth);
		glTexCoord2f(1.0f, 1.0f);
		glVertex3i(Width, Height, Depth);
		glTexCoord2f(0.0f, 1.0f);
		glVertex3i(-Width, Height, Depth);
	glEnd();

	glBindTexture(GL_TEXTURE_2D, TexID2);
	glBegin(GL_QUADS);
		glTexCoord2f(0.0f, 0.0f);
		glVertex3i(-Width, -Height, Depth);
		glTexCoord2f(1.0f, 0.0f);
		glVertex3i(-Width, -Height, -Depth);
		glTexCoord2f(1.0f, 1.0f);
		glVertex3i(-Width, Height, -Depth);
		glTexCoord2f(0.0f, 1.0f);
		glVertex3i(-Width, Height, Depth);
	glEnd();

	glBindTexture(GL_TEXTURE_2D, TexID3);
	glBegin(GL_QUADS);
		glTexCoord2f(0.0f, 0.0f);
		glVertex3i(Width, -Height, Depth);
		glTexCoord2f(1.0f, 0.0f);
		glVertex3i(Width, -Height, -Depth);
		glTexCoord2f(1.0f, 1.0f);
		glVertex3i(Width, Height, -Depth);
		glTexCoord2f(0.0f, 1.0f);
		glVertex3i(Width, Height, Depth);
	glEnd();

	glBindTexture(GL_TEXTURE_2D, TexID4);
	glBegin(GL_QUADS);
		glTexCoord2f(0.0f, 0.0f);
		glVertex3i(-Width, -Height, -Depth);
		glTexCoord2f(1.0f, 0.0f);
		glVertex3i(Width, -Height, -Depth);
		glTexCoord2f(1.0f, 1.0f);
		glVertex3i(Width, Height, -Depth);
		glTexCoord2f(0.0f, 1.0f);
		glVertex3i(-Width, Height, -Depth);
	glEnd();


	glFlush();
	glFinish();
	glutSwapBuffers();
}


void IdleFunc()
{
	glutShowWindow();
	glutPostRedisplay();
}


void KeyboardFunc(unsigned char cChar, int nMouseX, int nMouseY)
{
	if (cChar >= '0' && cChar <= '9') {
		ActiveImage = cChar - '0';
		CleanUp();
		GenSides();
		return;
	}
	if (cChar == '+' || cChar == '=') {
		ActiveImage++;
		CleanUp();
		GenSides();
		return;
	}
	if (cChar == '-' || cChar == '_') {
		if (ActiveImage == 0)
			return;
		ActiveImage--;
		CleanUp();
		GenSides();
		return;
	}

	CleanUp();
	glutDestroyWindow(Window);
#ifndef _WIN32
	/* Siigron: added exit(), since glutDestroyWindow() doesn't exit the
		program with "normal" GLUT */
	exit(0);
#endif
}


void KeySpecialFunc(int Key, int x, int y)
{
	switch (Key)
	{
		case GLUT_KEY_UP:
			TransFactor += 10.0f;
			//glTranslatef(0.0f, 0.0f, 10.0f);
			break;
		case GLUT_KEY_DOWN:
			TransFactor -= 10.0f;
			//glTranslatef(0.0f, 0.0f, -10.0f);
			break;
		case GLUT_KEY_RIGHT:
			Angle += 10.0f;
			//glRotatef(10.0f, 0.0f, 1.0f, 0.0f);
			break;
		case GLUT_KEY_LEFT:
			Angle -= 10.0f;
			//glRotatef(-10.0f, 0.0f, 1.0f, 0.0f);
			break;
	}
	glutPostRedisplay();

	return;
}


#define PI 3.14159265
void SetPerspective(float Fov)
{
	float fov = (float)tan(Fov * .5f * PI / 180.0f);
	float Aspect = 0.0f;
	
	if (Height != 0)
		Aspect = Width / (float)Height;

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glFrustum(-fov * Aspect, fov * Aspect, -fov, fov, 1.0f, 10000.0f);

	return;
}


ILboolean Setup()
{
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);  // or should this be GL_LESS?
	glClearDepth(1);

	glEnable(GL_TEXTURE_2D);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	ilutRenderer(ILUT_OPENGL);

	if (!GenSides())
		return IL_FALSE;

	TransFactor += -Depth;

	glTranslatef(0.0f, 0.0f, -100.0f);

	return IL_TRUE;
}


ILboolean GenSides()
{
	ILubyte	*Buffer, *Data, Bpp, Bpc;
	ILuint	TempImage;
	ILenum	Format, Type;
	ILint	SizePlane, Bps, c, y, z, i;

	ilActiveImage(ActiveImage);
	Bpp = ilGetInteger(IL_IMAGE_BPP);
	Bpc = ilGetInteger(IL_IMAGE_BPC);
	Format = ilGetInteger(IL_IMAGE_FORMAT);
	Type = ilGetInteger(IL_IMAGE_TYPE);

	// Front
	TexID1 = ilutGLBindTexImage();
	Width = ilGetInteger(IL_IMAGE_WIDTH);
	Height = ilGetInteger(IL_IMAGE_HEIGHT);
	Depth = ilGetInteger(IL_IMAGE_DEPTH);
	ilGenImages(1, &TempImage);

	SizePlane = ilGetInteger(IL_IMAGE_PLANESIZE);

	SizePlane = Width * Height * Bpp * Bpc;
	Bps = Width * Bpp * Bpc;
	Data = ilGetData();

	// Left
	i = 0;
	Buffer = (ILubyte*)malloc(Height * Depth * Bpp * Bpc);
	for (y = 0; y < Height; y++) {
		for (z = 0; z < Depth; z++) {
			for (c = 0; c < Bpp * Bpc; c++) {
				Buffer[i++] = Data[z * SizePlane + y * Bps + c];
			}
		}
	}
	ilBindImage(TempImage);
	ilTexImage(Depth, Height, 1, Bpp, Format, Type, Buffer);
	TexID2 = ilutGLBindTexImage();
	free(Buffer);

	// Right
	ilBindImage(ImgId);
	ilActiveImage(ActiveImage);
	i = 0;
	Buffer = (ILubyte*)malloc(Height * Depth * Bpp * Bpc);
	for (y = 0; y < Height; y++) {
		for (z = 0; z < Depth; z++) {
			for (c = 0; c < Bpp * Bpc; c++) {
				Buffer[i++] = Data[z * SizePlane + y * Bps + (Width - 1) * Bpp * Bpc + c];
			}
		}
	}
	ilBindImage(TempImage);
	ilTexImage(Depth, Height, 1, Bpp, Format, Type, Buffer);
	TexID3 = ilutGLBindTexImage();
	free(Buffer);

	// Back
	ilBindImage(ImgId);
	ilActiveImage(ActiveImage);
	Buffer = (ILubyte*)malloc(Width * Height * Bpp * Bpc);
	ilCopyPixels(0, 0, Depth-1, Width, Height, 1, Format, Type, Buffer);
	ilBindImage(TempImage);
	ilTexImage(Width, Height, 1, Bpp, Format, Type, Buffer);
	TexID4 = ilutGLBindTexImage();
	free(Buffer);

	//ilDeleteImages(1, &ImgId);
	ilDeleteImages(1, &TempImage);

	ilBindImage(ImgId);

	return IL_TRUE;
}


void CleanUp()
{
	glDeleteTextures(1, &TexID1);
	glDeleteTextures(1, &TexID2);
	glDeleteTextures(1, &TexID3);
	glDeleteTextures(1, &TexID4);
	return;
}


void ExitClean()
{
	if (!bCleaned) {
		glDeleteTextures(1, &TexID1);
		glDeleteTextures(1, &TexID2);
		glDeleteTextures(1, &TexID3);
		glDeleteTextures(1, &TexID4);
		ilDeleteImages(1, &ImgId);
	}
	bCleaned = IL_TRUE;
	return;
}
