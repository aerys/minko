/************************************************************************************

Filename    :   OVR_CAPI_GL.h
Content     :   GL specific structures used by the CAPI interface.
Created     :   November 7, 2013
Authors     :   Lee Cooper

Copyright   :   Copyright 2013 Oculus VR, Inc. All Rights reserved.

Use of this software is subject to the terms of the Oculus Inc license
agreement provided at the time of installation or download, or which
otherwise accompanies this software in either electronic or hard copy form.

************************************************************************************/
#ifndef OVR_CAPI_GL_h
#define OVR_CAPI_GL_h

/// @file OVR_CAPI_GL.h
/// OpenGL rendering support.

#include "OVR_CAPI.h"

//-----------------------------------------------------------------------------------
// ***** GL Specific

#if defined(OVR_OS_WIN32)
    #ifndef WIN32_LEAN_AND_MEAN
    #define WIN32_LEAN_AND_MEAN
    #endif
    #include <Windows.h>
    #include <GL/gl.h>
#elif defined(OVR_OS_MAC)
    #include <OpenGL/gl3.h>
    #include <OpenGL/OpenGL.h>
#else
    #include <GL/gl.h>
    #include <GL/glx.h>
#endif


/// Used to configure slave GL rendering (i.e. for devices created externally).
typedef struct ovrGLConfigData_s
{
    /// General device settings.
    ovrRenderAPIConfigHeader Header;

#if defined(OVR_OS_WIN32)
    /// The optional window handle. If unset, rendering will use the current window.
    HWND Window;
    /// The optional device context. If unset, rendering will use a new context.
    HDC  DC;
#elif defined(OVR_OS_LINUX)
    /// The optional display. If unset, rendering will use the current display.
    _XDisplay* Disp;
    /// The optional window. If unset, rendering will use the current window.
    Window     Win;
#endif
} ovrGLConfigData;

/// Contains OpenGL-specific rendering information.
union ovrGLConfig
{
    /// General device settings.
    ovrRenderAPIConfig Config;
    /// OpenGL-specific settings.
    ovrGLConfigData    OGL;
};

/// Used to pass GL eye texture data to ovrHmd_EndFrame.
typedef struct ovrGLTextureData_s
{
    /// General device settings.
    ovrTextureHeader Header;
    /// The OpenGL name for this texture.
    GLuint           TexId;       
} ovrGLTextureData;

/// Contains OpenGL-specific texture information.
typedef union ovrGLTexture_s
{
    /// General device settings.
    ovrTexture       Texture;
    /// OpenGL-specific settings.
    ovrGLTextureData OGL;
} ovrGLTexture;

#endif	// OVR_CAPI_GL_h
