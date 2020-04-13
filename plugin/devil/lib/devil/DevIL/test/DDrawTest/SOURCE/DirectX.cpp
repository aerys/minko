//============================================================//
// File:		DirectX.cpp
// 
// Date:		10-19-2000
//============================================================//
#include "../header/Render2DCore.h"

void dxutil_InitStructure(DDPIXELFORMAT& structure)
{
        ZeroMemory(&structure, sizeof(structure));
        structure.dwSize = sizeof(structure);
}

void dxutil_InitStructure(DDSURFACEDESC& structure)
{
        ZeroMemory(&structure, sizeof(structure));
        structure.dwSize                                 = sizeof(structure);
        structure.ddpfPixelFormat.dwSize = sizeof(structure);
}

void dxutil_InitStructure(DDSURFACEDESC2& structure)
{
        ZeroMemory(&structure, sizeof(structure));
        structure.dwSize                                 = sizeof(structure);
        structure.ddpfPixelFormat.dwSize = sizeof(structure);
}
