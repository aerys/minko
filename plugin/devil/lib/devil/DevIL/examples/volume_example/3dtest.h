#ifndef TEST3D_H
#define TEST3D_H

#define MAX_X 640
#define MAX_Y 480

void DisplayFunc(void);
void CleanUp(void);
void ExitClean(void);
void ResizeFunc(int NewWidth, int NewHeight);
void IdleFunc(void);
void KeyboardFunc(unsigned char cChar, int nMouseX, int nMouseY);
void KeySpecialFunc(int Key, int x, int y);
void SetPerspective(float Fov);
ILboolean Setup(void);
ILboolean GenSides(void);

ILboolean bCleaned = IL_FALSE;

GLuint	TexID1 = 0, TexID2 = 0, TexID3 = 0, TexID4 = 0;
ILuint	ImgId;

float	Angle = 0.0, TransFactor = -470.0;

#endif//TEST3D_H
