/************************************************************************************

Filename    :   CAPI_GLE.h
Content     :   OpenGL extensions support. Implements a stripped down glew-like 
                interface with some additional functionality.
Copyright   :   Copyright 2014 Oculus VR, LLC All Rights reserved.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.

************************************************************************************/

// This file provides functionality similar to a reduced version of GLEW, plus some
// additional functionality that's useful to us, such as function hooking.

#ifndef INC_OVR_CAPI_GLE_h
#define INC_OVR_CAPI_GLE_h


#include "../../Kernel/OVR_Types.h"
#include "CAPI_GLE_GL.h"


///////////////////////////////////////////////////////////////////////////////
// How to use this functionality
//
// - You call OpenGL functions just like you would if you were directly using OpenGL 
//   headers and declarations. The difference is that this module automatically loads
//   extensions on init and so you should never need to use GetProcAddress, wglGetProcAddress, etc.
//
// - OpenGL 1.1 functions can be called unilaterally without checking if they are present,
//   as it's assumed they are always present.
//
// - In order to use an OpenGL 1.2 or later function you can check the GLEContext::WholeVersion
//   variable to tell what version of OpenGL is present and active. Example usage:
//       if(GLEContext::GetCurrentContext()->WholeVersion >= 302) // If OpenGL 3.2 or later...
//
// - In order to use an OpenGL extension, you can check the GLE_ helper macro that exists for each
//   extension. For example, in order to check of the KHR_debug is present you could do this:
//        if(GLE_KHR_debug) ... 
//   You cannot check for the presence of extensions by testing the function pointer, because
//   when hooking is enabled then we aren't using function pointers and thus all functions will
//   look like they are present. 
//
// - You can test if the OpenGL implementation is OpenGL ES by checking the GLEContext IsGLES
//   member variable. For example: if(GLEContext::GetCurrentContext()->IsGLES) ...
//
// - You can test if the OpenGL implementation is a core profile ES by checking the GLEContext IsCoreProfile
//   member variable. For example: if(GLEContext::GetCurrentContext()->IsCoreProfile) ...
//
///////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
// How to add support for additional functions to this module.
//
// For an example of how to do this, search the source files for all cases of KHR_Debug and just copy
// the things that it does but for your new extension.
//
//     1) Add the appropriate extension declaration to CAPI_GLE_GL.h, preferably by
//        copying it from the standard header file it normally comes from. If it's
//        platform-specific (e.g. a Windows wgl function) then make sure it's declared
//        within the given platform section. Note that there are potentially #defines, typedefs, 
//        function typedefs, and function #defines. There is always a GLE_ macro declared which
//        lets the user know at runtime whether the extension is present.
//        e.g.  #ifndef GL_KHR_debug
//                  #define GL_KHR_debug 1
//                  #define GL_CONTEXT_FLAG_DEBUG_BIT 0x00000002 etc.
//                  typedef void (GLAPIENTRY * PFNGLPOPDEBUGGROUPPROC) ();
//                  #define glPopDebugGroup GLEGetCurrentFunction(glPopDebugGroup)
//                  #define GLE_KHR_debug GLEGetCurrentVariable(gl_KHR_debug)
//              #endif etc.
//
//     2) Add a hook function for in the hook section of the GLEContext class in this header, 
//        ideally in the same order it's declared in the CAPI_GLE_GL.h so it's easily readable.
//        e.g. void glDebugMessageControl_Hook(GLenum source, GLenum type, GLenum severity, GLsizei count, const GLuint* ids, GLboolean enabled); etc.
//
//     3) Add a declaration for each interface function to the GLEContext class in this header.
//        e.g. PFNGLDEBUGMESSAGECALLBACKPROC glDebugMessageCallback_Impl; etc.
//
//     4) Add code to GLEContext::InitExtensionLoad to load the function pointer.
//        e.g. GLELoadProc(glDebugMessageCallback_Impl, glDebugMessageCallback); etc.
//
//     5) Add code to GLEContext::InitExtensionSupport to detect the extension support.
//        e.g. { gl_KHR_debug, "GL_KHR_debug" }, etc.
//
//     6) Implement the GLEContext hook function(s) you declared.
//        e.g.  void OVR::GLEContext::glDebugMessageControl_Hook(GLenum source, GLenum type, GLenum severity, GLsizei count, const GLuint* ids, GLboolean enabled)
//              {
//                 if(glDebugMessageControl_Impl)
//                    glDebugMessageControl_Impl(source, type, severity, count, ids, enabled);
//                 PostHook();
//              }
//
// Note that if the extension is a WGL-, GLX-, or CGL-specific extension, they are handled like above 
// but are in their own section below the section for regular OpenGL extensions.
// 
// In some cases the given interface may already be present by currently commented out,
// in which case you can simply un-comment it to enable it.
///////////////////////////////////////////////////////////////////////////////


namespace OVR
{
    // Generic OpenGL GetProcAddress function interface. Maps to platform-specific functionality
    // internally. On Windows this is equivalent to wglGetProcAddress as opposed to global GetProcAddress.
    void* GLEGetProcAddress(const char* name);



