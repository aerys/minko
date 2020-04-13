//============================================================//
// File:		beTypes.h
// 
// Date:		10-19-2000
//============================================================//
#ifndef BETYPES_H
#define BETYPES_H

#define beDirectDraw                    IDirectDraw7
#define beDirectDrawIID                 IID_IDirectDraw7
#define beDirectDrawSurface             IDirectDrawSurface7
#define beDirectDrawSurfaceIID			IID_IDirectDrawSurface7

#define beSafeRelease(ptr)              if ((ptr) != NULL) { (ptr)->Release(); (ptr) = NULL; }

void dxutil_InitStructure(DDPIXELFORMAT& structure);
void dxutil_InitStructure(DDSURFACEDESC& structure);
void dxutil_InitStructure(DDSURFACEDESC2& structure);


#endif