    // Manages a collection of OpenGL extension interfaces.
    // If the application has multiple OpenGL unrelated contexts then you will want to create a
    // different instance of this class for each one you intend to use it with.
    //
    GLE_CLASS_EXPORT class GLEContext
    {
    public:
        GLEContext();
       ~GLEContext();
      
        // Loads all the extensions from the current OpenGL context.
        void Init();
        
        // Clears all the extensions
        void Shutdown();

        // Returns the default instance of this class.
        static GLEContext* GetCurrentContext();
        
        // Sets the default instance of this class. This should be called after enabling a new OpenGL context.
        static void SetCurrentContext(GLEContext*);
        
    public:
        // OpenGL version information
        int   MajorVersion;             // Best guess at major version
        int   MinorVersion;             // Best guess at minor version
        int   WholeVersion;             // Equals ((MajorVersion * 100) + MinorVersion). Example usage: if(glv.WholeVersion >= 302) // If OpenGL v3.02+ ...
        bool  IsGLES;                   // Open GL ES?
        bool  IsCoreProfile;            // Is the current OpenGL context a core profile context? Its trueness may be a false positive but will never be a false negative.

        void InitVersion();             // Initializes the version information (e.g. MajorVersion). Called by the public Init function.
        void InitExtensionLoad();       // Loads the function addresses into the function pointers.
        void InitExtensionSupport();    // Loads the boolean extension support booleans.
        
    public:
        // GL_VERSION_1_1
        // Not normally included because all OpenGL 1.1 functionality is always present. But if we have 
        // hooking enabled then we implement our own version of each function.
        #if defined(GLE_HOOKING_ENABLED)
            void PostHook();            // Called at the end of a hook function.

            void            glAccum_Hook(GLenum op, GLfloat value);
            void            glAlphaFunc_Hook(GLenum func, GLclampf ref);
            GLboolean       glAreTexturesResident_Hook(GLsizei n, const GLuint *textures, GLboolean *residences);
            void            glArrayElement_Hook(GLint i);
            void            glBegin_Hook(GLenum mode);
            void            glBindTexture_Hook(GLenum target, GLuint texture);
            void            glBitmap_Hook(GLsizei width, GLsizei height, GLfloat xorig, GLfloat yorig, GLfloat xmove, GLfloat ymove, const GLubyte *bitmap);
            void            glBlendFunc_Hook(GLenum sfactor, GLenum dfactor);
            void            glCallList_Hook(GLuint list);
            void            glCallLists_Hook(GLsizei n, GLenum type, const void *lists);
            void            glClear_Hook(GLbitfield mask);
            void            glClearAccum_Hook(GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha);
            void            glClearColor_Hook(GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha);
            void            glClearDepth_Hook(GLclampd depth);
            void            glClearIndex_Hook(GLfloat c);
            void            glClearStencil_Hook(GLint s);
            void            glClipPlane_Hook(GLenum plane, const GLdouble *equation);
            void            glColor3b_Hook(GLbyte red, GLbyte green, GLbyte blue);
            void            glColor3bv_Hook(const GLbyte *v);
            void            glColor3d_Hook(GLdouble red, GLdouble green, GLdouble blue);
            void            glColor3dv_Hook(const GLdouble *v);
            void            glColor3f_Hook(GLfloat red, GLfloat green, GLfloat blue);
            void            glColor3fv_Hook(const GLfloat *v);
            void            glColor3i_Hook(GLint red, GLint green, GLint blue);
            void            glColor3iv_Hook(const GLint *v);
            void            glColor3s_Hook(GLshort red, GLshort green, GLshort blue);
            void            glColor3sv_Hook(const GLshort *v);
            void            glColor3ub_Hook(GLubyte red, GLubyte green, GLubyte blue);
            void            glColor3ubv_Hook(const GLubyte *v);
            void            glColor3ui_Hook(GLuint red, GLuint green, GLuint blue);
            void            glColor3uiv_Hook(const GLuint *v);
            void            glColor3us_Hook(GLushort red, GLushort green, GLushort blue);
            void            glColor3usv_Hook(const GLushort *v);
            void            glColor4b_Hook(GLbyte red, GLbyte green, GLbyte blue, GLbyte alpha);
            void            glColor4bv_Hook(const GLbyte *v);
            void            glColor4d_Hook(GLdouble red, GLdouble green, GLdouble blue, GLdouble alpha);
            void            glColor4dv_Hook(const GLdouble *v);
            void            glColor4f_Hook(GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha);
            void            glColor4fv_Hook(const GLfloat *v);
            void            glColor4i_Hook(GLint red, GLint green, GLint blue, GLint alpha);
            void            glColor4iv_Hook(const GLint *v);
            void            glColor4s_Hook(GLshort red, GLshort green, GLshort blue, GLshort alpha);
            void            glColor4sv_Hook(const GLshort *v);
            void            glColor4ub_Hook(GLubyte red, GLubyte green, GLubyte blue, GLubyte alpha);
            void            glColor4ubv_Hook(const GLubyte *v);
            void            glColor4ui_Hook(GLuint red, GLuint green, GLuint blue, GLuint alpha);
            void            glColor4uiv_Hook(const GLuint *v);
            void            glColor4us_Hook(GLushort red, GLushort green, GLushort blue, GLushort alpha);
            void            glColor4usv_Hook(const GLushort *v);
            void            glColorMask_Hook(GLboolean red, GLboolean green, GLboolean blue, GLboolean alpha);
            void            glColorMaterial_Hook(GLenum face, GLenum mode);
            void            glColorPointer_Hook(GLint size, GLenum type, GLsizei stride, const void *pointer);
            void            glCopyPixels_Hook(GLint x, GLint y, GLsizei width, GLsizei height, GLenum type);
            void            glCopyTexImage1D_Hook(GLenum target, GLint level, GLenum internalFormat, GLint x, GLint y, GLsizei width, GLint border);
            void            glCopyTexImage2D_Hook(GLenum target, GLint level, GLenum internalFormat, GLint x, GLint y, GLsizei width, GLsizei height, GLint border);
            void            glCopyTexSubImage1D_Hook(GLenum target, GLint level, GLint xoffset, GLint x, GLint y, GLsizei width);
            void            glCopyTexSubImage2D_Hook(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint x, GLint y, GLsizei width, GLsizei height);
            void            glCullFace_Hook(GLenum mode);
            void            glDeleteLists_Hook(GLuint list, GLsizei range);
            void            glDeleteTextures_Hook(GLsizei n, const GLuint *textures);
            void            glDepthFunc_Hook(GLenum func);
            void            glDepthMask_Hook(GLboolean flag);
            void            glDepthRange_Hook(GLclampd zNear, GLclampd zFar);
            void            glDisable_Hook(GLenum cap);
            void            glDisableClientState_Hook(GLenum array);
            void            glDrawArrays_Hook(GLenum mode, GLint first, GLsizei count);
            void            glDrawBuffer_Hook(GLenum mode);
            void            glDrawElements_Hook(GLenum mode, GLsizei count, GLenum type, const void *indices);
            void            glDrawPixels_Hook(GLsizei width, GLsizei height, GLenum format, GLenum type, const void *pixels);
            void            glEdgeFlag_Hook(GLboolean flag);
            void            glEdgeFlagPointer_Hook(GLsizei stride, const void *pointer);
            void            glEdgeFlagv_Hook(const GLboolean *flag);
            void            glEnable_Hook(GLenum cap);
            void            glEnableClientState_Hook(GLenum array);
            void            glEnd_Hook(void);
            void            glEndList_Hook(void);
            void            glEvalCoord1d_Hook(GLdouble u);
            void            glEvalCoord1dv_Hook(const GLdouble *u);
            void            glEvalCoord1f_Hook(GLfloat u);
            void            glEvalCoord1fv_Hook(const GLfloat *u);
            void            glEvalCoord2d_Hook(GLdouble u, GLdouble v);
            void            glEvalCoord2dv_Hook(const GLdouble *u);
            void            glEvalCoord2f_Hook(GLfloat u, GLfloat v);
            void            glEvalCoord2fv_Hook(const GLfloat *u);
            void            glEvalMesh1_Hook(GLenum mode, GLint i1, GLint i2);
            void            glEvalMesh2_Hook(GLenum mode, GLint i1, GLint i2, GLint j1, GLint j2);
            void            glEvalPoint1_Hook(GLint i);
            void            glEvalPoint2_Hook(GLint i, GLint j);
            void            glFeedbackBuffer_Hook(GLsizei size, GLenum type, GLfloat *buffer);
            void            glFinish_Hook(void);
            void            glFlush_Hook(void);
            void            glFogf_Hook(GLenum pname, GLfloat param);
            void            glFogfv_Hook(GLenum pname, const GLfloat *params);
            void            glFogi_Hook(GLenum pname, GLint param);
            void            glFogiv_Hook(GLenum pname, const GLint *params);
            void            glFrontFace_Hook(GLenum mode);
            void            glFrustum_Hook(GLdouble left, GLdouble right, GLdouble bottom, GLdouble top, GLdouble zNear, GLdouble zFar);
            GLuint          glGenLists_Hook(GLsizei range);
            void            glGenTextures_Hook(GLsizei n, GLuint *textures);
            void            glGetBooleanv_Hook(GLenum pname, GLboolean *params);
            void            glGetClipPlane_Hook(GLenum plane, GLdouble *equation);
            void            glGetDoublev_Hook(GLenum pname, GLdouble *params);
            GLenum          glGetError_Hook(void);
            void            glGetFloatv_Hook(GLenum pname, GLfloat *params);
            void            glGetIntegerv_Hook(GLenum pname, GLint *params);
            void            glGetLightfv_Hook(GLenum light, GLenum pname, GLfloat *params);
            void            glGetLightiv_Hook(GLenum light, GLenum pname, GLint *params);
            void            glGetMapdv_Hook(GLenum target, GLenum query, GLdouble *v);
            void            glGetMapfv_Hook(GLenum target, GLenum query, GLfloat *v);
            void            glGetMapiv_Hook(GLenum target, GLenum query, GLint *v);
            void            glGetMaterialfv_Hook(GLenum face, GLenum pname, GLfloat *params);
            void            glGetMaterialiv_Hook(GLenum face, GLenum pname, GLint *params);
            void            glGetPixelMapfv_Hook(GLenum map, GLfloat *values);
            void            glGetPixelMapuiv_Hook(GLenum map, GLuint *values);
            void            glGetPixelMapusv_Hook(GLenum map, GLushort *values);
            void            glGetPointerv_Hook(GLenum pname, void* *params);
            void            glGetPolygonStipple_Hook(GLubyte *mask);
            const GLubyte * glGetString_Hook(GLenum name);
            void            glGetTexEnvfv_Hook(GLenum target, GLenum pname, GLfloat *params);
            void            glGetTexEnviv_Hook(GLenum target, GLenum pname, GLint *params);
            void            glGetTexGendv_Hook(GLenum coord, GLenum pname, GLdouble *params);
            void            glGetTexGenfv_Hook(GLenum coord, GLenum pname, GLfloat *params);
            void            glGetTexGeniv_Hook(GLenum coord, GLenum pname, GLint *params);
            void            glGetTexImage_Hook(GLenum target, GLint level, GLenum format, GLenum type, void *pixels);
            void            glGetTexLevelParameterfv_Hook(GLenum target, GLint level, GLenum pname, GLfloat *params);
            void            glGetTexLevelParameteriv_Hook(GLenum target, GLint level, GLenum pname, GLint *params);
            void            glGetTexParameterfv_Hook(GLenum target, GLenum pname, GLfloat *params);
            void            glGetTexParameteriv_Hook(GLenum target, GLenum pname, GLint *params);
            void            glHint_Hook(GLenum target, GLenum mode);
            void            glIndexMask_Hook(GLuint mask);
            void            glIndexPointer_Hook(GLenum type, GLsizei stride, const void *pointer);
            void            glIndexd_Hook(GLdouble c);
            void            glIndexdv_Hook(const GLdouble *c);
            void            glIndexf_Hook(GLfloat c);
            void            glIndexfv_Hook(const GLfloat *c);
            void            glIndexi_Hook(GLint c);
            void            glIndexiv_Hook(const GLint *c);
            void            glIndexs_Hook(GLshort c);
            void            glIndexsv_Hook(const GLshort *c);
            void            glIndexub_Hook(GLubyte c);
            void            glIndexubv_Hook(const GLubyte *c);
            void            glInitNames_Hook(void);
            void            glInterleavedArrays_Hook(GLenum format, GLsizei stride, const void *pointer);
            GLboolean       glIsEnabled_Hook(GLenum cap);
            GLboolean       glIsList_Hook(GLuint list);
            GLboolean       glIsTexture_Hook(GLuint texture);
            void            glLightModelf_Hook(GLenum pname, GLfloat param);
            void            glLightModelfv_Hook(GLenum pname, const GLfloat *params);
            void            glLightModeli_Hook(GLenum pname, GLint param);
            void            glLightModeliv_Hook(GLenum pname, const GLint *params);
            void            glLightf_Hook(GLenum light, GLenum pname, GLfloat param);
            void            glLightfv_Hook(GLenum light, GLenum pname, const GLfloat *params);
            void            glLighti_Hook(GLenum light, GLenum pname, GLint param);
            void            glLightiv_Hook(GLenum light, GLenum pname, const GLint *params);
            void            glLineStipple_Hook(GLint factor, GLushort pattern);
            void            glLineWidth_Hook(GLfloat width);
            void            glListBase_Hook(GLuint base);
            void            glLoadIdentity_Hook(void);
            void            glLoadMatrixd_Hook(const GLdouble *m);
            void            glLoadMatrixf_Hook(const GLfloat *m);
            void            glLoadName_Hook(GLuint name);
            void            glLogicOp_Hook(GLenum opcode);
            void            glMap1d_Hook(GLenum target, GLdouble u1, GLdouble u2, GLint stride, GLint order, const GLdouble *points);
            void            glMap1f_Hook(GLenum target, GLfloat u1, GLfloat u2, GLint stride, GLint order, const GLfloat *points);
            void            glMap2d_Hook(GLenum target, GLdouble u1, GLdouble u2, GLint ustride, GLint uorder, GLdouble v1, GLdouble v2, GLint vstride, GLint vorder, const GLdouble *points);
            void            glMap2f_Hook(GLenum target, GLfloat u1, GLfloat u2, GLint ustride, GLint uorder, GLfloat v1, GLfloat v2, GLint vstride, GLint vorder, const GLfloat *points);
            void            glMapGrid1d_Hook(GLint un, GLdouble u1, GLdouble u2);
            void            glMapGrid1f_Hook(GLint un, GLfloat u1, GLfloat u2);
            void            glMapGrid2d_Hook(GLint un, GLdouble u1, GLdouble u2, GLint vn, GLdouble v1, GLdouble v2);
            void            glMapGrid2f_Hook(GLint un, GLfloat u1, GLfloat u2, GLint vn, GLfloat v1, GLfloat v2);
            void            glMaterialf_Hook(GLenum face, GLenum pname, GLfloat param);
            void            glMaterialfv_Hook(GLenum face, GLenum pname, const GLfloat *params);
            void            glMateriali_Hook(GLenum face, GLenum pname, GLint param);
            void            glMaterialiv_Hook(GLenum face, GLenum pname, const GLint *params);
            void            glMatrixMode_Hook(GLenum mode);
            void            glMultMatrixd_Hook(const GLdouble *m);
            void            glMultMatrixf_Hook(const GLfloat *m);
            void            glNewList_Hook(GLuint list, GLenum mode);
            void            glNormal3b_Hook(GLbyte nx, GLbyte ny, GLbyte nz);
            void            glNormal3bv_Hook(const GLbyte *v);
            void            glNormal3d_Hook(GLdouble nx, GLdouble ny, GLdouble nz);
            void            glNormal3dv_Hook(const GLdouble *v);
            void            glNormal3f_Hook(GLfloat nx, GLfloat ny, GLfloat nz);
            void            glNormal3fv_Hook(const GLfloat *v);
            void            glNormal3i_Hook(GLint nx, GLint ny, GLint nz);
            void            glNormal3iv_Hook(const GLint *v);
            void            glNormal3s_Hook(GLshort nx, GLshort ny, GLshort nz);
            void            glNormal3sv_Hook(const GLshort *v);
            void            glNormalPointer_Hook(GLenum type, GLsizei stride, const void *pointer);
            void            glOrtho_Hook(GLdouble left, GLdouble right, GLdouble bottom, GLdouble top, GLdouble zNear, GLdouble zFar);
            void            glPassThrough_Hook(GLfloat token);
            void            glPixelMapfv_Hook(GLenum map, GLsizei mapsize, const GLfloat *values);
            void            glPixelMapuiv_Hook(GLenum map, GLsizei mapsize, const GLuint *values);
            void            glPixelMapusv_Hook(GLenum map, GLsizei mapsize, const GLushort *values);
            void            glPixelStoref_Hook(GLenum pname, GLfloat param);
            void            glPixelStorei_Hook(GLenum pname, GLint param);
            void            glPixelTransferf_Hook(GLenum pname, GLfloat param);
            void            glPixelTransferi_Hook(GLenum pname, GLint param);
            void            glPixelZoom_Hook(GLfloat xfactor, GLfloat yfactor);
            void            glPointSize_Hook(GLfloat size);
            void            glPolygonMode_Hook(GLenum face, GLenum mode);
            void            glPolygonOffset_Hook(GLfloat factor, GLfloat units);
            void            glPolygonStipple_Hook(const GLubyte *mask);
            void            glPopAttrib_Hook(void);
            void            glPopClientAttrib_Hook(void);
            void            glPopMatrix_Hook(void);
            void            glPopName_Hook(void);
            void            glPrioritizeTextures_Hook(GLsizei n, const GLuint *textures, const GLclampf *priorities);
            void            glPushAttrib_Hook(GLbitfield mask);
            void            glPushClientAttrib_Hook(GLbitfield mask);
            void            glPushMatrix_Hook(void);
            void            glPushName_Hook(GLuint name);
            void            glRasterPos2d_Hook(GLdouble x, GLdouble y);
            void            glRasterPos2dv_Hook(const GLdouble *v);
            void            glRasterPos2f_Hook(GLfloat x, GLfloat y);
            void            glRasterPos2fv_Hook(const GLfloat *v);
            void            glRasterPos2i_Hook(GLint x, GLint y);
            void            glRasterPos2iv_Hook(const GLint *v);
            void            glRasterPos2s_Hook(GLshort x, GLshort y);
            void            glRasterPos2sv_Hook(const GLshort *v);
            void            glRasterPos3d_Hook(GLdouble x, GLdouble y, GLdouble z);
            void            glRasterPos3dv_Hook(const GLdouble *v);
            void            glRasterPos3f_Hook(GLfloat x, GLfloat y, GLfloat z);
            void            glRasterPos3fv_Hook(const GLfloat *v);
            void            glRasterPos3i_Hook(GLint x, GLint y, GLint z);
            void            glRasterPos3iv_Hook(const GLint *v);
            void            glRasterPos3s_Hook(GLshort x, GLshort y, GLshort z);
            void            glRasterPos3sv_Hook(const GLshort *v);
            void            glRasterPos4d_Hook(GLdouble x, GLdouble y, GLdouble z, GLdouble w);
            void            glRasterPos4dv_Hook(const GLdouble *v);
            void            glRasterPos4f_Hook(GLfloat x, GLfloat y, GLfloat z, GLfloat w);
            void            glRasterPos4fv_Hook(const GLfloat *v);
            void            glRasterPos4i_Hook(GLint x, GLint y, GLint z, GLint w);
            void            glRasterPos4iv_Hook(const GLint *v);
            void            glRasterPos4s_Hook(GLshort x, GLshort y, GLshort z, GLshort w);
            void            glRasterPos4sv_Hook(const GLshort *v);
            void            glReadBuffer_Hook(GLenum mode);
            void            glReadPixels_Hook(GLint x, GLint y, GLsizei width, GLsizei height, GLenum format, GLenum type, void *pixels);
            void            glRectd_Hook(GLdouble x1, GLdouble y1, GLdouble x2, GLdouble y2);
            void            glRectdv_Hook(const GLdouble *v1, const GLdouble *v2);
            void            glRectf_Hook(GLfloat x1, GLfloat y1, GLfloat x2, GLfloat y2);
            void            glRectfv_Hook(const GLfloat *v1, const GLfloat *v2);
            void            glRecti_Hook(GLint x1, GLint y1, GLint x2, GLint y2);
            void            glRectiv_Hook(const GLint *v1, const GLint *v2);
            void            glRects_Hook(GLshort x1, GLshort y1, GLshort x2, GLshort y2);
            void            glRectsv_Hook(const GLshort *v1, const GLshort *v2);
            GLint           glRenderMode_Hook(GLenum mode);
            void            glRotated_Hook(GLdouble angle, GLdouble x, GLdouble y, GLdouble z);
            void            glRotatef_Hook(GLfloat angle, GLfloat x, GLfloat y, GLfloat z);
            void            glScaled_Hook(GLdouble x, GLdouble y, GLdouble z);
            void            glScalef_Hook(GLfloat x, GLfloat y, GLfloat z);
            void            glScissor_Hook(GLint x, GLint y, GLsizei width, GLsizei height);
            void            glSelectBuffer_Hook(GLsizei size, GLuint *buffer);
            void            glShadeModel_Hook(GLenum mode);
            void            glStencilFunc_Hook(GLenum func, GLint ref, GLuint mask);
            void            glStencilMask_Hook(GLuint mask);
            void            glStencilOp_Hook(GLenum fail, GLenum zfail, GLenum zpass);
            void            glTexCoord1d_Hook(GLdouble s);
            void            glTexCoord1dv_Hook(const GLdouble *v);
            void            glTexCoord1f_Hook(GLfloat s);
            void            glTexCoord1fv_Hook(const GLfloat *v);
            void            glTexCoord1i_Hook(GLint s);
            void            glTexCoord1iv_Hook(const GLint *v);
            void            glTexCoord1s_Hook(GLshort s);
            void            glTexCoord1sv_Hook(const GLshort *v);
            void            glTexCoord2d_Hook(GLdouble s, GLdouble t);
            void            glTexCoord2dv_Hook(const GLdouble *v);
            void            glTexCoord2f_Hook(GLfloat s, GLfloat t);
            void            glTexCoord2fv_Hook(const GLfloat *v);
            void            glTexCoord2i_Hook(GLint s, GLint t);
            void            glTexCoord2iv_Hook(const GLint *v);
            void            glTexCoord2s_Hook(GLshort s, GLshort t);
            void            glTexCoord2sv_Hook(const GLshort *v);
            void            glTexCoord3d_Hook(GLdouble s, GLdouble t, GLdouble r);
            void            glTexCoord3dv_Hook(const GLdouble *v);
            void            glTexCoord3f_Hook(GLfloat s, GLfloat t, GLfloat r);
            void            glTexCoord3fv_Hook(const GLfloat *v);
            void            glTexCoord3i_Hook(GLint s, GLint t, GLint r);
            void            glTexCoord3iv_Hook(const GLint *v);
            void            glTexCoord3s_Hook(GLshort s, GLshort t, GLshort r);
            void            glTexCoord3sv_Hook(const GLshort *v);
            void            glTexCoord4d_Hook(GLdouble s, GLdouble t, GLdouble r, GLdouble q);
            void            glTexCoord4dv_Hook(const GLdouble *v);
            void            glTexCoord4f_Hook(GLfloat s, GLfloat t, GLfloat r, GLfloat q);
            void            glTexCoord4fv_Hook(const GLfloat *v);
            void            glTexCoord4i_Hook(GLint s, GLint t, GLint r, GLint q);
            void            glTexCoord4iv_Hook(const GLint *v);
            void            glTexCoord4s_Hook(GLshort s, GLshort t, GLshort r, GLshort q);
            void            glTexCoord4sv_Hook(const GLshort *v);
            void            glTexCoordPointer_Hook(GLint size, GLenum type, GLsizei stride, const void *pointer);
            void            glTexEnvf_Hook(GLenum target, GLenum pname, GLfloat param);
            void            glTexEnvfv_Hook(GLenum target, GLenum pname, const GLfloat *params);
            void            glTexEnvi_Hook(GLenum target, GLenum pname, GLint param);
            void            glTexEnviv_Hook(GLenum target, GLenum pname, const GLint *params);
            void            glTexGend_Hook(GLenum coord, GLenum pname, GLdouble param);
            void            glTexGendv_Hook(GLenum coord, GLenum pname, const GLdouble *params);
            void            glTexGenf_Hook(GLenum coord, GLenum pname, GLfloat param);
            void            glTexGenfv_Hook(GLenum coord, GLenum pname, const GLfloat *params);
            void            glTexGeni_Hook(GLenum coord, GLenum pname, GLint param);
            void            glTexGeniv_Hook(GLenum coord, GLenum pname, const GLint *params);
            void            glTexImage1D_Hook(GLenum target, GLint level, GLint internalformat, GLsizei width, GLint border, GLenum format, GLenum type, const void *pixels);
            void            glTexImage2D_Hook(GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const void *pixels);
            void            glTexParameterf_Hook(GLenum target, GLenum pname, GLfloat param);
            void            glTexParameterfv_Hook(GLenum target, GLenum pname, const GLfloat *params);
            void            glTexParameteri_Hook(GLenum target, GLenum pname, GLint param);
            void            glTexParameteriv_Hook(GLenum target, GLenum pname, const GLint *params);
            void            glTexSubImage1D_Hook(GLenum target, GLint level, GLint xoffset, GLsizei width, GLenum format, GLenum type, const void *pixels);
            void            glTexSubImage2D_Hook(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const void *pixels);
            void            glTranslated_Hook(GLdouble x, GLdouble y, GLdouble z);
            void            glTranslatef_Hook(GLfloat x, GLfloat y, GLfloat z);
            void            glVertex2d_Hook(GLdouble x, GLdouble y);
            void            glVertex2dv_Hook(const GLdouble *v);
            void            glVertex2f_Hook(GLfloat x, GLfloat y);
            void            glVertex2fv_Hook(const GLfloat *v);
            void            glVertex2i_Hook(GLint x, GLint y);
            void            glVertex2iv_Hook(const GLint *v);
            void            glVertex2s_Hook(GLshort x, GLshort y);
            void            glVertex2sv_Hook(const GLshort *v);
            void            glVertex3d_Hook(GLdouble x, GLdouble y, GLdouble z);
            void            glVertex3dv_Hook(const GLdouble *v);
            void            glVertex3f_Hook(GLfloat x, GLfloat y, GLfloat z);
            void            glVertex3fv_Hook(const GLfloat *v);
            void            glVertex3i_Hook(GLint x, GLint y, GLint z);
            void            glVertex3iv_Hook(const GLint *v);
            void            glVertex3s_Hook(GLshort x, GLshort y, GLshort z);
            void            glVertex3sv_Hook(const GLshort *v);
            void            glVertex4d_Hook(GLdouble x, GLdouble y, GLdouble z, GLdouble w);
            void            glVertex4dv_Hook(const GLdouble *v);
            void            glVertex4f_Hook(GLfloat x, GLfloat y, GLfloat z, GLfloat w);
            void            glVertex4fv_Hook(const GLfloat *v);
            void            glVertex4i_Hook(GLint x, GLint y, GLint z, GLint w);
            void            glVertex4iv_Hook(const GLint *v);
            void            glVertex4s_Hook(GLshort x, GLshort y, GLshort z, GLshort w);
            void            glVertex4sv_Hook(const GLshort *v);
            void            glVertexPointer_Hook(GLint size, GLenum type, GLsizei stride, const void *pointer);
            void            glViewport_Hook(GLint x, GLint y, GLsizei width, GLsizei height);

            // GL_VERSION_1_2
            void glBlendColor_Hook(GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha);
            void glBlendEquation_Hook(GLenum mode);
            void glDrawRangeElements_Hook(GLenum mode, GLuint start, GLuint end, GLsizei count, GLenum type, const GLvoid *indices);
            void glTexImage3D_Hook(GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLsizei depth, GLint border, GLenum format, GLenum type, const GLvoid *pixels);
            void glTexSubImage3D_Hook(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum type, const GLvoid *pixels);
            void glCopyTexSubImage3D_Hook(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLint x, GLint y, GLsizei width, GLsizei height);

            // GL_VERSION_1_2 deprecated functions
            /* Not currently supported
            void glColorTable_Hook(GLenum target, GLenum internalformat, GLsizei width, GLenum format, GLenum type, const GLvoid *table);
            void glColorTableParameterfv_Hook(GLenum target, GLenum pname, const GLfloat *params);
            void glColorTableParameteriv_Hook(GLenum target, GLenum pname, const GLint *params);
            void glCopyColorTable_Hook(GLenum target, GLenum internalformat, GLint x, GLint y, GLsizei width);
            void glGetColorTable_Hook(GLenum target, GLenum format, GLenum type, GLvoid *table);
            void glGetColorTableParameterfv_Hook(GLenum target, GLenum pname, GLfloat *params);
            void glGetColorTableParameteriv_Hook(GLenum target, GLenum pname, GLint *params);
            void glColorSubTable_Hook(GLenum target, GLsizei start, GLsizei count, GLenum format, GLenum type, const GLvoid *data);
            void glCopyColorSubTable_Hook(GLenum target, GLsizei start, GLint x, GLint y, GLsizei width);
            void glConvolutionFilter1D_Hook(GLenum target, GLenum internalformat, GLsizei width, GLenum format, GLenum type, const GLvoid *image);
            void glConvolutionFilter2D_Hook(GLenum target, GLenum internalformat, GLsizei width, GLsizei height, GLenum format, GLenum type, const GLvoid *image);
            void glConvolutionParameterf_Hook(GLenum target, GLenum pname, GLfloat params);
            void glConvolutionParameterfv_Hook(GLenum target, GLenum pname, const GLfloat *params);
            void glConvolutionParameteri_Hook(GLenum target, GLenum pname, GLint params);
            void glConvolutionParameteriv_Hook(GLenum target, GLenum pname, const GLint *params);
            void glCopyConvolutionFilter1D_Hook(GLenum target, GLenum internalformat, GLint x, GLint y, GLsizei width);
            void glCopyConvolutionFilter2D_Hook(GLenum target, GLenum internalformat, GLint x, GLint y, GLsizei width, GLsizei height);
            void glGetConvolutionFilter_Hook(GLenum target, GLenum format, GLenum type, GLvoid *image);
            void glGetConvolutionParameterfv_Hook(GLenum target, GLenum pname, GLfloat *params);
            void glGetConvolutionParameteriv_Hook(GLenum target, GLenum pname, GLint *params);
            void glGetSeparableFilter_Hook(GLenum target, GLenum format, GLenum type, GLvoid *row, GLvoid *column, GLvoid *span);
            void glSeparableFilter2D_Hook(GLenum target, GLenum internalformat, GLsizei width, GLsizei height, GLenum format, GLenum type, const GLvoid *row, const GLvoid *column);
            void glGetHistogram_Hook(GLenum target, GLboolean reset, GLenum format, GLenum type, GLvoid *values);
            void glGetHistogramParameterfv_Hook(GLenum target, GLenum pname, GLfloat *params);
            void glGetHistogramParameteriv_Hook(GLenum target, GLenum pname, GLint *params);
            void glGetMinmax_Hook(GLenum target, GLboolean reset, GLenum format, GLenum type, GLvoid *values);
            void glGetMinmaxParameterfv_Hook(GLenum target, GLenum pname, GLfloat *params);
            void glGetMinmaxParameteriv_Hook(GLenum target, GLenum pname, GLint *params);
            void glHistogram_Hook(GLenum target, GLsizei width, GLenum internalformat, GLboolean sink);
            void glMinmax_Hook(GLenum target, GLenum internalformat, GLboolean sink);
            void glResetHistogram_Hook(GLenum target);
            void glResetMinmax_Hook(GLenum target);
            */
        
            // GL_VERSION_1_3
            void glActiveTexture_Hook(GLenum texture);
            void glSampleCoverage_Hook(GLclampf value, GLboolean invert);
            void glCompressedTexImage3D_Hook(GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth, GLint border, GLsizei imageSize, const GLvoid *data);
            void glCompressedTexImage2D_Hook(GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLint border, GLsizei imageSize, const GLvoid *data);
            void glCompressedTexImage1D_Hook(GLenum target, GLint level, GLenum internalformat, GLsizei width, GLint border, GLsizei imageSize, const GLvoid *data);
            void glCompressedTexSubImage3D_Hook(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLsizei imageSize, const GLvoid *data);
            void glCompressedTexSubImage2D_Hook(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLsizei imageSize, const GLvoid *data);
            void glCompressedTexSubImage1D_Hook(GLenum target, GLint level, GLint xoffset, GLsizei width, GLenum format, GLsizei imageSize, const GLvoid *data);
            void glGetCompressedTexImage_Hook(GLenum target, GLint level, GLvoid *img);

            // GL_VERSION_1_3 deprecated functions
            void glClientActiveTexture_Hook(GLenum texture);
            void glMultiTexCoord1d_Hook(GLenum target, GLdouble s);
            void glMultiTexCoord1dv_Hook(GLenum target, const GLdouble *v);
            void glMultiTexCoord1f_Hook(GLenum target, GLfloat s);
            void glMultiTexCoord1fv_Hook(GLenum target, const GLfloat *v);
            void glMultiTexCoord1i_Hook(GLenum target, GLint s);
            void glMultiTexCoord1iv_Hook(GLenum target, const GLint *v);
            void glMultiTexCoord1s_Hook(GLenum target, GLshort s);
            void glMultiTexCoord1sv_Hook(GLenum target, const GLshort *v);
            void glMultiTexCoord2d_Hook(GLenum target, GLdouble s, GLdouble t);
            void glMultiTexCoord2dv_Hook(GLenum target, const GLdouble *v);
            void glMultiTexCoord2f_Hook(GLenum target, GLfloat s, GLfloat t);
            void glMultiTexCoord2fv_Hook(GLenum target, const GLfloat *v);
            void glMultiTexCoord2i_Hook(GLenum target, GLint s, GLint t);
            void glMultiTexCoord2iv_Hook(GLenum target, const GLint *v);
            void glMultiTexCoord2s_Hook(GLenum target, GLshort s, GLshort t);
            void glMultiTexCoord2sv_Hook(GLenum target, const GLshort *v);
            void glMultiTexCoord3d_Hook(GLenum target, GLdouble s, GLdouble t, GLdouble r);
            void glMultiTexCoord3dv_Hook(GLenum target, const GLdouble *v);
            void glMultiTexCoord3f_Hook(GLenum target, GLfloat s, GLfloat t, GLfloat r);
            void glMultiTexCoord3fv_Hook(GLenum target, const GLfloat *v);
            void glMultiTexCoord3i_Hook(GLenum target, GLint s, GLint t, GLint r);
            void glMultiTexCoord3iv_Hook(GLenum target, const GLint *v);
            void glMultiTexCoord3s_Hook(GLenum target, GLshort s, GLshort t, GLshort r);
            void glMultiTexCoord3sv_Hook(GLenum target, const GLshort *v);
            void glMultiTexCoord4d_Hook(GLenum target, GLdouble s, GLdouble t, GLdouble r, GLdouble q);
            void glMultiTexCoord4dv_Hook(GLenum target, const GLdouble *v);
            void glMultiTexCoord4f_Hook(GLenum target, GLfloat s, GLfloat t, GLfloat r, GLfloat q);
            void glMultiTexCoord4fv_Hook(GLenum target, const GLfloat *v);
            void glMultiTexCoord4i_Hook(GLenum target, GLint s, GLint t, GLint r, GLint q);
            void glMultiTexCoord4iv_Hook(GLenum target, const GLint *v);
            void glMultiTexCoord4s_Hook(GLenum target, GLshort s, GLshort t, GLshort r, GLshort q);
            void glMultiTexCoord4sv_Hook(GLenum target, const GLshort *v);
            void glLoadTransposeMatrixf_Hook(const GLfloat *m);
            void glLoadTransposeMatrixd_Hook(const GLdouble *m);
            void glMultTransposeMatrixf_Hook(const GLfloat *m);
            void glMultTransposeMatrixd_Hook(const GLdouble *m);

            // GL_VERSION_1_4
            void glBlendFuncSeparate_Hook(GLenum sfactorRGB, GLenum dfactorRGB, GLenum sfactorAlpha, GLenum dfactorAlpha);
            void glMultiDrawArrays_Hook(GLenum mode, const GLint *first, const GLsizei *count, GLsizei primcount);
            void glMultiDrawElements_Hook(GLenum mode, const GLsizei *count, GLenum type, const GLvoid* *indices, GLsizei primcount);
            void glPointParameterf_Hook(GLenum pname, GLfloat param);
            void glPointParameterfv_Hook(GLenum pname, const GLfloat *params);
            void glPointParameteri_Hook(GLenum pname, GLint param);
            void glPointParameteriv_Hook(GLenum pname, const GLint *params);

            // GL_VERSION_1_4 deprecated functions
            void glFogCoordf_Hook(GLfloat coord);
            void glFogCoordfv_Hook(const GLfloat *coord);
            void glFogCoordd_Hook(GLdouble coord);
            void glFogCoorddv_Hook(const GLdouble *coord);
            void glFogCoordPointer_Hook(GLenum type, GLsizei stride, const GLvoid *pointer);
            void glSecondaryColor3b_Hook(GLbyte red, GLbyte green, GLbyte blue);
            void glSecondaryColor3bv_Hook(const GLbyte *v);
            void glSecondaryColor3d_Hook(GLdouble red, GLdouble green, GLdouble blue);
            void glSecondaryColor3dv_Hook(const GLdouble *v);
            void glSecondaryColor3f_Hook(GLfloat red, GLfloat green, GLfloat blue);
            void glSecondaryColor3fv_Hook(const GLfloat *v);
            void glSecondaryColor3i_Hook(GLint red, GLint green, GLint blue);
            void glSecondaryColor3iv_Hook(const GLint *v);
            void glSecondaryColor3s_Hook(GLshort red, GLshort green, GLshort blue);
            void glSecondaryColor3sv_Hook(const GLshort *v);
            void glSecondaryColor3ub_Hook(GLubyte red, GLubyte green, GLubyte blue);
            void glSecondaryColor3ubv_Hook(const GLubyte *v);
            void glSecondaryColor3ui_Hook(GLuint red, GLuint green, GLuint blue);
            void glSecondaryColor3uiv_Hook(const GLuint *v);
            void glSecondaryColor3us_Hook(GLushort red, GLushort green, GLushort blue);
            void glSecondaryColor3usv_Hook(const GLushort *v);
            void glSecondaryColorPointer_Hook(GLint size, GLenum type, GLsizei stride, const GLvoid *pointer);
            void glWindowPos2d_Hook(GLdouble x, GLdouble y);
            void glWindowPos2dv_Hook(const GLdouble *v);
            void glWindowPos2f_Hook(GLfloat x, GLfloat y);
            void glWindowPos2fv_Hook(const GLfloat *v);
            void glWindowPos2i_Hook(GLint x, GLint y);
            void glWindowPos2iv_Hook(const GLint *v);
            void glWindowPos2s_Hook(GLshort x, GLshort y);
            void glWindowPos2sv_Hook(const GLshort *v);
            void glWindowPos3d_Hook(GLdouble x, GLdouble y, GLdouble z);
            void glWindowPos3dv_Hook(const GLdouble *v);
            void glWindowPos3f_Hook(GLfloat x, GLfloat y, GLfloat z);
            void glWindowPos3fv_Hook(const GLfloat *v);
            void glWindowPos3i_Hook(GLint x, GLint y, GLint z);
            void glWindowPos3iv_Hook(const GLint *v);
            void glWindowPos3s_Hook(GLshort x, GLshort y, GLshort z);
            void glWindowPos3sv_Hook(const GLshort *v);

            // GL_VERSION_1_5
            void glGenQueries_Hook(GLsizei n, GLuint *ids);
            void glDeleteQueries_Hook(GLsizei n, const GLuint *ids);
            GLboolean glIsQuery_Hook(GLuint id);
            void glBeginQuery_Hook(GLenum target, GLuint id);
            void glEndQuery_Hook(GLenum target);
            void glGetQueryiv_Hook(GLenum target, GLenum pname, GLint *params);
            void glGetQueryObjectiv_Hook(GLuint id, GLenum pname, GLint *params);
            void glGetQueryObjectuiv_Hook(GLuint id, GLenum pname, GLuint *params);
            void glBindBuffer_Hook(GLenum target, GLuint buffer);
            void glDeleteBuffers_Hook(GLsizei n, const GLuint *buffers);
            void glGenBuffers_Hook(GLsizei n, GLuint *buffers);
            GLboolean glIsBuffer_Hook(GLuint buffer);
            void glBufferData_Hook(GLenum target, GLsizeiptr size, const GLvoid *data, GLenum usage);
            void glBufferSubData_Hook(GLenum target, GLintptr offset, GLsizeiptr size, const GLvoid *data);
            void glGetBufferSubData_Hook(GLenum target, GLintptr offset, GLsizeiptr size, GLvoid *data);
            GLvoid* glMapBuffer_Hook(GLenum target, GLenum access);
            GLboolean glUnmapBuffer_Hook(GLenum target);
            void glGetBufferParameteriv_Hook(GLenum target, GLenum pname, GLint *params);
            void glGetBufferPointerv_Hook(GLenum target, GLenum pname, GLvoid* *params);

            // GL_VERSION_2_0
            void glBlendEquationSeparate_Hook(GLenum modeRGB, GLenum modeAlpha);
            void glDrawBuffers_Hook(GLsizei n, const GLenum *bufs);
            void glStencilOpSeparate_Hook(GLenum face, GLenum sfail, GLenum dpfail, GLenum dppass);
            void glStencilFuncSeparate_Hook(GLenum face, GLenum func, GLint ref, GLuint mask);
            void glStencilMaskSeparate_Hook(GLenum face, GLuint mask);
            void glAttachShader_Hook(GLuint program, GLuint shader);
            void glBindAttribLocation_Hook(GLuint program, GLuint index, const GLchar *name);
            void glCompileShader_Hook(GLuint shader);
            GLuint glCreateProgram_Hook(void);
            GLuint glCreateShader_Hook(GLenum type);
            void glDeleteProgram_Hook(GLuint program);
            void glDeleteShader_Hook(GLuint shader);
            void glDetachShader_Hook(GLuint program, GLuint shader);
            void glDisableVertexAttribArray_Hook(GLuint index);
            void glEnableVertexAttribArray_Hook(GLuint index);
            void glGetActiveAttrib_Hook(GLuint program, GLuint index, GLsizei bufSize, GLsizei *length, GLint *size, GLenum *type, GLchar *name);
            void glGetActiveUniform_Hook(GLuint program, GLuint index, GLsizei bufSize, GLsizei *length, GLint *size, GLenum *type, GLchar *name);
            void glGetAttachedShaders_Hook(GLuint program, GLsizei maxCount, GLsizei *count, GLuint *obj);
            GLint glGetAttribLocation_Hook(GLuint program, const GLchar *name);
            void glGetProgramiv_Hook(GLuint program, GLenum pname, GLint *params);
            void glGetProgramInfoLog_Hook(GLuint program, GLsizei bufSize, GLsizei *length, GLchar *infoLog);
            void glGetShaderiv_Hook(GLuint shader, GLenum pname, GLint *params);
            void glGetShaderInfoLog_Hook(GLuint shader, GLsizei bufSize, GLsizei *length, GLchar *infoLog);
            void glGetShaderSource_Hook(GLuint shader, GLsizei bufSize, GLsizei *length, GLchar *source);
            GLint glGetUniformLocation_Hook(GLuint program, const GLchar *name);
            void glGetUniformfv_Hook(GLuint program, GLint location, GLfloat *params);
            void glGetUniformiv_Hook(GLuint program, GLint location, GLint *params);
            void glGetVertexAttribdv_Hook(GLuint index, GLenum pname, GLdouble *params);
            void glGetVertexAttribfv_Hook(GLuint index, GLenum pname, GLfloat *params);
            void glGetVertexAttribiv_Hook(GLuint index, GLenum pname, GLint *params);
            void glGetVertexAttribPointerv_Hook(GLuint index, GLenum pname, GLvoid* *pointer);
            GLboolean glIsProgram_Hook(GLuint program);
            GLboolean glIsShader_Hook(GLuint shader);
            void glLinkProgram_Hook(GLuint program);
            void glShaderSource_Hook(GLuint shader, GLsizei count, const GLchar* *string, const GLint *length);
            void glUseProgram_Hook(GLuint program);
            void glUniform1f_Hook(GLint location, GLfloat v0);
            void glUniform2f_Hook(GLint location, GLfloat v0, GLfloat v1);
            void glUniform3f_Hook(GLint location, GLfloat v0, GLfloat v1, GLfloat v2);
            void glUniform4f_Hook(GLint location, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3);
            void glUniform1i_Hook(GLint location, GLint v0);
            void glUniform2i_Hook(GLint location, GLint v0, GLint v1);
            void glUniform3i_Hook(GLint location, GLint v0, GLint v1, GLint v2);
            void glUniform4i_Hook(GLint location, GLint v0, GLint v1, GLint v2, GLint v3);
            void glUniform1fv_Hook(GLint location, GLsizei count, const GLfloat *value);
            void glUniform2fv_Hook(GLint location, GLsizei count, const GLfloat *value);
            void glUniform3fv_Hook(GLint location, GLsizei count, const GLfloat *value);
            void glUniform4fv_Hook(GLint location, GLsizei count, const GLfloat *value);
            void glUniform1iv_Hook(GLint location, GLsizei count, const GLint *value);
            void glUniform2iv_Hook(GLint location, GLsizei count, const GLint *value);
            void glUniform3iv_Hook(GLint location, GLsizei count, const GLint *value);
            void glUniform4iv_Hook(GLint location, GLsizei count, const GLint *value);
            void glUniformMatrix2fv_Hook(GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
            void glUniformMatrix3fv_Hook(GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
            void glUniformMatrix4fv_Hook(GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
            void glValidateProgram_Hook(GLuint program);
            void glVertexAttrib1d_Hook(GLuint index, GLdouble x);
            void glVertexAttrib1dv_Hook(GLuint index, const GLdouble *v);
            void glVertexAttrib1f_Hook(GLuint index, GLfloat x);
            void glVertexAttrib1fv_Hook(GLuint index, const GLfloat *v);
            void glVertexAttrib1s_Hook(GLuint index, GLshort x);
            void glVertexAttrib1sv_Hook(GLuint index, const GLshort *v);
            void glVertexAttrib2d_Hook(GLuint index, GLdouble x, GLdouble y);
            void glVertexAttrib2dv_Hook(GLuint index, const GLdouble *v);
            void glVertexAttrib2f_Hook(GLuint index, GLfloat x, GLfloat y);
            void glVertexAttrib2fv_Hook(GLuint index, const GLfloat *v);
            void glVertexAttrib2s_Hook(GLuint index, GLshort x, GLshort y);
            void glVertexAttrib2sv_Hook(GLuint index, const GLshort *v);
            void glVertexAttrib3d_Hook(GLuint index, GLdouble x, GLdouble y, GLdouble z);
            void glVertexAttrib3dv_Hook(GLuint index, const GLdouble *v);
            void glVertexAttrib3f_Hook(GLuint index, GLfloat x, GLfloat y, GLfloat z);
            void glVertexAttrib3fv_Hook(GLuint index, const GLfloat *v);
            void glVertexAttrib3s_Hook(GLuint index, GLshort x, GLshort y, GLshort z);
            void glVertexAttrib3sv_Hook(GLuint index, const GLshort *v);
            void glVertexAttrib4Nbv_Hook(GLuint index, const GLbyte *v);
            void glVertexAttrib4Niv_Hook(GLuint index, const GLint *v);
            void glVertexAttrib4Nsv_Hook(GLuint index, const GLshort *v);
            void glVertexAttrib4Nub_Hook(GLuint index, GLubyte x, GLubyte y, GLubyte z, GLubyte w);
            void glVertexAttrib4Nubv_Hook(GLuint index, const GLubyte *v);
            void glVertexAttrib4Nuiv_Hook(GLuint index, const GLuint *v);
            void glVertexAttrib4Nusv_Hook(GLuint index, const GLushort *v);
            void glVertexAttrib4bv_Hook(GLuint index, const GLbyte *v);
            void glVertexAttrib4d_Hook(GLuint index, GLdouble x, GLdouble y, GLdouble z, GLdouble w);
            void glVertexAttrib4dv_Hook(GLuint index, const GLdouble *v);
            void glVertexAttrib4f_Hook(GLuint index, GLfloat x, GLfloat y, GLfloat z, GLfloat w);
            void glVertexAttrib4fv_Hook(GLuint index, const GLfloat *v);
            void glVertexAttrib4iv_Hook(GLuint index, const GLint *v);
            void glVertexAttrib4s_Hook(GLuint index, GLshort x, GLshort y, GLshort z, GLshort w);
            void glVertexAttrib4sv_Hook(GLuint index, const GLshort *v);
            void glVertexAttrib4ubv_Hook(GLuint index, const GLubyte *v);
            void glVertexAttrib4uiv_Hook(GLuint index, const GLuint *v);
            void glVertexAttrib4usv_Hook(GLuint index, const GLushort *v);
            void glVertexAttribPointer_Hook(GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const GLvoid *pointer);

            // GL_VERSION_2_1
            void glUniformMatrix2x3fv_Hook(GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
            void glUniformMatrix3x2fv_Hook(GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
            void glUniformMatrix2x4fv_Hook(GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
            void glUniformMatrix4x2fv_Hook(GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
            void glUniformMatrix3x4fv_Hook(GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
            void glUniformMatrix4x3fv_Hook(GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);

            // GL_VERSION_3_0
            void glColorMaski_Hook(GLuint index, GLboolean r, GLboolean g, GLboolean b, GLboolean a);
            void glGetBooleani_v_Hook(GLenum target, GLuint index, GLboolean *data);
            void  glGetIntegeri_v_Hook(GLenum target, GLuint index, GLint *data);
            void   glEnablei_Hook(GLenum target, GLuint index);
            void     glDisablei_Hook(GLenum target, GLuint index);
            GLboolean glIsEnabledi_Hook(GLenum target, GLuint index);
            void     glBeginTransformFeedback_Hook(GLenum primitiveMode);
            void    glEndTransformFeedback_Hook(void);
            void  glBindBufferRange_Hook(GLenum target, GLuint index, GLuint buffer, GLintptr offset, GLsizeiptr size);
            void glBindBufferBase_Hook(GLenum target, GLuint index, GLuint buffer);
            void glTransformFeedbackVaryings_Hook(GLuint program, GLsizei count, const GLchar* *varyings, GLenum bufferMode);
            void glGetTransformFeedbackVarying_Hook(GLuint program, GLuint index, GLsizei bufSize, GLsizei *length, GLsizei *size, GLenum *type, GLchar *name);
            void glClampColor_Hook(GLenum target, GLenum clamp);
            void glBeginConditionalRender_Hook(GLuint id, GLenum mode);
            void glEndConditionalRender_Hook(void);
            void glVertexAttribIPointer_Hook(GLuint index, GLint size, GLenum type, GLsizei stride, const GLvoid *pointer);
            void glGetVertexAttribIiv_Hook(GLuint index, GLenum pname, GLint *params);
            void glGetVertexAttribIuiv_Hook(GLuint index, GLenum pname, GLuint *params);
            void glVertexAttribI1i_Hook(GLuint index, GLint x);
            void glVertexAttribI2i_Hook(GLuint index, GLint x, GLint y);
            void glVertexAttribI3i_Hook(GLuint index, GLint x, GLint y, GLint z);
            void glVertexAttribI4i_Hook(GLuint index, GLint x, GLint y, GLint z, GLint w);
            void glVertexAttribI1ui_Hook(GLuint index, GLuint x);
            void glVertexAttribI2ui_Hook(GLuint index, GLuint x, GLuint y);
            void glVertexAttribI3ui_Hook(GLuint index, GLuint x, GLuint y, GLuint z);
            void glVertexAttribI4ui_Hook(GLuint index, GLuint x, GLuint y, GLuint z, GLuint w);
            void glVertexAttribI1iv_Hook(GLuint index, const GLint *v);
            void glVertexAttribI2iv_Hook(GLuint index, const GLint *v);
            void glVertexAttribI3iv_Hook(GLuint index, const GLint *v);
            void glVertexAttribI4iv_Hook(GLuint index, const GLint *v);
            void glVertexAttribI1uiv_Hook(GLuint index, const GLuint *v);
            void glVertexAttribI2uiv_Hook(GLuint index, const GLuint *v);
            void glVertexAttribI3uiv_Hook(GLuint index, const GLuint *v);
            void glVertexAttribI4uiv_Hook(GLuint index, const GLuint *v);
            void glVertexAttribI4bv_Hook(GLuint index, const GLbyte *v);
            void glVertexAttribI4sv_Hook(GLuint index, const GLshort *v);
            void glVertexAttribI4ubv_Hook(GLuint index, const GLubyte *v);
            void glVertexAttribI4usv_Hook(GLuint index, const GLushort *v);
            void glGetUniformuiv_Hook(GLuint program, GLint location, GLuint *params);
            void  glBindFragDataLocation_Hook(GLuint program, GLuint color, const GLchar *name);
            GLint glGetFragDataLocation_Hook(GLuint program, const GLchar *name);
            void  glUniform1ui_Hook(GLint location, GLuint v0);
            void glUniform2ui_Hook(GLint location, GLuint v0, GLuint v1);
            void glUniform3ui_Hook(GLint location, GLuint v0, GLuint v1, GLuint v2);
            void glUniform4ui_Hook(GLint location, GLuint v0, GLuint v1, GLuint v2, GLuint v3);
            void glUniform1uiv_Hook(GLint location, GLsizei count, const GLuint *value);
            void glUniform2uiv_Hook(GLint location, GLsizei count, const GLuint *value);
            void glUniform3uiv_Hook(GLint location, GLsizei count, const GLuint *value);
            void glUniform4uiv_Hook(GLint location, GLsizei count, const GLuint *value);
            void glTexParameterIiv_Hook(GLenum target, GLenum pname, const GLint *params);
            void glTexParameterIuiv_Hook(GLenum target, GLenum pname, const GLuint *params);
            void glGetTexParameterIiv_Hook(GLenum target, GLenum pname, GLint *params);
            void   glGetTexParameterIuiv_Hook(GLenum target, GLenum pname, GLuint *params);
            void     glClearBufferiv_Hook(GLenum buffer, GLint drawbuffer, const GLint *value);
            void      glClearBufferuiv_Hook(GLenum buffer, GLint drawbuffer, const GLuint *value);
            void        glClearBufferfv_Hook(GLenum buffer, GLint drawbuffer, const GLfloat *value);
            void          glClearBufferfi_Hook(GLenum buffer, GLint drawbuffer, GLfloat depth, GLint stencil);
            const GLubyte* glGetStringi_Hook(GLenum name, GLuint index);

            // GL_VERSION_3_1
            void glDrawArraysInstanced_Hook(GLenum mode, GLint first, GLsizei count, GLsizei primcount);
            void glDrawElementsInstanced_Hook(GLenum mode, GLsizei count, GLenum type, const GLvoid *indices, GLsizei primcount);
            void glTexBuffer_Hook(GLenum target, GLenum internalformat, GLuint buffer);
            void glPrimitiveRestartIndex_Hook(GLuint index);

            // GL_VERSION_3_2
            void glGetInteger64i_v_Hook(GLenum target, GLuint index, GLint64 *data);
            void glGetBufferParameteri64v_Hook(GLenum target, GLenum pname, GLint64 *params);
            void glFramebufferTexture_Hook(GLenum target, GLenum attachment, GLuint texture, GLint level);

            // GL_VERSION_3_3
            void glVertexAttribDivisor_Hook(GLuint index, GLuint divisor);

            // GL_VERSION_4_0
            void glMinSampleShading_Hook(GLclampf value);
            void glBlendEquationi_Hook(GLuint buf, GLenum mode);
            void glBlendEquationSeparatei_Hook(GLuint buf, GLenum modeRGB, GLenum modeAlpha);
            void glBlendFunci_Hook(GLuint buf, GLenum src, GLenum dst);
            void glBlendFuncSeparatei_Hook(GLuint buf, GLenum srcRGB, GLenum dstRGB, GLenum srcAlpha, GLenum dstAlpha);

            // GL_AMD_debug_output
            void   glDebugMessageEnableAMD_Hook(GLenum category, GLenum severity, GLsizei count, const GLuint *ids, GLboolean enabled);
            void   glDebugMessageInsertAMD_Hook(GLenum category, GLenum severity, GLuint id, GLsizei length, const GLchar *buf);
            void   glDebugMessageCallbackAMD_Hook(GLDEBUGPROCAMD callback, GLvoid *userParam);
            GLuint glGetDebugMessageLogAMD_Hook(GLuint count, GLsizei bufsize, GLenum *categories, GLuint *severities, GLuint *ids, GLsizei *lengths, GLchar *message);

        #if defined(GLE_APPLE_ENABLED)
            // GL_APPLE_element_array
            void glElementPointerAPPLE_Hook(GLenum type, const GLvoid *pointer);
            void glDrawElementArrayAPPLE_Hook(GLenum mode, GLint first, GLsizei count);
            void glDrawRangeElementArrayAPPLE_Hook(GLenum mode, GLuint start, GLuint end, GLint first, GLsizei count);
            void glMultiDrawElementArrayAPPLE_Hook(GLenum mode, const GLint *first, const GLsizei *count, GLsizei primcount);
            void glMultiDrawRangeElementArrayAPPLE_Hook(GLenum mode, GLuint start, GLuint end, const GLint *first, const GLsizei *count, GLsizei primcount);

            // GL_APPLE_fence
            void glGenFencesAPPLE_Hook(GLsizei n, GLuint *fences);
            void glDeleteFencesAPPLE_Hook(GLsizei n, const GLuint *fences);
            void glSetFenceAPPLE_Hook(GLuint fence);
            GLboolean glIsFenceAPPLE_Hook(GLuint fence);
            GLboolean glTestFenceAPPLE_Hook(GLuint fence);
            void glFinishFenceAPPLE_Hook(GLuint fence);
            GLboolean glTestObjectAPPLE_Hook(GLenum object, GLuint name);
            void glFinishObjectAPPLE_Hook(GLenum object, GLint name);

            // GL_APPLE_flush_buffer_range
            void glBufferParameteriAPPLE_Hook(GLenum target, GLenum pname, GLint param);
            void glFlushMappedBufferRangeAPPLE_Hook(GLenum target, GLintptr offset, GLsizeiptr size);

            // GL_APPLE_object_purgeable
            GLenum glObjectPurgeableAPPLE_Hook(GLenum objectType, GLuint name, GLenum option);
            GLenum glObjectUnpurgeableAPPLE_Hook(GLenum objectType, GLuint name, GLenum option);
            void glGetObjectParameterivAPPLE_Hook(GLenum objectType, GLuint name, GLenum pname, GLint *params);

            // GL_APPLE_texture_range
            void glTextureRangeAPPLE_Hook(GLenum target, GLsizei length, const GLvoid *pointer);
            void glGetTexParameterPointervAPPLE_Hook(GLenum target, GLenum pname, GLvoid **params);

            // GL_APPLE_vertex_array_object
            void glBindVertexArrayAPPLE_Hook(GLuint array);
            void glDeleteVertexArraysAPPLE_Hook(GLsizei n, const GLuint *arrays);
            void glGenVertexArraysAPPLE_Hook(GLsizei n, GLuint *arrays);
            GLboolean glIsVertexArrayAPPLE_Hook(GLuint array);

            // GL_APPLE_vertex_array_range
            void glVertexArrayRangeAPPLE_Hook(GLsizei length, GLvoid *pointer);
            void glFlushVertexArrayRangeAPPLE_Hook(GLsizei length, GLvoid *pointer);
            void glVertexArrayParameteriAPPLE_Hook(GLenum pname, GLint param);

            // GL_APPLE_vertex_program_evaluators
            void glEnableVertexAttribAPPLE_Hook(GLuint index, GLenum pname);
            void glDisableVertexAttribAPPLE_Hook(GLuint index, GLenum pname);
            GLboolean glIsVertexAttribEnabledAPPLE_Hook(GLuint index, GLenum pname);
            void glMapVertexAttrib1dAPPLE_Hook(GLuint index, GLuint size, GLdouble u1, GLdouble u2, GLint stride, GLint order, const GLdouble *points);
            void glMapVertexAttrib1fAPPLE_Hook(GLuint index, GLuint size, GLfloat u1, GLfloat u2, GLint stride, GLint order, const GLfloat *points);
            void glMapVertexAttrib2dAPPLE_Hook(GLuint index, GLuint size, GLdouble u1, GLdouble u2, GLint ustride, GLint uorder, GLdouble v1, GLdouble v2, GLint vstride, GLint vorder, const GLdouble *points);
            void glMapVertexAttrib2fAPPLE_Hook(GLuint index, GLuint size, GLfloat u1, GLfloat u2, GLint ustride, GLint uorder, GLfloat v1, GLfloat v2, GLint vstride, GLint vorder, const GLfloat *points);
        #endif // GLE_APPLE_ENABLED

            // GL_ARB_debug_output
            void   glDebugMessageControlARB_Hook(GLenum source, GLenum type, GLenum severity, GLsizei count, const GLuint *ids, GLboolean enabled);
            void   glDebugMessageInsertARB_Hook(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar *buf);
            void   glDebugMessageCallbackARB_Hook(GLDEBUGPROCARB callback, const GLvoid *userParam);
            GLuint glGetDebugMessageLogARB_Hook(GLuint count, GLsizei bufsize, GLenum *sources, GLenum *types, GLuint *ids, GLenum *severities, GLsizei *lengths, GLchar *messageLog);

            // GL_ARB_ES2_compatibility
            void glReleaseShaderCompiler_Hook();
            void glShaderBinary_Hook(GLsizei count, const GLuint *shaders, GLenum binaryformat, const GLvoid *binary, GLsizei length);
            void glGetShaderPrecisionFormat_Hook(GLenum shadertype, GLenum precisiontype, GLint *range, GLint *precision);
            void glDepthRangef_Hook(GLclampf n, GLclampf f);
            void glClearDepthf_Hook(GLclampf d);

            // GL_ARB_framebuffer_object
            GLboolean glIsRenderbuffer_Hook(GLuint renderbuffer);
            void glBindRenderbuffer_Hook(GLenum target, GLuint renderbuffer);
            void glDeleteRenderbuffers_Hook(GLsizei n, const GLuint *renderbuffers);
            void glGenRenderbuffers_Hook(GLsizei n, GLuint *renderbuffers);
            void glRenderbufferStorage_Hook(GLenum target, GLenum internalformat, GLsizei width, GLsizei height);
            void glGetRenderbufferParameteriv_Hook(GLenum target, GLenum pname, GLint *params);
            GLboolean glIsFramebuffer_Hook(GLuint framebuffer);
            void glBindFramebuffer_Hook(GLenum target, GLuint framebuffer);
            void glDeleteFramebuffers_Hook(GLsizei n, const GLuint *framebuffers);
            void glGenFramebuffers_Hook(GLsizei n, GLuint *framebuffers);
            GLenum glCheckFramebufferStatus_Hook(GLenum target);
            void glFramebufferTexture1D_Hook(GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level);
            void glFramebufferTexture2D_Hook(GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level);
            void glFramebufferTexture3D_Hook(GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level, GLint zoffset);
            void glFramebufferRenderbuffer_Hook(GLenum target, GLenum attachment, GLenum renderbuffertarget, GLuint renderbuffer);
            void glGetFramebufferAttachmentParameteriv_Hook(GLenum target, GLenum attachment, GLenum pname, GLint *params);
            void glGenerateMipmap_Hook(GLenum target);
            void glBlitFramebuffer_Hook(GLint srcX0, GLint srcY0, GLint srcX1, GLint srcY1, GLint dstX0, GLint dstY0, GLint dstX1, GLint dstY1, GLbitfield mask, GLenum filter);
            void glRenderbufferStorageMultisample_Hook(GLenum target, GLsizei samples, GLenum internalformat, GLsizei width, GLsizei height);
            void glFramebufferTextureLayer_Hook(GLenum target, GLenum attachment, GLuint texture, GLint level, GLint layer);

            // GL_ARB_texture_multisample
            void glTexImage2DMultisample_Hook(GLenum target, GLsizei samples, GLint internalformat, GLsizei width, GLsizei height, GLboolean fixedsamplelocations);
            void glTexImage3DMultisample_Hook(GLenum target, GLsizei samples, GLint internalformat, GLsizei width, GLsizei height, GLsizei depth, GLboolean fixedsamplelocations);
            void glGetMultisamplefv_Hook(GLenum pname, GLuint index, GLfloat *val);
            void glSampleMaski_Hook(GLuint index, GLbitfield mask);

            // GL_ARB_timer_query
            void glQueryCounter_Hook(GLuint id, GLenum target);
            void glGetQueryObjecti64v_Hook(GLuint id, GLenum pname, GLint64 *params);
            void glGetQueryObjectui64v_Hook(GLuint id, GLenum pname, GLuint64 *params);

            // GL_ARB_vertex_array_object
            void      glBindVertexArray_Hook(GLuint array);
            void      glDeleteVertexArrays_Hook(GLsizei n, const GLuint *arrays);
            void      glGenVertexArrays_Hook(GLsizei n, GLuint *arrays);
            GLboolean glIsVertexArray_Hook(GLuint array);

            // GL_EXT_draw_buffers2
            void      glColorMaskIndexedEXT_Hook(GLuint index, GLboolean r, GLboolean g, GLboolean b, GLboolean a);
            void      glGetBooleanIndexedvEXT_Hook(GLenum target, GLuint index, GLboolean *data);
            void      glGetIntegerIndexedvEXT_Hook(GLenum target, GLuint index, GLint *data);
            void      glEnableIndexedEXT_Hook(GLenum target, GLuint index);
            void      glDisableIndexedEXT_Hook(GLenum target, GLuint index);
            GLboolean glIsEnabledIndexedEXT_Hook(GLenum target, GLuint index);

            // GL_KHR_debug
            void   glDebugMessageControl_Hook(GLenum source, GLenum type, GLenum severity, GLsizei count, const GLuint* ids, GLboolean enabled);
            void   glDebugMessageInsert_Hook(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const char* buf);
            void   glDebugMessageCallback_Hook(GLDEBUGPROC callback, const void* userParam);
            GLuint glGetDebugMessageLog_Hook(GLuint count, GLsizei bufSize, GLenum* sources, GLenum* types, GLuint* ids, GLenum* severities, GLsizei* lengths,  char* messageLog);
            void   glPushDebugGroup_Hook(GLenum source, GLuint id, GLsizei length, const char * message);
            void   glPopDebugGroup_Hook(void);
            void   glObjectLabel_Hook(GLenum identifier, GLuint name, GLsizei length, const char *label);
            void   glGetObjectLabel_Hook(GLenum identifier, GLuint name, GLsizei bufSize, GLsizei *length, char *label);
            void   glObjectPtrLabel_Hook(void* ptr, GLsizei length, const char *label);
            void   glGetObjectPtrLabel_Hook(void* ptr, GLsizei bufSize, GLsizei *length, char *label);

            // GL_WIN_swap_hint
            void glAddSwapHintRectWIN_Hook(GLint x, GLint y, GLsizei width, GLsizei height);

          #if defined(GLE_WINDOWS_ENABLED)
            void PostWGLHook();

            // WGL
            BOOL  wglCopyContext_Hook(HGLRC, HGLRC, UINT);
            HGLRC wglCreateContext_Hook(HDC);
            HGLRC wglCreateLayerContext_Hook(HDC, int);
            BOOL  wglDeleteContext_Hook(HGLRC);
            HGLRC wglGetCurrentContext_Hook(VOID);
            HDC   wglGetCurrentDC_Hook(VOID);
            PROC  wglGetProcAddress_Hook(LPCSTR);
            BOOL  wglMakeCurrent_Hook(HDC, HGLRC);
            BOOL  wglShareLists_Hook(HGLRC, HGLRC);
            BOOL  wglUseFontBitmapsA_Hook(HDC, DWORD, DWORD, DWORD);
            BOOL  wglUseFontBitmapsW_Hook(HDC, DWORD, DWORD, DWORD);
            BOOL  wglUseFontOutlinesA_Hook(HDC, DWORD, DWORD, DWORD, FLOAT, FLOAT, int, LPGLYPHMETRICSFLOAT);
            BOOL  wglUseFontOutlinesW_Hook(HDC, DWORD, DWORD, DWORD, FLOAT, FLOAT, int, LPGLYPHMETRICSFLOAT);
            BOOL  wglDescribeLayerPlane_Hook(HDC, int, int, UINT, LPLAYERPLANEDESCRIPTOR);
            int   wglSetLayerPaletteEntries_Hook(HDC, int, int, int, CONST COLORREF *);
            int   wglGetLayerPaletteEntries_Hook(HDC, int, int, int, COLORREF *);
            BOOL  wglRealizeLayerPalette_Hook(HDC, int, BOOL);
            BOOL  wglSwapLayerBuffers_Hook(HDC, UINT);
            DWORD wglSwapMultipleBuffers_Hook(UINT, CONST WGLSWAP *);

            // WGL_ARB_buffer_region
            HANDLE wglCreateBufferRegionARB_Hook (HDC hDC, int iLayerPlane, UINT uType);
            VOID wglDeleteBufferRegionARB_Hook (HANDLE hRegion);
            BOOL wglSaveBufferRegionARB_Hook (HANDLE hRegion, int x, int y, int width, int height);
            BOOL wglRestoreBufferRegionARB_Hook (HANDLE hRegion, int x, int y, int width, int height, int xSrc, int ySrc);

            // WGL_ARB_extensions_string
            const char * wglGetExtensionsStringARB_Hook (HDC hdc);

            // WGL_ARB_pixel_format
            BOOL wglGetPixelFormatAttribivARB_Hook (HDC hdc, int iPixelFormat, int iLayerPlane, UINT nAttributes, const int *piAttributes, int *piValues);
            BOOL wglGetPixelFormatAttribfvARB_Hook (HDC hdc, int iPixelFormat, int iLayerPlane, UINT nAttributes, const int *piAttributes, FLOAT *pfValues);
            BOOL wglChoosePixelFormatARB_Hook (HDC hdc, const int *piAttribIList, const FLOAT *pfAttribFList, UINT nMaxFormats, int *piFormats, UINT *nNumFormats);

            // WGL_ARB_make_current_read
            BOOL wglMakeContextCurrentARB_Hook (HDC hDrawDC, HDC hReadDC, HGLRC hglrc);
            HDC wglGetCurrentReadDCARB_Hook (void);

            // WGL_ARB_pbuffer
            HPBUFFERARB wglCreatePbufferARB_Hook (HDC hDC, int iPixelFormat, int iWidth, int iHeight, const int *piAttribList);
            HDC wglGetPbufferDCARB_Hook (HPBUFFERARB hPbuffer);
            int wglReleasePbufferDCARB_Hook (HPBUFFERARB hPbuffer, HDC hDC);
            BOOL wglDestroyPbufferARB_Hook (HPBUFFERARB hPbuffer);
            BOOL wglQueryPbufferARB_Hook (HPBUFFERARB hPbuffer, int iAttribute, int *piValue);

            // WGL_ARB_render_texture
            BOOL wglBindTexImageARB_Hook (HPBUFFERARB hPbuffer, int iBuffer);
            BOOL wglReleaseTexImageARB_Hook (HPBUFFERARB hPbuffer, int iBuffer);
            BOOL wglSetPbufferAttribARB_Hook (HPBUFFERARB hPbuffer, const int *piAttribList);

            // WGL_NV_present_video
            int wglEnumerateVideoDevicesNV_Hook (HDC hDC, HVIDEOOUTPUTDEVICENV *phDeviceList);
            BOOL wglBindVideoDeviceNV_Hook (HDC hDC, unsigned int uVideoSlot, HVIDEOOUTPUTDEVICENV hVideoDevice, const int *piAttribList);
            BOOL wglQueryCurrentContextNV_Hook (int iAttribute, int *piValue);

            // WGL_ARB_create_context
            HGLRC wglCreateContextAttribsARB_Hook (HDC hDC, HGLRC hShareContext, const int *attribList);

            // WGL_EXT_extensions_string
            const char * wglGetExtensionsStringEXT_Hook ();

            // WGL_EXT_swap_control
            BOOL wglSwapIntervalEXT_Hook(int interval);
            int  wglGetSwapIntervalEXT_Hook();

            // WGL_OML_sync_control
            BOOL  wglGetSyncValuesOML_Hook (HDC hdc, INT64 *ust, INT64 *msc, INT64 *sbc);
            BOOL  wglGetMscRateOML_Hook (HDC hdc, INT32 *numerator, INT32 *denominator);
            INT64 wglSwapBuffersMscOML_Hook (HDC hdc, INT64 target_msc, INT64 divisor, INT64 remainder);
            INT64 wglSwapLayerBuffersMscOML_Hook (HDC hdc, int fuPlanes, INT64 target_msc, INT64 divisor, INT64 remainder);
            BOOL  wglWaitForMscOML_Hook (HDC hdc, INT64 target_msc, INT64 divisor, INT64 remainder, INT64 *ust, INT64 *msc, INT64 *sbc);
            BOOL  wglWaitForSbcOML_Hook (HDC hdc, INT64 target_sbc, INT64 *ust, INT64 *msc, INT64 *sbc);

             // WGL_NV_video_output
            BOOL wglGetVideoDeviceNV_Hook (HDC hDC, int numDevices, HPVIDEODEV *hVideoDevice);
            BOOL wglReleaseVideoDeviceNV_Hook (HPVIDEODEV hVideoDevice);
            BOOL wglBindVideoImageNV_Hook (HPVIDEODEV hVideoDevice, HPBUFFERARB hPbuffer, int iVideoBuffer);
            BOOL wglReleaseVideoImageNV_Hook (HPBUFFERARB hPbuffer, int iVideoBuffer);
            BOOL wglSendPbufferToVideoNV_Hook (HPBUFFERARB hPbuffer, int iBufferType, unsigned long *pulCounterPbuffer, BOOL bBlock);
            BOOL wglGetVideoInfoNV_Hook (HPVIDEODEV hpVideoDevice, unsigned long *pulCounterOutputPbuffer, unsigned long *pulCounterOutputVideo);

             // WGL_NV_swap_group
            BOOL wglJoinSwapGroupNV_Hook (HDC hDC, GLuint group);
            BOOL wglBindSwapBarrierNV_Hook (GLuint group, GLuint barrier);
            BOOL wglQuerySwapGroupNV_Hook (HDC hDC, GLuint *group, GLuint *barrier);
            BOOL wglQueryMaxSwapGroupsNV_Hook (HDC hDC, GLuint *maxGroups, GLuint *maxBarriers);
            BOOL wglQueryFrameCountNV_Hook (HDC hDC, GLuint *count);
            BOOL wglResetFrameCountNV_Hook (HDC hDC);

             // WGL_NV_video_capture
            BOOL wglBindVideoCaptureDeviceNV_Hook (UINT uVideoSlot, HVIDEOINPUTDEVICENV hDevice);
            UINT wglEnumerateVideoCaptureDevicesNV_Hook (HDC hDc, HVIDEOINPUTDEVICENV *phDeviceList);
            BOOL wglLockVideoCaptureDeviceNV_Hook (HDC hDc, HVIDEOINPUTDEVICENV hDevice);
            BOOL wglQueryVideoCaptureDeviceNV_Hook (HDC hDc, HVIDEOINPUTDEVICENV hDevice, int iAttribute, int *piValue);
            BOOL wglReleaseVideoCaptureDeviceNV_Hook (HDC hDc, HVIDEOINPUTDEVICENV hDevice);

            // WGL_NV_copy_image
            BOOL wglCopyImageSubDataNV_Hook (HGLRC hSrcRC, GLuint srcName, GLenum srcTarget, GLint srcLevel, GLint srcX, GLint srcY, GLint srcZ, HGLRC hDstRC, GLuint dstName, GLenum dstTarget, GLint dstLevel, GLint dstX, GLint dstY, GLint dstZ, GLsizei width, GLsizei height, GLsizei depth);

            // WGL_NV_DX_interop
            BOOL   wglDXSetResourceShareHandleNV_Hook(void *dxObject, HANDLE shareHandle);
            HANDLE wglDXOpenDeviceNV_Hook(void *dxDevice);
            BOOL   wglDXCloseDeviceNV_Hook(HANDLE hDevice);
            HANDLE wglDXRegisterObjectNV_Hook(HANDLE hDevice, void *dxObject, GLuint name, GLenum type, GLenum access);
            BOOL   wglDXUnregisterObjectNV_Hook(HANDLE hDevice, HANDLE hObject);
            BOOL   wglDXObjectAccessNV_Hook(HANDLE hObject, GLenum access);
            BOOL   wglDXLockObjectsNV_Hook(HANDLE hDevice, GLint count, HANDLE *hObjects);
            BOOL   wglDXUnlockObjectsNV_Hook(HANDLE hDevice, GLint count, HANDLE *hObjects);
          #endif // GLE_WINDOWS_ENABLED

          #if defined(GLE_UNIX_ENABLED)
            void PostGLXHook();

            // GLX_VERSION_1_0
            // GLX_VERSION_1_1
            // We don't currently do hooking of these.

            // GLX_VERSION_1_2
            ::Display* glXGetCurrentDisplay_Hook(void);

            // GLX_VERSION_1_3
            GLXFBConfig* glXChooseFBConfig_Hook(::Display *dpy, int screen, const int *attrib_list, int *nelements);
            GLXContext   glXCreateNewContext_Hook(::Display *dpy, GLXFBConfig config, int render_type, GLXContext share_list, Bool direct);
            GLXPbuffer   glXCreatePbuffer_Hook(::Display *dpy, GLXFBConfig config, const int *attrib_list);
            GLXPixmap    glXCreatePixmap_Hook(::Display *dpy, GLXFBConfig config, Pixmap pixmap, const int *attrib_list);
            GLXWindow    glXCreateWindow_Hook(::Display *dpy, GLXFBConfig config, Window win, const int *attrib_list);
            void         glXDestroyPbuffer_Hook(::Display *dpy, GLXPbuffer pbuf);
            void         glXDestroyPixmap_Hook(::Display *dpy, GLXPixmap pixmap);
            void         glXDestroyWindow_Hook(::Display *dpy, GLXWindow win);
            GLXDrawable  glXGetCurrentReadDrawable_Hook(void);
            int          glXGetFBConfigAttrib_Hook(::Display *dpy, GLXFBConfig config, int attribute, int *value);
            GLXFBConfig* glXGetFBConfigs_Hook(::Display *dpy, int screen, int *nelements);
            void         glXGetSelectedEvent_Hook(::Display *dpy, GLXDrawable draw, unsigned long *event_mask);
            XVisualInfo* glXGetVisualFromFBConfig_Hook(::Display *dpy, GLXFBConfig config);
            Bool         glXMakeContextCurrent_Hook(::Display *display, GLXDrawable draw, GLXDrawable read, GLXContext ctx);
            int          glXQueryContext_Hook(::Display *dpy, GLXContext ctx, int attribute, int *value);
            void         glXQueryDrawable_Hook(::Display *dpy, GLXDrawable draw, int attribute, unsigned int *value);
            void         glXSelectEvent_Hook(::Display *dpy, GLXDrawable draw, unsigned long event_mask);

            // GLX_VERSION_1_4
            // We don't do hooking of this.

            // GLX_EXT_swap_control
            void glXSwapIntervalEXT_Hook(::Display* dpy, GLXDrawable drawable, int interval);

            // GLX_OML_sync_control
			Bool    glXGetMscRateOML_Hook(::Display* dpy, GLXDrawable drawable, int32_t* numerator, int32_t* denominator);
			Bool    glXGetSyncValuesOML_Hook(::Display* dpy, GLXDrawable drawable, int64_t* ust, int64_t* msc, int64_t* sbc);
			int64_t glXSwapBuffersMscOML_Hook(::Display* dpy, GLXDrawable drawable, int64_t target_msc, int64_t divisor, int64_t remainder);
			Bool    glXWaitForMscOML_Hook(::Display* dpy, GLXDrawable drawable, int64_t target_msc, int64_t divisor, int64_t remainder, int64_t* ust, int64_t* msc, int64_t* sbc);
			Bool    glXWaitForSbcOML_Hook(::Display* dpy, GLXDrawable drawable, int64_t target_sbc, int64_t* ust, int64_t* msc, int64_t* sbc);

          #endif // GLE_UNIX_ENABLED

        #endif // #if defined(GLE_HOOKING_ENABLED)

        // GL_VERSION_1_1
        // These are not represented by function pointers.
        
        // GL_VERSION_1_2
        PFNGLCOPYTEXSUBIMAGE3DPROC glCopyTexSubImage3D_Impl;
        PFNGLDRAWRANGEELEMENTSPROC glDrawRangeElements_Impl;
        PFNGLTEXIMAGE3DPROC glTexImage3D_Impl;
        PFNGLTEXSUBIMAGE3DPROC glTexSubImage3D_Impl;

        // GL_VERSION_1_2 deprecated functions
        /* Not currently supported
        PFNGLCOLORTABLEPROC glColorTable_Impl;
        PFNGLCOLORTABLEPARAMETERFVPROC glColorTableParameterfv_Impl;
        PFNGLCOLORTABLEPARAMETERIVPROC glColorTableParameteriv_Impl;
        PFNGLCOPYCOLORTABLEPROC glCopyColorTable_Impl;
        PFNGLGETCOLORTABLEPROC glGetColorTable_Impl;
        PFNGLGETCOLORTABLEPARAMETERFVPROC glGetColorTableParameterfv_Impl;
        PFNGLGETCOLORTABLEPARAMETERIVPROC glGetColorTableParameteriv_Impl;
        PFNGLCOLORSUBTABLEPROC glColorSubTable_Impl;
        PFNGLCOPYCOLORSUBTABLEPROC glCopyColorSubTable_Impl;
        PFNGLCONVOLUTIONFILTER1DPROC glConvolutionFilter1D_Impl;
        PFNGLCONVOLUTIONFILTER2DPROC glConvolutionFilter2D_Impl;
        PFNGLCONVOLUTIONPARAMETERFPROC glConvolutionParameterf_Impl;
        PFNGLCONVOLUTIONPARAMETERFVPROC glConvolutionParameterfv_Impl;
        PFNGLCONVOLUTIONPARAMETERIPROC glConvolutionParameteri_Impl;
        PFNGLCONVOLUTIONPARAMETERIVPROC glConvolutionParameteriv_Impl;
        PFNGLCOPYCONVOLUTIONFILTER1DPROC glCopyConvolutionFilter1D_Impl;
        PFNGLCOPYCONVOLUTIONFILTER2DPROC glCopyConvolutionFilter2D_Impl;
        PFNGLGETCONVOLUTIONFILTERPROC glGetConvolutionFilter_Impl;
        PFNGLGETCONVOLUTIONPARAMETERFVPROC glGetConvolutionParameterfv_Impl;
        PFNGLGETCONVOLUTIONPARAMETERIVPROC glGetConvolutionParameteriv_Impl;
        PFNGLGETSEPARABLEFILTERPROC glGetSeparableFilter_Impl;
        PFNGLSEPARABLEFILTER2DPROC glSeparableFilter2D_Impl;
        PFNGLGETHISTOGRAMPROC glGetHistogram_Impl;
        PFNGLGETHISTOGRAMPARAMETERFVPROC glGetHistogramParameterfv_Impl;
        PFNGLGETHISTOGRAMPARAMETERIVPROC glGetHistogramParameteriv_Impl;
        PFNGLGETMINMAXPROC glGetMinmax_Impl;
        PFNGLGETMINMAXPARAMETERFVPROC glGetMinmaxParameterfv_Impl;
        PFNGLGETMINMAXPARAMETERIVPROC glGetMinmaxParameteriv_Impl;
        PFNGLHISTOGRAMPROC glHistogram_Impl;
        PFNGLMINMAXPROC glMinmax_Impl;
        PFNGLRESETHISTOGRAMPROC glResetHistogram_Impl;
        PFNGLRESETMINMAXPROC glResetMinmax_Impl;
        */

        // GL_VERSION_1_3
        PFNGLACTIVETEXTUREPROC glActiveTexture_Impl;
        PFNGLCLIENTACTIVETEXTUREPROC glClientActiveTexture_Impl;
        PFNGLCOMPRESSEDTEXIMAGE1DPROC glCompressedTexImage1D_Impl;
        PFNGLCOMPRESSEDTEXIMAGE2DPROC glCompressedTexImage2D_Impl;
        PFNGLCOMPRESSEDTEXIMAGE3DPROC glCompressedTexImage3D_Impl;
        PFNGLCOMPRESSEDTEXSUBIMAGE1DPROC glCompressedTexSubImage1D_Impl;
        PFNGLCOMPRESSEDTEXSUBIMAGE2DPROC glCompressedTexSubImage2D_Impl;
        PFNGLCOMPRESSEDTEXSUBIMAGE3DPROC glCompressedTexSubImage3D_Impl;
        PFNGLGETCOMPRESSEDTEXIMAGEPROC glGetCompressedTexImage_Impl;
        PFNGLLOADTRANSPOSEMATRIXDPROC glLoadTransposeMatrixd_Impl;
        PFNGLLOADTRANSPOSEMATRIXFPROC glLoadTransposeMatrixf_Impl;
        PFNGLMULTTRANSPOSEMATRIXDPROC glMultTransposeMatrixd_Impl;
        PFNGLMULTTRANSPOSEMATRIXFPROC glMultTransposeMatrixf_Impl;
        PFNGLMULTITEXCOORD1DPROC glMultiTexCoord1d_Impl;
        PFNGLMULTITEXCOORD1DVPROC glMultiTexCoord1dv_Impl;
        PFNGLMULTITEXCOORD1FPROC glMultiTexCoord1f_Impl;
        PFNGLMULTITEXCOORD1FVPROC glMultiTexCoord1fv_Impl;
        PFNGLMULTITEXCOORD1IPROC glMultiTexCoord1i_Impl;
        PFNGLMULTITEXCOORD1IVPROC glMultiTexCoord1iv_Impl;
        PFNGLMULTITEXCOORD1SPROC glMultiTexCoord1s_Impl;
        PFNGLMULTITEXCOORD1SVPROC glMultiTexCoord1sv_Impl;
        PFNGLMULTITEXCOORD2DPROC glMultiTexCoord2d_Impl;
        PFNGLMULTITEXCOORD2DVPROC glMultiTexCoord2dv_Impl;
        PFNGLMULTITEXCOORD2FPROC glMultiTexCoord2f_Impl;
        PFNGLMULTITEXCOORD2FVPROC glMultiTexCoord2fv_Impl;
        PFNGLMULTITEXCOORD2IPROC glMultiTexCoord2i_Impl;
        PFNGLMULTITEXCOORD2IVPROC glMultiTexCoord2iv_Impl;
        PFNGLMULTITEXCOORD2SPROC glMultiTexCoord2s_Impl;
        PFNGLMULTITEXCOORD2SVPROC glMultiTexCoord2sv_Impl;
        PFNGLMULTITEXCOORD3DPROC glMultiTexCoord3d_Impl;
        PFNGLMULTITEXCOORD3DVPROC glMultiTexCoord3dv_Impl;
        PFNGLMULTITEXCOORD3FPROC glMultiTexCoord3f_Impl;
        PFNGLMULTITEXCOORD3FVPROC glMultiTexCoord3fv_Impl;
        PFNGLMULTITEXCOORD3IPROC glMultiTexCoord3i_Impl;
        PFNGLMULTITEXCOORD3IVPROC glMultiTexCoord3iv_Impl;
        PFNGLMULTITEXCOORD3SPROC glMultiTexCoord3s_Impl;
        PFNGLMULTITEXCOORD3SVPROC glMultiTexCoord3sv_Impl;
        PFNGLMULTITEXCOORD4DPROC glMultiTexCoord4d_Impl;
        PFNGLMULTITEXCOORD4DVPROC glMultiTexCoord4dv_Impl;
        PFNGLMULTITEXCOORD4FPROC glMultiTexCoord4f_Impl;
        PFNGLMULTITEXCOORD4FVPROC glMultiTexCoord4fv_Impl;
        PFNGLMULTITEXCOORD4IPROC glMultiTexCoord4i_Impl;
        PFNGLMULTITEXCOORD4IVPROC glMultiTexCoord4iv_Impl;
        PFNGLMULTITEXCOORD4SPROC glMultiTexCoord4s_Impl;
        PFNGLMULTITEXCOORD4SVPROC glMultiTexCoord4sv_Impl;
        PFNGLSAMPLECOVERAGEPROC glSampleCoverage_Impl;

        // GL_VERSION_1_4
        PFNGLBLENDCOLORPROC glBlendColor_Impl;
        PFNGLBLENDEQUATIONPROC glBlendEquation_Impl;
        PFNGLBLENDFUNCSEPARATEPROC glBlendFuncSeparate_Impl;
        PFNGLFOGCOORDPOINTERPROC glFogCoordPointer_Impl;
        PFNGLFOGCOORDDPROC glFogCoordd_Impl;
        PFNGLFOGCOORDDVPROC glFogCoorddv_Impl;
        PFNGLFOGCOORDFPROC glFogCoordf_Impl;
        PFNGLFOGCOORDFVPROC glFogCoordfv_Impl;
        PFNGLMULTIDRAWARRAYSPROC glMultiDrawArrays_Impl;
        PFNGLMULTIDRAWELEMENTSPROC glMultiDrawElements_Impl;
        PFNGLPOINTPARAMETERFPROC glPointParameterf_Impl;
        PFNGLPOINTPARAMETERFVPROC glPointParameterfv_Impl;
        PFNGLPOINTPARAMETERIPROC glPointParameteri_Impl;
        PFNGLPOINTPARAMETERIVPROC glPointParameteriv_Impl;
        PFNGLSECONDARYCOLOR3BPROC glSecondaryColor3b_Impl;
        PFNGLSECONDARYCOLOR3BVPROC glSecondaryColor3bv_Impl;
        PFNGLSECONDARYCOLOR3DPROC glSecondaryColor3d_Impl;
        PFNGLSECONDARYCOLOR3DVPROC glSecondaryColor3dv_Impl;
        PFNGLSECONDARYCOLOR3FPROC glSecondaryColor3f_Impl;
        PFNGLSECONDARYCOLOR3FVPROC glSecondaryColor3fv_Impl;
        PFNGLSECONDARYCOLOR3IPROC glSecondaryColor3i_Impl;
        PFNGLSECONDARYCOLOR3IVPROC glSecondaryColor3iv_Impl;
        PFNGLSECONDARYCOLOR3SPROC glSecondaryColor3s_Impl;
        PFNGLSECONDARYCOLOR3SVPROC glSecondaryColor3sv_Impl;
        PFNGLSECONDARYCOLOR3UBPROC glSecondaryColor3ub_Impl;
        PFNGLSECONDARYCOLOR3UBVPROC glSecondaryColor3ubv_Impl;
        PFNGLSECONDARYCOLOR3UIPROC glSecondaryColor3ui_Impl;
        PFNGLSECONDARYCOLOR3UIVPROC glSecondaryColor3uiv_Impl;
        PFNGLSECONDARYCOLOR3USPROC glSecondaryColor3us_Impl;
        PFNGLSECONDARYCOLOR3USVPROC glSecondaryColor3usv_Impl;
        PFNGLSECONDARYCOLORPOINTERPROC glSecondaryColorPointer_Impl;
        PFNGLWINDOWPOS2DPROC glWindowPos2d_Impl;
        PFNGLWINDOWPOS2DVPROC glWindowPos2dv_Impl;
        PFNGLWINDOWPOS2FPROC glWindowPos2f_Impl;
        PFNGLWINDOWPOS2FVPROC glWindowPos2fv_Impl;
        PFNGLWINDOWPOS2IPROC glWindowPos2i_Impl;
        PFNGLWINDOWPOS2IVPROC glWindowPos2iv_Impl;
        PFNGLWINDOWPOS2SPROC glWindowPos2s_Impl;
        PFNGLWINDOWPOS2SVPROC glWindowPos2sv_Impl;
        PFNGLWINDOWPOS3DPROC glWindowPos3d_Impl;
        PFNGLWINDOWPOS3DVPROC glWindowPos3dv_Impl;
        PFNGLWINDOWPOS3FPROC glWindowPos3f_Impl;
        PFNGLWINDOWPOS3FVPROC glWindowPos3fv_Impl;
        PFNGLWINDOWPOS3IPROC glWindowPos3i_Impl;
        PFNGLWINDOWPOS3IVPROC glWindowPos3iv_Impl;
        PFNGLWINDOWPOS3SPROC glWindowPos3s_Impl;
        PFNGLWINDOWPOS3SVPROC glWindowPos3sv_Impl;

        // GL_VERSION_1_5
        PFNGLBEGINQUERYPROC glBeginQuery_Impl;
        PFNGLBINDBUFFERPROC glBindBuffer_Impl;
        PFNGLBUFFERDATAPROC glBufferData_Impl;
        PFNGLBUFFERSUBDATAPROC glBufferSubData_Impl;
        PFNGLDELETEBUFFERSPROC glDeleteBuffers_Impl;
        PFNGLDELETEQUERIESPROC glDeleteQueries_Impl;
        PFNGLENDQUERYPROC glEndQuery_Impl;
        PFNGLGENBUFFERSPROC glGenBuffers_Impl;
        PFNGLGENQUERIESPROC glGenQueries_Impl;
        PFNGLGETBUFFERPARAMETERIVPROC glGetBufferParameteriv_Impl;
        PFNGLGETBUFFERPOINTERVPROC glGetBufferPointerv_Impl;
        PFNGLGETBUFFERSUBDATAPROC glGetBufferSubData_Impl;
        PFNGLGETQUERYOBJECTIVPROC glGetQueryObjectiv_Impl;
        PFNGLGETQUERYOBJECTUIVPROC glGetQueryObjectuiv_Impl;
        PFNGLGETQUERYIVPROC glGetQueryiv_Impl;
        PFNGLISBUFFERPROC glIsBuffer_Impl;
        PFNGLISQUERYPROC glIsQuery_Impl;
        PFNGLMAPBUFFERPROC glMapBuffer_Impl;
        PFNGLUNMAPBUFFERPROC glUnmapBuffer_Impl;

        // GL_VERSION_2_0
        PFNGLATTACHSHADERPROC glAttachShader_Impl;
        PFNGLBINDATTRIBLOCATIONPROC glBindAttribLocation_Impl;
        PFNGLBLENDEQUATIONSEPARATEPROC glBlendEquationSeparate_Impl;
        PFNGLCOMPILESHADERPROC glCompileShader_Impl;
        PFNGLCREATEPROGRAMPROC glCreateProgram_Impl;
        PFNGLCREATESHADERPROC glCreateShader_Impl;
        PFNGLDELETEPROGRAMPROC glDeleteProgram_Impl;
        PFNGLDELETESHADERPROC glDeleteShader_Impl;
        PFNGLDETACHSHADERPROC glDetachShader_Impl;
        PFNGLDISABLEVERTEXATTRIBARRAYPROC glDisableVertexAttribArray_Impl;
        PFNGLDRAWBUFFERSPROC glDrawBuffers_Impl;
        PFNGLENABLEVERTEXATTRIBARRAYPROC glEnableVertexAttribArray_Impl;
        PFNGLGETACTIVEATTRIBPROC glGetActiveAttrib_Impl;
        PFNGLGETACTIVEUNIFORMPROC glGetActiveUniform_Impl;
        PFNGLGETATTACHEDSHADERSPROC glGetAttachedShaders_Impl;
        PFNGLGETATTRIBLOCATIONPROC glGetAttribLocation_Impl;
        PFNGLGETPROGRAMINFOLOGPROC glGetProgramInfoLog_Impl;
        PFNGLGETPROGRAMIVPROC glGetProgramiv_Impl;
        PFNGLGETSHADERINFOLOGPROC glGetShaderInfoLog_Impl;
        PFNGLGETSHADERSOURCEPROC glGetShaderSource_Impl;
        PFNGLGETSHADERIVPROC glGetShaderiv_Impl;
        PFNGLGETUNIFORMLOCATIONPROC glGetUniformLocation_Impl;
        PFNGLGETUNIFORMFVPROC glGetUniformfv_Impl;
        PFNGLGETUNIFORMIVPROC glGetUniformiv_Impl;
        PFNGLGETVERTEXATTRIBPOINTERVPROC glGetVertexAttribPointerv_Impl;
        PFNGLGETVERTEXATTRIBDVPROC glGetVertexAttribdv_Impl;
        PFNGLGETVERTEXATTRIBFVPROC glGetVertexAttribfv_Impl;
        PFNGLGETVERTEXATTRIBIVPROC glGetVertexAttribiv_Impl;
        PFNGLISPROGRAMPROC glIsProgram_Impl;
        PFNGLISSHADERPROC glIsShader_Impl;
        PFNGLLINKPROGRAMPROC glLinkProgram_Impl;
        PFNGLSHADERSOURCEPROC glShaderSource_Impl;
        PFNGLSTENCILFUNCSEPARATEPROC glStencilFuncSeparate_Impl;
        PFNGLSTENCILMASKSEPARATEPROC glStencilMaskSeparate_Impl;
        PFNGLSTENCILOPSEPARATEPROC glStencilOpSeparate_Impl;
        PFNGLUNIFORM1FPROC glUniform1f_Impl;
        PFNGLUNIFORM1FVPROC glUniform1fv_Impl;
        PFNGLUNIFORM1IPROC glUniform1i_Impl;
        PFNGLUNIFORM1IVPROC glUniform1iv_Impl;
        PFNGLUNIFORM2FPROC glUniform2f_Impl;
        PFNGLUNIFORM2FVPROC glUniform2fv_Impl;
        PFNGLUNIFORM2IPROC glUniform2i_Impl;
        PFNGLUNIFORM2IVPROC glUniform2iv_Impl;
        PFNGLUNIFORM3FPROC glUniform3f_Impl;
        PFNGLUNIFORM3FVPROC glUniform3fv_Impl;
        PFNGLUNIFORM3IPROC glUniform3i_Impl;
        PFNGLUNIFORM3IVPROC glUniform3iv_Impl;
        PFNGLUNIFORM4FPROC glUniform4f_Impl;
        PFNGLUNIFORM4FVPROC glUniform4fv_Impl;
        PFNGLUNIFORM4IPROC glUniform4i_Impl;
        PFNGLUNIFORM4IVPROC glUniform4iv_Impl;
        PFNGLUNIFORMMATRIX2FVPROC glUniformMatrix2fv_Impl;
        PFNGLUNIFORMMATRIX3FVPROC glUniformMatrix3fv_Impl;
        PFNGLUNIFORMMATRIX4FVPROC glUniformMatrix4fv_Impl;
        PFNGLUSEPROGRAMPROC glUseProgram_Impl;
        PFNGLVALIDATEPROGRAMPROC glValidateProgram_Impl;
        PFNGLVERTEXATTRIB1DPROC glVertexAttrib1d_Impl;
        PFNGLVERTEXATTRIB1DVPROC glVertexAttrib1dv_Impl;
        PFNGLVERTEXATTRIB1FPROC glVertexAttrib1f_Impl;
        PFNGLVERTEXATTRIB1FVPROC glVertexAttrib1fv_Impl;
        PFNGLVERTEXATTRIB1SPROC glVertexAttrib1s_Impl;
        PFNGLVERTEXATTRIB1SVPROC glVertexAttrib1sv_Impl;
        PFNGLVERTEXATTRIB2DPROC glVertexAttrib2d_Impl;
        PFNGLVERTEXATTRIB2DVPROC glVertexAttrib2dv_Impl;
        PFNGLVERTEXATTRIB2FPROC glVertexAttrib2f_Impl;
        PFNGLVERTEXATTRIB2FVPROC glVertexAttrib2fv_Impl;
        PFNGLVERTEXATTRIB2SPROC glVertexAttrib2s_Impl;
        PFNGLVERTEXATTRIB2SVPROC glVertexAttrib2sv_Impl;
        PFNGLVERTEXATTRIB3DPROC glVertexAttrib3d_Impl;
        PFNGLVERTEXATTRIB3DVPROC glVertexAttrib3dv_Impl;
        PFNGLVERTEXATTRIB3FPROC glVertexAttrib3f_Impl;
        PFNGLVERTEXATTRIB3FVPROC glVertexAttrib3fv_Impl;
        PFNGLVERTEXATTRIB3SPROC glVertexAttrib3s_Impl;
        PFNGLVERTEXATTRIB3SVPROC glVertexAttrib3sv_Impl;
        PFNGLVERTEXATTRIB4NBVPROC glVertexAttrib4Nbv_Impl;
        PFNGLVERTEXATTRIB4NIVPROC glVertexAttrib4Niv_Impl;
        PFNGLVERTEXATTRIB4NSVPROC glVertexAttrib4Nsv_Impl;
        PFNGLVERTEXATTRIB4NUBPROC glVertexAttrib4Nub_Impl;
        PFNGLVERTEXATTRIB4NUBVPROC glVertexAttrib4Nubv_Impl;
        PFNGLVERTEXATTRIB4NUIVPROC glVertexAttrib4Nuiv_Impl;
        PFNGLVERTEXATTRIB4NUSVPROC glVertexAttrib4Nusv_Impl;
        PFNGLVERTEXATTRIB4BVPROC glVertexAttrib4bv_Impl;
        PFNGLVERTEXATTRIB4DPROC glVertexAttrib4d_Impl;
        PFNGLVERTEXATTRIB4DVPROC glVertexAttrib4dv_Impl;
        PFNGLVERTEXATTRIB4FPROC glVertexAttrib4f_Impl;
        PFNGLVERTEXATTRIB4FVPROC glVertexAttrib4fv_Impl;
        PFNGLVERTEXATTRIB4IVPROC glVertexAttrib4iv_Impl;
        PFNGLVERTEXATTRIB4SPROC glVertexAttrib4s_Impl;
        PFNGLVERTEXATTRIB4SVPROC glVertexAttrib4sv_Impl;
        PFNGLVERTEXATTRIB4UBVPROC glVertexAttrib4ubv_Impl;
        PFNGLVERTEXATTRIB4UIVPROC glVertexAttrib4uiv_Impl;
        PFNGLVERTEXATTRIB4USVPROC glVertexAttrib4usv_Impl;
        PFNGLVERTEXATTRIBPOINTERPROC glVertexAttribPointer_Impl;

        // GL_VERSION_2_1
        PFNGLUNIFORMMATRIX2X3FVPROC glUniformMatrix2x3fv_Impl;
        PFNGLUNIFORMMATRIX2X4FVPROC glUniformMatrix2x4fv_Impl;
        PFNGLUNIFORMMATRIX3X2FVPROC glUniformMatrix3x2fv_Impl;
        PFNGLUNIFORMMATRIX3X4FVPROC glUniformMatrix3x4fv_Impl;
        PFNGLUNIFORMMATRIX4X2FVPROC glUniformMatrix4x2fv_Impl;
        PFNGLUNIFORMMATRIX4X3FVPROC glUniformMatrix4x3fv_Impl;

        // GL_VERSION_3_0
        PFNGLBEGINCONDITIONALRENDERPROC glBeginConditionalRender_Impl;
        PFNGLBEGINTRANSFORMFEEDBACKPROC glBeginTransformFeedback_Impl;
        PFNGLBINDFRAGDATALOCATIONPROC glBindFragDataLocation_Impl;
        PFNGLCLAMPCOLORPROC glClampColor_Impl;
        PFNGLCLEARBUFFERFIPROC glClearBufferfi_Impl;
        PFNGLCLEARBUFFERFVPROC glClearBufferfv_Impl;
        PFNGLCLEARBUFFERIVPROC glClearBufferiv_Impl;
        PFNGLCLEARBUFFERUIVPROC glClearBufferuiv_Impl;
        PFNGLCOLORMASKIPROC glColorMaski_Impl;
        PFNGLDISABLEIPROC glDisablei_Impl;
        PFNGLENABLEIPROC glEnablei_Impl;
        PFNGLENDCONDITIONALRENDERPROC glEndConditionalRender_Impl;
        PFNGLENDTRANSFORMFEEDBACKPROC glEndTransformFeedback_Impl;
        PFNGLBINDBUFFERRANGEPROC glBindBufferRange_Impl;
        PFNGLBINDBUFFERBASEPROC glBindBufferBase_Impl;
        PFNGLGETBOOLEANI_VPROC glGetBooleani_v_Impl;
        PFNGLGETINTEGERI_VPROC glGetIntegeri_v_Impl;
        PFNGLGETFRAGDATALOCATIONPROC glGetFragDataLocation_Impl;
        PFNGLGETSTRINGIPROC glGetStringi_Impl;
        PFNGLGETTEXPARAMETERIIVPROC glGetTexParameterIiv_Impl;
        PFNGLGETTEXPARAMETERIUIVPROC glGetTexParameterIuiv_Impl;
        PFNGLGETTRANSFORMFEEDBACKVARYINGPROC glGetTransformFeedbackVarying_Impl;
        PFNGLGETUNIFORMUIVPROC glGetUniformuiv_Impl;
        PFNGLGETVERTEXATTRIBIIVPROC glGetVertexAttribIiv_Impl;
        PFNGLGETVERTEXATTRIBIUIVPROC glGetVertexAttribIuiv_Impl;
        PFNGLISENABLEDIPROC glIsEnabledi_Impl;
        PFNGLTEXPARAMETERIIVPROC glTexParameterIiv_Impl;
        PFNGLTEXPARAMETERIUIVPROC glTexParameterIuiv_Impl;
        PFNGLTRANSFORMFEEDBACKVARYINGSPROC glTransformFeedbackVaryings_Impl;
        PFNGLUNIFORM1UIPROC glUniform1ui_Impl;
        PFNGLUNIFORM1UIVPROC glUniform1uiv_Impl;
        PFNGLUNIFORM2UIPROC glUniform2ui_Impl;
        PFNGLUNIFORM2UIVPROC glUniform2uiv_Impl;
        PFNGLUNIFORM3UIPROC glUniform3ui_Impl;
        PFNGLUNIFORM3UIVPROC glUniform3uiv_Impl;
        PFNGLUNIFORM4UIPROC glUniform4ui_Impl;
        PFNGLUNIFORM4UIVPROC glUniform4uiv_Impl;
        PFNGLVERTEXATTRIBI1IPROC glVertexAttribI1i_Impl;
        PFNGLVERTEXATTRIBI1IVPROC glVertexAttribI1iv_Impl;
        PFNGLVERTEXATTRIBI1UIPROC glVertexAttribI1ui_Impl;
        PFNGLVERTEXATTRIBI1UIVPROC glVertexAttribI1uiv_Impl;
        PFNGLVERTEXATTRIBI2IPROC glVertexAttribI2i_Impl;
        PFNGLVERTEXATTRIBI2IVPROC glVertexAttribI2iv_Impl;
        PFNGLVERTEXATTRIBI2UIPROC glVertexAttribI2ui_Impl;
        PFNGLVERTEXATTRIBI2UIVPROC glVertexAttribI2uiv_Impl;
        PFNGLVERTEXATTRIBI3IPROC glVertexAttribI3i_Impl;
        PFNGLVERTEXATTRIBI3IVPROC glVertexAttribI3iv_Impl;
        PFNGLVERTEXATTRIBI3UIPROC glVertexAttribI3ui_Impl;
        PFNGLVERTEXATTRIBI3UIVPROC glVertexAttribI3uiv_Impl;
        PFNGLVERTEXATTRIBI4BVPROC glVertexAttribI4bv_Impl;
        PFNGLVERTEXATTRIBI4IPROC glVertexAttribI4i_Impl;
        PFNGLVERTEXATTRIBI4IVPROC glVertexAttribI4iv_Impl;
        PFNGLVERTEXATTRIBI4SVPROC glVertexAttribI4sv_Impl;
        PFNGLVERTEXATTRIBI4UBVPROC glVertexAttribI4ubv_Impl;
        PFNGLVERTEXATTRIBI4UIPROC glVertexAttribI4ui_Impl;
        PFNGLVERTEXATTRIBI4UIVPROC glVertexAttribI4uiv_Impl;
        PFNGLVERTEXATTRIBI4USVPROC glVertexAttribI4usv_Impl;
        PFNGLVERTEXATTRIBIPOINTERPROC glVertexAttribIPointer_Impl;

        // GL_VERSION_3_1
        PFNGLDRAWARRAYSINSTANCEDPROC glDrawArraysInstanced_Impl;
        PFNGLDRAWELEMENTSINSTANCEDPROC glDrawElementsInstanced_Impl;
        PFNGLPRIMITIVERESTARTINDEXPROC glPrimitiveRestartIndex_Impl;
        PFNGLTEXBUFFERPROC glTexBuffer_Impl;

        // GL_VERSION_3_2
        PFNGLFRAMEBUFFERTEXTUREPROC glFramebufferTexture_Impl;
        PFNGLGETBUFFERPARAMETERI64VPROC glGetBufferParameteri64v_Impl;
        PFNGLGETINTEGER64I_VPROC glGetInteger64i_v_Impl;

        // GL_VERSION_3_3
        PFNGLVERTEXATTRIBDIVISORPROC glVertexAttribDivisor_Impl;

        // GL_VERSION_4_0
        PFNGLBLENDEQUATIONSEPARATEIPROC glBlendEquationSeparatei_Impl;
        PFNGLBLENDEQUATIONIPROC glBlendEquationi_Impl;
        PFNGLBLENDFUNCSEPARATEIPROC glBlendFuncSeparatei_Impl;
        PFNGLBLENDFUNCIPROC glBlendFunci_Impl;
        PFNGLMINSAMPLESHADINGPROC glMinSampleShading_Impl;

        // GL_AMD_debug_output
        PFNGLDEBUGMESSAGECALLBACKAMDPROC glDebugMessageCallbackAMD_Impl;
        PFNGLDEBUGMESSAGEENABLEAMDPROC glDebugMessageEnableAMD_Impl;
        PFNGLDEBUGMESSAGEINSERTAMDPROC glDebugMessageInsertAMD_Impl;
        PFNGLGETDEBUGMESSAGELOGAMDPROC glGetDebugMessageLogAMD_Impl;

      #if defined(GLE_APPLE_ENABLED)
        // GL_APPLE_aux_depth_stencil
        // (no functions)

        // GL_APPLE_client_storage
        // (no functions)

        // GL_APPLE_element_array
        PFNGLDRAWELEMENTARRAYAPPLEPROC glDrawElementArrayAPPLE_Impl;
        PFNGLDRAWRANGEELEMENTARRAYAPPLEPROC glDrawRangeElementArrayAPPLE_Impl;
        PFNGLELEMENTPOINTERAPPLEPROC glElementPointerAPPLE_Impl;
        PFNGLMULTIDRAWELEMENTARRAYAPPLEPROC glMultiDrawElementArrayAPPLE_Impl;
        PFNGLMULTIDRAWRANGEELEMENTARRAYAPPLEPROC glMultiDrawRangeElementArrayAPPLE_Impl;

        // GL_APPLE_fence
        PFNGLDELETEFENCESAPPLEPROC glDeleteFencesAPPLE_Impl;
        PFNGLFINISHFENCEAPPLEPROC glFinishFenceAPPLE_Impl;
        PFNGLFINISHOBJECTAPPLEPROC glFinishObjectAPPLE_Impl;
        PFNGLGENFENCESAPPLEPROC glGenFencesAPPLE_Impl;
        PFNGLISFENCEAPPLEPROC glIsFenceAPPLE_Impl;
        PFNGLSETFENCEAPPLEPROC glSetFenceAPPLE_Impl;
        PFNGLTESTFENCEAPPLEPROC glTestFenceAPPLE_Impl;
        PFNGLTESTOBJECTAPPLEPROC glTestObjectAPPLE_Impl;

        // GL_APPLE_float_pixels
        // (no functions)

        // GL_APPLE_flush_buffer_range
        PFNGLBUFFERPARAMETERIAPPLEPROC glBufferParameteriAPPLE_Impl;
        PFNGLFLUSHMAPPEDBUFFERRANGEAPPLEPROC glFlushMappedBufferRangeAPPLE_Impl;

        // GL_APPLE_object_purgeable
        PFNGLGETOBJECTPARAMETERIVAPPLEPROC glGetObjectParameterivAPPLE_Impl;
        PFNGLOBJECTPURGEABLEAPPLEPROC glObjectPurgeableAPPLE_Impl;
        PFNGLOBJECTUNPURGEABLEAPPLEPROC glObjectUnpurgeableAPPLE_Impl;

        // GL_APPLE_pixel_buffer
        // (no functions)

        // GL_APPLE_rgb_422
        // (no functions)

        // GL_APPLE_row_bytes
        // (no functions)

        // GL_APPLE_specular_vector
        // (no functions)

        // GL_APPLE_texture_range
        PFNGLGETTEXPARAMETERPOINTERVAPPLEPROC glGetTexParameterPointervAPPLE_Impl;
        PFNGLTEXTURERANGEAPPLEPROC glTextureRangeAPPLE_Impl;

        // GL_APPLE_transform_hint
        // (no functions)

        // GL_APPLE_vertex_array_object
        PFNGLBINDVERTEXARRAYAPPLEPROC glBindVertexArrayAPPLE_Impl;
        PFNGLDELETEVERTEXARRAYSAPPLEPROC glDeleteVertexArraysAPPLE_Impl;
        PFNGLGENVERTEXARRAYSAPPLEPROC glGenVertexArraysAPPLE_Impl;
        PFNGLISVERTEXARRAYAPPLEPROC glIsVertexArrayAPPLE_Impl;

        // GL_APPLE_vertex_array_range
        PFNGLFLUSHVERTEXARRAYRANGEAPPLEPROC glFlushVertexArrayRangeAPPLE_Impl;
        PFNGLVERTEXARRAYPARAMETERIAPPLEPROC glVertexArrayParameteriAPPLE_Impl;
        PFNGLVERTEXARRAYRANGEAPPLEPROC glVertexArrayRangeAPPLE_Impl;

        // GL_APPLE_vertex_program_evaluators
        PFNGLDISABLEVERTEXATTRIBAPPLEPROC glDisableVertexAttribAPPLE_Impl;
        PFNGLENABLEVERTEXATTRIBAPPLEPROC glEnableVertexAttribAPPLE_Impl;
        PFNGLISVERTEXATTRIBENABLEDAPPLEPROC glIsVertexAttribEnabledAPPLE_Impl;
        PFNGLMAPVERTEXATTRIB1DAPPLEPROC glMapVertexAttrib1dAPPLE_Impl;
        PFNGLMAPVERTEXATTRIB1FAPPLEPROC glMapVertexAttrib1fAPPLE_Impl;
        PFNGLMAPVERTEXATTRIB2DAPPLEPROC glMapVertexAttrib2dAPPLE_Impl;
        PFNGLMAPVERTEXATTRIB2FAPPLEPROC glMapVertexAttrib2fAPPLE_Impl;
      #endif // GLE_APPLE_ENABLED
      
        // GL_ARB_debug_output
        PFNGLDEBUGMESSAGECALLBACKARBPROC glDebugMessageCallbackARB_Impl;
        PFNGLDEBUGMESSAGECONTROLARBPROC glDebugMessageControlARB_Impl;
        PFNGLDEBUGMESSAGEINSERTARBPROC glDebugMessageInsertARB_Impl;
        PFNGLGETDEBUGMESSAGELOGARBPROC glGetDebugMessageLogARB_Impl;

        // GL_ARB_ES2_compatibility
        PFNGLCLEARDEPTHFPROC glClearDepthf_Impl;
        PFNGLDEPTHRANGEFPROC glDepthRangef_Impl;
        PFNGLGETSHADERPRECISIONFORMATPROC glGetShaderPrecisionFormat_Impl;
        PFNGLRELEASESHADERCOMPILERPROC glReleaseShaderCompiler_Impl;
        PFNGLSHADERBINARYPROC glShaderBinary_Impl;

        // GL_ARB_framebuffer_object
        PFNGLBINDFRAMEBUFFERPROC glBindFramebuffer_Impl;
        PFNGLBINDRENDERBUFFERPROC glBindRenderbuffer_Impl;
        PFNGLBLITFRAMEBUFFERPROC glBlitFramebuffer_Impl;
        PFNGLCHECKFRAMEBUFFERSTATUSPROC glCheckFramebufferStatus_Impl;
        PFNGLDELETEFRAMEBUFFERSPROC glDeleteFramebuffers_Impl;
        PFNGLDELETERENDERBUFFERSPROC glDeleteRenderbuffers_Impl;
        PFNGLFRAMEBUFFERRENDERBUFFERPROC glFramebufferRenderbuffer_Impl;
        PFNGLFRAMEBUFFERTEXTURE1DPROC glFramebufferTexture1D_Impl;
        PFNGLFRAMEBUFFERTEXTURE2DPROC glFramebufferTexture2D_Impl;
        PFNGLFRAMEBUFFERTEXTURE3DPROC glFramebufferTexture3D_Impl;
        PFNGLFRAMEBUFFERTEXTURELAYERPROC glFramebufferTextureLayer_Impl;
        PFNGLGENFRAMEBUFFERSPROC glGenFramebuffers_Impl;
        PFNGLGENRENDERBUFFERSPROC glGenRenderbuffers_Impl;
        PFNGLGENERATEMIPMAPPROC glGenerateMipmap_Impl;
        PFNGLGETFRAMEBUFFERATTACHMENTPARAMETERIVPROC glGetFramebufferAttachmentParameteriv_Impl;
        PFNGLGETRENDERBUFFERPARAMETERIVPROC glGetRenderbufferParameteriv_Impl;
        PFNGLISFRAMEBUFFERPROC glIsFramebuffer_Impl;
        PFNGLISRENDERBUFFERPROC glIsRenderbuffer_Impl;
        PFNGLRENDERBUFFERSTORAGEPROC glRenderbufferStorage_Impl;
        PFNGLRENDERBUFFERSTORAGEMULTISAMPLEPROC glRenderbufferStorageMultisample_Impl;
    
        // GL_ARB_framebuffer_sRGB
        // (no functions)

        // GL_ARB_texture_multisample
        PFNGLGETMULTISAMPLEFVPROC glGetMultisamplefv_Impl;
        PFNGLSAMPLEMASKIPROC glSampleMaski_Impl;
        PFNGLTEXIMAGE2DMULTISAMPLEPROC glTexImage2DMultisample_Impl;
        PFNGLTEXIMAGE3DMULTISAMPLEPROC glTexImage3DMultisample_Impl;

        // GL_ARB_texture_non_power_of_two
        // (no functions)

        // GL_ARB_timer_query
        PFNGLGETQUERYOBJECTI64VPROC glGetQueryObjecti64v_Impl;
        PFNGLGETQUERYOBJECTUI64VPROC glGetQueryObjectui64v_Impl;
        PFNGLQUERYCOUNTERPROC glQueryCounter_Impl;

        // GL_ARB_vertex_array_object
        PFNGLBINDVERTEXARRAYPROC glBindVertexArray_Impl;
        PFNGLDELETEVERTEXARRAYSPROC glDeleteVertexArrays_Impl;
        PFNGLGENVERTEXARRAYSPROC glGenVertexArrays_Impl;
        PFNGLISVERTEXARRAYPROC glIsVertexArray_Impl;

        // GL_EXT_draw_buffers2
        PFNGLCOLORMASKINDEXEDEXTPROC glColorMaskIndexedEXT_Impl;
        PFNGLDISABLEINDEXEDEXTPROC glDisableIndexedEXT_Impl;
        PFNGLENABLEINDEXEDEXTPROC glEnableIndexedEXT_Impl;
        PFNGLGETBOOLEANINDEXEDVEXTPROC glGetBooleanIndexedvEXT_Impl;
        PFNGLGETINTEGERINDEXEDVEXTPROC glGetIntegerIndexedvEXT_Impl;
        PFNGLISENABLEDINDEXEDEXTPROC glIsEnabledIndexedEXT_Impl;

        // GL_EXT_texture_filter_anisotropic
        // (no functions)

        // GL_KHR_debug
        PFNGLDEBUGMESSAGECALLBACKPROC glDebugMessageCallback_Impl;
        PFNGLDEBUGMESSAGECONTROLPROC glDebugMessageControl_Impl;
        PFNGLDEBUGMESSAGEINSERTPROC glDebugMessageInsert_Impl;
        PFNGLGETDEBUGMESSAGELOGPROC glGetDebugMessageLog_Impl;
        PFNGLGETOBJECTLABELPROC glGetObjectLabel_Impl;
        PFNGLGETOBJECTPTRLABELPROC glGetObjectPtrLabel_Impl;
        PFNGLOBJECTLABELPROC glObjectLabel_Impl;
        PFNGLOBJECTPTRLABELPROC glObjectPtrLabel_Impl;
        PFNGLPOPDEBUGGROUPPROC glPopDebugGroup_Impl;
        PFNGLPUSHDEBUGGROUPPROC glPushDebugGroup_Impl;

        // GL_KHR_robust_buffer_access_behavior
        
        // GL_WIN_swap_hint
        PFNGLADDSWAPHINTRECTWINPROC glAddSwapHintRectWIN_Impl;

      #if defined(GLE_WINDOWS_ENABLED)
        // WGL
        // We don't declare pointers for these because we statically link to the implementations, same as with the OpenGL 1.1 functions.
        // BOOL  wglCopyContext_Hook(HGLRC, HGLRC, UINT);
        // HGLRC wglCreateContext_Hook(HDC);
        // HGLRC wglCreateLayerContext_Hook(HDC, int);
        // BOOL  wglDeleteContext_Hook(HGLRC);
        // HGLRC wglGetCurrentContext_Hook(VOID);
        // HDC   wglGetCurrentDC_Hook(VOID);
        // PROC  wglGetProcAddress_Hook(LPCSTR);
        // BOOL  wglMakeCurrent_Hook(HDC, HGLRC);
        // BOOL  wglShareLists_Hook(HGLRC, HGLRC);
        // BOOL  wglUseFontBitmapsA_Hook(HDC, DWORD, DWORD, DWORD);
        // BOOL  wglUseFontBitmapsW_Hook(HDC, DWORD, DWORD, DWORD);
        // BOOL  wglUseFontOutlinesA_Hook(HDC, DWORD, DWORD, DWORD, FLOAT, FLOAT, int, LPGLYPHMETRICSFLOAT);
        // BOOL  wglUseFontOutlinesW_Hook(HDC, DWORD, DWORD, DWORD, FLOAT, FLOAT, int, LPGLYPHMETRICSFLOAT);
        // BOOL  wglDescribeLayerPlane_Hook(HDC, int, int, UINT, LPLAYERPLANEDESCRIPTOR);
        // int   wglSetLayerPaletteEntries_Hook(HDC, int, int, int, CONST COLORREF *);
        // int   wglGetLayerPaletteEntries_Hook(HDC, int, int, int, COLORREF *);
        // BOOL  wglRealizeLayerPalette_Hook(HDC, int, BOOL);
        // BOOL  wglSwapLayerBuffers_Hook(HDC, UINT);
        // DWORD wglSwapMultipleBuffers_Hook(UINT, CONST WGLSWAP *);

        // WGL_ARB_buffer_region
        PFNWGLCREATEBUFFERREGIONARBPROC  wglCreateBufferRegionARB_Impl;
        PFNWGLDELETEBUFFERREGIONARBPROC  wglDeleteBufferRegionARB_Impl;
        PFNWGLSAVEBUFFERREGIONARBPROC    wglSaveBufferRegionARB_Impl;
        PFNWGLRESTOREBUFFERREGIONARBPROC wglRestoreBufferRegionARB_Impl;

        // WGL_ARB_extensions_string
        PFNWGLGETEXTENSIONSSTRINGARBPROC wglGetExtensionsStringARB_Impl;

        // WGL_ARB_pixel_format
        PFNWGLGETPIXELFORMATATTRIBIVARBPROC wglGetPixelFormatAttribivARB_Impl;
        PFNWGLGETPIXELFORMATATTRIBFVARBPROC wglGetPixelFormatAttribfvARB_Impl;
        PFNWGLCHOOSEPIXELFORMATARBPROC      wglChoosePixelFormatARB_Impl;

        // WGL_ARB_make_current_read
        PFNWGLMAKECONTEXTCURRENTARBPROC wglMakeContextCurrentARB_Impl;
        PFNWGLGETCURRENTREADDCARBPROC   wglGetCurrentReadDCARB_Impl;

        // WGL_ARB_pbuffer
        PFNWGLCREATEPBUFFERARBPROC    wglCreatePbufferARB_Impl;
        PFNWGLGETPBUFFERDCARBPROC     wglGetPbufferDCARB_Impl;
        PFNWGLRELEASEPBUFFERDCARBPROC wglReleasePbufferDCARB_Impl;
        PFNWGLDESTROYPBUFFERARBPROC   wglDestroyPbufferARB_Impl;
        PFNWGLQUERYPBUFFERARBPROC     wglQueryPbufferARB_Impl;

        // WGL_ARB_render_texture
        PFNWGLBINDTEXIMAGEARBPROC     wglBindTexImageARB_Impl;
        PFNWGLRELEASETEXIMAGEARBPROC  wglReleaseTexImageARB_Impl;
        PFNWGLSETPBUFFERATTRIBARBPROC wglSetPbufferAttribARB_Impl;

        // WGL_ARB_pixel_format_float
        // (no functions)
        
        // WGL_ARB_framebuffer_sRGB
        // (no functions)

        // WGL_NV_present_video
        PFNWGLENUMERATEVIDEODEVICESNVPROC wglEnumerateVideoDevicesNV_Impl;
        PFNWGLBINDVIDEODEVICENVPROC       wglBindVideoDeviceNV_Impl;
        PFNWGLQUERYCURRENTCONTEXTNVPROC   wglQueryCurrentContextNV_Impl;

        // WGL_ARB_create_context
        PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARB_Impl;

        // WGL_ARB_create_context_profile
        // (no functions)

        // WGL_ARB_create_context_robustness
        // (no functions)

        // WGL_EXT_extensions_string
        PFNWGLGETEXTENSIONSSTRINGEXTPROC wglGetExtensionsStringEXT_Impl;

        // WGL_EXT_swap_control
        PFNWGLGETSWAPINTERVALEXTPROC wglGetSwapIntervalEXT_Impl;
        PFNWGLSWAPINTERVALEXTPROC    wglSwapIntervalEXT_Impl;

        // WGL_OML_sync_control
        PFNWGLGETSYNCVALUESOMLPROC       wglGetSyncValuesOML_Impl;
        PFNWGLGETMSCRATEOMLPROC          wglGetMscRateOML_Impl;
        PFNWGLSWAPBUFFERSMSCOMLPROC      wglSwapBuffersMscOML_Impl;
        PFNWGLSWAPLAYERBUFFERSMSCOMLPROC wglSwapLayerBuffersMscOML_Impl;
        PFNWGLWAITFORMSCOMLPROC          wglWaitForMscOML_Impl;
        PFNWGLWAITFORSBCOMLPROC          wglWaitForSbcOML_Impl;

        // WGL_EXT_framebuffer_sRGB
        // (no functions)

        // WGL_NV_video_output
        PFNWGLGETVIDEODEVICENVPROC     wglGetVideoDeviceNV_Impl;
        PFNWGLRELEASEVIDEODEVICENVPROC wglReleaseVideoDeviceNV_Impl;
        PFNWGLBINDVIDEOIMAGENVPROC     wglBindVideoImageNV_Impl;
        PFNWGLRELEASEVIDEOIMAGENVPROC  wglReleaseVideoImageNV_Impl;
        PFNWGLSENDPBUFFERTOVIDEONVPROC wglSendPbufferToVideoNV_Impl;
        PFNWGLGETVIDEOINFONVPROC       wglGetVideoInfoNV_Impl;

        // WGL_NV_swap_group
        PFNWGLJOINSWAPGROUPNVPROC      wglJoinSwapGroupNV_Impl;
        PFNWGLBINDSWAPBARRIERNVPROC    wglBindSwapBarrierNV_Impl;
        PFNWGLQUERYSWAPGROUPNVPROC     wglQuerySwapGroupNV_Impl;
        PFNWGLQUERYMAXSWAPGROUPSNVPROC wglQueryMaxSwapGroupsNV_Impl;
        PFNWGLQUERYFRAMECOUNTNVPROC    wglQueryFrameCountNV_Impl;
        PFNWGLRESETFRAMECOUNTNVPROC    wglResetFrameCountNV_Impl;

        // WGL_NV_video_capture
        PFNWGLBINDVIDEOCAPTUREDEVICENVPROC       wglBindVideoCaptureDeviceNV_Impl;
        PFNWGLENUMERATEVIDEOCAPTUREDEVICESNVPROC wglEnumerateVideoCaptureDevicesNV_Impl;
        PFNWGLLOCKVIDEOCAPTUREDEVICENVPROC       wglLockVideoCaptureDeviceNV_Impl;
        PFNWGLQUERYVIDEOCAPTUREDEVICENVPROC      wglQueryVideoCaptureDeviceNV_Impl;
        PFNWGLRELEASEVIDEOCAPTUREDEVICENVPROC    wglReleaseVideoCaptureDeviceNV_Impl;

        // WGL_NV_copy_image
        PFNWGLCOPYIMAGESUBDATANVPROC wglCopyImageSubDataNV_Impl;
    
        // WGL_NV_DX_interop
        PFNWGLDXCLOSEDEVICENVPROC            wglDXCloseDeviceNV_Impl;
        PFNWGLDXLOCKOBJECTSNVPROC            wglDXLockObjectsNV_Impl;
        PFNWGLDXOBJECTACCESSNVPROC           wglDXObjectAccessNV_Impl;
        PFNWGLDXOPENDEVICENVPROC             wglDXOpenDeviceNV_Impl;
        PFNWGLDXREGISTEROBJECTNVPROC         wglDXRegisterObjectNV_Impl;
        PFNWGLDXSETRESOURCESHAREHANDLENVPROC wglDXSetResourceShareHandleNV_Impl;
        PFNWGLDXUNLOCKOBJECTSNVPROC          wglDXUnlockObjectsNV_Impl;
        PFNWGLDXUNREGISTEROBJECTNVPROC       wglDXUnregisterObjectNV_Impl;

      #endif // GLE_WINDOWS_ENABLED
      
      #if defined(GLE_UNIX_ENABLED)
        // GLX_VERSION_1_1
        // We don't create any pointers, because we assume these functions are always present.
        
        // GLX_VERSION_1_2
        PFNGLXGETCURRENTDISPLAYPROC      glXGetCurrentDisplay_Impl;

        // GLX_VERSION_1_3
        PFNGLXCHOOSEFBCONFIGPROC         glXChooseFBConfig_Impl;
        PFNGLXCREATENEWCONTEXTPROC       glXCreateNewContext_Impl;
        PFNGLXCREATEPBUFFERPROC          glXCreatePbuffer_Impl;
        PFNGLXCREATEPIXMAPPROC           glXCreatePixmap_Impl;
        PFNGLXCREATEWINDOWPROC           glXCreateWindow_Impl;
        PFNGLXDESTROYPBUFFERPROC         glXDestroyPbuffer_Impl;
        PFNGLXDESTROYPIXMAPPROC          glXDestroyPixmap_Impl;
        PFNGLXDESTROYWINDOWPROC          glXDestroyWindow_Impl;
        PFNGLXGETCURRENTREADDRAWABLEPROC glXGetCurrentReadDrawable_Impl;
        PFNGLXGETFBCONFIGATTRIBPROC      glXGetFBConfigAttrib_Impl;
        PFNGLXGETFBCONFIGSPROC           glXGetFBConfigs_Impl;
        PFNGLXGETSELECTEDEVENTPROC       glXGetSelectedEvent_Impl;
        PFNGLXGETVISUALFROMFBCONFIGPROC  glXGetVisualFromFBConfig_Impl;
        PFNGLXMAKECONTEXTCURRENTPROC     glXMakeContextCurrent_Impl;
        PFNGLXQUERYCONTEXTPROC           glXQueryContext_Impl;
        PFNGLXQUERYDRAWABLEPROC          glXQueryDrawable_Impl;
        PFNGLXSELECTEVENTPROC            glXSelectEvent_Impl;

        // GLX_VERSION_1_4
        // Nothing to declare
        
        // GLX_EXT_swap_control
        PFNGLXSWAPINTERVALEXTPROC glXSwapIntervalEXT_Impl;

        // GLX_OML_sync_control
        PFNGLXGETMSCRATEOMLPROC     glXGetMscRateOML_Impl;
        PFNGLXGETSYNCVALUESOMLPROC  glXGetSyncValuesOML_Impl;
        PFNGLXSWAPBUFFERSMSCOMLPROC glXSwapBuffersMscOML_Impl;
        PFNGLXWAITFORMSCOMLPROC     glXWaitForMscOML_Impl;
        PFNGLXWAITFORSBCOMLPROC     glXWaitForSbcOML_Impl;

      #endif // GLE_UNIX_ENABLED

        
        // Boolean extension support indicators. Each of these identifies the
        // presence or absence of the given extension. A better solution here
        // might be to use an STL map<const char*, bool>.
        bool gl_AMD_debug_output;
      //bool gl_AMD_performance_monitor;
        bool gl_APPLE_aux_depth_stencil;
        bool gl_APPLE_client_storage;
        bool gl_APPLE_element_array;
        bool gl_APPLE_fence;
        bool gl_APPLE_float_pixels;
        bool gl_APPLE_flush_buffer_range;
        bool gl_APPLE_object_purgeable;
        bool gl_APPLE_pixel_buffer;
        bool gl_APPLE_rgb_422;
        bool gl_APPLE_row_bytes;
        bool gl_APPLE_specular_vector;
        bool gl_APPLE_texture_range;
        bool gl_APPLE_transform_hint;
        bool gl_APPLE_vertex_array_object;
        bool gl_APPLE_vertex_array_range;
        bool gl_APPLE_vertex_program_evaluators;
        bool gl_APPLE_ycbcr_422;
        bool gl_ARB_debug_output;
      //bool gl_ARB_direct_state_access;
        bool gl_ARB_ES2_compatibility;
        bool gl_ARB_framebuffer_object;
        bool gl_ARB_framebuffer_sRGB;
        bool gl_ARB_texture_multisample;
        bool gl_ARB_texture_non_power_of_two;
        bool gl_ARB_timer_query;
        bool gl_ARB_vertex_array_object;
      //bool gl_ARB_vertex_attrib_binding;
        bool gl_EXT_draw_buffers2;
        bool gl_EXT_texture_filter_anisotropic;
      //bool gl_KHR_context_flush_control;
        bool gl_KHR_debug;
      //bool gl_KHR_robust_buffer_access_behavior;
      //bool gl_KHR_robustness;
        bool gl_WIN_swap_hint;
        
      #if defined(GLE_WINDOWS_ENABLED)
        bool gl_WGL_ARB_buffer_region;
        bool gl_WGL_ARB_extensions_string;
        bool gl_WGL_ARB_pixel_format;
        bool gl_WGL_ARB_make_current_read;
        bool gl_WGL_ARB_pbuffer;
        bool gl_WGL_ARB_render_texture;
        bool gl_WGL_TYPE_RGBA_FLOAT_ARB;
        bool gl_WGL_ARB_framebuffer_sRGB;
        bool gl_WGL_NV_present_video;
        bool gl_WGL_ARB_create_context;
        bool gl_WGL_ARB_create_context_profile;
        bool gl_WGL_ARB_create_context_robustness;
        bool gl_WGL_EXT_extensions_string;
        bool gl_WGL_EXT_swap_control;
        bool gl_WGL_OML_sync_control;
        bool gl_WGL_EXT_framebuffer_sRGB;
        bool gl_WGL_NV_video_output;
        bool gl_WGL_NV_swap_group;
        bool gl_WGL_NV_video_capture;
        bool gl_WGL_NV_copy_image;
        bool gl_WGL_NV_DX_interop;
      #elif defined(GLE_UNIX_ENABLED)
        bool gl_GLX_EXT_swap_control;
        bool gl_GLX_OML_sync_control;
      #endif
        
    }; // class GLEContext


} // namespace OVR


#endif // Header include guard
