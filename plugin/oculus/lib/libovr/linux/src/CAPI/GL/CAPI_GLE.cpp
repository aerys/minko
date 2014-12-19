/************************************************************************************

Filename    :   Render_GLE.cpp
Content     :   OpenGL Extensions support. Implements a stripped down glew-like 
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

#include "CAPI_GLE.h"
#include "../../Kernel/OVR_Log.h"
#include <string.h>


#if defined(_WIN32)
    #if !defined(WINAPI)
        #ifndef WIN32_LEAN_AND_MEAN
            #define WIN32_LEAN_AND_MEAN 1
        #endif
        #include <windows.h>
    #endif

    #pragma comment(lib, "opengl32.lib")
#elif defined(__APPLE__)
    #include <stdlib.h>
    #include <string.h>
    #include <AvailabilityMacros.h>
    #include <dlfcn.h>
#endif



//namespace OVR
//{
    // OVRTypeof
    // Acts the same as the C++11 decltype expression, though with reduced requirements.
    #if !defined(OVRTypeof)
        #if defined(_MSC_VER)
            #define OVRTypeof(x) decltype(x)    // VS2010+ unilaterally supports decltype
        #else
            #define OVRTypeof(x) __typeof__(x)  // Other compilers support decltype, but usually not unless C++11 support is present and explicitly enabled.
        #endif
    #endif

    
    // GLELoadProc
    // Macro which implements dynamically looking up and assigning an OpenGL function.
    //
    // Example usage:
    //     GLELoadProc(glCopyTexSubImage3D, glCopyTexSubImage3D);
    // Expands to:
    //     gleCopyTexSubImage3D = (OVRTypeof(gleCopyTexSubImage3D)) GLEGetProcAddress("glCopyTexSubImage3D");
    
    #define GLELoadProc(var, name) var = (OVRTypeof(var))GLEGetProcAddress(#name)
    

    // Disable some #defines, as we need to call these functions directly.
    #if defined(GLE_HOOKING_ENABLED)
        #if defined(_WIN32)
            #undef wglGetProcAddress
            extern "C" { GLAPI PROC GLAPIENTRY wglGetProcAddress(LPCSTR lpszProc); }
        #endif

        #undef glGetString
        extern "C" { GLAPI const GLubyte * GLAPIENTRY glGetString(GLenum name); }
    #endif


    // Generic OpenGL GetProcAddress function interface. Maps to platform-specific functionality
    // internally. On Windows this is equivalent to wglGetProcAddress as opposed to global GetProcAddress.
    void* OVR::GLEGetProcAddress(const char* name)
    {
        #if defined(_WIN32)
            return wglGetProcAddress(name);
        
        #elif defined(__APPLE__)
            // Requires the OS 10.3 SDK or later.
            static void* dlImage = NULL;
            void* addr = nullptr;
        
            if(!dlImage)
                dlImage = dlopen("/System/Library/Frameworks/OpenGL.framework/Versions/Current/OpenGL", RTLD_LAZY);
        
            if(dlImage)
                addr = dlsym(dlImage, name);
        
            return addr;
        
        #elif defined(__ANDROID__)
            return eglGetProcAddress(name);
        
        #else
            // This was glXGetProcAddressARB in GLX versions prior to v1.4, but that was ten years ago.
            return (void*)glXGetProcAddress((const GLubyte*)name);
        #endif
    }



    // Current context functionality
    static OVR::GLEContext* GLECurrentContext = NULL;
    
    OVR::GLEContext* OVR::GLEContext::GetCurrentContext()
    {
        return GLECurrentContext;
    }

    void OVR::GLEContext::SetCurrentContext(OVR::GLEContext* p)
    {
        GLECurrentContext = p;
    }
        

    
    OVR::GLEContext::GLEContext()
    {
        // The following sequence is not thread-safe. Two threads could set the context to this at the same time.
        if(GetCurrentContext() == NULL)
            SetCurrentContext(this);
    }
    
    
    OVR::GLEContext::~GLEContext()
    {
        // Currently empty
    }
    
    
    void OVR::GLEContext::Init()
    {
        InitVersion();
        InitExtensionLoad();
        InitExtensionSupport();
    }
    
    
    void OVR::GLEContext::Shutdown()
    {
        // This memset is valid only if this class has no virtual functions (similar to concept of POD).
        // We cannot assert this because restrictions prevent us from using C++11 type traits here.
        memset(this, 0, sizeof(GLEContext));
    }


    void OVR::GLEContext::InitVersion()
    {
        const char* version = (const char*)glGetString(GL_VERSION);
        int fields = 0, major = 0, minor = 0;
        bool isGLES = false;

        OVR_ASSERT(version);
        if (version)
        {
            OVR_DEBUG_LOG(("GL_VERSION: %s", (const char*)version));

            // Skip all leading non-digits before reading %d.
            // Example GL_VERSION strings:
            //   "1.5 ATI-1.4.18"
            //   "OpenGL ES-CM 3.2"
            OVR_DISABLE_MSVC_WARNING(4996) // "scanf may be unsafe"
            fields = sscanf(version, isdigit(*version) ? "%d.%d" : "%*[^0-9]%d.%d", &major, &minor);
            isGLES = (strstr(version, "OpenGL ES") != NULL);
            OVR_RESTORE_MSVC_WARNING()
        }
        else
        {
            LogText("Warning: GL_VERSION was NULL\n");
        }

        // If two fields were not found,
        if (fields != 2)
        {
            static_assert(sizeof(major) == sizeof(GLint), "type mis-match");

            glGetIntegerv(GL_MAJOR_VERSION, &major);
            glGetIntegerv(GL_MINOR_VERSION, &minor);
        }

        GLint profileMask = 0;
        if(WholeVersion >= 302)
        {
            // Older NVidia drivers have a bug with this on at least Windows.
            // https://www.opengl.org/discussion_boards/showthread.php/171380-NVIDIA-drivers-not-returning-the-right-profile-mas
            // A workaround could be to check for the GL_ARB_compatibility extension, which indicates if OpenGL is in compatibility mode,
            // and if not then we are in core profile mode. On Apple another solution would be to use NSOpeNGLPixelFormat
            // NSOpenGLView::pixelFormat to get the core profile attribute.
            glGetIntegerv(GL_CONTEXT_PROFILE_MASK, &profileMask);
        }
        
        // Write version data
        MajorVersion  = major;
        MinorVersion  = minor;
        WholeVersion  = (major * 100) + minor;
        IsGLES        = isGLES;
        IsCoreProfile = (profileMask == GL_CONTEXT_CORE_PROFILE_BIT); // There's also GL_CONTEXT_COMPATIBILITY_PROFILE_BIT
    }
    
    
    void OVR::GLEContext::InitExtensionLoad()
    {
        // GL_VERSION_1_1
        // We don't load these but rather link to them directly.
        
        // GL_VERSION_1_2
        GLELoadProc(glCopyTexSubImage3D_Impl, glCopyTexSubImage3D);  // This expands to a get proc address call (e.g. wglGetProcAddress on Windows).
        GLELoadProc(glDrawRangeElements_Impl, glDrawRangeElements);
        GLELoadProc(glTexImage3D_Impl, glTexImage3D);
        GLELoadProc(glTexSubImage3D_Impl, glTexSubImage3D);

        // GL_VERSION_1_3
        GLELoadProc(glActiveTexture_Impl, glActiveTexture);
        GLELoadProc(glClientActiveTexture_Impl, glClientActiveTexture);
        GLELoadProc(glCompressedTexImage1D_Impl, glCompressedTexImage1D);
        GLELoadProc(glCompressedTexImage2D_Impl, glCompressedTexImage2D);
        GLELoadProc(glCompressedTexImage3D_Impl, glCompressedTexImage3D);
        GLELoadProc(glCompressedTexSubImage1D_Impl, glCompressedTexSubImage1D);
        GLELoadProc(glCompressedTexSubImage2D_Impl, glCompressedTexSubImage2D);
        GLELoadProc(glCompressedTexSubImage3D_Impl, glCompressedTexSubImage3D);
        GLELoadProc(glGetCompressedTexImage_Impl, glGetCompressedTexImage);
        GLELoadProc(glLoadTransposeMatrixd_Impl, glLoadTransposeMatrixd);
        GLELoadProc(glLoadTransposeMatrixf_Impl, glLoadTransposeMatrixf);
        GLELoadProc(glMultTransposeMatrixd_Impl, glMultTransposeMatrixd);
        GLELoadProc(glMultTransposeMatrixf_Impl, glMultTransposeMatrixf);
        GLELoadProc(glMultiTexCoord1d_Impl, glMultiTexCoord1d);
        GLELoadProc(glMultiTexCoord1dv_Impl, glMultiTexCoord1dv);
        GLELoadProc(glMultiTexCoord1f_Impl, glMultiTexCoord1f);
        GLELoadProc(glMultiTexCoord1fv_Impl, glMultiTexCoord1fv);
        GLELoadProc(glMultiTexCoord1i_Impl, glMultiTexCoord1i);
        GLELoadProc(glMultiTexCoord1iv_Impl, glMultiTexCoord1iv);
        GLELoadProc(glMultiTexCoord1s_Impl, glMultiTexCoord1s);
        GLELoadProc(glMultiTexCoord1sv_Impl, glMultiTexCoord1sv);
        GLELoadProc(glMultiTexCoord2d_Impl, glMultiTexCoord2d);
        GLELoadProc(glMultiTexCoord2dv_Impl, glMultiTexCoord2dv);
        GLELoadProc(glMultiTexCoord2f_Impl, glMultiTexCoord2f);
        GLELoadProc(glMultiTexCoord2fv_Impl, glMultiTexCoord2fv);
        GLELoadProc(glMultiTexCoord2i_Impl, glMultiTexCoord2i);
        GLELoadProc(glMultiTexCoord2iv_Impl, glMultiTexCoord2iv);
        GLELoadProc(glMultiTexCoord2s_Impl, glMultiTexCoord2s);
        GLELoadProc(glMultiTexCoord2sv_Impl, glMultiTexCoord2sv);
        GLELoadProc(glMultiTexCoord3d_Impl, glMultiTexCoord3d);
        GLELoadProc(glMultiTexCoord3dv_Impl, glMultiTexCoord3dv);
        GLELoadProc(glMultiTexCoord3f_Impl, glMultiTexCoord3f);
        GLELoadProc(glMultiTexCoord3fv_Impl, glMultiTexCoord3fv);
        GLELoadProc(glMultiTexCoord3i_Impl, glMultiTexCoord3i);
        GLELoadProc(glMultiTexCoord3iv_Impl, glMultiTexCoord3iv);
        GLELoadProc(glMultiTexCoord3s_Impl, glMultiTexCoord3s);
        GLELoadProc(glMultiTexCoord3sv_Impl, glMultiTexCoord3sv);
        GLELoadProc(glMultiTexCoord4d_Impl, glMultiTexCoord4d);
        GLELoadProc(glMultiTexCoord4dv_Impl, glMultiTexCoord4dv);
        GLELoadProc(glMultiTexCoord4f_Impl, glMultiTexCoord4f);
        GLELoadProc(glMultiTexCoord4fv_Impl, glMultiTexCoord4fv);
        GLELoadProc(glMultiTexCoord4i_Impl, glMultiTexCoord4i);
        GLELoadProc(glMultiTexCoord4iv_Impl, glMultiTexCoord4iv);
        GLELoadProc(glMultiTexCoord4s_Impl, glMultiTexCoord4s);
        GLELoadProc(glMultiTexCoord4sv_Impl, glMultiTexCoord4sv);
        GLELoadProc(glSampleCoverage_Impl, glSampleCoverage);

        // GL_VERSION_1_4
        GLELoadProc(glBlendColor_Impl, glBlendColor);
        GLELoadProc(glBlendEquation_Impl, glBlendEquation);
        GLELoadProc(glBlendFuncSeparate_Impl, glBlendFuncSeparate);
        GLELoadProc(glFogCoordPointer_Impl, glFogCoordPointer);
        GLELoadProc(glFogCoordd_Impl, glFogCoordd);
        GLELoadProc(glFogCoorddv_Impl, glFogCoorddv);
        GLELoadProc(glFogCoordf_Impl, glFogCoordf);
        GLELoadProc(glFogCoordfv_Impl, glFogCoordfv);
        GLELoadProc(glMultiDrawArrays_Impl, glMultiDrawArrays);
        GLELoadProc(glMultiDrawElements_Impl, glMultiDrawElements);
        GLELoadProc(glPointParameterf_Impl, glPointParameterf);
        GLELoadProc(glPointParameterfv_Impl, glPointParameterfv);
        GLELoadProc(glPointParameteri_Impl, glPointParameteri);
        GLELoadProc(glPointParameteriv_Impl, glPointParameteriv);
        GLELoadProc(glSecondaryColor3b_Impl, glSecondaryColor3b);
        GLELoadProc(glSecondaryColor3bv_Impl, glSecondaryColor3bv);
        GLELoadProc(glSecondaryColor3d_Impl, glSecondaryColor3d);
        GLELoadProc(glSecondaryColor3dv_Impl, glSecondaryColor3dv);
        GLELoadProc(glSecondaryColor3f_Impl, glSecondaryColor3f);
        GLELoadProc(glSecondaryColor3fv_Impl, glSecondaryColor3fv);
        GLELoadProc(glSecondaryColor3i_Impl, glSecondaryColor3i);
        GLELoadProc(glSecondaryColor3iv_Impl, glSecondaryColor3iv);
        GLELoadProc(glSecondaryColor3s_Impl, glSecondaryColor3s);
        GLELoadProc(glSecondaryColor3sv_Impl, glSecondaryColor3sv);
        GLELoadProc(glSecondaryColor3ub_Impl, glSecondaryColor3ub);
        GLELoadProc(glSecondaryColor3ubv_Impl, glSecondaryColor3ubv);
        GLELoadProc(glSecondaryColor3ui_Impl, glSecondaryColor3ui);
        GLELoadProc(glSecondaryColor3uiv_Impl, glSecondaryColor3uiv);
        GLELoadProc(glSecondaryColor3us_Impl, glSecondaryColor3us);
        GLELoadProc(glSecondaryColor3usv_Impl, glSecondaryColor3usv);
        GLELoadProc(glSecondaryColorPointer_Impl, glSecondaryColorPointer);
        GLELoadProc(glWindowPos2d_Impl, glWindowPos2d);
        GLELoadProc(glWindowPos2dv_Impl, glWindowPos2dv);
        GLELoadProc(glWindowPos2f_Impl, glWindowPos2f);
        GLELoadProc(glWindowPos2fv_Impl, glWindowPos2fv);
        GLELoadProc(glWindowPos2i_Impl, glWindowPos2i);
        GLELoadProc(glWindowPos2iv_Impl, glWindowPos2iv);
        GLELoadProc(glWindowPos2s_Impl, glWindowPos2s);
        GLELoadProc(glWindowPos2sv_Impl, glWindowPos2sv);
        GLELoadProc(glWindowPos3d_Impl, glWindowPos3d);
        GLELoadProc(glWindowPos3dv_Impl, glWindowPos3dv);
        GLELoadProc(glWindowPos3f_Impl, glWindowPos3f);
        GLELoadProc(glWindowPos3fv_Impl, glWindowPos3fv);
        GLELoadProc(glWindowPos3i_Impl, glWindowPos3i);
        GLELoadProc(glWindowPos3iv_Impl, glWindowPos3iv);
        GLELoadProc(glWindowPos3s_Impl, glWindowPos3s);
        GLELoadProc(glWindowPos3sv_Impl, glWindowPos3sv);

        // GL_VERSION_1_5
        GLELoadProc(glBeginQuery_Impl, glBeginQuery);
        GLELoadProc(glBindBuffer_Impl, glBindBuffer);
        GLELoadProc(glBufferData_Impl, glBufferData);
        GLELoadProc(glBufferSubData_Impl, glBufferSubData);
        GLELoadProc(glDeleteBuffers_Impl, glDeleteBuffers);
        GLELoadProc(glDeleteQueries_Impl, glDeleteQueries);
        GLELoadProc(glEndQuery_Impl, glEndQuery);
        GLELoadProc(glGenBuffers_Impl, glGenBuffers);
        GLELoadProc(glGenQueries_Impl, glGenQueries);
        GLELoadProc(glGetBufferParameteriv_Impl, glGetBufferParameteriv);
        GLELoadProc(glGetBufferPointerv_Impl, glGetBufferPointerv);
        GLELoadProc(glGetBufferSubData_Impl, glGetBufferSubData);
        GLELoadProc(glGetQueryObjectiv_Impl, glGetQueryObjectiv);
        GLELoadProc(glGetQueryObjectuiv_Impl, glGetQueryObjectuiv);
        GLELoadProc(glGetQueryiv_Impl, glGetQueryiv);
        GLELoadProc(glIsBuffer_Impl, glIsBuffer);
        GLELoadProc(glIsQuery_Impl, glIsQuery);
        GLELoadProc(glMapBuffer_Impl, glMapBuffer);
        GLELoadProc(glUnmapBuffer_Impl, glUnmapBuffer);

        // GL_VERSION_2_0
        GLELoadProc(glAttachShader_Impl, glAttachShader);
        GLELoadProc(glBindAttribLocation_Impl, glBindAttribLocation);
        GLELoadProc(glBlendEquationSeparate_Impl, glBlendEquationSeparate);
        GLELoadProc(glCompileShader_Impl, glCompileShader);
        GLELoadProc(glCreateProgram_Impl, glCreateProgram);
        GLELoadProc(glCreateShader_Impl, glCreateShader);
        GLELoadProc(glDeleteProgram_Impl, glDeleteProgram);
        GLELoadProc(glDeleteShader_Impl, glDeleteShader);
        GLELoadProc(glDetachShader_Impl, glDetachShader);
        GLELoadProc(glDisableVertexAttribArray_Impl, glDisableVertexAttribArray);
        GLELoadProc(glDrawBuffers_Impl, glDrawBuffers);
        GLELoadProc(glEnableVertexAttribArray_Impl, glEnableVertexAttribArray);
        GLELoadProc(glGetActiveAttrib_Impl, glGetActiveAttrib);
        GLELoadProc(glGetActiveUniform_Impl, glGetActiveUniform);
        GLELoadProc(glGetAttachedShaders_Impl, glGetAttachedShaders);
        GLELoadProc(glGetAttribLocation_Impl, glGetAttribLocation);
        GLELoadProc(glGetProgramInfoLog_Impl, glGetProgramInfoLog);
        GLELoadProc(glGetProgramiv_Impl, glGetProgramiv);
        GLELoadProc(glGetShaderInfoLog_Impl, glGetShaderInfoLog);
        GLELoadProc(glGetShaderSource_Impl, glGetShaderSource);
        GLELoadProc(glGetShaderiv_Impl, glGetShaderiv);
        GLELoadProc(glGetUniformLocation_Impl, glGetUniformLocation);
        GLELoadProc(glGetUniformfv_Impl, glGetUniformfv);
        GLELoadProc(glGetUniformiv_Impl, glGetUniformiv);
        GLELoadProc(glGetVertexAttribPointerv_Impl, glGetVertexAttribPointerv);
        GLELoadProc(glGetVertexAttribdv_Impl, glGetVertexAttribdv);
        GLELoadProc(glGetVertexAttribfv_Impl, glGetVertexAttribfv);
        GLELoadProc(glGetVertexAttribiv_Impl, glGetVertexAttribiv);
        GLELoadProc(glIsProgram_Impl, glIsProgram);
        GLELoadProc(glIsShader_Impl, glIsShader);
        GLELoadProc(glLinkProgram_Impl, glLinkProgram);
        GLELoadProc(glShaderSource_Impl, glShaderSource);
        GLELoadProc(glStencilFuncSeparate_Impl, glStencilFuncSeparate);
        GLELoadProc(glStencilMaskSeparate_Impl, glStencilMaskSeparate);
        GLELoadProc(glStencilOpSeparate_Impl, glStencilOpSeparate);
        GLELoadProc(glUniform1f_Impl, glUniform1f);
        GLELoadProc(glUniform1fv_Impl, glUniform1fv);
        GLELoadProc(glUniform1i_Impl, glUniform1i);
        GLELoadProc(glUniform1iv_Impl, glUniform1iv);
        GLELoadProc(glUniform2f_Impl, glUniform2f);
        GLELoadProc(glUniform2fv_Impl, glUniform2fv);
        GLELoadProc(glUniform2i_Impl, glUniform2i);
        GLELoadProc(glUniform2iv_Impl, glUniform2iv);
        GLELoadProc(glUniform3f_Impl, glUniform3f);
        GLELoadProc(glUniform3fv_Impl, glUniform3fv);
        GLELoadProc(glUniform3i_Impl, glUniform3i);
        GLELoadProc(glUniform3iv_Impl, glUniform3iv);
        GLELoadProc(glUniform4f_Impl, glUniform4f);
        GLELoadProc(glUniform4fv_Impl, glUniform4fv);
        GLELoadProc(glUniform4i_Impl, glUniform4i);
        GLELoadProc(glUniform4iv_Impl, glUniform4iv);
        GLELoadProc(glUniformMatrix2fv_Impl, glUniformMatrix2fv);
        GLELoadProc(glUniformMatrix3fv_Impl, glUniformMatrix3fv);
        GLELoadProc(glUniformMatrix4fv_Impl, glUniformMatrix4fv);
        GLELoadProc(glUseProgram_Impl, glUseProgram);
        GLELoadProc(glValidateProgram_Impl, glValidateProgram);
        GLELoadProc(glVertexAttrib1d_Impl, glVertexAttrib1d);
        GLELoadProc(glVertexAttrib1dv_Impl, glVertexAttrib1dv);
        GLELoadProc(glVertexAttrib1f_Impl, glVertexAttrib1f);
        GLELoadProc(glVertexAttrib1fv_Impl, glVertexAttrib1fv);
        GLELoadProc(glVertexAttrib1s_Impl, glVertexAttrib1s);
        GLELoadProc(glVertexAttrib1sv_Impl, glVertexAttrib1sv);
        GLELoadProc(glVertexAttrib2d_Impl, glVertexAttrib2d);
        GLELoadProc(glVertexAttrib2dv_Impl, glVertexAttrib2dv);
        GLELoadProc(glVertexAttrib2f_Impl, glVertexAttrib2f);
        GLELoadProc(glVertexAttrib2fv_Impl, glVertexAttrib2fv);
        GLELoadProc(glVertexAttrib2s_Impl, glVertexAttrib2s);
        GLELoadProc(glVertexAttrib2sv_Impl, glVertexAttrib2sv);
        GLELoadProc(glVertexAttrib3d_Impl, glVertexAttrib3d);
        GLELoadProc(glVertexAttrib3dv_Impl, glVertexAttrib3dv);
        GLELoadProc(glVertexAttrib3f_Impl, glVertexAttrib3f);
        GLELoadProc(glVertexAttrib3fv_Impl, glVertexAttrib3fv);
        GLELoadProc(glVertexAttrib3s_Impl, glVertexAttrib3s);
        GLELoadProc(glVertexAttrib3sv_Impl, glVertexAttrib3sv);
        GLELoadProc(glVertexAttrib4Nbv_Impl, glVertexAttrib4Nbv);
        GLELoadProc(glVertexAttrib4Niv_Impl, glVertexAttrib4Niv);
        GLELoadProc(glVertexAttrib4Nsv_Impl, glVertexAttrib4Nsv);
        GLELoadProc(glVertexAttrib4Nub_Impl, glVertexAttrib4Nub);
        GLELoadProc(glVertexAttrib4Nubv_Impl, glVertexAttrib4Nubv);
        GLELoadProc(glVertexAttrib4Nuiv_Impl, glVertexAttrib4Nuiv);
        GLELoadProc(glVertexAttrib4Nusv_Impl, glVertexAttrib4Nusv);
        GLELoadProc(glVertexAttrib4bv_Impl, glVertexAttrib4bv);
        GLELoadProc(glVertexAttrib4d_Impl, glVertexAttrib4d);
        GLELoadProc(glVertexAttrib4dv_Impl, glVertexAttrib4dv);
        GLELoadProc(glVertexAttrib4f_Impl, glVertexAttrib4f);
        GLELoadProc(glVertexAttrib4fv_Impl, glVertexAttrib4fv);
        GLELoadProc(glVertexAttrib4iv_Impl, glVertexAttrib4iv);
        GLELoadProc(glVertexAttrib4s_Impl, glVertexAttrib4s);
        GLELoadProc(glVertexAttrib4sv_Impl, glVertexAttrib4sv);
        GLELoadProc(glVertexAttrib4ubv_Impl, glVertexAttrib4ubv);
        GLELoadProc(glVertexAttrib4uiv_Impl, glVertexAttrib4uiv);
        GLELoadProc(glVertexAttrib4usv_Impl, glVertexAttrib4usv);
        GLELoadProc(glVertexAttribPointer_Impl, glVertexAttribPointer);

        // GL_VERSION_2_1
        GLELoadProc(glUniformMatrix2x3fv_Impl, glUniformMatrix2x3fv);
        GLELoadProc(glUniformMatrix2x4fv_Impl, glUniformMatrix2x4fv);
        GLELoadProc(glUniformMatrix3x2fv_Impl, glUniformMatrix3x2fv);
        GLELoadProc(glUniformMatrix3x4fv_Impl, glUniformMatrix3x4fv);
        GLELoadProc(glUniformMatrix4x2fv_Impl, glUniformMatrix4x2fv);
        GLELoadProc(glUniformMatrix4x3fv_Impl, glUniformMatrix4x3fv);

        // GL_VERSION_3_0
        GLELoadProc(glBeginConditionalRender_Impl, glBeginConditionalRender);
        GLELoadProc(glBeginTransformFeedback_Impl, glBeginTransformFeedback);
        GLELoadProc(glBindFragDataLocation_Impl, glBindFragDataLocation);
        GLELoadProc(glClampColor_Impl, glClampColor);
        GLELoadProc(glClearBufferfi_Impl, glClearBufferfi);
        GLELoadProc(glClearBufferfv_Impl, glClearBufferfv);
        GLELoadProc(glClearBufferiv_Impl, glClearBufferiv);
        GLELoadProc(glClearBufferuiv_Impl, glClearBufferuiv);
        GLELoadProc(glColorMaski_Impl, glColorMaski);
        GLELoadProc(glDisablei_Impl, glDisablei);
        GLELoadProc(glEnablei_Impl, glEnablei);
        GLELoadProc(glEndConditionalRender_Impl, glEndConditionalRender);
        GLELoadProc(glEndTransformFeedback_Impl, glEndTransformFeedback);
        GLELoadProc(glBindBufferRange_Impl, glBindBufferRange);
        GLELoadProc(glBindBufferBase_Impl, glBindBufferBase);
        GLELoadProc(glGetBooleani_v_Impl, glGetBooleani_v);
        GLELoadProc(glGetIntegeri_v_Impl, glGetIntegeri_v);
        GLELoadProc(glGetFragDataLocation_Impl, glGetFragDataLocation);
        GLELoadProc(glGetStringi_Impl, glGetStringi);
        GLELoadProc(glGetTexParameterIiv_Impl, glGetTexParameterIiv);
        GLELoadProc(glGetTexParameterIuiv_Impl, glGetTexParameterIuiv);
        GLELoadProc(glGetTransformFeedbackVarying_Impl, glGetTransformFeedbackVarying);
        GLELoadProc(glGetUniformuiv_Impl, glGetUniformuiv);
        GLELoadProc(glGetVertexAttribIiv_Impl, glGetVertexAttribIiv);
        GLELoadProc(glGetVertexAttribIuiv_Impl, glGetVertexAttribIuiv);
        GLELoadProc(glIsEnabledi_Impl, glIsEnabledi);
        GLELoadProc(glTexParameterIiv_Impl, glTexParameterIiv);
        GLELoadProc(glTexParameterIuiv_Impl, glTexParameterIuiv);
        GLELoadProc(glTransformFeedbackVaryings_Impl, glTransformFeedbackVaryings);
        GLELoadProc(glUniform1ui_Impl, glUniform1ui);
        GLELoadProc(glUniform1uiv_Impl, glUniform1uiv);
        GLELoadProc(glUniform2ui_Impl, glUniform2ui);
        GLELoadProc(glUniform2uiv_Impl, glUniform2uiv);
        GLELoadProc(glUniform3ui_Impl, glUniform3ui);
        GLELoadProc(glUniform3uiv_Impl, glUniform3uiv);
        GLELoadProc(glUniform4ui_Impl, glUniform4ui);
        GLELoadProc(glUniform4uiv_Impl, glUniform4uiv);
        GLELoadProc(glVertexAttribI1i_Impl, glVertexAttribI1i);
        GLELoadProc(glVertexAttribI1iv_Impl, glVertexAttribI1iv);
        GLELoadProc(glVertexAttribI1ui_Impl, glVertexAttribI1ui);
        GLELoadProc(glVertexAttribI1uiv_Impl, glVertexAttribI1uiv);
        GLELoadProc(glVertexAttribI2i_Impl, glVertexAttribI2i);
        GLELoadProc(glVertexAttribI2iv_Impl, glVertexAttribI2iv);
        GLELoadProc(glVertexAttribI2ui_Impl, glVertexAttribI2ui);
        GLELoadProc(glVertexAttribI2uiv_Impl, glVertexAttribI2uiv);
        GLELoadProc(glVertexAttribI3i_Impl, glVertexAttribI3i);
        GLELoadProc(glVertexAttribI3iv_Impl, glVertexAttribI3iv);
        GLELoadProc(glVertexAttribI3ui_Impl, glVertexAttribI3ui);
        GLELoadProc(glVertexAttribI3uiv_Impl, glVertexAttribI3uiv);
        GLELoadProc(glVertexAttribI4bv_Impl, glVertexAttribI4bv);
        GLELoadProc(glVertexAttribI4i_Impl, glVertexAttribI4i);
        GLELoadProc(glVertexAttribI4iv_Impl, glVertexAttribI4iv);
        GLELoadProc(glVertexAttribI4sv_Impl, glVertexAttribI4sv);
        GLELoadProc(glVertexAttribI4ubv_Impl, glVertexAttribI4ubv);
        GLELoadProc(glVertexAttribI4ui_Impl, glVertexAttribI4ui);
        GLELoadProc(glVertexAttribI4uiv_Impl, glVertexAttribI4uiv);
        GLELoadProc(glVertexAttribI4usv_Impl, glVertexAttribI4usv);
        GLELoadProc(glVertexAttribIPointer_Impl, glVertexAttribIPointer);

        // GL_VERSION_3_1
        GLELoadProc(glDrawArraysInstanced_Impl, glDrawArraysInstanced);
        GLELoadProc(glDrawElementsInstanced_Impl, glDrawElementsInstanced);
        GLELoadProc(glPrimitiveRestartIndex_Impl, glPrimitiveRestartIndex);
        GLELoadProc(glTexBuffer_Impl, glTexBuffer);

        // GL_VERSION_3_2
        GLELoadProc(glFramebufferTexture_Impl, glFramebufferTexture);
        GLELoadProc(glGetBufferParameteri64v_Impl, glGetBufferParameteri64v);
        GLELoadProc(glGetInteger64i_v_Impl, glGetInteger64i_v);

        // GL_VERSION_3_3
        GLELoadProc(glVertexAttribDivisor_Impl, glVertexAttribDivisor);

        // GL_VERSION_4_0
        GLELoadProc(glBlendEquationSeparatei_Impl, glBlendEquationSeparatei);
        GLELoadProc(glBlendEquationi_Impl, glBlendEquationi);
        GLELoadProc(glBlendFuncSeparatei_Impl, glBlendFuncSeparatei);
        GLELoadProc(glBlendFunci_Impl, glBlendFunci);
        GLELoadProc(glMinSampleShading_Impl, glMinSampleShading);

        // GL_AMD_debug_output
        GLELoadProc(glDebugMessageCallbackAMD_Impl, glDebugMessageCallbackAMD);
        GLELoadProc(glDebugMessageEnableAMD_Impl, glDebugMessageEnableAMD);
        GLELoadProc(glDebugMessageInsertAMD_Impl, glDebugMessageInsertAMD);
        GLELoadProc(glGetDebugMessageLogAMD_Impl, glGetDebugMessageLogAMD);

      #if defined(GLE_APPLE_ENABLED)
        // GL_APPLE_element_array
        GLELoadProc(glDrawElementArrayAPPLE_Impl, glDrawElementArrayAPPLE);
        GLELoadProc(glDrawRangeElementArrayAPPLE_Impl, glDrawRangeElementArrayAPPLE);
        GLELoadProc(glElementPointerAPPLE_Impl, glElementPointerAPPLE);
        GLELoadProc(glMultiDrawElementArrayAPPLE_Impl, glMultiDrawElementArrayAPPLE);
        GLELoadProc(glMultiDrawRangeElementArrayAPPLE_Impl, glMultiDrawRangeElementArrayAPPLE);

        // GL_APPLE_fence
        GLELoadProc(glDeleteFencesAPPLE_Impl, glDeleteFencesAPPLE);
        GLELoadProc(glFinishFenceAPPLE_Impl, glFinishFenceAPPLE);
        GLELoadProc(glFinishObjectAPPLE_Impl, glFinishObjectAPPLE);
        GLELoadProc(glGenFencesAPPLE_Impl, glGenFencesAPPLE);
        GLELoadProc(glIsFenceAPPLE_Impl, glIsFenceAPPLE);
        GLELoadProc(glSetFenceAPPLE_Impl, glSetFenceAPPLE);
        GLELoadProc(glTestFenceAPPLE_Impl, glTestFenceAPPLE);
        GLELoadProc(glTestObjectAPPLE_Impl, glTestObjectAPPLE);

        // GL_APPLE_flush_buffer_range
        GLELoadProc(glBufferParameteriAPPLE_Impl, glMultiDrawRangeElementArrayAPPLE);
        GLELoadProc(glFlushMappedBufferRangeAPPLE_Impl, glFlushMappedBufferRangeAPPLE);

        // GL_APPLE_object_purgeable
        GLELoadProc(glGetObjectParameterivAPPLE_Impl, glGetObjectParameterivAPPLE);
        GLELoadProc(glObjectPurgeableAPPLE_Impl, glObjectPurgeableAPPLE);
        GLELoadProc(glObjectUnpurgeableAPPLE_Impl, glObjectUnpurgeableAPPLE);

        // GL_APPLE_texture_range
        GLELoadProc(glGetTexParameterPointervAPPLE_Impl, glGetTexParameterPointervAPPLE);
        GLELoadProc(glTextureRangeAPPLE_Impl, glTextureRangeAPPLE);

        // GL_APPLE_vertex_array_object
        GLELoadProc(glBindVertexArrayAPPLE_Impl, glBindVertexArrayAPPLE);
        GLELoadProc(glDeleteVertexArraysAPPLE_Impl, glDeleteVertexArraysAPPLE);
        GLELoadProc(glGenVertexArraysAPPLE_Impl, glGenVertexArraysAPPLE);
        GLELoadProc(glIsVertexArrayAPPLE_Impl, glIsVertexArrayAPPLE);

        // GL_APPLE_vertex_array_range
        GLELoadProc(glFlushVertexArrayRangeAPPLE_Impl, glFlushVertexArrayRangeAPPLE);
        GLELoadProc(glVertexArrayParameteriAPPLE_Impl, glVertexArrayParameteriAPPLE);
        GLELoadProc(glVertexArrayRangeAPPLE_Impl, glVertexArrayRangeAPPLE);

        // GL_APPLE_vertex_program_evaluators
        GLELoadProc(glDisableVertexAttribAPPLE_Impl, glDisableVertexAttribAPPLE);
        GLELoadProc(glEnableVertexAttribAPPLE_Impl, glEnableVertexAttribAPPLE);
        GLELoadProc(glIsVertexAttribEnabledAPPLE_Impl, glIsVertexAttribEnabledAPPLE);
        GLELoadProc(glMapVertexAttrib1dAPPLE_Impl, glMapVertexAttrib1dAPPLE);
        GLELoadProc(glMapVertexAttrib1fAPPLE_Impl, glMapVertexAttrib1fAPPLE);
        GLELoadProc(glMapVertexAttrib2dAPPLE_Impl, glMapVertexAttrib2dAPPLE);
        GLELoadProc(glMapVertexAttrib2fAPPLE_Impl, glMapVertexAttrib2fAPPLE);
        
      #endif // GLE_APPLE_ENABLED
      
        // GL_ARB_debug_output
        GLELoadProc(glDebugMessageCallbackARB_Impl, glDebugMessageCallbackARB);
        GLELoadProc(glDebugMessageControlARB_Impl, glDebugMessageControlARB);
        GLELoadProc(glDebugMessageInsertARB_Impl, glDebugMessageInsertARB);
        GLELoadProc(glGetDebugMessageLogARB_Impl, glGetDebugMessageLogARB);
        
        // GL_ARB_ES2_compatibility
        GLELoadProc(glClearDepthf_Impl, glClearDepthf);
        GLELoadProc(glDepthRangef_Impl, glDepthRangef);
        GLELoadProc(glGetShaderPrecisionFormat_Impl, glGetShaderPrecisionFormat);
        GLELoadProc(glReleaseShaderCompiler_Impl, glReleaseShaderCompiler);
        GLELoadProc(glShaderBinary_Impl, glShaderBinary);

        // GL_ARB_framebuffer_object
        GLELoadProc(glBindFramebuffer_Impl, glBindFramebuffer);
        GLELoadProc(glBindRenderbuffer_Impl, glBindRenderbuffer);
        GLELoadProc(glBlitFramebuffer_Impl, glBlitFramebuffer);
        GLELoadProc(glCheckFramebufferStatus_Impl, glCheckFramebufferStatus);
        GLELoadProc(glDeleteFramebuffers_Impl, glDeleteFramebuffers);
        GLELoadProc(glDeleteRenderbuffers_Impl, glDeleteRenderbuffers);
        GLELoadProc(glFramebufferRenderbuffer_Impl, glFramebufferRenderbuffer);
        GLELoadProc(glFramebufferTexture1D_Impl, glFramebufferTexture1D);
        GLELoadProc(glFramebufferTexture2D_Impl, glFramebufferTexture2D);
        GLELoadProc(glFramebufferTexture3D_Impl, glFramebufferTexture3D);
        GLELoadProc(glFramebufferTextureLayer_Impl, glFramebufferTextureLayer);
        GLELoadProc(glGenFramebuffers_Impl, glGenFramebuffers);
        GLELoadProc(glGenRenderbuffers_Impl, glGenRenderbuffers);
        GLELoadProc(glGenerateMipmap_Impl, glGenerateMipmap);
        GLELoadProc(glGetFramebufferAttachmentParameteriv_Impl, glGetFramebufferAttachmentParameteriv);
        GLELoadProc(glGetRenderbufferParameteriv_Impl, glGetRenderbufferParameteriv);
        GLELoadProc(glIsFramebuffer_Impl, glIsFramebuffer);
        GLELoadProc(glIsRenderbuffer_Impl, glIsRenderbuffer);
        GLELoadProc(glRenderbufferStorage_Impl, glRenderbufferStorage);
        GLELoadProc(glRenderbufferStorageMultisample_Impl, glRenderbufferStorageMultisample);
        
        // GL_ARB_texture_multisample
        GLELoadProc(glGetMultisamplefv_Impl, glGetMultisamplefv);
        GLELoadProc(glSampleMaski_Impl, glSampleMaski);
        GLELoadProc(glTexImage2DMultisample_Impl, glTexImage2DMultisample);
        GLELoadProc(glTexImage3DMultisample_Impl, glTexImage3DMultisample);

        // GL_ARB_timer_query
        GLELoadProc(glGetQueryObjecti64v_Impl, glGetQueryObjecti64v);
        GLELoadProc(glGetQueryObjectui64v_Impl, glGetQueryObjectui64v);
        GLELoadProc(glQueryCounter_Impl, glQueryCounter);

        // GL_ARB_vertex_array_object
        GLELoadProc(glBindVertexArray_Impl, glBindVertexArray);
        GLELoadProc(glDeleteVertexArrays_Impl, glDeleteVertexArrays);
        GLELoadProc(glGenVertexArrays_Impl, glGenVertexArrays);
        GLELoadProc(glIsVertexArray_Impl, glIsVertexArray);

        #if defined(GLE_APPLE_ENABLED)
            if(WholeVersion < 302) // It turns out that Apple OpenGL versions prior to 3.2 have glBindVertexArray, etc. but they silently fail by default. So always use the APPLE version.
            {
                glBindVertexArray_Impl    = glBindVertexArrayAPPLE_Impl;
                glDeleteVertexArrays_Impl = glDeleteVertexArraysAPPLE_Impl;
                glGenVertexArrays_Impl    = (OVRTypeof(glGenVertexArrays_Impl)) glGenVertexArraysAPPLE_Impl; // There is a const cast of the arrays argument here due to a slight difference in the Apple behavior. For our purposes it should be OK.
                glIsVertexArray_Impl      = glIsVertexArrayAPPLE_Impl;
                
                if(glBindVertexArray_Impl)
                    gl_ARB_vertex_array_object = true; // We are routing the APPLE version through our version, with the assumption that we use the ARB version the same as we would use the APPLE version.
            }
        #endif
        
        // GL_EXT_draw_buffers2
        GLELoadProc(glColorMaskIndexedEXT_Impl, glColorMaskIndexedEXT);
        GLELoadProc(glDisableIndexedEXT_Impl, glDisableIndexedEXT);
        GLELoadProc(glEnableIndexedEXT_Impl, glEnableIndexedEXT);
        GLELoadProc(glGetBooleanIndexedvEXT_Impl, glGetBooleanIndexedvEXT);
        GLELoadProc(glGetIntegerIndexedvEXT_Impl, glGetIntegerIndexedvEXT);
        GLELoadProc(glIsEnabledIndexedEXT_Impl, glIsEnabledIndexedEXT);

        // GL_KHR_debug
        GLELoadProc(glDebugMessageCallback_Impl, glDebugMessageCallback);
        GLELoadProc(glDebugMessageControl_Impl, glDebugMessageControl);
        GLELoadProc(glDebugMessageInsert_Impl, glDebugMessageInsert);
        GLELoadProc(glGetDebugMessageLog_Impl, glGetDebugMessageLog);
        GLELoadProc(glGetObjectLabel_Impl, glGetObjectLabel);
        GLELoadProc(glGetObjectPtrLabel_Impl, glGetObjectPtrLabel);
        GLELoadProc(glObjectLabel_Impl, glObjectLabel);
        GLELoadProc(glObjectPtrLabel_Impl, glObjectPtrLabel);
        GLELoadProc(glPopDebugGroup_Impl, glPopDebugGroup);
        GLELoadProc(glPushDebugGroup_Impl, glPushDebugGroup);

        // GL_WIN_swap_hint
        GLELoadProc(glAddSwapHintRectWIN_Impl, glAddSwapHintRectWIN);

      #if defined(GLE_WINDOWS_ENABLED)
        // WGL
        // We don't load these as function pointers but rather statically link to them.

        GLELoadProc(wglCreateBufferRegionARB_Impl, wglCreateBufferRegionARB);
        GLELoadProc(wglDeleteBufferRegionARB_Impl, wglDeleteBufferRegionARB);
        GLELoadProc(wglSaveBufferRegionARB_Impl, wglSaveBufferRegionARB);
        GLELoadProc(wglRestoreBufferRegionARB_Impl, wglRestoreBufferRegionARB);

        // WGL_ARB_extensions_string
        GLELoadProc(wglGetExtensionsStringARB_Impl, wglGetExtensionsStringARB);

        // WGL_ARB_pixel_format
        GLELoadProc(wglGetPixelFormatAttribivARB_Impl, wglGetPixelFormatAttribivARB);
        GLELoadProc(wglGetPixelFormatAttribfvARB_Impl, wglGetPixelFormatAttribfvARB);
        GLELoadProc(wglChoosePixelFormatARB_Impl, wglChoosePixelFormatARB);

        // WGL_ARB_make_current_read
        GLELoadProc(wglMakeContextCurrentARB_Impl, wglMakeContextCurrentARB);
        GLELoadProc(wglGetCurrentReadDCARB_Impl, wglGetCurrentReadDCARB);

        // WGL_ARB_pbuffer
        GLELoadProc(wglCreatePbufferARB_Impl, wglCreatePbufferARB);
        GLELoadProc(wglGetPbufferDCARB_Impl, wglGetPbufferDCARB);
        GLELoadProc(wglReleasePbufferDCARB_Impl, wglReleasePbufferDCARB);
        GLELoadProc(wglDestroyPbufferARB_Impl, wglDestroyPbufferARB);
        GLELoadProc(wglQueryPbufferARB_Impl, wglQueryPbufferARB);

        // WGL_ARB_render_texture
        GLELoadProc(wglBindTexImageARB_Impl, wglBindTexImageARB);
        GLELoadProc(wglReleaseTexImageARB_Impl, wglReleaseTexImageARB);
        GLELoadProc(wglSetPbufferAttribARB_Impl, wglSetPbufferAttribARB);

        // WGL_NV_present_video
        GLELoadProc(wglEnumerateVideoDevicesNV_Impl, wglEnumerateVideoDevicesNV);
        GLELoadProc(wglBindVideoDeviceNV_Impl, wglBindVideoDeviceNV);
        GLELoadProc(wglQueryCurrentContextNV_Impl, wglQueryCurrentContextNV);

        // WGL_ARB_create_context
        GLELoadProc(wglCreateContextAttribsARB_Impl, wglCreateContextAttribsARB);

        // WGL_EXT_extensions_string
        GLELoadProc(wglGetExtensionsStringEXT_Impl, wglGetExtensionsStringEXT);

        // WGL_EXT_swap_control
        GLELoadProc(wglGetSwapIntervalEXT_Impl, wglGetSwapIntervalEXT);
        GLELoadProc(wglSwapIntervalEXT_Impl, wglSwapIntervalEXT);
        
        // WGL_OML_sync_control
        GLELoadProc(wglGetSyncValuesOML_Impl, wglGetSyncValuesOML);
        GLELoadProc(wglGetMscRateOML_Impl, wglGetMscRateOML);
        GLELoadProc(wglSwapBuffersMscOML_Impl, wglSwapBuffersMscOML);
        GLELoadProc(wglSwapLayerBuffersMscOML_Impl, wglSwapLayerBuffersMscOML);
        GLELoadProc(wglWaitForMscOML_Impl, wglWaitForMscOML);
        GLELoadProc(wglWaitForSbcOML_Impl, wglWaitForSbcOML);

        // WGL_NV_video_output
        GLELoadProc(wglGetVideoDeviceNV_Impl, wglGetVideoDeviceNV);
        GLELoadProc(wglReleaseVideoDeviceNV_Impl, wglReleaseVideoDeviceNV);
        GLELoadProc(wglBindVideoImageNV_Impl, wglBindVideoImageNV);
        GLELoadProc(wglReleaseVideoImageNV_Impl, wglReleaseVideoImageNV);
        GLELoadProc(wglSendPbufferToVideoNV_Impl, wglSendPbufferToVideoNV);
        GLELoadProc(wglGetVideoInfoNV_Impl, wglGetVideoInfoNV);

        // WGL_NV_swap_group
        GLELoadProc(wglJoinSwapGroupNV_Impl, wglJoinSwapGroupNV);
        GLELoadProc(wglBindSwapBarrierNV_Impl, wglBindSwapBarrierNV);
        GLELoadProc(wglQuerySwapGroupNV_Impl, wglQuerySwapGroupNV);
        GLELoadProc(wglQueryMaxSwapGroupsNV_Impl, wglQueryMaxSwapGroupsNV);
        GLELoadProc(wglQueryFrameCountNV_Impl, wglQueryFrameCountNV);
        GLELoadProc(wglResetFrameCountNV_Impl, wglResetFrameCountNV);

        // WGL_NV_video_capture
        GLELoadProc(wglBindVideoCaptureDeviceNV_Impl, wglBindVideoCaptureDeviceNV);
        GLELoadProc(wglEnumerateVideoCaptureDevicesNV_Impl, wglEnumerateVideoCaptureDevicesNV);
        GLELoadProc(wglLockVideoCaptureDeviceNV_Impl, wglLockVideoCaptureDeviceNV);
        GLELoadProc(wglQueryVideoCaptureDeviceNV_Impl, wglQueryVideoCaptureDeviceNV);
        GLELoadProc(wglReleaseVideoCaptureDeviceNV_Impl, wglReleaseVideoCaptureDeviceNV);

        // WGL_NV_copy_image
        GLELoadProc(wglCopyImageSubDataNV_Impl, wglCopyImageSubDataNV);

        // WGL_NV_DX_interop
        GLELoadProc(wglDXCloseDeviceNV_Impl, wglDXCloseDeviceNV);
        GLELoadProc(wglDXLockObjectsNV_Impl, wglDXLockObjectsNV);
        GLELoadProc(wglDXObjectAccessNV_Impl, wglDXObjectAccessNV);
        GLELoadProc(wglDXOpenDeviceNV_Impl, wglDXOpenDeviceNV);
        GLELoadProc(wglDXRegisterObjectNV_Impl, wglDXRegisterObjectNV);
        GLELoadProc(wglDXSetResourceShareHandleNV_Impl, wglDXSetResourceShareHandleNV);
        GLELoadProc(wglDXUnlockObjectsNV_Impl, wglDXUnlockObjectsNV);
        GLELoadProc(wglDXUnregisterObjectNV_Impl, wglDXUnregisterObjectNV);
      #endif
      
      #if defined(GLE_UNIX_ENABLED)
        // GLX_VERSION_1_1
        // We don't create any pointers_Impl, because we assume these functions are always present.
        
        // GLX_VERSION_1_2
        GLELoadProc(glXGetCurrentDisplay_Impl, glXGetCurrentDisplay);

        // GLX_VERSION_1_3
        GLELoadProc(glXChooseFBConfig_Impl, glXChooseFBConfig);
        GLELoadProc(glXCreateNewContext_Impl, glXCreateNewContext);
        GLELoadProc(glXCreatePbuffer_Impl, glXCreatePbuffer);
        GLELoadProc(glXCreatePixmap_Impl, glXCreatePixmap);
        GLELoadProc(glXCreateWindow_Impl, glXCreateWindow);
        GLELoadProc(glXDestroyPbuffer_Impl, glXDestroyPbuffer);
        GLELoadProc(glXDestroyPixmap_Impl, glXDestroyPixmap);
        GLELoadProc(glXDestroyWindow_Impl, glXDestroyWindow);
        GLELoadProc(glXGetCurrentReadDrawable_Impl, glXGetCurrentReadDrawable);
        GLELoadProc(glXGetFBConfigAttrib_Impl, glXGetFBConfigAttrib);
        GLELoadProc(glXGetFBConfigs_Impl, glXGetFBConfigs);
        GLELoadProc(glXGetSelectedEvent_Impl, glXGetSelectedEvent);
        GLELoadProc(glXGetVisualFromFBConfig_Impl, glXGetVisualFromFBConfig);
        GLELoadProc(glXMakeContextCurrent_Impl, glXMakeContextCurrent);
        GLELoadProc(glXQueryContext_Impl, glXQueryContext);
        GLELoadProc(glXQueryDrawable_Impl, glXQueryDrawable);
        GLELoadProc(glXSelectEvent_Impl, glXSelectEvent);

        // GLX_VERSION_1_4
        // Nothing to declare
        
        // GLX_EXT_swap_control
        GLELoadProc(glXSwapIntervalEXT_Impl, glXSwapIntervalEXT);
 
        // GLX_OML_sync_control
        GLELoadProc(glXGetMscRateOML_Impl, glXGetMscRateOML);
        GLELoadProc(glXGetSyncValuesOML_Impl, glXGetSyncValuesOML);
        GLELoadProc(glXGetSyncValuesOML_Impl, glXSwapBuffersMscOML);
        GLELoadProc(glXSwapBuffersMscOML_Impl, glXSwapBuffersMscOML);
        GLELoadProc(glXWaitForSbcOML_Impl, glXWaitForSbcOML);
     #endif
    }
    


    OVR_DISABLE_MSVC_WARNING(4510 4512 4610) // default constructor could not be generated,
    struct ValueStringPair
    {
        bool& IsPresent;
        const char* ExtensionName;
    };


    // Helper function for InitExtensionSupport.
    static void CheckExtensions(ValueStringPair* pValueStringPairArray, size_t arrayCount, const char* extensions)
    {
        // We search the extesion list string for each of the individual extensions we are interested in. 
        // We do this by walking over the string and comparing each entry in turn to our array of entries of interest.
        // Example string (with patholigical extra spaces): "   ext1 ext2   ext3  "
           
        char extension[64];
        const char* p = extensions; // p points to the beginning of the current word
        const char* pEnd;           // pEnd points to one-past the last character of the current word. It is where the trailing '\0' of the string would be.
           
        while(*p)
        {
            while(*p == ' ') // Find the next word begin.
                ++p;
                
            pEnd = p;
               
            while((*pEnd != '\0') && (*pEnd != ' ')) // Find the next word end.
                ++pEnd;
               
            if(((pEnd - p) > 0) && ((size_t)(pEnd - p) < OVR_ARRAY_COUNT(extension)))
            {
                memcpy(extension, p, pEnd - p); // To consider: Revise this code to directly read from p/pEnd instead of doing a memcpy.
                extension[pEnd - p] = '\0';
                   
                for(size_t i = 0; i < arrayCount; i++) // For each extension we are interested in...
                {
                    ValueStringPair& vsp = pValueStringPairArray[i];
 
                    if(strcmp(extension, vsp.ExtensionName) == 0) // case-sensitive compare
                        pValueStringPairArray[i].IsPresent = true;
                }
            }
               
            p = pEnd;
        }
    }


    void OVR::GLEContext::InitExtensionSupport()
    {
        // It may be better in the long run to use a member STL map<const char*, bool>.
        // It would make this loading code cleaner, though it would make lookups slower.

        ValueStringPair vspArray[] =
        {
            { gl_AMD_debug_output, "GL_AMD_debug_output" },
          #if defined(GLE_APPLE_ENABLED)
            { gl_APPLE_aux_depth_stencil, "GL_APPLE_aux_depth_stencil" },
            { gl_APPLE_client_storage, "GL_APPLE_client_storage" },
            { gl_APPLE_element_array, "GL_APPLE_element_array" },
            { gl_APPLE_fence, "GL_APPLE_fence" },
            { gl_APPLE_float_pixels, "GL_APPLE_float_pixels" },
            { gl_APPLE_flush_buffer_range, "GL_APPLE_flush_buffer_range" },
            { gl_APPLE_object_purgeable, "GL_APPLE_object_purgeable" },
            { gl_APPLE_pixel_buffer, "GL_APPLE_pixel_buffer" },
            { gl_APPLE_rgb_422, "GL_APPLE_rgb_422" },
            { gl_APPLE_row_bytes, "GL_APPLE_row_bytes" },
            { gl_APPLE_specular_vector, "GL_APPLE_specular_vector" },
            { gl_APPLE_texture_range, "GL_APPLE_texture_range" },
            { gl_APPLE_transform_hint, "GL_APPLE_transform_hint" },
            { gl_APPLE_vertex_array_object, "GL_APPLE_vertex_array_object" },
            { gl_APPLE_vertex_array_range, "GL_APPLE_vertex_array_range" },
            { gl_APPLE_vertex_program_evaluators, "GL_APPLE_vertex_program_evaluators" },
            { gl_APPLE_ycbcr_422, "GL_APPLE_ycbcr_422" },
          #endif
            { gl_ARB_debug_output, "GL_ARB_debug_output" },
            { gl_ARB_ES2_compatibility, "GL_ARB_ES2_compatibility" },
            { gl_ARB_framebuffer_object, "GL_ARB_framebuffer_object" },
            { gl_ARB_framebuffer_sRGB, "GL_ARB_framebuffer_sRGB" },
            { gl_ARB_texture_multisample, "GL_ARB_texture_multisample" },
            { gl_ARB_texture_non_power_of_two, "GL_ARB_texture_non_power_of_two" },
            { gl_ARB_timer_query, "GL_ARB_timer_query" },
            { gl_ARB_vertex_array_object, "GL_ARB_vertex_array_object" },
            { gl_EXT_draw_buffers2, "GL_EXT_draw_buffers2" },
            { gl_EXT_texture_filter_anisotropic, "GL_EXT_texture_filter_anisotropic" },
            { gl_KHR_debug, "GL_KHR_debug" },
            { gl_WIN_swap_hint, "GL_WIN_swap_hint" }
            // Windows WGL, Unix GLX, and Apple CGL extensions are handled below, as they require different calls from glGetString(GL_EXTENSIONS).
        };

        // We cannot use glGetString(GL_EXTENSIONS) when an OpenGL core profile is active,
        // as it's deprecated in favor of using OpenGL 3+ glGetStringi.
        const char* extensions = (MajorVersion < 3) ? (const char*)glGetString(GL_EXTENSIONS) : "";
       
        if (extensions && *extensions) // If we have a space-delimited extension string to search for individual extensions...
        {
            OVR_DEBUG_LOG(("GL_EXTENSIONS: %s", (const char*)extensions));
            CheckExtensions(vspArray, OVR_ARRAY_COUNT(vspArray), extensions); // Call our shared helper function for this.
        }
        else
        {
            if(MajorVersion >= 3) // If glGetIntegerv(GL_NUM_EXTENSIONS, ...) is supported...
            {
                // In this case we need to match an array of individual extensions against an array of
                // externsions provided by glGetStringi. This is an O(n^2) operation, but at least we
                // are doing this only once on startup. There are a few tricks we can employ to speed
                // up the logic below, but they may not be worth much.
               
                GLint extensionCount = 0;
                glGetIntegerv(GL_NUM_EXTENSIONS, &extensionCount);
                GLenum err = glGetError();
               
                if(err == 0)
                {
                    #ifdef OVR_BUILD_DEBUG
                    OVR::StringBuffer extensionsStr;
                    #endif
 
                    for(GLint e = 0; e != extensionCount; ++e) // For each extension supported...
                    {
                        const char* extension = (const char*)glGetStringi(GL_EXTENSIONS, (GLuint)e);
 
                        if(extension) // glGetStringi returns NULL upon error.
                        {
                            #ifdef OVR_BUILD_DEBUG
                                extensionsStr.AppendFormat(" %s", extension);
                            #endif
 
                            for(size_t i = 0; i < OVR_ARRAY_COUNT(vspArray); i++) // For each extension we are interested in...
                            {
                                ValueStringPair& vsp = vspArray[i];
 
                                if(strcmp(extension, vsp.ExtensionName) == 0) // case-sensitive compare
                                    vspArray[i].IsPresent = true;
                            }
                        }
                        else
                            break;
                    }
 
                    OVR_DEBUG_LOG(("GL_EXTENSIONS: %s", extensionsStr.ToCStr()));
                }
            }
            // Else we have a problem: no means to read the extensions was successful.
        }

      #if defined(GLE_WINDOWS_ENABLED)
        // We need to use wglGetExtensionsStringARB or wglGetExtensionsStringEXT as opposed to above with glGetString(GL_EXTENSIONS).
        ValueStringPair vspWGLArray[] =
        {
            { gl_WGL_ARB_buffer_region, "WGL_ARB_buffer_region" }
           ,{ gl_WGL_ARB_extensions_string, "WGL_ARB_extensions_string" }
           ,{ gl_WGL_ARB_pixel_format, "WGL_ARB_pixel_format" }
           ,{ gl_WGL_ARB_make_current_read, "WGL_ARB_make_current_read" }
           ,{ gl_WGL_ARB_pbuffer, "WGL_ARB_pbuffer" }
           ,{ gl_WGL_ARB_render_texture, "WGL_ARB_render_texture" }
           ,{ gl_WGL_TYPE_RGBA_FLOAT_ARB, "WGL_TYPE_RGBA_FLOAT_ARB" }
           ,{ gl_WGL_ARB_framebuffer_sRGB, "WGL_ARB_framebuffer_sRGB" }
           ,{ gl_WGL_NV_present_video, "WGL_NV_present_video" }
           ,{ gl_WGL_ARB_create_context, "WGL_ARB_create_context" }
           ,{ gl_WGL_ARB_create_context_profile, "WGL_ARB_create_context_profile" }
           ,{ gl_WGL_ARB_create_context_robustness, "WGL_ARB_create_context_robustness" }
           ,{ gl_WGL_EXT_extensions_string, "WGL_EXT_extensions_string" }
           ,{ gl_WGL_EXT_swap_control, "WGL_EXT_swap_control" }
           ,{ gl_WGL_OML_sync_control, "WGL_OML_sync_control" }
           ,{ gl_WGL_EXT_framebuffer_sRGB, "WGL_EXT_framebuffer_sRGB" }
           ,{ gl_WGL_NV_video_output, "WGL_NV_video_output" }
           ,{ gl_WGL_NV_swap_group, "WGL_NV_swap_group" }
           ,{ gl_WGL_NV_video_capture, "WGL_NV_video_capture" }
           ,{ gl_WGL_NV_copy_image, "WGL_NV_copy_image" }
           ,{ gl_WGL_NV_DX_interop, "WGL_NV_DX_interop" }
        };

        if(wglGetExtensionsStringARB_Impl)
            extensions = wglGetExtensionsStringARB_Impl(wglGetCurrentDC());
        else if(wglGetExtensionsStringEXT_Impl)
            extensions = wglGetExtensionsStringEXT_Impl();

        if (extensions && *extensions)
        {
            OVR_DEBUG_LOG(("WGL_EXTENSIONS: %s", (const char*)extensions));
            CheckExtensions(vspWGLArray, OVR_ARRAY_COUNT(vspWGLArray), extensions);
        }
      #elif defined(GLE_UNIX_ENABLED)
        ValueStringPair vspGLXArray[] =
        {
            { gl_GLX_EXT_swap_control, "GLX_EXT_swap_control" }
           ,{ gl_GLX_OML_sync_control, "GLX_OML_sync_control" }
        };

        extensions = glXGetClientString(glXGetCurrentDisplay(), GLX_EXTENSIONS); // We could alternatively use glXQueryExtensionsString(glXGetCurrentDisplay(), 0)

        if (extensions && *extensions)
        {
            OVR_DEBUG_LOG(("GLX_EXTENSIONS: %s", (const char*)extensions));
            CheckExtensions(vspGLXArray, OVR_ARRAY_COUNT(vspGLXArray), extensions);
        }
      #elif defined(GLE_APPLE_ENABLED)
        // We don't currently use this.
      #endif

    } // GLEContext::InitExtensionSupport()
        

    #if defined(GLE_HOOKING_ENABLED)

        #undef glGetError
        extern "C" { GLAPI GLenum GLAPIENTRY glGetError(); }
        void OVR::GLEContext::PostHook()
        {
            int err = glGetError();
            if(err != 0)
            {
                OVR_DEBUG_LOG(("GL Error: %d", err));
            }
        }


        // OpenGL 1.1 link-based functions
        #undef glAccum // Undefine the macro from our header so that we can directly call the real version of this function.
        extern "C" { GLAPI void GLAPIENTRY glAccum(GLenum op, GLfloat value); }
        void OVR::GLEContext::glAccum_Hook(GLenum op, GLfloat value)
        {
            glAccum(op, value);
            PostHook();
        }

        #undef glAlphaFunc
        extern "C" { GLAPI void GLAPIENTRY glAlphaFunc(GLenum func, GLclampf ref); }
        void OVR::GLEContext::glAlphaFunc_Hook(GLenum func, GLclampf ref)
        {
            glAlphaFunc(func, ref);
            PostHook();
        }

        #undef glAreTexturesResident
        extern "C" { GLAPI GLboolean GLAPIENTRY glAreTexturesResident(GLsizei n, const GLuint *textures, GLboolean *residences); }
        GLboolean OVR::GLEContext::glAreTexturesResident_Hook(GLsizei n, const GLuint *textures, GLboolean *residences)
        {
            GLboolean b = glAreTexturesResident(n, textures, residences);
            PostHook();
            return b;
        }

        #undef glArrayElement
        extern "C" { GLAPI void GLAPIENTRY glArrayElement(GLint i); }
        void OVR::GLEContext::glArrayElement_Hook(GLint i)
        {
            glArrayElement(i);
            PostHook();
        }

        #undef glBegin
        extern "C" { GLAPI void GLAPIENTRY glBegin(GLenum mode); }
        void OVR::GLEContext::glBegin_Hook(GLenum mode)
        {
            glBegin(mode);
            PostHook();
        }

        #undef glBindTexture
        extern "C" { GLAPI void GLAPIENTRY glBindTexture(GLenum target, GLuint texture); }
        void OVR::GLEContext::glBindTexture_Hook(GLenum target, GLuint texture)
        {
            glBindTexture(target, texture);
            PostHook();
        }

        #undef glBitmap
        extern "C" { GLAPI void GLAPIENTRY glBitmap(GLsizei width, GLsizei height, GLfloat xorig, GLfloat yorig, GLfloat xmove, GLfloat ymove, const GLubyte *bitmap); }
        void OVR::GLEContext::glBitmap_Hook(GLsizei width, GLsizei height, GLfloat xorig, GLfloat yorig, GLfloat xmove, GLfloat ymove, const GLubyte *bitmap)
        {
            glBitmap(width, height, xorig, yorig, xmove, ymove, bitmap);
            PostHook();
        }

        #undef glBlendFunc
        extern "C" { GLAPI void GLAPIENTRY glBlendFunc(GLenum sfactor, GLenum dfactor); }
        void OVR::GLEContext::glBlendFunc_Hook(GLenum sfactor, GLenum dfactor)
        {
            glBlendFunc(sfactor, dfactor);
            PostHook();
        }

        #undef glCallList
        extern "C" { GLAPI void GLAPIENTRY glCallList(GLuint list); }
        void OVR::GLEContext::glCallList_Hook(GLuint list)
        {
            glCallList(list);
            PostHook();
        }

        #undef glCallLists
        extern "C" { GLAPI void GLAPIENTRY glCallLists(GLsizei n, GLenum type, const void *lists); }
        void OVR::GLEContext::glCallLists_Hook(GLsizei n, GLenum type, const void *lists)
        {
            glCallLists(n, type, lists);
            PostHook();
        }

        #undef glClear
        extern "C" { GLAPI void GLAPIENTRY glClear(GLbitfield mask); }
        void OVR::GLEContext::glClear_Hook(GLbitfield mask)
        {
            glClear(mask);
            PostHook();
        }

        #undef glClearAccum
        extern "C" { GLAPI void GLAPIENTRY glClearAccum(GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha); }
        void OVR::GLEContext::glClearAccum_Hook(GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha)
        {
            glClearAccum(red, green, blue, alpha);
            PostHook();
        }

        #undef glClearColor
        extern "C" { GLAPI void GLAPIENTRY glClearColor(GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha); }
        void OVR::GLEContext::glClearColor_Hook(GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha)
        {
            glClearColor(red, green, blue, alpha);
            PostHook();
        }

        #undef glClearDepth
        extern "C" { GLAPI void GLAPIENTRY glClearDepth(GLclampd depth); }
        void OVR::GLEContext::glClearDepth_Hook(GLclampd depth)
        {
            glClearDepth(depth);
            PostHook();
        }

        #undef glClearIndex
        extern "C" { GLAPI void GLAPIENTRY glClearIndex(GLfloat c); }
        void OVR::GLEContext::glClearIndex_Hook(GLfloat c)
        {
            glClearIndex(c);
            PostHook();
        }

        #undef glClearStencil
        extern "C" { GLAPI void GLAPIENTRY glClearStencil(GLint s); }
        void OVR::GLEContext::glClearStencil_Hook(GLint s)
        {
            glClearStencil(s);
            PostHook();
        }

        #undef glClipPlane
        extern "C" { GLAPI void GLAPIENTRY glClipPlane(GLenum plane, const GLdouble *equation); }
        void OVR::GLEContext::glClipPlane_Hook(GLenum plane, const GLdouble *equation)
        {
            glClipPlane(plane, equation);
            PostHook();
        }

        #undef glColor3b
        extern "C" { GLAPI void GLAPIENTRY glColor3b(GLbyte red, GLbyte green, GLbyte blue); }
        void OVR::GLEContext::glColor3b_Hook(GLbyte red, GLbyte green, GLbyte blue)
        {
            glColor3b(red, green, blue);
            PostHook();
        }

        #undef glColor3bv
        extern "C" { GLAPI void GLAPIENTRY glColor3bv(const GLbyte *v); }
        void OVR::GLEContext::glColor3bv_Hook(const GLbyte *v)
        {
            glColor3bv(v);
            PostHook();
        }

        #undef glColor3d
        extern "C" { GLAPI void GLAPIENTRY glColor3d(GLdouble red, GLdouble green, GLdouble blue); }
        void OVR::GLEContext::glColor3d_Hook(GLdouble red, GLdouble green, GLdouble blue)
        {
            glColor3d(red, green, blue);
            PostHook();
        }

        #undef glColor3dv
        extern "C" { GLAPI void GLAPIENTRY glColor3dv(const GLdouble *v); }
        void OVR::GLEContext::glColor3dv_Hook(const GLdouble *v)
        {
            glColor3dv(v);
            PostHook();
        }

        #undef glColor3f
        extern "C" { GLAPI void GLAPIENTRY glColor3f(GLfloat red, GLfloat green, GLfloat blue); }
        void OVR::GLEContext::glColor3f_Hook(GLfloat red, GLfloat green, GLfloat blue)
        {
            glColor3f(red, green, blue);
            PostHook();
        }

        #undef glColor3fv
        extern "C" { GLAPI void GLAPIENTRY glColor3fv(const GLfloat *v); }
        void OVR::GLEContext::glColor3fv_Hook(const GLfloat *v)
        {
            glColor3fv(v);
            PostHook();
        }

        #undef glColor3i
        extern "C" { GLAPI void GLAPIENTRY glColor3i(GLint red, GLint green, GLint blue); }
        void OVR::GLEContext::glColor3i_Hook(GLint red, GLint green, GLint blue)
        {
            glColor3i(red, green, blue);
            PostHook();
        }

        #undef glColor3iv
        extern "C" { GLAPI void GLAPIENTRY glColor3iv(const GLint *v); }
        void OVR::GLEContext::glColor3iv_Hook(const GLint *v)
        {
            glColor3iv(v);
            PostHook();
        }

        #undef glColor3s
        extern "C" { GLAPI void GLAPIENTRY glColor3s(GLshort red, GLshort green, GLshort blue); }
        void OVR::GLEContext::glColor3s_Hook(GLshort red, GLshort green, GLshort blue)
        {
            glColor3s(red, green, blue);
            PostHook();
        }

        #undef glColor3sv
        extern "C" { GLAPI void GLAPIENTRY glColor3sv(const GLshort *v); }
        void OVR::GLEContext::glColor3sv_Hook(const GLshort *v)
        {
            glColor3sv(v);
            PostHook();
        }

        #undef glColor3ub
        extern "C" { GLAPI void GLAPIENTRY glColor3ub(GLubyte red, GLubyte green, GLubyte blue); }
        void OVR::GLEContext::glColor3ub_Hook(GLubyte red, GLubyte green, GLubyte blue)
        {
            glColor3ub(red, green, blue);
            PostHook();
        }

        #undef glColor3ubv
        extern "C" { GLAPI void GLAPIENTRY glColor3ubv(const GLubyte *v); }
        void OVR::GLEContext::glColor3ubv_Hook(const GLubyte *v)
        {
            glColor3ubv(v);
            PostHook();
        }

        #undef glColor3ui
        extern "C" { GLAPI void GLAPIENTRY glColor3ui(GLuint red, GLuint green, GLuint blue); }
        void OVR::GLEContext::glColor3ui_Hook(GLuint red, GLuint green, GLuint blue)
        {
            glColor3ui(red, green, blue);
            PostHook();
        }

        #undef glColor3uiv
        extern "C" { GLAPI void GLAPIENTRY glColor3uiv(const GLuint *v); }
        void OVR::GLEContext::glColor3uiv_Hook(const GLuint *v)
        {
            glColor3uiv(v);
            PostHook();
        }

        #undef glColor3us
        extern "C" { GLAPI void GLAPIENTRY glColor3us(GLushort red, GLushort green, GLushort blue); }
        void OVR::GLEContext::glColor3us_Hook(GLushort red, GLushort green, GLushort blue)
        {
            glColor3us(red, green, blue);
            PostHook();
        }

        #undef glColor3usv
        extern "C" { GLAPI void GLAPIENTRY glColor3usv(const GLushort *v); }
        void OVR::GLEContext::glColor3usv_Hook(const GLushort *v)
        {
            glColor3usv(v);
            PostHook();
        }

        #undef glColor4b
        extern "C" { GLAPI void GLAPIENTRY glColor4b(GLbyte red, GLbyte green, GLbyte blue, GLbyte alpha); }
        void OVR::GLEContext::glColor4b_Hook(GLbyte red, GLbyte green, GLbyte blue, GLbyte alpha)
        {
            glColor4b(red, green, blue, alpha);
            PostHook();
        }

        #undef glColor4bv
        extern "C" { GLAPI void GLAPIENTRY glColor4bv(const GLbyte *v); }
        void OVR::GLEContext::glColor4bv_Hook(const GLbyte *v)
        {
            glColor4bv(v);
            PostHook();
        }

        #undef glColor4d
        extern "C" { GLAPI void GLAPIENTRY glColor4d(GLdouble red, GLdouble green, GLdouble blue, GLdouble alpha); }
        void OVR::GLEContext::glColor4d_Hook(GLdouble red, GLdouble green, GLdouble blue, GLdouble alpha)
        {
            glColor4d(red, green, blue, alpha);
            PostHook();
        }

        #undef glColor4dv
        extern "C" { GLAPI void GLAPIENTRY glColor4dv(const GLdouble *v); }
        void OVR::GLEContext::glColor4dv_Hook(const GLdouble *v)
        {
            glColor4dv(v);
            PostHook();
        }

        #undef glColor4f
        extern "C" { GLAPI void GLAPIENTRY glColor4f(GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha); }
        void OVR::GLEContext::glColor4f_Hook(GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha)
        {
            glColor4f(red, green, blue, alpha);
            PostHook();
        }

        #undef glColor4fv
        extern "C" { GLAPI void GLAPIENTRY glColor4fv(const GLfloat *v); }
        void OVR::GLEContext::glColor4fv_Hook(const GLfloat *v)
        {
            glColor4fv(v);
            PostHook();
        }

        #undef glColor4i
        extern "C" { GLAPI void GLAPIENTRY glColor4i(GLint red, GLint green, GLint blue, GLint alpha); }
        void OVR::GLEContext::glColor4i_Hook(GLint red, GLint green, GLint blue, GLint alpha)
        {
            glColor4i(red, green, blue, alpha);
            PostHook();
        }

        #undef glColor4iv
        extern "C" { GLAPI void GLAPIENTRY glColor4iv(const GLint *v); }
        void OVR::GLEContext::glColor4iv_Hook(const GLint *v)
        {
            glColor4iv(v);
            PostHook();
        }

        #undef glColor4s
        extern "C" { GLAPI void GLAPIENTRY glColor4s(GLshort red, GLshort green, GLshort blue, GLshort alpha); }
        void OVR::GLEContext::glColor4s_Hook(GLshort red, GLshort green, GLshort blue, GLshort alpha)
        {
            glColor4s(red, green, blue, alpha);
            PostHook();
        }

        #undef glColor4sv
        extern "C" { GLAPI void GLAPIENTRY glColor4sv(const GLshort *v); }
        void OVR::GLEContext::glColor4sv_Hook(const GLshort *v)
        {
            glColor4sv(v);
            PostHook();
        }

        #undef glColor4ub
        extern "C" { GLAPI void GLAPIENTRY glColor4ub(GLubyte red, GLubyte green, GLubyte blue, GLubyte alpha); }
        void OVR::GLEContext::glColor4ub_Hook(GLubyte red, GLubyte green, GLubyte blue, GLubyte alpha)
        {
            glColor4ub(red, green, blue, alpha);
            PostHook();
        }

        #undef glColor4ubv
        extern "C" { GLAPI void GLAPIENTRY glColor4ubv(const GLubyte *v); }
        void OVR::GLEContext::glColor4ubv_Hook(const GLubyte *v)
        {
            glColor4ubv(v);
            PostHook();
        }

        #undef glColor4ui
        extern "C" { GLAPI void GLAPIENTRY glColor4ui(GLuint red, GLuint green, GLuint blue, GLuint alpha); }
        void OVR::GLEContext::glColor4ui_Hook(GLuint red, GLuint green, GLuint blue, GLuint alpha)
        {
            glColor4ui(red, green, blue, alpha);
            PostHook();
        }

        #undef glColor4uiv
        extern "C" { GLAPI void GLAPIENTRY glColor4uiv(const GLuint *v); }
        void OVR::GLEContext::glColor4uiv_Hook(const GLuint *v)
        {
            glColor4uiv(v);
            PostHook();
        }

        #undef glColor4us
        extern "C" { GLAPI void GLAPIENTRY glColor4us(GLushort red, GLushort green, GLushort blue, GLushort alpha); }
        void OVR::GLEContext::glColor4us_Hook(GLushort red, GLushort green, GLushort blue, GLushort alpha)
        {
            glColor4us(red, green, blue, alpha);
            PostHook();
        }

        #undef glColor4usv
        extern "C" { GLAPI void GLAPIENTRY glColor4usv(const GLushort *v); }
        void OVR::GLEContext::glColor4usv_Hook(const GLushort *v)
        {
            glColor4usv(v);
            PostHook();
        }

        #undef glColorMask
        extern "C" { GLAPI void GLAPIENTRY glColorMask(GLboolean red, GLboolean green, GLboolean blue, GLboolean alpha); }
        void OVR::GLEContext::glColorMask_Hook(GLboolean red, GLboolean green, GLboolean blue, GLboolean alpha)
        {
            glColorMask(red, green, blue, alpha);
            PostHook();
        }

        #undef glColorMaterial
        extern "C" { GLAPI void GLAPIENTRY glColorMaterial(GLenum face, GLenum mode); }
        void OVR::GLEContext::glColorMaterial_Hook(GLenum face, GLenum mode)
        {
            glColorMaterial(face, mode);
            PostHook();
        }

        #undef glColorPointer
        extern "C" { GLAPI void GLAPIENTRY glColorPointer(GLint size, GLenum type, GLsizei stride, const void *pointer); }
        void OVR::GLEContext::glColorPointer_Hook(GLint size, GLenum type, GLsizei stride, const void *pointer)
        {
            glColorPointer(size, type, stride, pointer);
            PostHook();
        }

        #undef glCopyPixels
        extern "C" { GLAPI void GLAPIENTRY glCopyPixels(GLint x, GLint y, GLsizei width, GLsizei height, GLenum type); }
        void OVR::GLEContext::glCopyPixels_Hook(GLint x, GLint y, GLsizei width, GLsizei height, GLenum type)
        {
            glCopyPixels(x, y, width, height, type);
            PostHook();
        }

        #undef glCopyTexImage1D
        extern "C" { GLAPI void GLAPIENTRY glCopyTexImage1D(GLenum target, GLint level, GLenum internalFormat, GLint x, GLint y, GLsizei width, GLint border); }
        void OVR::GLEContext::glCopyTexImage1D_Hook(GLenum target, GLint level, GLenum internalFormat, GLint x, GLint y, GLsizei width, GLint border)
        {
            glCopyTexImage1D(target, level, internalFormat, x, y, width, border);
            PostHook();
        }

        #undef glCopyTexImage2D
        extern "C" { GLAPI void GLAPIENTRY glCopyTexImage2D(GLenum target, GLint level, GLenum internalFormat, GLint x, GLint y, GLsizei width, GLsizei height, GLint border); }
        void OVR::GLEContext::glCopyTexImage2D_Hook(GLenum target, GLint level, GLenum internalFormat, GLint x, GLint y, GLsizei width, GLsizei height, GLint border)
        {
            glCopyTexImage2D(target, level, internalFormat, x, y, width, height, border);
            PostHook();
        }

        #undef glCopyTexSubImage1D
        extern "C" { GLAPI void GLAPIENTRY glCopyTexSubImage1D(GLenum target, GLint level, GLint xoffset, GLint x, GLint y, GLsizei width); }
        void OVR::GLEContext::glCopyTexSubImage1D_Hook(GLenum target, GLint level, GLint xoffset, GLint x, GLint y, GLsizei width)
        {
            glCopyTexSubImage1D(target, level, xoffset, x, y, width);
            PostHook();
        }

        #undef glCopyTexSubImage2D
        extern "C" { GLAPI void GLAPIENTRY glCopyTexSubImage2D(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint x, GLint y, GLsizei width, GLsizei height); }
        void OVR::GLEContext::glCopyTexSubImage2D_Hook(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint x, GLint y, GLsizei width, GLsizei height)
        {
            glCopyTexSubImage2D(target, level, xoffset, yoffset, x, y, width, height);
            PostHook();
        }

        #undef glCullFace
        extern "C" { GLAPI void GLAPIENTRY glCullFace(GLenum mode); }
        void OVR::GLEContext::glCullFace_Hook(GLenum mode)
        {
            glCullFace(mode);
            PostHook();
        }

        #undef glDeleteLists
        extern "C" { GLAPI void GLAPIENTRY glDeleteLists(GLuint list, GLsizei range); }
        void OVR::GLEContext::glDeleteLists_Hook(GLuint list, GLsizei range)
        {
            glDeleteLists(list, range);
            PostHook();
        }

        #undef glDeleteTextures
        extern "C" { GLAPI void GLAPIENTRY glDeleteTextures(GLsizei n, const GLuint *textures); }
        void OVR::GLEContext::glDeleteTextures_Hook(GLsizei n, const GLuint *textures)
        {
            glDeleteTextures(n, textures);
            PostHook();
        }

        #undef glDepthFunc
        extern "C" { GLAPI void GLAPIENTRY glDepthFunc(GLenum func); }
        void OVR::GLEContext::glDepthFunc_Hook(GLenum func)
        {
            glDepthFunc(func);
            PostHook();
        }

        #undef glDepthMask
        extern "C" { GLAPI void GLAPIENTRY glDepthMask(GLboolean flag); }
        void OVR::GLEContext::glDepthMask_Hook(GLboolean flag)
        {
            glDepthMask(flag);
            PostHook();
        }

        #undef glDepthRange
        extern "C" { GLAPI void GLAPIENTRY glDepthRange(GLclampd zNear, GLclampd zFar); }
        void OVR::GLEContext::glDepthRange_Hook(GLclampd zNear, GLclampd zFar)
        {
            glDepthRange(zNear, zFar);
            PostHook();
        }

        #undef glDisable
        extern "C" { GLAPI void GLAPIENTRY glDisable(GLenum cap); }
        void OVR::GLEContext::glDisable_Hook(GLenum cap)
        {
            glDisable(cap);
            PostHook();
        }

        #undef glDisableClientState
        extern "C" { GLAPI void GLAPIENTRY glDisableClientState(GLenum array); }
        void OVR::GLEContext::glDisableClientState_Hook(GLenum array)
        {
            glDisableClientState(array);
            PostHook();
        }

        #undef glDrawArrays
        extern "C" { GLAPI void GLAPIENTRY glDrawArrays(GLenum mode, GLint first, GLsizei count); }
        void OVR::GLEContext::glDrawArrays_Hook(GLenum mode, GLint first, GLsizei count)
        {
            glDrawArrays(mode, first, count);
            PostHook();
        }

        #undef glDrawBuffer
        extern "C" { GLAPI void GLAPIENTRY glDrawBuffer(GLenum mode); }
        void OVR::GLEContext::glDrawBuffer_Hook(GLenum mode)
        {
            glDrawBuffer(mode);
            PostHook();
        }

        #undef glDrawElements
        extern "C" { GLAPI void GLAPIENTRY glDrawElements(GLenum mode, GLsizei count, GLenum type, const void *indices); }
        void OVR::GLEContext::glDrawElements_Hook(GLenum mode, GLsizei count, GLenum type, const void *indices)
        {
            glDrawElements(mode, count, type, indices);
            PostHook();
        }

        #undef glDrawPixels
        extern "C" { GLAPI void GLAPIENTRY glDrawPixels(GLsizei width, GLsizei height, GLenum format, GLenum type, const void *pixels); }
        void OVR::GLEContext::glDrawPixels_Hook(GLsizei width, GLsizei height, GLenum format, GLenum type, const void *pixels)
        {
            glDrawPixels(width, height, format, type, pixels);
            PostHook();
        }

        #undef glEdgeFlag
        extern "C" { GLAPI void GLAPIENTRY glEdgeFlag(GLboolean flag); }
        void OVR::GLEContext::glEdgeFlag_Hook(GLboolean flag)
        {
            glEdgeFlag(flag);
            PostHook();
        }

        #undef glEdgeFlagPointer
        extern "C" { GLAPI void GLAPIENTRY glEdgeFlagPointer(GLsizei stride, const void *pointer); }
        void OVR::GLEContext::glEdgeFlagPointer_Hook(GLsizei stride, const void *pointer)
        {
            glEdgeFlagPointer(stride, pointer);
            PostHook();
        }

        #undef glEdgeFlagv
        extern "C" { GLAPI void GLAPIENTRY glEdgeFlagv(const GLboolean *flag); }
        void OVR::GLEContext::glEdgeFlagv_Hook(const GLboolean *flag)
        {
            glEdgeFlagv(flag);
            PostHook();
        }

        #undef glEnable
        extern "C" { GLAPI void GLAPIENTRY glEnable(GLenum cap); }
        namespace OVR {
        void GLEContext::glEnable_Hook(GLenum cap)
        {
            glEnable(cap);
            PostHook();
        }
        }

        #undef glEnableClientState
        extern "C" { GLAPI void GLAPIENTRY glEnableClientState(GLenum array); }
        void OVR::GLEContext::glEnableClientState_Hook(GLenum array)
        {
            glEnableClientState(array);
            PostHook();
        }

        #undef glEnd
        extern "C" { GLAPI void GLAPIENTRY glEnd(); }
        void OVR::GLEContext::glEnd_Hook()
        {
            glEnd();
            PostHook();
        }

        #undef glEndList
        extern "C" { GLAPI void GLAPIENTRY glEndList(); }
        void OVR::GLEContext::glEndList_Hook()
        {
            glEndList();
            PostHook();
        }

        #undef glEvalCoord1d
        extern "C" { GLAPI void GLAPIENTRY glEvalCoord1d(GLdouble u); }
        void OVR::GLEContext::glEvalCoord1d_Hook(GLdouble u)
        {
            glEvalCoord1d(u);
            PostHook();
        }

        #undef glEvalCoord1dv
        extern "C" { GLAPI void GLAPIENTRY glEvalCoord1dv(const GLdouble *u); }
        void OVR::GLEContext::glEvalCoord1dv_Hook(const GLdouble *u)
        {
            glEvalCoord1dv(u);
            PostHook();
        }

        #undef glEvalCoord1f
        extern "C" { GLAPI void GLAPIENTRY glEvalCoord1f(GLfloat u); }
        void OVR::GLEContext::glEvalCoord1f_Hook(GLfloat u)
        {
            glEvalCoord1f(u);
            PostHook();
        }

        #undef glEvalCoord1fv
        extern "C" { GLAPI void GLAPIENTRY glEvalCoord1fv(const GLfloat *u); }
        void OVR::GLEContext::glEvalCoord1fv_Hook(const GLfloat *u)
        {
            glEvalCoord1fv(u);
            PostHook();
        }

        #undef glEvalCoord2d
        extern "C" { GLAPI void GLAPIENTRY glEvalCoord2d(GLdouble u, GLdouble v); }
        void OVR::GLEContext::glEvalCoord2d_Hook(GLdouble u, GLdouble v)
        {
            glEvalCoord2d(u, v);
            PostHook();
        }

        #undef glEvalCoord2dv
        extern "C" { GLAPI void GLAPIENTRY glEvalCoord2dv(const GLdouble *u); }
        void OVR::GLEContext::glEvalCoord2dv_Hook(const GLdouble *u)
        {
            glEvalCoord2dv(u);
            PostHook();
        }

        #undef glEvalCoord2f
        extern "C" { GLAPI void GLAPIENTRY glEvalCoord2f(GLfloat u, GLfloat v); }
        void OVR::GLEContext::glEvalCoord2f_Hook(GLfloat u, GLfloat v)
        {
            glEvalCoord2f(u, v);
            PostHook();
        }

        #undef glEvalCoord2fv
        extern "C" { GLAPI void GLAPIENTRY glEvalCoord2fv(const GLfloat *u); }
        void OVR::GLEContext::glEvalCoord2fv_Hook(const GLfloat *u)
        {
            glEvalCoord2fv(u);
            PostHook();
        }

        #undef glEvalMesh1
        extern "C" { GLAPI void GLAPIENTRY glEvalMesh1(GLenum mode, GLint i1, GLint i2); }
        void OVR::GLEContext::glEvalMesh1_Hook(GLenum mode, GLint i1, GLint i2)
        {
            glEvalMesh1(mode, i1, i2);
            PostHook();
        }

        #undef glEvalMesh2
        extern "C" { GLAPI void GLAPIENTRY glEvalMesh2(GLenum mode, GLint i1, GLint i2, GLint j1, GLint j2); }
        void OVR::GLEContext::glEvalMesh2_Hook(GLenum mode, GLint i1, GLint i2, GLint j1, GLint j2)
        {
            glEvalMesh2(mode, i1, i2, j1, j2);
            PostHook();
        }

        #undef glEvalPoint1
        extern "C" { GLAPI void GLAPIENTRY glEvalPoint1(GLint i); }
        void OVR::GLEContext::glEvalPoint1_Hook(GLint i)
        {
            glEvalPoint1(i);
            PostHook();
        }

        #undef glEvalPoint2
        extern "C" { GLAPI void GLAPIENTRY glEvalPoint2(GLint i, GLint j); }
        void OVR::GLEContext::glEvalPoint2_Hook(GLint i, GLint j)
        {
            glEvalPoint2(i, j);
            PostHook();
        }

        #undef glFeedbackBuffer
        extern "C" { GLAPI void GLAPIENTRY glFeedbackBuffer(GLsizei size, GLenum type, GLfloat *buffer); }
        void OVR::GLEContext::glFeedbackBuffer_Hook(GLsizei size, GLenum type, GLfloat *buffer)
        {
            glFeedbackBuffer(size, type, buffer);
            PostHook();
        }

        #undef glFinish
        extern "C" { GLAPI void GLAPIENTRY glFinish(); }
        void OVR::GLEContext::glFinish_Hook()
        {
            glFinish();
            PostHook();
        }

        #undef glFlush
        extern "C" { GLAPI void GLAPIENTRY glFlush(); }
        void OVR::GLEContext::glFlush_Hook()
        {
            glFlush();
            PostHook();
        }

        #undef glFogf
        extern "C" { GLAPI void GLAPIENTRY glFogf(GLenum pname, GLfloat param); }
        void OVR::GLEContext::glFogf_Hook(GLenum pname, GLfloat param)
        {
            glFogf(pname, param);
            PostHook();
        }

        #undef glFogfv
        extern "C" { GLAPI void GLAPIENTRY glFogfv(GLenum pname, const GLfloat *params); }
        void OVR::GLEContext::glFogfv_Hook(GLenum pname, const GLfloat *params)
        {
            glFogfv(pname, params);
            PostHook();
        }

        #undef glFogi
        extern "C" { GLAPI void GLAPIENTRY glFogi(GLenum pname, GLint param); }
        void OVR::GLEContext::glFogi_Hook(GLenum pname, GLint param)
        {
            glFogi(pname, param);
            PostHook();
        }

        #undef glFogiv
        extern "C" { GLAPI void GLAPIENTRY glFogiv(GLenum pname, const GLint *params); }
        void OVR::GLEContext::glFogiv_Hook(GLenum pname, const GLint *params)
        {
            glFogiv(pname, params);
            PostHook();
        }

        #undef glFrontFace
        extern "C" { GLAPI void GLAPIENTRY glFrontFace(GLenum mode); }
        void OVR::GLEContext::glFrontFace_Hook(GLenum mode)
        {
            glFrontFace(mode);
            PostHook();
        }

        #undef glFrustum
        extern "C" { GLAPI void GLAPIENTRY glFrustum(GLdouble left, GLdouble right, GLdouble bottom, GLdouble top, GLdouble zNear, GLdouble zFar); }
        void OVR::GLEContext::glFrustum_Hook(GLdouble left, GLdouble right, GLdouble bottom, GLdouble top, GLdouble zNear, GLdouble zFar)
        {
            glFrustum(left, right, bottom, top, zNear, zFar);
            PostHook();
        }

        #undef glGenLists
        extern "C" { GLAPI GLuint GLAPIENTRY glGenLists(GLsizei range); }
        GLuint OVR::GLEContext::glGenLists_Hook(GLsizei range)
        {
            GLuint u = glGenLists(range);
            PostHook();
            return u;
        }

        #undef glGenTextures
        extern "C" { GLAPI void GLAPIENTRY glGenTextures(GLsizei n, GLuint *textures); }
        void OVR::GLEContext::glGenTextures_Hook(GLsizei n, GLuint *textures)
        {
            glGenTextures(n, textures);
            PostHook();
        }

        #undef glGetBooleanv
        extern "C" { GLAPI void GLAPIENTRY glGetBooleanv(GLenum pname, GLboolean *params); }
        void OVR::GLEContext::glGetBooleanv_Hook(GLenum pname, GLboolean *params)
        {
            glGetBooleanv(pname, params);
            PostHook();
        }

        #undef glGetClipPlane
        extern "C" { GLAPI void GLAPIENTRY glGetClipPlane(GLenum plane, GLdouble *equation); }
        void OVR::GLEContext::glGetClipPlane_Hook(GLenum plane, GLdouble *equation)
        {
            glGetClipPlane(plane, equation);
            PostHook();
        }

        #undef glGetDoublev
        extern "C" { GLAPI void GLAPIENTRY glGetDoublev(GLenum pname, GLdouble *params); }
        void OVR::GLEContext::glGetDoublev_Hook(GLenum pname, GLdouble *params)
        {
            glGetDoublev(pname, params);
            PostHook();
        }

        //#undef glGetError Not needed because we happen to do this above already.
        //extern "C" { GLAPI GLenum GLAPIENTRY glGetError(); }
        GLenum OVR::GLEContext::glGetError_Hook()
        {
            GLenum e = glGetError();
            PostHook();
            return e;
        }

        #undef glGetFloatv
        extern "C" { GLAPI void GLAPIENTRY glGetFloatv(GLenum pname, GLfloat *params); }
        void OVR::GLEContext::glGetFloatv_Hook(GLenum pname, GLfloat *params)
        {
            glGetFloatv(pname, params);
            PostHook();
        }

        #undef glGetIntegerv
        extern "C" { GLAPI void GLAPIENTRY glGetIntegerv(GLenum pname, GLint *params); }
        void OVR::GLEContext::glGetIntegerv_Hook(GLenum pname, GLint *params)
        {
            glGetIntegerv(pname, params);
            PostHook();
        }

        #undef glGetLightfv
        extern "C" { GLAPI void GLAPIENTRY glGetLightfv(GLenum light, GLenum pname, GLfloat *params); }
        void OVR::GLEContext::glGetLightfv_Hook(GLenum light, GLenum pname, GLfloat *params)
        {
            glGetLightfv(light, pname, params);
            PostHook();
        }

        #undef glGetLightiv
        extern "C" { GLAPI void GLAPIENTRY glGetLightiv(GLenum light, GLenum pname, GLint *params); }
        void OVR::GLEContext::glGetLightiv_Hook(GLenum light, GLenum pname, GLint *params)
        {
            glGetLightiv(light, pname, params);
            PostHook();
        }

        #undef glGetMapdv
        extern "C" { GLAPI void GLAPIENTRY glGetMapdv(GLenum target, GLenum query, GLdouble *v); }
        void OVR::GLEContext::glGetMapdv_Hook(GLenum target, GLenum query, GLdouble *v)
        {
            glGetMapdv(target, query, v);
            PostHook();
        }

        #undef glGetMapfv
        extern "C" { GLAPI void GLAPIENTRY glGetMapfv(GLenum target, GLenum query, GLfloat *v); }
        void OVR::GLEContext::glGetMapfv_Hook(GLenum target, GLenum query, GLfloat *v)
        {
            glGetMapfv(target, query, v);
            PostHook();
        }

        #undef glGetMapiv
        extern "C" { GLAPI void GLAPIENTRY glGetMapiv(GLenum target, GLenum query, GLint *v); }
        void OVR::GLEContext::glGetMapiv_Hook(GLenum target, GLenum query, GLint *v)
        {
            glGetMapiv(target, query, v);
            PostHook();
        }

        #undef glGetMaterialfv
        extern "C" { GLAPI void GLAPIENTRY glGetMaterialfv(GLenum face, GLenum pname, GLfloat *params); }
        void OVR::GLEContext::glGetMaterialfv_Hook(GLenum face, GLenum pname, GLfloat *params)
        {
            glGetMaterialfv(face, pname, params);
            PostHook();
        }

        #undef glGetMaterialiv
        extern "C" { GLAPI void GLAPIENTRY glGetMaterialiv(GLenum face, GLenum pname, GLint *params); }
        void OVR::GLEContext::glGetMaterialiv_Hook(GLenum face, GLenum pname, GLint *params)
        {
            glGetMaterialiv(face, pname, params);
            PostHook();
        }

        #undef glGetPixelMapfv
        extern "C" { GLAPI void GLAPIENTRY glGetPixelMapfv(GLenum map, GLfloat *values); }
        void OVR::GLEContext::glGetPixelMapfv_Hook(GLenum map, GLfloat *values)
        {
            glGetPixelMapfv(map, values);
            PostHook();
        }

        #undef glGetPixelMapuiv
        extern "C" { GLAPI void GLAPIENTRY glGetPixelMapuiv(GLenum map, GLuint *values); }
        void OVR::GLEContext::glGetPixelMapuiv_Hook(GLenum map, GLuint *values)
        {
            glGetPixelMapuiv(map, values);
            PostHook();
        }

        #undef glGetPixelMapusv
        extern "C" { GLAPI void GLAPIENTRY glGetPixelMapusv(GLenum map, GLushort *values); }
        void OVR::GLEContext::glGetPixelMapusv_Hook(GLenum map, GLushort *values)
        {
            glGetPixelMapusv(map, values);
            PostHook();
        }

        #undef glGetPointerv
        extern "C" { GLAPI void GLAPIENTRY glGetPointerv(GLenum pname, void* *params); }
        void OVR::GLEContext::glGetPointerv_Hook(GLenum pname, void* *params)
        {
            glGetPointerv(pname, params);
            PostHook();
        }

        #undef glGetPolygonStipple
        extern "C" { GLAPI void GLAPIENTRY glGetPolygonStipple(GLubyte *mask); }
        void OVR::GLEContext::glGetPolygonStipple_Hook(GLubyte *mask)
        {
            glGetPolygonStipple(mask);
            PostHook();
        }

        // #undef glGetString // This was already disabled above.
        // extern "C" { GLAPI const GLubyte * GLAPIENTRY glGetString(GLenum name); }
        const GLubyte * OVR::GLEContext::glGetString_Hook(GLenum name)
        {
            const GLubyte * p = glGetString(name);
            PostHook();
            return p;
        }

        #undef glGetTexEnvfv
        extern "C" { GLAPI void GLAPIENTRY glGetTexEnvfv(GLenum target, GLenum pname, GLfloat *params); }
        void OVR::GLEContext::glGetTexEnvfv_Hook(GLenum target, GLenum pname, GLfloat *params)
        {
            glGetTexEnvfv(target, pname, params);
            PostHook();
        }

        #undef glGetTexEnviv
        extern "C" { GLAPI void GLAPIENTRY glGetTexEnviv(GLenum target, GLenum pname, GLint *params); }
        void OVR::GLEContext::glGetTexEnviv_Hook(GLenum target, GLenum pname, GLint *params)
        {
            glGetTexEnviv(target, pname, params);
            PostHook();
        }

        #undef glGetTexGendv
        extern "C" { GLAPI void GLAPIENTRY glGetTexGendv(GLenum coord, GLenum pname, GLdouble *params); }
        void OVR::GLEContext::glGetTexGendv_Hook(GLenum coord, GLenum pname, GLdouble *params)
        {
            glGetTexGendv(coord, pname, params);
            PostHook();
        }

        #undef glGetTexGenfv
        extern "C" { GLAPI void GLAPIENTRY glGetTexGenfv(GLenum coord, GLenum pname, GLfloat *params); }
        void OVR::GLEContext::glGetTexGenfv_Hook(GLenum coord, GLenum pname, GLfloat *params)
        {
            glGetTexGenfv(coord, pname, params);
            PostHook();
        }

        #undef glGetTexGeniv
        extern "C" { GLAPI void GLAPIENTRY glGetTexGeniv(GLenum coord, GLenum pname, GLint *params); }
        void OVR::GLEContext::glGetTexGeniv_Hook(GLenum coord, GLenum pname, GLint *params)
        {
            glGetTexGeniv(coord, pname, params);
            PostHook();
        }

        #undef glGetTexImage
        extern "C" { GLAPI void GLAPIENTRY glGetTexImage(GLenum target, GLint level, GLenum format, GLenum type, void *pixels); }
        void OVR::GLEContext::glGetTexImage_Hook(GLenum target, GLint level, GLenum format, GLenum type, void *pixels)
        {
            glGetTexImage(target, level, format, type, pixels);
            PostHook();
        }

        #undef glGetTexLevelParameterfv
        extern "C" { GLAPI void GLAPIENTRY glGetTexLevelParameterfv(GLenum target, GLint level, GLenum pname, GLfloat *params); }
        void OVR::GLEContext::glGetTexLevelParameterfv_Hook(GLenum target, GLint level, GLenum pname, GLfloat *params)
        {
            glGetTexLevelParameterfv(target, level, pname, params);
            PostHook();
        }

        #undef glGetTexLevelParameteriv
        extern "C" { GLAPI void GLAPIENTRY glGetTexLevelParameteriv(GLenum target, GLint level, GLenum pname, GLint *params); }
        void OVR::GLEContext::glGetTexLevelParameteriv_Hook(GLenum target, GLint level, GLenum pname, GLint *params)
        {
            glGetTexLevelParameteriv(target, level, pname, params);
            PostHook();
        }

        #undef glGetTexParameterfv
        extern "C" { GLAPI void GLAPIENTRY glGetTexParameterfv(GLenum target, GLenum pname, GLfloat *params); }
        void OVR::GLEContext::glGetTexParameterfv_Hook(GLenum target, GLenum pname, GLfloat *params)
        {
            glGetTexParameterfv(target, pname, params);
            PostHook();
        }

        #undef glGetTexParameteriv
        extern "C" { GLAPI void GLAPIENTRY glGetTexParameteriv(GLenum target, GLenum pname, GLint *params); }
        void OVR::GLEContext::glGetTexParameteriv_Hook(GLenum target, GLenum pname, GLint *params)
        {
            glGetTexParameteriv(target, pname, params);
            PostHook();
        }

        #undef glHint
        extern "C" { GLAPI void GLAPIENTRY glHint(GLenum target, GLenum mode); }
        void OVR::GLEContext::glHint_Hook(GLenum target, GLenum mode)
        {
            glHint(target, mode);
            PostHook();
        }

        #undef glIndexMask
        extern "C" { GLAPI void GLAPIENTRY glIndexMask(GLuint mask); }
        void OVR::GLEContext::glIndexMask_Hook(GLuint mask)
        {
            glIndexMask(mask);
            PostHook();
        }

        #undef glIndexPointer
        extern "C" { GLAPI void GLAPIENTRY glIndexPointer(GLenum type, GLsizei stride, const void *pointer); }
        void OVR::GLEContext::glIndexPointer_Hook(GLenum type, GLsizei stride, const void *pointer)
        {
            glIndexPointer(type, stride, pointer);
            PostHook();
        }

        #undef glIndexd
        extern "C" { GLAPI void GLAPIENTRY glIndexd(GLdouble c); }
        void OVR::GLEContext::glIndexd_Hook(GLdouble c)
        {
            glIndexd(c);
            PostHook();
        }

        #undef glIndexdv
        extern "C" { GLAPI void GLAPIENTRY glIndexdv(const GLdouble *c); }
        void OVR::GLEContext::glIndexdv_Hook(const GLdouble *c)
        {
            glIndexdv(c);
            PostHook();
        }

        #undef glIndexf
        extern "C" { GLAPI void GLAPIENTRY glIndexf(GLfloat c); }
        void OVR::GLEContext::glIndexf_Hook(GLfloat c)
        {
            glIndexf(c);
            PostHook();
        }

        #undef glIndexfv
        extern "C" { GLAPI void GLAPIENTRY glIndexfv(const GLfloat *c); }
        void OVR::GLEContext::glIndexfv_Hook(const GLfloat *c)
        {
            glIndexfv(c);
            PostHook();
        }

        #undef glIndexi
        extern "C" { GLAPI void GLAPIENTRY glIndexi(GLint c); }
        void OVR::GLEContext::glIndexi_Hook(GLint c)
        {
            glIndexi(c);
            PostHook();
        }

        #undef glIndexiv
        extern "C" { GLAPI void GLAPIENTRY glIndexiv(const GLint *c); }
        void OVR::GLEContext::glIndexiv_Hook(const GLint *c)
        {
            glIndexiv(c);
            PostHook();
        }

        #undef glIndexs
        extern "C" { GLAPI void GLAPIENTRY glIndexs(GLshort c); }
        void OVR::GLEContext::glIndexs_Hook(GLshort c)
        {
            glIndexs(c);
            PostHook();
        }

        #undef glIndexsv
        extern "C" { GLAPI void GLAPIENTRY glIndexsv(const GLshort *c); }
        void OVR::GLEContext::glIndexsv_Hook(const GLshort *c)
        {
            glIndexsv(c);
            PostHook();
        }

        #undef glIndexub
        extern "C" { GLAPI void GLAPIENTRY glIndexub(GLubyte c); }
        void OVR::GLEContext::glIndexub_Hook(GLubyte c)
        {
            glIndexub(c);
            PostHook();
        }

        #undef glIndexubv
        extern "C" { GLAPI void GLAPIENTRY glIndexubv(const GLubyte *c); }
        void OVR::GLEContext::glIndexubv_Hook(const GLubyte *c)
        {
            glIndexubv(c);
            PostHook();
        }

        #undef glInitNames
        extern "C" { GLAPI void GLAPIENTRY glInitNames(); }
        void OVR::GLEContext::glInitNames_Hook()
        {
            glInitNames();
            PostHook();
        }

        #undef glInterleavedArrays
        extern "C" { GLAPI void GLAPIENTRY glInterleavedArrays(GLenum format, GLsizei stride, const void *pointer); }
        void OVR::GLEContext::glInterleavedArrays_Hook(GLenum format, GLsizei stride, const void *pointer)
        {
            glInterleavedArrays(format, stride, pointer);
            PostHook();
        }

        #undef glIsEnabled
        extern "C" { GLAPI GLboolean GLAPIENTRY glIsEnabled(GLenum cap); }
        GLboolean OVR::GLEContext::glIsEnabled_Hook(GLenum cap)
        {
            GLboolean b = glIsEnabled(cap);
            PostHook();
            return b;
        }

        #undef glIsList
        extern "C" { GLAPI GLboolean GLAPIENTRY glIsList(GLuint list); }
        GLboolean OVR::GLEContext::glIsList_Hook(GLuint list)
        {
            GLboolean b = glIsList(list);
            PostHook();
            return b;
        }

        #undef glIsTexture
        extern "C" { GLAPI GLboolean GLAPIENTRY glIsTexture(GLuint texture); }
        GLboolean OVR::GLEContext::glIsTexture_Hook(GLuint texture)
        {
            GLboolean b = glIsTexture(texture);
            PostHook();
            return b;
        }

        #undef glLightModelf
        extern "C" { GLAPI void GLAPIENTRY glLightModelf(GLenum pname, GLfloat param); }
        void OVR::GLEContext::glLightModelf_Hook(GLenum pname, GLfloat param)
        {
            glLightModelf(pname, param);
            PostHook();
        }

        #undef glLightModelfv
        extern "C" { GLAPI void GLAPIENTRY glLightModelfv(GLenum pname, const GLfloat *params); }
        void OVR::GLEContext::glLightModelfv_Hook(GLenum pname, const GLfloat *params)
        {
            glLightModelfv(pname, params);
            PostHook();
        }

        #undef glLightModeli
        extern "C" { GLAPI void GLAPIENTRY glLightModeli(GLenum pname, GLint param); }
        void OVR::GLEContext::glLightModeli_Hook(GLenum pname, GLint param)
        {
            glLightModeli(pname, param);
            PostHook();
        }

        #undef glLightModeliv
        extern "C" { GLAPI void GLAPIENTRY glLightModeliv(GLenum pname, const GLint *params); }
        void OVR::GLEContext::glLightModeliv_Hook(GLenum pname, const GLint *params)
        {
            glLightModeliv(pname, params);
            PostHook();
        }

        #undef glLightf
        extern "C" { GLAPI void GLAPIENTRY glLightf(GLenum light, GLenum pname, GLfloat param); }
        void OVR::GLEContext::glLightf_Hook(GLenum light, GLenum pname, GLfloat param)
        {
            glLightf(light, pname, param);
            PostHook();
        }

        #undef glLightfv
        extern "C" { GLAPI void GLAPIENTRY glLightfv(GLenum light, GLenum pname, const GLfloat *params); }
        void OVR::GLEContext::glLightfv_Hook(GLenum light, GLenum pname, const GLfloat *params)
        {
            glLightfv(light, pname, params);
            PostHook();
        }

        #undef glLighti
        extern "C" { GLAPI void GLAPIENTRY glLighti(GLenum light, GLenum pname, GLint param); }
        void OVR::GLEContext::glLighti_Hook(GLenum light, GLenum pname, GLint param)
        {
            glLighti(light, pname, param);
            PostHook();
        }

        #undef glLightiv
        extern "C" { GLAPI void GLAPIENTRY glLightiv(GLenum light, GLenum pname, const GLint *params); }
        void OVR::GLEContext::glLightiv_Hook(GLenum light, GLenum pname, const GLint *params)
        {
            glLightiv(light, pname, params);
            PostHook();
        }

        #undef glLineStipple
        extern "C" { GLAPI void GLAPIENTRY glLineStipple(GLint factor, GLushort pattern); }
        void OVR::GLEContext::glLineStipple_Hook(GLint factor, GLushort pattern)
        {
            glLineStipple(factor, pattern);
            PostHook();
        }

        #undef glLineWidth
        extern "C" { GLAPI void GLAPIENTRY glLineWidth(GLfloat width); }
        void OVR::GLEContext::glLineWidth_Hook(GLfloat width)
        {
            glLineWidth(width);
            PostHook();
        }

        #undef glListBase
        extern "C" { GLAPI void GLAPIENTRY glListBase(GLuint base); }
        void OVR::GLEContext::glListBase_Hook(GLuint base)
        {
            glListBase(base);
            PostHook();
        }

        #undef glLoadIdentity
        extern "C" { GLAPI void GLAPIENTRY glLoadIdentity(); }
        void OVR::GLEContext::glLoadIdentity_Hook()
        {
            glLoadIdentity();
            PostHook();
        }

        #undef glLoadMatrixd
        extern "C" { GLAPI void GLAPIENTRY glLoadMatrixd(const GLdouble *m); }
        void OVR::GLEContext::glLoadMatrixd_Hook(const GLdouble *m)
        {
            glLoadMatrixd(m);
            PostHook();
        }

        #undef glLoadMatrixf
        extern "C" { GLAPI void GLAPIENTRY glLoadMatrixf(const GLfloat *m); }
        void OVR::GLEContext::glLoadMatrixf_Hook(const GLfloat *m)
        {
            glLoadMatrixf(m);
            PostHook();
        }

        #undef glLoadName
        extern "C" { GLAPI void GLAPIENTRY glLoadName(GLuint name); }
        void OVR::GLEContext::glLoadName_Hook(GLuint name)
        {
            glLoadName(name);
            PostHook();
        }

        #undef glLogicOp
        extern "C" { GLAPI void GLAPIENTRY glLogicOp(GLenum opcode); }
        void OVR::GLEContext::glLogicOp_Hook(GLenum opcode)
        {
            glLogicOp(opcode);
            PostHook();
        }

        #undef glMap1d
        extern "C" { GLAPI void GLAPIENTRY glMap1d(GLenum target, GLdouble u1, GLdouble u2, GLint stride, GLint order, const GLdouble *points); }
        void OVR::GLEContext::glMap1d_Hook(GLenum target, GLdouble u1, GLdouble u2, GLint stride, GLint order, const GLdouble *points)
        {
            glMap1d(target, u1, u2, stride, order, points);
            PostHook();
        }

        #undef glMap1f
        extern "C" { GLAPI void GLAPIENTRY glMap1f(GLenum target, GLfloat u1, GLfloat u2, GLint stride, GLint order, const GLfloat *points); }
        void OVR::GLEContext::glMap1f_Hook(GLenum target, GLfloat u1, GLfloat u2, GLint stride, GLint order, const GLfloat *points)
        {
            glMap1f(target, u1, u2, stride, order, points);
            PostHook();
        }

        #undef glMap2d
        extern "C" { GLAPI void GLAPIENTRY glMap2d(GLenum target, GLdouble u1, GLdouble u2, GLint ustride, GLint uorder, GLdouble v1, GLdouble v2, GLint vstride, GLint vorder, const GLdouble *points); }
        void OVR::GLEContext::glMap2d_Hook(GLenum target, GLdouble u1, GLdouble u2, GLint ustride, GLint uorder, GLdouble v1, GLdouble v2, GLint vstride, GLint vorder, const GLdouble *points)
        {
            glMap2d(target, u1, u2, ustride, uorder, v1, v2, vstride, vorder, points);
            PostHook();
        }

        #undef glMap2f
        extern "C" { GLAPI void GLAPIENTRY glMap2f(GLenum target, GLfloat u1, GLfloat u2, GLint ustride, GLint uorder, GLfloat v1, GLfloat v2, GLint vstride, GLint vorder, const GLfloat *points); }
        void OVR::GLEContext::glMap2f_Hook(GLenum target, GLfloat u1, GLfloat u2, GLint ustride, GLint uorder, GLfloat v1, GLfloat v2, GLint vstride, GLint vorder, const GLfloat *points)
        {
            glMap2f(target, u1, u2, ustride, uorder, v1, v2, vstride, vorder, points);
            PostHook();
        }

        #undef glMapGrid1d
        extern "C" { GLAPI void GLAPIENTRY glMapGrid1d(GLint un, GLdouble u1, GLdouble u2); }
        void OVR::GLEContext::glMapGrid1d_Hook(GLint un, GLdouble u1, GLdouble u2)
        {
            glMapGrid1d(un, u1, u2);
            PostHook();
        }

        #undef glMapGrid1f
        extern "C" { GLAPI void GLAPIENTRY glMapGrid1f(GLint un, GLfloat u1, GLfloat u2); }
        void OVR::GLEContext::glMapGrid1f_Hook(GLint un, GLfloat u1, GLfloat u2)
        {
            glMapGrid1f(un, u1, u2);
            PostHook();
        }

        #undef glMapGrid2d
        extern "C" { GLAPI void GLAPIENTRY glMapGrid2d(GLint un, GLdouble u1, GLdouble u2, GLint vn, GLdouble v1, GLdouble v2); }
        void OVR::GLEContext::glMapGrid2d_Hook(GLint un, GLdouble u1, GLdouble u2, GLint vn, GLdouble v1, GLdouble v2)
        {
            glMapGrid2d(un, u1, u2, vn, v1, v2);
            PostHook();
        }

        #undef glMapGrid2f
        extern "C" { GLAPI void GLAPIENTRY glMapGrid2f(GLint un, GLfloat u1, GLfloat u2, GLint vn, GLfloat v1, GLfloat v2); }
        void OVR::GLEContext::glMapGrid2f_Hook(GLint un, GLfloat u1, GLfloat u2, GLint vn, GLfloat v1, GLfloat v2)
        {
            glMapGrid2f(un, u1, u2, vn, v1, v2);
            PostHook();
        }

        #undef glMaterialf
        extern "C" { GLAPI void GLAPIENTRY glMaterialf(GLenum face, GLenum pname, GLfloat param); }
        void OVR::GLEContext::glMaterialf_Hook(GLenum face, GLenum pname, GLfloat param)
        {
            glMaterialf(face, pname, param);
            PostHook();
        }

        #undef glMaterialfv
        extern "C" { GLAPI void GLAPIENTRY glMaterialfv(GLenum face, GLenum pname, const GLfloat *params); }
        void OVR::GLEContext::glMaterialfv_Hook(GLenum face, GLenum pname, const GLfloat *params)
        {
            glMaterialfv(face, pname, params);
            PostHook();
        }

        #undef glMateriali
        extern "C" { GLAPI void GLAPIENTRY glMateriali(GLenum face, GLenum pname, GLint param); }
        void OVR::GLEContext::glMateriali_Hook(GLenum face, GLenum pname, GLint param)
        {
            glMateriali(face, pname, param);
            PostHook();
        }

        #undef glMaterialiv
        extern "C" { GLAPI void GLAPIENTRY glMaterialiv(GLenum face, GLenum pname, const GLint *params); }
        void OVR::GLEContext::glMaterialiv_Hook(GLenum face, GLenum pname, const GLint *params)
        {
            glMaterialiv(face, pname, params);
            PostHook();
        }

        #undef glMatrixMode
        extern "C" { GLAPI void GLAPIENTRY glMatrixMode(GLenum mode); }
        void OVR::GLEContext::glMatrixMode_Hook(GLenum mode)
        {
            glMatrixMode(mode);
            PostHook();
        }

        #undef glMultMatrixd
        extern "C" { GLAPI void GLAPIENTRY glMultMatrixd(const GLdouble *m); }
        void OVR::GLEContext::glMultMatrixd_Hook(const GLdouble *m)
        {
            glMultMatrixd(m);
            PostHook();
        }

        #undef glMultMatrixf
        extern "C" { GLAPI void GLAPIENTRY glMultMatrixf(const GLfloat *m); }
        void OVR::GLEContext::glMultMatrixf_Hook(const GLfloat *m)
        {
            glMultMatrixf(m);
            PostHook();
        }

        #undef glNewList
        extern "C" { GLAPI void GLAPIENTRY glNewList(GLuint list, GLenum mode); }
        void OVR::GLEContext::glNewList_Hook(GLuint list, GLenum mode)
        {
            glNewList(list, mode);
            PostHook();
        }

        #undef glNormal3b
        extern "C" { GLAPI void GLAPIENTRY glNormal3b(GLbyte nx, GLbyte ny, GLbyte nz); }
        void OVR::GLEContext::glNormal3b_Hook(GLbyte nx, GLbyte ny, GLbyte nz)
        {
            glNormal3b(nx, ny, nz);
            PostHook();
        }

        #undef glNormal3bv
        extern "C" { GLAPI void GLAPIENTRY glNormal3bv(const GLbyte *v); }
        void OVR::GLEContext::glNormal3bv_Hook(const GLbyte *v)
        {
            glNormal3bv(v);
            PostHook();
        }

        #undef glNormal3d
        extern "C" { GLAPI void GLAPIENTRY glNormal3d(GLdouble nx, GLdouble ny, GLdouble nz); }
        void OVR::GLEContext::glNormal3d_Hook(GLdouble nx, GLdouble ny, GLdouble nz)
        {
            glNormal3d(nx, ny, nz);
            PostHook();
        }

        #undef glNormal3dv
        extern "C" { GLAPI void GLAPIENTRY glNormal3dv(const GLdouble *v); }
        void OVR::GLEContext::glNormal3dv_Hook(const GLdouble *v)
        {
            glNormal3dv(v);
            PostHook();
        }

        #undef glNormal3f
        extern "C" { GLAPI void GLAPIENTRY glNormal3f(GLfloat nx, GLfloat ny, GLfloat nz); }
        void OVR::GLEContext::glNormal3f_Hook(GLfloat nx, GLfloat ny, GLfloat nz)
        {
            glNormal3f(nx, ny, nz);
            PostHook();
        }

        #undef glNormal3fv
        extern "C" { GLAPI void GLAPIENTRY glNormal3fv(const GLfloat *v); }
        void OVR::GLEContext::glNormal3fv_Hook(const GLfloat *v)
        {
            glNormal3fv(v);
            PostHook();
        }

        #undef glNormal3i
        extern "C" { GLAPI void GLAPIENTRY glNormal3i(GLint nx, GLint ny, GLint nz); }
        void OVR::GLEContext::glNormal3i_Hook(GLint nx, GLint ny, GLint nz)
        {
            glNormal3i(nx, ny, nz);
            PostHook();
        }

        #undef glNormal3iv
        extern "C" { GLAPI void GLAPIENTRY glNormal3iv(const GLint *v); }
        void OVR::GLEContext::glNormal3iv_Hook(const GLint *v)
        {
            glNormal3iv(v);
            PostHook();
        }

        #undef glNormal3s
        extern "C" { GLAPI void GLAPIENTRY glNormal3s(GLshort nx, GLshort ny, GLshort nz); }
        void OVR::GLEContext::glNormal3s_Hook(GLshort nx, GLshort ny, GLshort nz)
        {
            glNormal3s(nx, ny, nz);
            PostHook();
        }

        #undef glNormal3sv
        extern "C" { GLAPI void GLAPIENTRY glNormal3sv(const GLshort *v); }
        void OVR::GLEContext::glNormal3sv_Hook(const GLshort *v)
        {
            glNormal3sv(v);
            PostHook();
        }

        #undef glNormalPointer
        extern "C" { GLAPI void GLAPIENTRY glNormalPointer(GLenum type, GLsizei stride, const void *pointer); }
        void OVR::GLEContext::glNormalPointer_Hook(GLenum type, GLsizei stride, const void *pointer)
        {
            glNormalPointer(type, stride, pointer);
            PostHook();
        }

        #undef glOrtho
        extern "C" { GLAPI void GLAPIENTRY glOrtho(GLdouble left, GLdouble right, GLdouble bottom, GLdouble top, GLdouble zNear, GLdouble zFar); }
        void OVR::GLEContext::glOrtho_Hook(GLdouble left, GLdouble right, GLdouble bottom, GLdouble top, GLdouble zNear, GLdouble zFar)
        {
            glOrtho(left, right, bottom, top, zNear, zFar);
            PostHook();
        }

        #undef glPassThrough
        extern "C" { GLAPI void GLAPIENTRY glPassThrough(GLfloat token); }
        void OVR::GLEContext::glPassThrough_Hook(GLfloat token)
        {
            glPassThrough(token);
            PostHook();
        }

        #undef glPixelMapfv
        extern "C" { GLAPI void GLAPIENTRY glPixelMapfv(GLenum map, GLsizei mapsize, const GLfloat *values); }
        void OVR::GLEContext::glPixelMapfv_Hook(GLenum map, GLsizei mapsize, const GLfloat *values)
        {
            glPixelMapfv(map, mapsize, values);
            PostHook();
        }

        #undef glPixelMapuiv
        extern "C" { GLAPI void GLAPIENTRY glPixelMapuiv(GLenum map, GLsizei mapsize, const GLuint *values); }
        void OVR::GLEContext::glPixelMapuiv_Hook(GLenum map, GLsizei mapsize, const GLuint *values)
        {
            glPixelMapuiv(map, mapsize, values);
            PostHook();
        }

        #undef glPixelMapusv
        extern "C" { GLAPI void GLAPIENTRY glPixelMapusv(GLenum map, GLsizei mapsize, const GLushort *values); }
        void OVR::GLEContext::glPixelMapusv_Hook(GLenum map, GLsizei mapsize, const GLushort *values)
        {
            glPixelMapusv(map, mapsize, values);
            PostHook();
        }

        #undef glPixelStoref
        extern "C" { GLAPI void GLAPIENTRY glPixelStoref(GLenum pname, GLfloat param); }
        void OVR::GLEContext::glPixelStoref_Hook(GLenum pname, GLfloat param)
        {
            glPixelStoref(pname, param);
            PostHook();
        }

        #undef glPixelStorei
        extern "C" { GLAPI void GLAPIENTRY glPixelStorei(GLenum pname, GLint param); }
        void OVR::GLEContext::glPixelStorei_Hook(GLenum pname, GLint param)
        {
            glPixelStorei(pname, param);
            PostHook();
        }

        #undef glPixelTransferf
        extern "C" { GLAPI void GLAPIENTRY glPixelTransferf(GLenum pname, GLfloat param); }
        void OVR::GLEContext::glPixelTransferf_Hook(GLenum pname, GLfloat param)
        {
            glPixelTransferf(pname, param);
            PostHook();
        }

        #undef glPixelTransferi
        extern "C" { GLAPI void GLAPIENTRY glPixelTransferi(GLenum pname, GLint param); }
        void OVR::GLEContext::glPixelTransferi_Hook(GLenum pname, GLint param)
        {
            glPixelTransferi(pname, param);
            PostHook();
        }

        #undef glPixelZoom
        extern "C" { GLAPI void GLAPIENTRY glPixelZoom(GLfloat xfactor, GLfloat yfactor); }
        void OVR::GLEContext::glPixelZoom_Hook(GLfloat xfactor, GLfloat yfactor)
        {
            glPixelZoom(xfactor, yfactor);
            PostHook();
        }

        #undef glPointSize
        extern "C" { GLAPI void GLAPIENTRY glPointSize(GLfloat size); }
        void OVR::GLEContext::glPointSize_Hook(GLfloat size)
        {
            glPointSize(size);
            PostHook();
        }

        #undef glPolygonMode
        extern "C" { GLAPI void GLAPIENTRY glPolygonMode(GLenum face, GLenum mode); }
        void OVR::GLEContext::glPolygonMode_Hook(GLenum face, GLenum mode)
        {
            glPolygonMode(face, mode);
            PostHook();
        }

        #undef glPolygonOffset
        extern "C" { GLAPI void GLAPIENTRY glPolygonOffset(GLfloat factor, GLfloat units); }
        void OVR::GLEContext::glPolygonOffset_Hook(GLfloat factor, GLfloat units)
        {
            glPolygonOffset(factor, units);
            PostHook();
        }

        #undef glPolygonStipple
        extern "C" { GLAPI void GLAPIENTRY glPolygonStipple(const GLubyte *mask); }
        void OVR::GLEContext::glPolygonStipple_Hook(const GLubyte *mask)
        {
            glPolygonStipple(mask);
            PostHook();
        }

        #undef glPopAttrib
        extern "C" { GLAPI void GLAPIENTRY glPopAttrib(); }
        void OVR::GLEContext::glPopAttrib_Hook()
        {
            glPopAttrib();
            PostHook();
        }

        #undef glPopClientAttrib
        extern "C" { GLAPI void GLAPIENTRY glPopClientAttrib(); }
        void OVR::GLEContext::glPopClientAttrib_Hook()
        {
            glPopClientAttrib();
            PostHook();
        }

        #undef glPopMatrix
        extern "C" { GLAPI void GLAPIENTRY glPopMatrix(); }
        void OVR::GLEContext::glPopMatrix_Hook()
        {
            glPopMatrix();
            PostHook();
        }

        #undef glPopName
        extern "C" { GLAPI void GLAPIENTRY glPopName(); }
        void OVR::GLEContext::glPopName_Hook()
        {
            glPopName();
            PostHook();
        }

        #undef glPrioritizeTextures
        extern "C" { GLAPI void GLAPIENTRY glPrioritizeTextures(GLsizei n, const GLuint *textures, const GLclampf *priorities); }
        void OVR::GLEContext::glPrioritizeTextures_Hook(GLsizei n, const GLuint *textures, const GLclampf *priorities)
        {
            glPrioritizeTextures(n, textures, priorities);
            PostHook();
        }

        #undef glPushAttrib
        extern "C" { GLAPI void GLAPIENTRY glPushAttrib(GLbitfield mask); }
        void OVR::GLEContext::glPushAttrib_Hook(GLbitfield mask)
        {
            glPushAttrib(mask);
            PostHook();
        }

        #undef glPushClientAttrib
        extern "C" { GLAPI void GLAPIENTRY glPushClientAttrib(GLbitfield mask); }
        void OVR::GLEContext::glPushClientAttrib_Hook(GLbitfield mask)
        {
            glPushClientAttrib(mask);
            PostHook();
        }

        #undef glPushMatrix
        extern "C" { GLAPI void GLAPIENTRY glPushMatrix(); }
        void OVR::GLEContext::glPushMatrix_Hook()
        {
            glPushMatrix();
            PostHook();
        }

        #undef glPushName
        extern "C" { GLAPI void GLAPIENTRY glPushName(GLuint name); }
        void OVR::GLEContext::glPushName_Hook(GLuint name)
        {
            glPushName(name);
            PostHook();
        }

        #undef glRasterPos2d
        extern "C" { GLAPI void GLAPIENTRY glRasterPos2d(GLdouble x, GLdouble y); }
        void OVR::GLEContext::glRasterPos2d_Hook(GLdouble x, GLdouble y)
        {
            glRasterPos2d(x, y);
            PostHook();
        }

        #undef glRasterPos2dv
        extern "C" { GLAPI void GLAPIENTRY glRasterPos2dv(const GLdouble *v); }
        void OVR::GLEContext::glRasterPos2dv_Hook(const GLdouble *v)
        {
            glRasterPos2dv(v);
            PostHook();
        }

        #undef glRasterPos2f
        extern "C" { GLAPI void GLAPIENTRY glRasterPos2f(GLfloat x, GLfloat y); }
        void OVR::GLEContext::glRasterPos2f_Hook(GLfloat x, GLfloat y)
        {
            glRasterPos2f(x, y);
            PostHook();
        }

        #undef glRasterPos2fv
        extern "C" { GLAPI void GLAPIENTRY glRasterPos2fv(const GLfloat *v); }
        void OVR::GLEContext::glRasterPos2fv_Hook(const GLfloat *v)
        {
            glRasterPos2fv(v);
            PostHook();
        }

        #undef glRasterPos2i
        extern "C" { GLAPI void GLAPIENTRY glRasterPos2i(GLint x, GLint y); }
        void OVR::GLEContext::glRasterPos2i_Hook(GLint x, GLint y)
        {
            glRasterPos2i(x, y);
            PostHook();
        }

        #undef glRasterPos2iv
        extern "C" { GLAPI void GLAPIENTRY glRasterPos2iv(const GLint *v); }
        void OVR::GLEContext::glRasterPos2iv_Hook(const GLint *v)
        {
            glRasterPos2iv(v);
            PostHook();
        }

        #undef glRasterPos2s
        extern "C" { GLAPI void GLAPIENTRY glRasterPos2s(GLshort x, GLshort y); }
        void OVR::GLEContext::glRasterPos2s_Hook(GLshort x, GLshort y)
        {
            glRasterPos2s(x, y);
            PostHook();
        }

        #undef glRasterPos2sv
        extern "C" { GLAPI void GLAPIENTRY glRasterPos2sv(const GLshort *v); }
        void OVR::GLEContext::glRasterPos2sv_Hook(const GLshort *v)
        {
            glRasterPos2sv(v);
            PostHook();
        }

        #undef glRasterPos3d
        extern "C" { GLAPI void GLAPIENTRY glRasterPos3d(GLdouble x, GLdouble y, GLdouble z); }
        void OVR::GLEContext::glRasterPos3d_Hook(GLdouble x, GLdouble y, GLdouble z)
        {
            glRasterPos3d(x, y, z);
            PostHook();
        }

        #undef glRasterPos3dv
        extern "C" { GLAPI void GLAPIENTRY glRasterPos3dv(const GLdouble *v); }
        void OVR::GLEContext::glRasterPos3dv_Hook(const GLdouble *v)
        {
            glRasterPos3dv(v);
            PostHook();
        }

        #undef glRasterPos3f
        extern "C" { GLAPI void GLAPIENTRY glRasterPos3f(GLfloat x, GLfloat y, GLfloat z); }
        void OVR::GLEContext::glRasterPos3f_Hook(GLfloat x, GLfloat y, GLfloat z)
        {
            glRasterPos3f(x, y, z);
            PostHook();
        }

        #undef glRasterPos3fv
        extern "C" { GLAPI void GLAPIENTRY glRasterPos3fv(const GLfloat *v); }
        void OVR::GLEContext::glRasterPos3fv_Hook(const GLfloat *v)
        {
            glRasterPos3fv(v);
            PostHook();
        }

        #undef glRasterPos3i
        extern "C" { GLAPI void GLAPIENTRY glRasterPos3i(GLint x, GLint y, GLint z); }
        void OVR::GLEContext::glRasterPos3i_Hook(GLint x, GLint y, GLint z)
        {
            glRasterPos3i(x, y, z);
            PostHook();
        }

        #undef glRasterPos3iv
        extern "C" { GLAPI void GLAPIENTRY glRasterPos3iv(const GLint *v); }
        void OVR::GLEContext::glRasterPos3iv_Hook(const GLint *v)
        {
            glRasterPos3iv(v);
            PostHook();
        }

        #undef glRasterPos3s
        extern "C" { GLAPI void GLAPIENTRY glRasterPos3s(GLshort x, GLshort y, GLshort z); }
        void OVR::GLEContext::glRasterPos3s_Hook(GLshort x, GLshort y, GLshort z)
        {
            glRasterPos3s(x, y, z);
            PostHook();
        }

        #undef glRasterPos3sv
        extern "C" { GLAPI void GLAPIENTRY glRasterPos3sv(const GLshort *v); }
        void OVR::GLEContext::glRasterPos3sv_Hook(const GLshort *v)
        {
            glRasterPos3sv(v);
            PostHook();
        }

        #undef glRasterPos4d
        extern "C" { GLAPI void GLAPIENTRY glRasterPos4d(GLdouble x, GLdouble y, GLdouble z, GLdouble w); }
        void OVR::GLEContext::glRasterPos4d_Hook(GLdouble x, GLdouble y, GLdouble z, GLdouble w)
        {
            glRasterPos4d(x, y, z, w);
            PostHook();
        }

        #undef glRasterPos4dv
        extern "C" { GLAPI void GLAPIENTRY glRasterPos4dv(const GLdouble *v); }
        void OVR::GLEContext::glRasterPos4dv_Hook(const GLdouble *v)
        {
            glRasterPos4dv(v);
            PostHook();
        }

        #undef glRasterPos4f
        extern "C" { GLAPI void GLAPIENTRY glRasterPos4f(GLfloat x, GLfloat y, GLfloat z, GLfloat w); }
        void OVR::GLEContext::glRasterPos4f_Hook(GLfloat x, GLfloat y, GLfloat z, GLfloat w)
        {
            glRasterPos4f(x, y, z, w);
            PostHook();
        }

        #undef glRasterPos4fv
        extern "C" { GLAPI void GLAPIENTRY glRasterPos4fv(const GLfloat *v); }
        void OVR::GLEContext::glRasterPos4fv_Hook(const GLfloat *v)
        {
            glRasterPos4fv(v);
            PostHook();
        }

        #undef glRasterPos4i
        extern "C" { GLAPI void GLAPIENTRY glRasterPos4i(GLint x, GLint y, GLint z, GLint w); }
        void OVR::GLEContext::glRasterPos4i_Hook(GLint x, GLint y, GLint z, GLint w)
        {
            glRasterPos4i(x, y, z, w);
            PostHook();
        }

        #undef glRasterPos4iv
        extern "C" { GLAPI void GLAPIENTRY glRasterPos4iv(const GLint *v); }
        void OVR::GLEContext::glRasterPos4iv_Hook(const GLint *v)
        {
            glRasterPos4iv(v);
            PostHook();
        }

        #undef glRasterPos4s
        extern "C" { GLAPI void GLAPIENTRY glRasterPos4s(GLshort x, GLshort y, GLshort z, GLshort w); }
        void OVR::GLEContext::glRasterPos4s_Hook(GLshort x, GLshort y, GLshort z, GLshort w)
        {
            glRasterPos4s(x, y, z, w);
            PostHook();
        }

        #undef glRasterPos4sv
        extern "C" { GLAPI void GLAPIENTRY glRasterPos4sv(const GLshort *v); }
        void OVR::GLEContext::glRasterPos4sv_Hook(const GLshort *v)
        {
            glRasterPos4sv(v);
            PostHook();
        }

        #undef glReadBuffer
        extern "C" { GLAPI void GLAPIENTRY glReadBuffer(GLenum mode); }
        void OVR::GLEContext::glReadBuffer_Hook(GLenum mode)
        {
            glReadBuffer(mode);
            PostHook();
        }

        #undef glReadPixels
        extern "C" { GLAPI void GLAPIENTRY glReadPixels(GLint x, GLint y, GLsizei width, GLsizei height, GLenum format, GLenum type, void *pixels); }
        void OVR::GLEContext::glReadPixels_Hook(GLint x, GLint y, GLsizei width, GLsizei height, GLenum format, GLenum type, void *pixels)
        {
            glReadPixels(x, y, width, height, format, type, pixels);
            PostHook();
        }

        #undef glRectd
        extern "C" { GLAPI void GLAPIENTRY glRectd(GLdouble x1, GLdouble y1, GLdouble x2, GLdouble y2); }
        void OVR::GLEContext::glRectd_Hook(GLdouble x1, GLdouble y1, GLdouble x2, GLdouble y2)
        {
            glRectd(x1, y1, x2, y2);
            PostHook();
        }

        #undef glRectdv
        extern "C" { GLAPI void GLAPIENTRY glRectdv(const GLdouble *v1, const GLdouble *v2); }
        void OVR::GLEContext::glRectdv_Hook(const GLdouble *v1, const GLdouble *v2)
        {
            glRectdv(v1, v2);
            PostHook();
        }

        #undef glRectf
        extern "C" { GLAPI void GLAPIENTRY glRectf(GLfloat x1, GLfloat y1, GLfloat x2, GLfloat y2); }
        void OVR::GLEContext::glRectf_Hook(GLfloat x1, GLfloat y1, GLfloat x2, GLfloat y2)
        {
            glRectf(x1, y1, x2, y2);
            PostHook();
        }

        #undef glRectfv
        extern "C" { GLAPI void GLAPIENTRY glRectfv(const GLfloat *v1, const GLfloat *v2); }
        void OVR::GLEContext::glRectfv_Hook(const GLfloat *v1, const GLfloat *v2)
        {
            glRectfv(v1, v2);
            PostHook();
        }

        #undef glRecti
        extern "C" { GLAPI void GLAPIENTRY glRecti(GLint x1, GLint y1, GLint x2, GLint y2); }
        void OVR::GLEContext::glRecti_Hook(GLint x1, GLint y1, GLint x2, GLint y2)
        {
            glRecti(x1, y1, x2, y2);
            PostHook();
        }

        #undef glRectiv
        extern "C" { GLAPI void GLAPIENTRY glRectiv(const GLint *v1, const GLint *v2); }
        void OVR::GLEContext::glRectiv_Hook(const GLint *v1, const GLint *v2)
        {
            glRectiv(v1, v2);
            PostHook();
        }

        #undef glRects
        extern "C" { GLAPI void GLAPIENTRY glRects(GLshort x1, GLshort y1, GLshort x2, GLshort y2); }
        void OVR::GLEContext::glRects_Hook(GLshort x1, GLshort y1, GLshort x2, GLshort y2)
        {
            glRects(x1, y1, x2, y2);
            PostHook();
        }

        #undef glRectsv
        extern "C" { GLAPI void GLAPIENTRY glRectsv(const GLshort *v1, const GLshort *v2); }
        void OVR::GLEContext::glRectsv_Hook(const GLshort *v1, const GLshort *v2)
        {
            glRectsv(v1, v2);
            PostHook();
        }

        #undef glRenderMode
        extern "C" { GLAPI GLint GLAPIENTRY glRenderMode(GLenum mode); }
        GLint OVR::GLEContext::glRenderMode_Hook(GLenum mode)
        {
            GLint  i = glRenderMode(mode);
            PostHook();
            return i;
        }

        #undef glRotated
        extern "C" { GLAPI void GLAPIENTRY glRotated(GLdouble angle, GLdouble x, GLdouble y, GLdouble z); }
        void OVR::GLEContext::glRotated_Hook(GLdouble angle, GLdouble x, GLdouble y, GLdouble z)
        {
            glRotated(angle, x, y, z);
            PostHook();
        }

        #undef glRotatef
        extern "C" { GLAPI void GLAPIENTRY glRotatef(GLfloat angle, GLfloat x, GLfloat y, GLfloat z); }
        void OVR::GLEContext::glRotatef_Hook(GLfloat angle, GLfloat x, GLfloat y, GLfloat z)
        {
            glRotatef(angle, x, y, z);
            PostHook();
        }

        #undef glScaled
        extern "C" { GLAPI void GLAPIENTRY glScaled(GLdouble x, GLdouble y, GLdouble z); }
        void OVR::GLEContext::glScaled_Hook(GLdouble x, GLdouble y, GLdouble z)
        {
            glScaled(x, y, z);
            PostHook();
        }

        #undef glScalef
        extern "C" { GLAPI void GLAPIENTRY glScalef(GLfloat x, GLfloat y, GLfloat z); }
        void OVR::GLEContext::glScalef_Hook(GLfloat x, GLfloat y, GLfloat z)
        {
            glScalef(x, y, z);
            PostHook();
        }

        #undef glScissor
        extern "C" { GLAPI void GLAPIENTRY glScissor(GLint x, GLint y, GLsizei width, GLsizei height); }
        void OVR::GLEContext::glScissor_Hook(GLint x, GLint y, GLsizei width, GLsizei height)
        {
            glScissor(x, y, width, height);
            PostHook();
        }

        #undef glSelectBuffer
        extern "C" { GLAPI void GLAPIENTRY glSelectBuffer(GLsizei size, GLuint *buffer); }
        void OVR::GLEContext::glSelectBuffer_Hook(GLsizei size, GLuint *buffer)
        {
            glSelectBuffer(size, buffer);
            PostHook();
        }

        #undef glShadeModel
        extern "C" { GLAPI void GLAPIENTRY glShadeModel(GLenum mode); }
        void OVR::GLEContext::glShadeModel_Hook(GLenum mode)
        {
            glShadeModel(mode);
            PostHook();
        }

        #undef glStencilFunc
        extern "C" { GLAPI void GLAPIENTRY glStencilFunc(GLenum func, GLint ref, GLuint mask); }
        void OVR::GLEContext::glStencilFunc_Hook(GLenum func, GLint ref, GLuint mask)
        {
            glStencilFunc(func, ref, mask);
            PostHook();
        }

        #undef glStencilMask
        extern "C" { GLAPI void GLAPIENTRY glStencilMask(GLuint mask); }
        void OVR::GLEContext::glStencilMask_Hook(GLuint mask)
        {
            glStencilMask(mask);
            PostHook();
        }

        #undef glStencilOp
        extern "C" { GLAPI void GLAPIENTRY glStencilOp(GLenum fail, GLenum zfail, GLenum zpass); }
        void OVR::GLEContext::glStencilOp_Hook(GLenum fail, GLenum zfail, GLenum zpass)
        {
            glStencilOp(fail, zfail, zpass);
            PostHook();
        }

        #undef glTexCoord1d
        extern "C" { GLAPI void GLAPIENTRY glTexCoord1d(GLdouble s); }
        void OVR::GLEContext::glTexCoord1d_Hook(GLdouble s)
        {
            glTexCoord1d(s);
            PostHook();
        }

        #undef glTexCoord1dv
        extern "C" { GLAPI void GLAPIENTRY glTexCoord1dv(const GLdouble *v); }
        void OVR::GLEContext::glTexCoord1dv_Hook(const GLdouble *v)
        {
            glTexCoord1dv(v);
            PostHook();
        }

        #undef glTexCoord1f
        extern "C" { GLAPI void GLAPIENTRY glTexCoord1f(GLfloat s); }
        void OVR::GLEContext::glTexCoord1f_Hook(GLfloat s)
        {
            glTexCoord1f(s);
            PostHook();
        }

        #undef glTexCoord1fv
        extern "C" { GLAPI void GLAPIENTRY glTexCoord1fv(const GLfloat *v); }
        void OVR::GLEContext::glTexCoord1fv_Hook(const GLfloat *v)
        {
            glTexCoord1fv(v);
            PostHook();
        }

        #undef glTexCoord1i
        extern "C" { GLAPI void GLAPIENTRY glTexCoord1i(GLint s); }
        void OVR::GLEContext::glTexCoord1i_Hook(GLint s)
        {
            glTexCoord1i(s);
            PostHook();
        }

        #undef glTexCoord1iv
        extern "C" { GLAPI void GLAPIENTRY glTexCoord1iv(const GLint *v); }
        void OVR::GLEContext::glTexCoord1iv_Hook(const GLint *v)
        {
            glTexCoord1iv(v);
            PostHook();
        }

        #undef glTexCoord1s
        extern "C" { GLAPI void GLAPIENTRY glTexCoord1s(GLshort s); }
        void OVR::GLEContext::glTexCoord1s_Hook(GLshort s)
        {
            glTexCoord1s(s);
            PostHook();
        }

        #undef glTexCoord1sv
        extern "C" { GLAPI void GLAPIENTRY glTexCoord1sv(const GLshort *v); }
        void OVR::GLEContext::glTexCoord1sv_Hook(const GLshort *v)
        {
            glTexCoord1sv(v);
            PostHook();
        }

        #undef glTexCoord2d
        extern "C" { GLAPI void GLAPIENTRY glTexCoord2d(GLdouble s, GLdouble t); }
        void OVR::GLEContext::glTexCoord2d_Hook(GLdouble s, GLdouble t)
        {
            glTexCoord2d(s, t);
            PostHook();
        }

        #undef glTexCoord2dv
        extern "C" { GLAPI void GLAPIENTRY glTexCoord2dv(const GLdouble *v); }
        void OVR::GLEContext::glTexCoord2dv_Hook(const GLdouble *v)
        {
            glTexCoord2dv(v);
            PostHook();
        }

        #undef glTexCoord2f
        extern "C" { GLAPI void GLAPIENTRY glTexCoord2f(GLfloat s, GLfloat t); }
        void OVR::GLEContext::glTexCoord2f_Hook(GLfloat s, GLfloat t)
        {
            glTexCoord2f(s, t);
            PostHook();
        }

        #undef glTexCoord2fv
        extern "C" { GLAPI void GLAPIENTRY glTexCoord2fv(const GLfloat *v); }
        void OVR::GLEContext::glTexCoord2fv_Hook(const GLfloat *v)
        {
            glTexCoord2fv(v);
            PostHook();
        }

        #undef glTexCoord2i
        extern "C" { GLAPI void GLAPIENTRY glTexCoord2i(GLint s, GLint t); }
        void OVR::GLEContext::glTexCoord2i_Hook(GLint s, GLint t)
        {
            glTexCoord2i(s, t);
            PostHook();
        }

        #undef glTexCoord2iv
        extern "C" { GLAPI void GLAPIENTRY glTexCoord2iv(const GLint *v); }
        void OVR::GLEContext::glTexCoord2iv_Hook(const GLint *v)
        {
            glTexCoord2iv(v);
            PostHook();
        }

        #undef glTexCoord2s
        extern "C" { GLAPI void GLAPIENTRY glTexCoord2s(GLshort s, GLshort t); }
        void OVR::GLEContext::glTexCoord2s_Hook(GLshort s, GLshort t)
        {
            glTexCoord2s(s, t);
            PostHook();
        }

        #undef glTexCoord2sv
        extern "C" { GLAPI void GLAPIENTRY glTexCoord2sv(const GLshort *v); }
        void OVR::GLEContext::glTexCoord2sv_Hook(const GLshort *v)
        {
            glTexCoord2sv(v);
            PostHook();
        }

        #undef glTexCoord3d
        extern "C" { GLAPI void GLAPIENTRY glTexCoord3d(GLdouble s, GLdouble t, GLdouble r); }
        void OVR::GLEContext::glTexCoord3d_Hook(GLdouble s, GLdouble t, GLdouble r)
        {
            glTexCoord3d(s, t, r);
            PostHook();
        }

        #undef glTexCoord3dv
        extern "C" { GLAPI void GLAPIENTRY glTexCoord3dv(const GLdouble *v); }
        void OVR::GLEContext::glTexCoord3dv_Hook(const GLdouble *v)
        {
            glTexCoord3dv(v);
            PostHook();
        }

        #undef glTexCoord3f
        extern "C" { GLAPI void GLAPIENTRY glTexCoord3f(GLfloat s, GLfloat t, GLfloat r); }
        void OVR::GLEContext::glTexCoord3f_Hook(GLfloat s, GLfloat t, GLfloat r)
        {
            glTexCoord3f(s, t, r);
            PostHook();
        }

        #undef glTexCoord3fv
        extern "C" { GLAPI void GLAPIENTRY glTexCoord3fv(const GLfloat *v); }
        void OVR::GLEContext::glTexCoord3fv_Hook(const GLfloat *v)
        {
            glTexCoord3fv(v);
            PostHook();
        }

        #undef glTexCoord3i
        extern "C" { GLAPI void GLAPIENTRY glTexCoord3i(GLint s, GLint t, GLint r); }
        void OVR::GLEContext::glTexCoord3i_Hook(GLint s, GLint t, GLint r)
        {
            glTexCoord3i(s, t, r);
            PostHook();
        }

        #undef glTexCoord3iv
        extern "C" { GLAPI void GLAPIENTRY glTexCoord3iv(const GLint *v); }
        void OVR::GLEContext::glTexCoord3iv_Hook(const GLint *v)
        {
            glTexCoord3iv(v);
            PostHook();
        }

        #undef glTexCoord3s
        extern "C" { GLAPI void GLAPIENTRY glTexCoord3s(GLshort s, GLshort t, GLshort r); }
        void OVR::GLEContext::glTexCoord3s_Hook(GLshort s, GLshort t, GLshort r)
        {
            glTexCoord3s(s, t, r);
            PostHook();
        }

        #undef glTexCoord3sv
        extern "C" { GLAPI void GLAPIENTRY glTexCoord3sv(const GLshort *v); }
        void OVR::GLEContext::glTexCoord3sv_Hook(const GLshort *v)
        {
            glTexCoord3sv(v);
            PostHook();
        }

        #undef glTexCoord4d
        extern "C" { GLAPI void GLAPIENTRY glTexCoord4d(GLdouble s, GLdouble t, GLdouble r, GLdouble q); }
        void OVR::GLEContext::glTexCoord4d_Hook(GLdouble s, GLdouble t, GLdouble r, GLdouble q)
        {
            glTexCoord4d(s, t, r, q);
            PostHook();
        }

        #undef glTexCoord4dv
        extern "C" { GLAPI void GLAPIENTRY glTexCoord4dv(const GLdouble *v); }
        void OVR::GLEContext::glTexCoord4dv_Hook(const GLdouble *v)
        {
            glTexCoord4dv(v);
            PostHook();
        }

        #undef glTexCoord4f
        extern "C" { GLAPI void GLAPIENTRY glTexCoord4f(GLfloat s, GLfloat t, GLfloat r, GLfloat q); }
        void OVR::GLEContext::glTexCoord4f_Hook(GLfloat s, GLfloat t, GLfloat r, GLfloat q)
        {
            glTexCoord4f(s, t, r, q);
            PostHook();
        }

        #undef glTexCoord4fv
        extern "C" { GLAPI void GLAPIENTRY glTexCoord4fv(const GLfloat *v); }
        void OVR::GLEContext::glTexCoord4fv_Hook(const GLfloat *v)
        {
            glTexCoord4fv(v);
            PostHook();
        }

        #undef glTexCoord4i
        extern "C" { GLAPI void GLAPIENTRY glTexCoord4i(GLint s, GLint t, GLint r, GLint q); }
        void OVR::GLEContext::glTexCoord4i_Hook(GLint s, GLint t, GLint r, GLint q)
        {
            glTexCoord4i(s, t, r, q);
            PostHook();
        }

        #undef glTexCoord4iv
        extern "C" { GLAPI void GLAPIENTRY glTexCoord4iv(const GLint *v); }
        void OVR::GLEContext::glTexCoord4iv_Hook(const GLint *v)
        {
            glTexCoord4iv(v);
            PostHook();
        }

        #undef glTexCoord4s
        extern "C" { GLAPI void GLAPIENTRY glTexCoord4s(GLshort s, GLshort t, GLshort r, GLshort q); }
        void OVR::GLEContext::glTexCoord4s_Hook(GLshort s, GLshort t, GLshort r, GLshort q)
        {
            glTexCoord4s(s, t, r, q);
            PostHook();
        }

        #undef glTexCoord4sv
        extern "C" { GLAPI void GLAPIENTRY glTexCoord4sv(const GLshort *v); }
        void OVR::GLEContext::glTexCoord4sv_Hook(const GLshort *v)
        {
            glTexCoord4sv(v);
            PostHook();
        }

        #undef glTexCoordPointer
        extern "C" { GLAPI void GLAPIENTRY glTexCoordPointer(GLint size, GLenum type, GLsizei stride, const void *pointer); }
        void OVR::GLEContext::glTexCoordPointer_Hook(GLint size, GLenum type, GLsizei stride, const void *pointer)
        {
            glTexCoordPointer(size, type, stride, pointer);
            PostHook();
        }

        #undef glTexEnvf
        extern "C" { GLAPI void GLAPIENTRY glTexEnvf(GLenum target, GLenum pname, GLfloat param); }
        void OVR::GLEContext::glTexEnvf_Hook(GLenum target, GLenum pname, GLfloat param)
        {
            glTexEnvf(target, pname, param);
            PostHook();
        }

        #undef glTexEnvfv
        extern "C" { GLAPI void GLAPIENTRY glTexEnvfv(GLenum target, GLenum pname, const GLfloat *params); }
        void OVR::GLEContext::glTexEnvfv_Hook(GLenum target, GLenum pname, const GLfloat *params)
        {
            glTexEnvfv(target, pname, params);
            PostHook();
        }

        #undef glTexEnvi
        extern "C" { GLAPI void GLAPIENTRY glTexEnvi(GLenum target, GLenum pname, GLint param); }
        void OVR::GLEContext::glTexEnvi_Hook(GLenum target, GLenum pname, GLint param)
        {
            glTexEnvi(target, pname, param);
            PostHook();
        }

        #undef glTexEnviv
        extern "C" { GLAPI void GLAPIENTRY glTexEnviv(GLenum target, GLenum pname, const GLint *params); }
        void OVR::GLEContext::glTexEnviv_Hook(GLenum target, GLenum pname, const GLint *params)
        {
            glTexEnviv(target, pname, params);
            PostHook();
        }

        #undef glTexGend
        extern "C" { GLAPI void GLAPIENTRY glTexGend(GLenum coord, GLenum pname, GLdouble param); }
        void OVR::GLEContext::glTexGend_Hook(GLenum coord, GLenum pname, GLdouble param)
        {
            glTexGend(coord, pname, param);
            PostHook();
        }

        #undef glTexGendv
        extern "C" { GLAPI void GLAPIENTRY glTexGendv(GLenum coord, GLenum pname, const GLdouble *params); }
        void OVR::GLEContext::glTexGendv_Hook(GLenum coord, GLenum pname, const GLdouble *params)
        {
            glTexGendv(coord, pname, params);
            PostHook();
        }

        #undef glTexGenf
        extern "C" { GLAPI void GLAPIENTRY glTexGenf(GLenum coord, GLenum pname, GLfloat param); }
        void OVR::GLEContext::glTexGenf_Hook(GLenum coord, GLenum pname, GLfloat param)
        {
            glTexGenf(coord, pname, param);
            PostHook();
        }

        #undef glTexGenfv
        extern "C" { GLAPI void GLAPIENTRY glTexGenfv(GLenum coord, GLenum pname, const GLfloat *params); }
        void OVR::GLEContext::glTexGenfv_Hook(GLenum coord, GLenum pname, const GLfloat *params)
        {
            glTexGenfv(coord, pname, params);
            PostHook();
        }

        #undef glTexGeni
        extern "C" { GLAPI void GLAPIENTRY glTexGeni(GLenum coord, GLenum pname, GLint param); }
        void OVR::GLEContext::glTexGeni_Hook(GLenum coord, GLenum pname, GLint param)
        {
            glTexGeni(coord, pname, param);
            PostHook();
        }

        #undef glTexGeniv
        extern "C" { GLAPI void GLAPIENTRY glTexGeniv(GLenum coord, GLenum pname, const GLint *params); }
        void OVR::GLEContext::glTexGeniv_Hook(GLenum coord, GLenum pname, const GLint *params)
        {
            glTexGeniv(coord, pname, params);
            PostHook();
        }

        #undef glTexImage1D
        extern "C" { GLAPI void GLAPIENTRY glTexImage1D(GLenum target, GLint level, GLint internalformat, GLsizei width, GLint border, GLenum format, GLenum type, const void *pixels); }
        void OVR::GLEContext::glTexImage1D_Hook(GLenum target, GLint level, GLint internalformat, GLsizei width, GLint border, GLenum format, GLenum type, const void *pixels)
        {
            glTexImage1D(target, level, internalformat, width, border, format, type, pixels);
            PostHook();
        }

        #undef glTexImage2D
        extern "C" { GLAPI void GLAPIENTRY glTexImage2D(GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const void *pixels); }
        void OVR::GLEContext::glTexImage2D_Hook(GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const void *pixels)
        {
            glTexImage2D(target, level, internalformat, width, height, border, format, type, pixels);
            PostHook();
        }

        #undef glTexParameterf
        extern "C" { GLAPI void GLAPIENTRY glTexParameterf(GLenum target, GLenum pname, GLfloat param); }
        void OVR::GLEContext::glTexParameterf_Hook(GLenum target, GLenum pname, GLfloat param)
        {
            glTexParameterf(target, pname, param);
            PostHook();
        }

        #undef glTexParameterfv
        extern "C" { GLAPI void GLAPIENTRY glTexParameterfv(GLenum target, GLenum pname, const GLfloat *params); }
        void OVR::GLEContext::glTexParameterfv_Hook(GLenum target, GLenum pname, const GLfloat *params)
        {
            glTexParameterfv(target, pname, params);
            PostHook();
        }

        #undef glTexParameteri
        extern "C" { GLAPI void GLAPIENTRY glTexParameteri(GLenum target, GLenum pname, GLint param); }
        void OVR::GLEContext::glTexParameteri_Hook(GLenum target, GLenum pname, GLint param)
        {
            glTexParameteri(target, pname, param);
            PostHook();
        }

        #undef glTexParameteriv
        extern "C" { GLAPI void GLAPIENTRY glTexParameteriv(GLenum target, GLenum pname, const GLint *params); }
        void OVR::GLEContext::glTexParameteriv_Hook(GLenum target, GLenum pname, const GLint *params)
        {
            glTexParameteriv(target, pname, params);
            PostHook();
        }

        #undef glTexSubImage1D
        extern "C" { GLAPI void GLAPIENTRY glTexSubImage1D(GLenum target, GLint level, GLint xoffset, GLsizei width, GLenum format, GLenum type, const void *pixels); }
        void OVR::GLEContext::glTexSubImage1D_Hook(GLenum target, GLint level, GLint xoffset, GLsizei width, GLenum format, GLenum type, const void *pixels)
        {
            glTexSubImage1D(target, level, xoffset, width, format, type, pixels);
            PostHook();
        }

        #undef glTexSubImage2D
        extern "C" { GLAPI void GLAPIENTRY glTexSubImage2D(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const void *pixels); }
        void OVR::GLEContext::glTexSubImage2D_Hook(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const void *pixels)
        {
            glTexSubImage2D(target, level, xoffset, yoffset, width, height, format, type, pixels);
            PostHook();
        }

        #undef glTranslated
        extern "C" { GLAPI void GLAPIENTRY glTranslated(GLdouble x, GLdouble y, GLdouble z); }
        void OVR::GLEContext::glTranslated_Hook(GLdouble x, GLdouble y, GLdouble z)
        {
            glTranslated(x, y, z);
            PostHook();
        }

        #undef glTranslatef
        extern "C" { GLAPI void GLAPIENTRY glTranslatef(GLfloat x, GLfloat y, GLfloat z); }
        void OVR::GLEContext::glTranslatef_Hook(GLfloat x, GLfloat y, GLfloat z)
        {
            glTranslatef(x, y, z);
            PostHook();
        }

        #undef glVertex2d
        extern "C" { GLAPI void GLAPIENTRY glVertex2d(GLdouble x, GLdouble y); }
        void OVR::GLEContext::glVertex2d_Hook(GLdouble x, GLdouble y)
        {
            glVertex2d(x, y);
            PostHook();
        }

        #undef glVertex2dv
        extern "C" { GLAPI void GLAPIENTRY glVertex2dv(const GLdouble *v); }
        void OVR::GLEContext::glVertex2dv_Hook(const GLdouble *v)
        {
            glVertex2dv(v);
            PostHook();
        }

        #undef glVertex2f
        extern "C" { GLAPI void GLAPIENTRY glVertex2f(GLfloat x, GLfloat y); }
        void OVR::GLEContext::glVertex2f_Hook(GLfloat x, GLfloat y)
        {
            glVertex2f(x, y);
            PostHook();
        }

        #undef glVertex2fv
        extern "C" { GLAPI void GLAPIENTRY glVertex2fv(const GLfloat *v); }
        void OVR::GLEContext::glVertex2fv_Hook(const GLfloat *v)
        {
            glVertex2fv(v);
            PostHook();
        }

        #undef glVertex2i
        extern "C" { GLAPI void GLAPIENTRY glVertex2i(GLint x, GLint y); }
        void OVR::GLEContext::glVertex2i_Hook(GLint x, GLint y)
        {
            glVertex2i(x, y);
            PostHook();
        }

        #undef glVertex2iv
        extern "C" { GLAPI void GLAPIENTRY glVertex2iv(const GLint *v); }
        void OVR::GLEContext::glVertex2iv_Hook(const GLint *v)
        {
            glVertex2iv(v);
            PostHook();
        }

        #undef glVertex2s
        extern "C" { GLAPI void GLAPIENTRY glVertex2s(GLshort x, GLshort y); }
        void OVR::GLEContext::glVertex2s_Hook(GLshort x, GLshort y)
        {
            glVertex2s(x, y);
            PostHook();
        }

        #undef glVertex2sv
        extern "C" { GLAPI void GLAPIENTRY glVertex2sv(const GLshort *v); }
        void OVR::GLEContext::glVertex2sv_Hook(const GLshort *v)
        {
            glVertex2sv(v);
            PostHook();
        }

        #undef glVertex3d
        extern "C" { GLAPI void GLAPIENTRY glVertex3d(GLdouble x, GLdouble y, GLdouble z); }
        void OVR::GLEContext::glVertex3d_Hook(GLdouble x, GLdouble y, GLdouble z)
        {
            glVertex3d(x, y, z);
            PostHook();
        }

        #undef glVertex3dv
        extern "C" { GLAPI void GLAPIENTRY glVertex3dv(const GLdouble *v); }
        void OVR::GLEContext::glVertex3dv_Hook(const GLdouble *v)
        {
            glVertex3dv(v);
            PostHook();
        }

        #undef glVertex3f
        extern "C" { GLAPI void GLAPIENTRY glVertex3f(GLfloat x, GLfloat y, GLfloat z); }
        void OVR::GLEContext::glVertex3f_Hook(GLfloat x, GLfloat y, GLfloat z)
        {
            glVertex3f(x, y, z);
            PostHook();
        }

        #undef glVertex3fv
        extern "C" { GLAPI void GLAPIENTRY glVertex3fv(const GLfloat *v); }
        void OVR::GLEContext::glVertex3fv_Hook(const GLfloat *v)
        {
            glVertex3fv(v);
            PostHook();
        }

        #undef glVertex3i
        extern "C" { GLAPI void GLAPIENTRY glVertex3i(GLint x, GLint y, GLint z); }
        void OVR::GLEContext::glVertex3i_Hook(GLint x, GLint y, GLint z)
        {
            glVertex3i(x, y, z);
            PostHook();
        }

        #undef glVertex3iv
        extern "C" { GLAPI void GLAPIENTRY glVertex3iv(const GLint *v); }
        void OVR::GLEContext::glVertex3iv_Hook(const GLint *v)
        {
            glVertex3iv(v);
            PostHook();
        }

        #undef glVertex3s
        extern "C" { GLAPI void GLAPIENTRY glVertex3s(GLshort x, GLshort y, GLshort z); }
        void OVR::GLEContext::glVertex3s_Hook(GLshort x, GLshort y, GLshort z)
        {
            glVertex3s(x, y, z);
            PostHook();
        }

        #undef glVertex3sv
        extern "C" { GLAPI void GLAPIENTRY glVertex3sv(const GLshort *v); }
        void OVR::GLEContext::glVertex3sv_Hook(const GLshort *v)
        {
            glVertex3sv(v);
            PostHook();
        }

        #undef glVertex4d
        extern "C" { GLAPI void GLAPIENTRY glVertex4d(GLdouble x, GLdouble y, GLdouble z, GLdouble w); }
        void OVR::GLEContext::glVertex4d_Hook(GLdouble x, GLdouble y, GLdouble z, GLdouble w)
        {
            glVertex4d(x, y, z, w);
            PostHook();
        }

        #undef glVertex4dv
        extern "C" { GLAPI void GLAPIENTRY glVertex4dv(const GLdouble *v); }
        void OVR::GLEContext::glVertex4dv_Hook(const GLdouble *v)
        {
            glVertex4dv(v);
            PostHook();
        }

        #undef glVertex4f
        extern "C" { GLAPI void GLAPIENTRY glVertex4f(GLfloat x, GLfloat y, GLfloat z, GLfloat w); }
        void OVR::GLEContext::glVertex4f_Hook(GLfloat x, GLfloat y, GLfloat z, GLfloat w)
        {
            glVertex4f(x, y, z, w);
            PostHook();
        }

        #undef glVertex4fv
        extern "C" { GLAPI void GLAPIENTRY glVertex4fv(const GLfloat *v); }
        void OVR::GLEContext::glVertex4fv_Hook(const GLfloat *v)
        {
            glVertex4fv(v);
            PostHook();
        }

        #undef glVertex4i
        extern "C" { GLAPI void GLAPIENTRY glVertex4i(GLint x, GLint y, GLint z, GLint w); }
        void OVR::GLEContext::glVertex4i_Hook(GLint x, GLint y, GLint z, GLint w)
        {
            glVertex4i(x, y, z, w);
            PostHook();
        }

        #undef glVertex4iv
        extern "C" { GLAPI void GLAPIENTRY glVertex4iv(const GLint *v); }
        void OVR::GLEContext::glVertex4iv_Hook(const GLint *v)
        {
            glVertex4iv(v);
            PostHook();
        }

        #undef glVertex4s
        extern "C" { GLAPI void GLAPIENTRY glVertex4s(GLshort x, GLshort y, GLshort z, GLshort w); }
        void OVR::GLEContext::glVertex4s_Hook(GLshort x, GLshort y, GLshort z, GLshort w)
        {
            glVertex4s(x, y, z, w);
            PostHook();
        }

        #undef glVertex4sv
        extern "C" { GLAPI void GLAPIENTRY glVertex4sv(const GLshort *v); }
        void OVR::GLEContext::glVertex4sv_Hook(const GLshort *v)
        {
            glVertex4sv(v);
            PostHook();
        }

        #undef glVertexPointer
        extern "C" { GLAPI void GLAPIENTRY glVertexPointer(GLint size, GLenum type, GLsizei stride, const void *pointer); }
        void OVR::GLEContext::glVertexPointer_Hook(GLint size, GLenum type, GLsizei stride, const void *pointer)
        {
            glVertexPointer(size, type, stride, pointer);
            PostHook();
        }

        #undef glViewport
        extern "C" { GLAPI void GLAPIENTRY glViewport(GLint x, GLint y, GLsizei width, GLsizei height); }
        void OVR::GLEContext::glViewport_Hook(GLint x, GLint y, GLsizei width, GLsizei height)
        {
            glViewport(x, y, width, height);
            PostHook();
        }



        // Pointer-based functions
        void OVR::GLEContext::glBlendColor_Hook(GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha)
        {
            if(glBlendColor_Impl)
                glBlendColor_Impl(red, green, blue, alpha);
            PostHook();
        }

        void OVR::GLEContext::glBlendEquation_Hook(GLenum mode)
        {
            if(glBlendEquation_Impl)
                glBlendEquation_Impl(mode);
            PostHook();
        }

        void OVR::GLEContext::glDrawRangeElements_Hook(GLenum mode, GLuint start, GLuint end, GLsizei count, GLenum type, const GLvoid *indices)
        {
            if(glDrawRangeElements_Impl)
                glDrawRangeElements_Impl(mode, start, end, count, type, indices);
            PostHook();
        }

        void OVR::GLEContext::glTexImage3D_Hook(GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLsizei depth, GLint border, GLenum format, GLenum type, const GLvoid *pixels)
        {
            if(glTexImage3D_Impl)
                glTexImage3D_Impl(target, level, internalformat, width, height, depth, border, format, type, pixels);
            PostHook();
        }

        void OVR::GLEContext::glTexSubImage3D_Hook(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum type, const GLvoid *pixels)
        {
            if(glTexSubImage3D_Impl)
                glTexSubImage3D_Impl(target, level, xoffset, yoffset, zoffset, width, height, depth, format, type, pixels);
            PostHook();
        }


        void OVR::GLEContext::glCopyTexSubImage3D_Hook(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLint x, GLint y, GLsizei width, GLsizei height)
        {
            if(glCopyTexSubImage3D_Impl)
                glCopyTexSubImage3D_Impl(target, level, xoffset, yoffset, zoffset, x, y, width, height);
            PostHook();
        }

        // GL_VERSION_1_2 deprecated functions
        /* Not currently supported
        void OVR::GLEContext::glColorTable_Hook(GLenum target, GLenum internalformat, GLsizei width, GLenum format, GLenum type, const GLvoid *table)
        {
            if(glColorTable_Impl)
                glColorTable_Impl(target, internalformat, width, format, type, table);
            PostHook();
        }

        void OVR::GLEContext::glColorTableParameterfv_Hook(GLenum target, GLenum pname, const GLfloat *params)
        {
            if(glColorTableParameterfv_Impl)
                glColorTableParameterfv_Impl(target, pname, params);
            PostHook();
        }

        void OVR::GLEContext::glColorTableParameteriv_Hook(GLenum target, GLenum pname, const GLint *params)
        {
            if(glColorTableParameteriv_Impl)
                glColorTableParameteriv_Impl(target, pname, params);
            PostHook();
        }

        void OVR::GLEContext::glCopyColorTable_Hook(GLenum target, GLenum internalformat, GLint x, GLint y, GLsizei width)
        {
            if(glCopyColorTable_Impl)
                glCopyColorTable_Impl(target, internalformat, x, y, width);
            PostHook();
        }

        void OVR::GLEContext::glGetColorTable_Hook(GLenum target, GLenum format, GLenum type, GLvoid *table)
        {
            if(glGetColorTable_Impl)
                glGetColorTable_Impl(target, format, type, table);
            PostHook();
        }

        void OVR::GLEContext::glGetColorTableParameterfv_Hook(GLenum target, GLenum pname, GLfloat *params)
        {
            if(glGetColorTableParameterfv_Impl)
                glGetColorTableParameterfv_Impl(target, pname, params);
            PostHook();
        }

        void OVR::GLEContext::glGetColorTableParameteriv_Hook(GLenum target, GLenum pname, GLint *params)
        {
            if(glGetColorTableParameteriv_Impl)
                glGetColorTableParameteriv_Impl(target, pname, params);
            PostHook();
        }

        void OVR::GLEContext::glColorSubTable_Hook(GLenum target, GLsizei start, GLsizei count, GLenum format, GLenum type, const GLvoid *data)
        {
            if(glColorSubTable_Impl)
                glColorSubTable_Impl(target, start, count, format, type, data);
            PostHook();
        }

        void OVR::GLEContext::glCopyColorSubTable_Hook(GLenum target, GLsizei start, GLint x, GLint y, GLsizei width)
        {
            if(glCopyColorSubTable_Impl)
                glCopyColorSubTable_Impl(target, start, x, y, width);
            PostHook();
        }

        void OVR::GLEContext::glConvolutionFilter1D_Hook(GLenum target, GLenum internalformat, GLsizei width, GLenum format, GLenum type, const GLvoid *image)
        {
            if(glConvolutionFilter1D_Impl)
                glConvolutionFilter1D_Impl(target, internalformat, width, format, type, image);
            PostHook();
        }

        void OVR::GLEContext::glConvolutionFilter2D_Hook(GLenum target, GLenum internalformat, GLsizei width, GLsizei height, GLenum format, GLenum type, const GLvoid *image)
        {
            if(glConvolutionFilter2D_Impl)
                glConvolutionFilter2D_Impl(target, internalformat, width, height, format, type, image);
            PostHook();
        }

        void OVR::GLEContext::glConvolutionParameterf_Hook(GLenum target, GLenum pname, GLfloat params)
        {
            if(glConvolutionParameterf_Impl)
                glConvolutionParameterf_Impl(target, pname, params);
            PostHook();
        }

        void OVR::GLEContext::glConvolutionParameterfv_Hook(GLenum target, GLenum pname, const GLfloat *params)
        {
            if(glConvolutionParameterfv_Impl)
                glConvolutionParameterfv_Impl(target, pname, params);
            PostHook();
        }

        void OVR::GLEContext::glConvolutionParameteri_Hook(GLenum target, GLenum pname, GLint params)
        {
            if(glConvolutionParameteri_Impl)
                glConvolutionParameteri_Impl(target, pname, params);
            PostHook();
        }

        void OVR::GLEContext::glConvolutionParameteriv_Hook(GLenum target, GLenum pname, const GLint *params)
        {
            if(glConvolutionParameteriv_Impl)
                glConvolutionParameteriv_Impl(target, pname, params);
            PostHook();
        }

        void OVR::GLEContext::glCopyConvolutionFilter1D_Hook(GLenum target, GLenum internalformat, GLint x, GLint y, GLsizei width)
        {
            if(glCopyConvolutionFilter1D_Impl)
                glCopyConvolutionFilter1D_Impl(target, internalformat, x, y, width);
            PostHook();
        }

        void OVR::GLEContext::glCopyConvolutionFilter2D_Hook(GLenum target, GLenum internalformat, GLint x, GLint y, GLsizei width, GLsizei height)
        {
            if(glCopyConvolutionFilter2D_Impl)
                glCopyConvolutionFilter2D_Impl(target, internalformat, x, y, width, height);
            PostHook();
        }

        void OVR::GLEContext::glGetConvolutionFilter_Hook(GLenum target, GLenum format, GLenum type, GLvoid *image)
        {
            if(glGetConvolutionFilter_Impl)
                glGetConvolutionFilter_Impl(target, format, type, image);
            PostHook();
        }

        void OVR::GLEContext::glGetConvolutionParameterfv_Hook(GLenum target, GLenum pname, GLfloat *params)
        {
            if(glGetConvolutionParameterfv_Impl)
                glGetConvolutionParameterfv_Impl(target, pname, params);
            PostHook();
        }

        void OVR::GLEContext::glGetConvolutionParameteriv_Hook(GLenum target, GLenum pname, GLint *params)
        {
            if(glGetConvolutionParameteriv_Impl)
                glGetConvolutionParameteriv_Impl(target, pname, params);
            PostHook();
        }

        void OVR::GLEContext::glGetSeparableFilter_Hook(GLenum target, GLenum format, GLenum type, GLvoid *row, GLvoid *column, GLvoid *span)
        {
            if(glGetSeparableFilter_Impl)
                glGetSeparableFilter_Impl(target, format, type, row, column, span);
            PostHook();
        }

        void OVR::GLEContext::glSeparableFilter2D_Hook(GLenum target, GLenum internalformat, GLsizei width, GLsizei height, GLenum format, GLenum type, const GLvoid *row, const GLvoid *column)
        {
            if(glSeparableFilter2D_Impl)
                glSeparableFilter2D_Impl(target, internalformat, width, height, format, type, row, column);
            PostHook();
        }

        void OVR::GLEContext::glGetHistogram_Hook(GLenum target, GLboolean reset, GLenum format, GLenum type, GLvoid *values)
        {
            if(glGetHistogram_Impl)
                glGetHistogram_Impl(target, reset, format, type, values);
            PostHook();
        }

        void OVR::GLEContext::glGetHistogramParameterfv_Hook(GLenum target, GLenum pname, GLfloat *params)
        {
            if(glGetHistogramParameterfv_Impl)
                glGetHistogramParameterfv_Impl(target, pname, params);
            PostHook();
        }

        void OVR::GLEContext::glGetHistogramParameteriv_Hook(GLenum target, GLenum pname, GLint *params)
        {
            if(glGetHistogramParameteriv_Impl)
                glGetHistogramParameteriv_Impl(target, pname, params);
            PostHook();
        }

        void OVR::GLEContext::glGetMinmax_Hook(GLenum target, GLboolean reset, GLenum format, GLenum type, GLvoid *values)
        {
            if(glGetMinmax_Impl)
                glGetMinmax_Impl(target, reset, format, type, values);
            PostHook();
        }

        void OVR::GLEContext::glGetMinmaxParameterfv_Hook(GLenum target, GLenum pname, GLfloat *params)
        {
            if(glGetMinmaxParameterfv_Impl)
                glGetMinmaxParameterfv_Impl(target, pname, params);
            PostHook();
        }

        void OVR::GLEContext::glGetMinmaxParameteriv_Hook(GLenum target, GLenum pname, GLint *params)
        {
            if(glGetMinmaxParameteriv_Impl)
                glGetMinmaxParameteriv_Impl(target, pname, params);
            PostHook();
        }

        void OVR::GLEContext::glHistogram_Hook(GLenum target, GLsizei width, GLenum internalformat, GLboolean sink)
        {
            if(glHistogram_Impl)
                glHistogram_Impl(target, width, internalformat, sink);
            PostHook();
        }

        void OVR::GLEContext::glMinmax_Hook(GLenum target, GLenum internalformat, GLboolean sink)
        {
            if(glMinmax_Impl)
                glMinmax_Impl(target, internalformat, sink);
            PostHook();
        }

        void OVR::GLEContext::glResetHistogram_Hook(GLenum target)
        {
            if(glResetHistogram_Impl)
                glResetHistogram_Impl(target);
            PostHook();
        }

        void OVR::GLEContext::glResetMinmax_Hook(GLenum target)
        {
            if(glResetMinmax_Impl)
                glResetMinmax_Impl(target);
            PostHook();
        }
        */

        // GL_VERSION_1_3
        void OVR::GLEContext::glActiveTexture_Hook(GLenum texture)
        {
            if(glActiveTexture_Impl)
                glActiveTexture_Impl(texture);
            PostHook();
        }

        void OVR::GLEContext::glSampleCoverage_Hook(GLclampf value, GLboolean invert)
        {
            if(glSampleCoverage_Impl)
                glSampleCoverage_Impl(value, invert);
            PostHook();
        }

        void OVR::GLEContext::glCompressedTexImage3D_Hook(GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth, GLint border, GLsizei imageSize, const GLvoid *data)
        {
            if(glCompressedTexImage3D_Impl)
                glCompressedTexImage3D_Impl(target, level, internalformat, width, height, depth, border, imageSize, data);
            PostHook();
        }

        void OVR::GLEContext::glCompressedTexImage2D_Hook(GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLint border, GLsizei imageSize, const GLvoid *data)
        {
            if(glCompressedTexImage2D_Impl)
                glCompressedTexImage2D_Impl(target, level, internalformat, width, height, border, imageSize, data);
            PostHook();
        }

        void OVR::GLEContext::glCompressedTexImage1D_Hook(GLenum target, GLint level, GLenum internalformat, GLsizei width, GLint border, GLsizei imageSize, const GLvoid *data)
        {
            if(glCompressedTexImage1D_Impl)
                glCompressedTexImage1D_Impl(target, level, internalformat, width, border, imageSize, data);
            PostHook();
        }

        void OVR::GLEContext::glCompressedTexSubImage3D_Hook(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLsizei imageSize, const GLvoid *data)
        {
            if(glCompressedTexSubImage3D_Impl)
                glCompressedTexSubImage3D_Impl(target, level, xoffset, yoffset, zoffset, width, height, depth, format, imageSize, data);
            PostHook();
        }

        void OVR::GLEContext::glCompressedTexSubImage2D_Hook(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLsizei imageSize, const GLvoid *data)
        {
            if(glCompressedTexSubImage2D_Impl)
                glCompressedTexSubImage2D_Impl(target, level, xoffset, yoffset, width, height, format, imageSize, data);
            PostHook();
        }

        void OVR::GLEContext::glCompressedTexSubImage1D_Hook(GLenum target, GLint level, GLint xoffset, GLsizei width, GLenum format, GLsizei imageSize, const GLvoid *data)
        {
            if(glCompressedTexSubImage1D_Impl)
                glCompressedTexSubImage1D_Impl(target, level, xoffset, width, format, imageSize, data);
            PostHook();
        }

        void OVR::GLEContext::glGetCompressedTexImage_Hook(GLenum target, GLint level, GLvoid *img)
        {
            if(glGetCompressedTexImage_Impl)
                glGetCompressedTexImage_Impl(target, level, img);
            PostHook();
        }


        // GL_VERSION_1_3 deprecated functions
        void OVR::GLEContext::glClientActiveTexture_Hook(GLenum texture)
        {
            if(glClientActiveTexture_Impl)
                glClientActiveTexture_Impl(texture);
            PostHook();
        }

        void OVR::GLEContext::glMultiTexCoord1d_Hook(GLenum target, GLdouble s)
        {
            if(glMultiTexCoord1d_Impl)
                glMultiTexCoord1d_Impl(target, s);
            PostHook();
        }

        void OVR::GLEContext::glMultiTexCoord1dv_Hook(GLenum target, const GLdouble *v)
        {
            if(glMultiTexCoord1dv_Impl)
                glMultiTexCoord1dv_Impl(target, v);
            PostHook();
        }

        void OVR::GLEContext::glMultiTexCoord1f_Hook(GLenum target, GLfloat s)
        {
            if(glMultiTexCoord1f_Impl)
                glMultiTexCoord1f_Impl(target, s);
            PostHook();
        }

        void OVR::GLEContext::glMultiTexCoord1fv_Hook(GLenum target, const GLfloat *v)
        {
            if(glMultiTexCoord1fv_Impl)
                glMultiTexCoord1fv_Impl(target, v);
            PostHook();
        }

        void OVR::GLEContext::glMultiTexCoord1i_Hook(GLenum target, GLint s)
        {
            if(glMultiTexCoord1i_Impl)
                glMultiTexCoord1i_Impl(target, s);
            PostHook();
        }

        void OVR::GLEContext::glMultiTexCoord1iv_Hook(GLenum target, const GLint *v)
        {
            if(glMultiTexCoord1iv_Impl)
                glMultiTexCoord1iv_Impl(target, v);
            PostHook();
        }

        void OVR::GLEContext::glMultiTexCoord1s_Hook(GLenum target, GLshort s)
        {
            if(glMultiTexCoord1s_Impl)
                glMultiTexCoord1s_Impl(target, s);
            PostHook();
        }

        void OVR::GLEContext::glMultiTexCoord1sv_Hook(GLenum target, const GLshort *v)
        {
            if(glMultiTexCoord1sv_Impl)
                glMultiTexCoord1sv_Impl(target, v);
            PostHook();
        }

        void OVR::GLEContext::glMultiTexCoord2d_Hook(GLenum target, GLdouble s, GLdouble t)
        {
            if(glMultiTexCoord2d_Impl)
                glMultiTexCoord2d_Impl(target, s, t);
            PostHook();
        }

        void OVR::GLEContext::glMultiTexCoord2dv_Hook(GLenum target, const GLdouble *v)
        {
            if(glMultiTexCoord2dv_Impl)
                glMultiTexCoord2dv_Impl(target, v);
            PostHook();
        }

        void OVR::GLEContext::glMultiTexCoord2f_Hook(GLenum target, GLfloat s, GLfloat t)
        {
            if(glMultiTexCoord2f_Impl)
                glMultiTexCoord2f_Impl(target, s, t);
            PostHook();
        }

        void OVR::GLEContext::glMultiTexCoord2fv_Hook(GLenum target, const GLfloat *v)
        {
            if(glMultiTexCoord2fv_Impl)
                glMultiTexCoord2fv_Impl(target, v);
            PostHook();
        }

        void OVR::GLEContext::glMultiTexCoord2i_Hook(GLenum target, GLint s, GLint t)
        {
            if(glMultiTexCoord2i_Impl)
                glMultiTexCoord2i_Impl(target, s, t);
            PostHook();
        }

        void OVR::GLEContext::glMultiTexCoord2iv_Hook(GLenum target, const GLint *v)
        {
            if(glMultiTexCoord2iv_Impl)
                glMultiTexCoord2iv_Impl(target, v);
            PostHook();
        }

        void OVR::GLEContext::glMultiTexCoord2s_Hook(GLenum target, GLshort s, GLshort t)
        {
            if(glMultiTexCoord2s_Impl)
                glMultiTexCoord2s_Impl(target, s, t);
            PostHook();
        }

        void OVR::GLEContext::glMultiTexCoord2sv_Hook(GLenum target, const GLshort *v)
        {
            if(glMultiTexCoord2sv_Impl)
                glMultiTexCoord2sv_Impl(target, v);
            PostHook();
        }

        void OVR::GLEContext::glMultiTexCoord3d_Hook(GLenum target, GLdouble s, GLdouble t, GLdouble r)
        {
            if(glMultiTexCoord3d_Impl)
                glMultiTexCoord3d_Impl(target, s, t, r);
            PostHook();
        }

        void OVR::GLEContext::glMultiTexCoord3dv_Hook(GLenum target, const GLdouble *v)
        {
            if(glMultiTexCoord3dv_Impl)
                glMultiTexCoord3dv_Impl(target, v);
            PostHook();
        }

        void OVR::GLEContext::glMultiTexCoord3f_Hook(GLenum target, GLfloat s, GLfloat t, GLfloat r)
        {
            if(glMultiTexCoord3f_Impl)
                glMultiTexCoord3f_Impl(target, s, t, r);
            PostHook();
        }

        void OVR::GLEContext::glMultiTexCoord3fv_Hook(GLenum target, const GLfloat *v)
        {
            if(glMultiTexCoord3fv_Impl)
                glMultiTexCoord3fv_Impl(target, v);
            PostHook();
        }

        void OVR::GLEContext::glMultiTexCoord3i_Hook(GLenum target, GLint s, GLint t, GLint r)
        {
            if(glMultiTexCoord3i_Impl)
                glMultiTexCoord3i_Impl(target, s, t, r);
            PostHook();
        }

        void OVR::GLEContext::glMultiTexCoord3iv_Hook(GLenum target, const GLint *v)
        {
            if(glMultiTexCoord3iv_Impl)
                glMultiTexCoord3iv_Impl(target, v);
            PostHook();
        }

        void OVR::GLEContext::glMultiTexCoord3s_Hook(GLenum target, GLshort s, GLshort t, GLshort r)
        {
            if(glMultiTexCoord3s_Impl)
                glMultiTexCoord3s_Impl(target, s, t, r);
            PostHook();
        }

        void OVR::GLEContext::glMultiTexCoord3sv_Hook(GLenum target, const GLshort *v)
        {
            if(glMultiTexCoord3sv_Impl)
                glMultiTexCoord3sv_Impl(target, v);
            PostHook();
        }

        void OVR::GLEContext::glMultiTexCoord4d_Hook(GLenum target, GLdouble s, GLdouble t, GLdouble r, GLdouble q)
        {
            if(glMultiTexCoord4d_Impl)
                glMultiTexCoord4d_Impl(target, s, t, r, q);
            PostHook();
        }

        void OVR::GLEContext::glMultiTexCoord4dv_Hook(GLenum target, const GLdouble *v)
        {
            if(glMultiTexCoord4dv_Impl)
                glMultiTexCoord4dv_Impl(target, v);
            PostHook();
        }

        void OVR::GLEContext::glMultiTexCoord4f_Hook(GLenum target, GLfloat s, GLfloat t, GLfloat r, GLfloat q)
        {
            if(glMultiTexCoord4f_Impl)
                glMultiTexCoord4f_Impl(target, s, t, r, q);
            PostHook();
        }

        void OVR::GLEContext::glMultiTexCoord4fv_Hook(GLenum target, const GLfloat *v)
        {
            if(glMultiTexCoord4fv_Impl)
                glMultiTexCoord4fv_Impl(target, v);
            PostHook();
        }

        void OVR::GLEContext::glMultiTexCoord4i_Hook(GLenum target, GLint s, GLint t, GLint r, GLint q)
        {
            if(glMultiTexCoord4i_Impl)
                glMultiTexCoord4i_Impl(target, s, t, r, q);
            PostHook();
        }

        void OVR::GLEContext::glMultiTexCoord4iv_Hook(GLenum target, const GLint *v)
        {
            if(glMultiTexCoord4iv_Impl)
                glMultiTexCoord4iv_Impl(target, v);
            PostHook();
        }

        void OVR::GLEContext::glMultiTexCoord4s_Hook(GLenum target, GLshort s, GLshort t, GLshort r, GLshort q)
        {
            if(glMultiTexCoord4s_Impl)
                glMultiTexCoord4s_Impl(target, s, t, r, q);
            PostHook();
        }

        void OVR::GLEContext::glMultiTexCoord4sv_Hook(GLenum target, const GLshort *v)
        {
            if(glMultiTexCoord4sv_Impl)
                glMultiTexCoord4sv_Impl(target, v);
            PostHook();
        }

        void OVR::GLEContext::glLoadTransposeMatrixf_Hook(const GLfloat *m)
        {
            if(glLoadTransposeMatrixf_Impl)
                glLoadTransposeMatrixf_Impl(m);
            PostHook();
        }

        void OVR::GLEContext::glLoadTransposeMatrixd_Hook(const GLdouble *m)
        {
            if(glLoadTransposeMatrixd_Impl)
                glLoadTransposeMatrixd_Impl(m);
            PostHook();
        }

        void OVR::GLEContext::glMultTransposeMatrixf_Hook(const GLfloat *m)
        {
            if(glMultTransposeMatrixf_Impl)
                glMultTransposeMatrixf_Impl(m);
            PostHook();
        }

        void OVR::GLEContext::glMultTransposeMatrixd_Hook(const GLdouble *m)
        {
            if(glMultTransposeMatrixd_Impl)
                glMultTransposeMatrixd_Impl(m);
            PostHook();
        }


        // GL_VERSION_1_4
        void OVR::GLEContext::glBlendFuncSeparate_Hook(GLenum sfactorRGB, GLenum dfactorRGB, GLenum sfactorAlpha, GLenum dfactorAlpha)
        {
            if(glBlendFuncSeparate_Impl)
                glBlendFuncSeparate_Impl(sfactorRGB, dfactorRGB, sfactorAlpha, dfactorAlpha);
            PostHook();
        }

        void OVR::GLEContext::glMultiDrawArrays_Hook(GLenum mode, const GLint *first, const GLsizei *count, GLsizei primcount)
        {
            if(glMultiDrawArrays_Impl)
                glMultiDrawArrays_Impl(mode, first, count, primcount);
            PostHook();
        }

        void OVR::GLEContext::glMultiDrawElements_Hook(GLenum mode, const GLsizei *count, GLenum type, const GLvoid* *indices, GLsizei primcount)
        {
            if(glMultiDrawElements_Impl)
                glMultiDrawElements_Impl(mode, count, type, indices, primcount);
            PostHook();
        }

        void OVR::GLEContext::glPointParameterf_Hook(GLenum pname, GLfloat param)
        {
            if(glPointParameterf_Impl)
                glPointParameterf_Impl(pname, param);
            PostHook();
        }

        void OVR::GLEContext::glPointParameterfv_Hook(GLenum pname, const GLfloat *params)
        {
            if(glPointParameterfv_Impl)
                glPointParameterfv_Impl(pname, params);
            PostHook();
        }

        void OVR::GLEContext::glPointParameteri_Hook(GLenum pname, GLint param)
        {
            if(glPointParameteri_Impl)
                glPointParameteri_Impl(pname, param);
            PostHook();
        }

        void OVR::GLEContext::glPointParameteriv_Hook(GLenum pname, const GLint *params)
        {
            if(glPointParameteriv_Impl)
                glPointParameteriv_Impl(pname, params);
            PostHook();
        }


        // GL_VERSION_1_4 deprecated functions
        void OVR::GLEContext::glFogCoordf_Hook(GLfloat coord)
        {
            if(glFogCoordf_Impl)
                glFogCoordf_Impl(coord);
            PostHook();
        }

        void OVR::GLEContext::glFogCoordfv_Hook(const GLfloat *coord)
        {
            if(glFogCoordfv_Impl)
                glFogCoordfv_Impl(coord);
            PostHook();
        }

        void OVR::GLEContext::glFogCoordd_Hook(GLdouble coord)
        {
            if(glFogCoordd_Impl)
                glFogCoordd_Impl(coord);
            PostHook();
        }

        void OVR::GLEContext::glFogCoorddv_Hook(const GLdouble *coord)
        {
            if(glFogCoorddv_Impl)
                glFogCoorddv_Impl(coord);
            PostHook();
        }

        void OVR::GLEContext::glFogCoordPointer_Hook(GLenum type, GLsizei stride, const GLvoid *pointer)
        {
            if(glFogCoordPointer_Impl)
                glFogCoordPointer_Impl(type, stride, pointer);
            PostHook();
        }

        void OVR::GLEContext::glSecondaryColor3b_Hook(GLbyte red, GLbyte green, GLbyte blue)
        {
            if(glSecondaryColor3b_Impl)
                glSecondaryColor3b_Impl(red, green, blue);
            PostHook();
        }

        void OVR::GLEContext::glSecondaryColor3bv_Hook(const GLbyte *v)
        {
            if(glSecondaryColor3bv_Impl)
                glSecondaryColor3bv_Impl(v);
            PostHook();
        }

        void OVR::GLEContext::glSecondaryColor3d_Hook(GLdouble red, GLdouble green, GLdouble blue)
        {
            if(glSecondaryColor3d_Impl)
                glSecondaryColor3d_Impl(red, green, blue);
            PostHook();
        }

        void OVR::GLEContext::glSecondaryColor3dv_Hook(const GLdouble *v)
        {
            if(glSecondaryColor3dv_Impl)
                glSecondaryColor3dv_Impl(v);
            PostHook();
        }

        void OVR::GLEContext::glSecondaryColor3f_Hook(GLfloat red, GLfloat green, GLfloat blue)
        {
            if(glSecondaryColor3f_Impl)
                glSecondaryColor3f_Impl(red, green, blue);
            PostHook();
        }

        void OVR::GLEContext::glSecondaryColor3fv_Hook(const GLfloat *v)
        {
            if(glSecondaryColor3fv_Impl)
                glSecondaryColor3fv_Impl(v);
            PostHook();
        }

        void OVR::GLEContext::glSecondaryColor3i_Hook(GLint red, GLint green, GLint blue)
        {
            if(glSecondaryColor3i_Impl)
                glSecondaryColor3i_Impl(red, green, blue);
            PostHook();
        }

        void OVR::GLEContext::glSecondaryColor3iv_Hook(const GLint *v)
        {
            if(glSecondaryColor3iv_Impl)
                glSecondaryColor3iv_Impl(v);
            PostHook();
        }

        void OVR::GLEContext::glSecondaryColor3s_Hook(GLshort red, GLshort green, GLshort blue)
        {
            if(glSecondaryColor3s_Impl)
                glSecondaryColor3s_Impl(red, green, blue);
            PostHook();
        }

        void OVR::GLEContext::glSecondaryColor3sv_Hook(const GLshort *v)
        {
            if(glSecondaryColor3sv_Impl)
                glSecondaryColor3sv_Impl(v);
            PostHook();
        }

        void OVR::GLEContext::glSecondaryColor3ub_Hook(GLubyte red, GLubyte green, GLubyte blue)
        {
            if(glSecondaryColor3ub_Impl)
                glSecondaryColor3ub_Impl(red, green, blue);
            PostHook();
        }

        void OVR::GLEContext::glSecondaryColor3ubv_Hook(const GLubyte *v)
        {
            if(glSecondaryColor3ubv_Impl)
                glSecondaryColor3ubv_Impl(v);
            PostHook();
        }

        void OVR::GLEContext::glSecondaryColor3ui_Hook(GLuint red, GLuint green, GLuint blue)
        {
            if(glSecondaryColor3ui_Impl)
                glSecondaryColor3ui_Impl(red, green, blue);
            PostHook();
        }

        void OVR::GLEContext::glSecondaryColor3uiv_Hook(const GLuint *v)
        {
            if(glSecondaryColor3uiv_Impl)
                glSecondaryColor3uiv_Impl(v);
            PostHook();
        }

        void OVR::GLEContext::glSecondaryColor3us_Hook(GLushort red, GLushort green, GLushort blue)
        {
            if(glSecondaryColor3us_Impl)
                glSecondaryColor3us_Impl(red, green, blue);
            PostHook();
        }

        void OVR::GLEContext::glSecondaryColor3usv_Hook(const GLushort *v)
        {
            if(glSecondaryColor3usv_Impl)
                glSecondaryColor3usv_Impl(v);
            PostHook();
        }

        void OVR::GLEContext::glSecondaryColorPointer_Hook(GLint size, GLenum type, GLsizei stride, const GLvoid *pointer)
        {
            if(glSecondaryColorPointer_Impl)
                glSecondaryColorPointer_Impl(size, type, stride, pointer);
            PostHook();
        }

        void OVR::GLEContext::glWindowPos2d_Hook(GLdouble x, GLdouble y)
        {
            if(glWindowPos2d_Impl)
                glWindowPos2d_Impl(x, y);
            PostHook();
        }

        void OVR::GLEContext::glWindowPos2dv_Hook(const GLdouble *v)
        {
            if(glWindowPos2dv_Impl)
                glWindowPos2dv_Impl(v);
            PostHook();
        }

        void OVR::GLEContext::glWindowPos2f_Hook(GLfloat x, GLfloat y)
        {
            if(glWindowPos2f_Impl)
                glWindowPos2f_Impl(x, y);
            PostHook();
        }

        void OVR::GLEContext::glWindowPos2fv_Hook(const GLfloat *v)
        {
            if(glWindowPos2fv_Impl)
                glWindowPos2fv_Impl(v);
            PostHook();
        }

        void OVR::GLEContext::glWindowPos2i_Hook(GLint x, GLint y)
        {
            if(glWindowPos2i_Impl)
                glWindowPos2i_Impl(x, y);
            PostHook();
        }

        void OVR::GLEContext::glWindowPos2iv_Hook(const GLint *v)
        {
            if(glWindowPos2iv_Impl)
                glWindowPos2iv_Impl(v);
            PostHook();
        }

        void OVR::GLEContext::glWindowPos2s_Hook(GLshort x, GLshort y)
        {
            if(glWindowPos2s_Impl)
                glWindowPos2s_Impl(x, y);
            PostHook();
        }

        void OVR::GLEContext::glWindowPos2sv_Hook(const GLshort *v)
        {
            if(glWindowPos2sv_Impl)
                glWindowPos2sv_Impl(v);
            PostHook();
        }

        void OVR::GLEContext::glWindowPos3d_Hook(GLdouble x, GLdouble y, GLdouble z)
        {
            if(glWindowPos3d_Impl)
                glWindowPos3d_Impl(x, y, z);
            PostHook();
        }

        void OVR::GLEContext::glWindowPos3dv_Hook(const GLdouble *v)
        {
            if(glWindowPos3dv_Impl)
                glWindowPos3dv_Impl(v);
            PostHook();
        }

        void OVR::GLEContext::glWindowPos3f_Hook(GLfloat x, GLfloat y, GLfloat z)
        {
            if(glWindowPos3f_Impl)
                glWindowPos3f_Impl(x, y, z);
            PostHook();
        }

        void OVR::GLEContext::glWindowPos3fv_Hook(const GLfloat *v)
        {
            if(glWindowPos3fv_Impl)
                glWindowPos3fv_Impl(v);
            PostHook();
        }

        void OVR::GLEContext::glWindowPos3i_Hook(GLint x, GLint y, GLint z)
        {
            if(glWindowPos3i_Impl)
                glWindowPos3i_Impl(x, y, z);
            PostHook();
        }

        void OVR::GLEContext::glWindowPos3iv_Hook(const GLint *v)
        {
            if(glWindowPos3iv_Impl)
                glWindowPos3iv_Impl(v);
            PostHook();
        }

        void OVR::GLEContext::glWindowPos3s_Hook(GLshort x, GLshort y, GLshort z)
        {
            if(glWindowPos3s_Impl)
                glWindowPos3s_Impl(x, y, z);
            PostHook();
        }

        void OVR::GLEContext::glWindowPos3sv_Hook(const GLshort *v)
        {
            if(glWindowPos3sv_Impl)
                glWindowPos3sv_Impl(v);
            PostHook();
        }


        // GL_VERSION_1_5
        void OVR::GLEContext::glGenQueries_Hook(GLsizei n, GLuint *ids)
        {
            if(glGenQueries_Impl)
                glGenQueries_Impl(n, ids);
            PostHook();
        }

        void OVR::GLEContext::glDeleteQueries_Hook(GLsizei n, const GLuint *ids)
        {
            if(glDeleteQueries_Impl)
                glDeleteQueries_Impl(n, ids);
            PostHook();
        }

        GLboolean OVR::GLEContext::glIsQuery_Hook(GLuint id)
        {
            GLboolean b = GL_FALSE;
            if(glIsQuery_Impl)
                b = glIsQuery_Impl(id);
            PostHook();
            return b;
        }

        void OVR::GLEContext::glBeginQuery_Hook(GLenum target, GLuint id)
        {
            if(glBeginQuery_Impl)
                glBeginQuery_Impl(target, id);
            PostHook();
        }

        void OVR::GLEContext::glEndQuery_Hook(GLenum target)
        {
            if(glEndQuery_Impl)
                glEndQuery_Impl(target);
            PostHook();
        }

        void OVR::GLEContext::glGetQueryiv_Hook(GLenum target, GLenum pname, GLint *params)
        {
            if(glGetQueryiv_Impl)
                glGetQueryiv_Impl(target, pname, params);
            PostHook();
        }

        void OVR::GLEContext::glGetQueryObjectiv_Hook(GLuint id, GLenum pname, GLint *params)
        {
            if(glGetQueryObjectiv_Impl)
                glGetQueryObjectiv_Impl(id, pname, params);
            PostHook();
        }

        void OVR::GLEContext::glGetQueryObjectuiv_Hook(GLuint id, GLenum pname, GLuint *params)
        {
            if(glGetQueryObjectuiv_Impl)
                glGetQueryObjectuiv_Impl(id, pname, params);
            PostHook();
        }

        void OVR::GLEContext::glBindBuffer_Hook(GLenum target, GLuint buffer)
        {
            if(glBindBuffer_Impl)
                glBindBuffer_Impl(target, buffer);
            PostHook();
        }

        void OVR::GLEContext::glDeleteBuffers_Hook(GLsizei n, const GLuint *buffers)
        {
            if(glDeleteBuffers_Impl)
                glDeleteBuffers_Impl(n, buffers);
            PostHook();
        }

        void OVR::GLEContext::glGenBuffers_Hook(GLsizei n, GLuint *buffers)
        {
            if(glGenBuffers_Impl)
                glGenBuffers_Impl(n, buffers);
            PostHook();
        }

        GLboolean OVR::GLEContext::glIsBuffer_Hook(GLuint buffer)
        {
            GLboolean b = GL_FALSE;
            if(glIsBuffer_Impl)
                b = glIsBuffer_Impl(buffer);
            PostHook();
            return b;
        }

        void OVR::GLEContext::glBufferData_Hook(GLenum target, GLsizeiptr size, const GLvoid *data, GLenum usage)
        {
            if(glBufferData_Impl)
                glBufferData_Impl(target, size, data, usage);
            PostHook();
        }

        void OVR::GLEContext::glBufferSubData_Hook(GLenum target, GLintptr offset, GLsizeiptr size, const GLvoid *data)
        {
            if(glBufferSubData_Impl)
                glBufferSubData_Impl(target, offset, size, data);
            PostHook();
        }

        void OVR::GLEContext::glGetBufferSubData_Hook(GLenum target, GLintptr offset, GLsizeiptr size, GLvoid *data)
        {
            if(glGetBufferSubData_Impl)
                glGetBufferSubData_Impl(target, offset, size, data);
            PostHook();
        }

        GLvoid* OVR::GLEContext::glMapBuffer_Hook(GLenum target, GLenum access)
        {
            GLvoid* p = NULL;
            if(glMapBuffer_Impl)
                p = glMapBuffer_Impl(target, access);
            PostHook();
            return p;
        }

        GLboolean OVR::GLEContext::glUnmapBuffer_Hook(GLenum target)
        {
            GLboolean b = GL_FALSE;
            if(glUnmapBuffer_Impl)
                b = glUnmapBuffer_Impl(target);
            PostHook();
            return b;
        }

        void OVR::GLEContext::glGetBufferParameteriv_Hook(GLenum target, GLenum pname, GLint *params)
        {
            if(glGetBufferParameteriv_Impl)
                glGetBufferParameteriv_Impl(target, pname, params);
            PostHook();
        }

        void OVR::GLEContext::glGetBufferPointerv_Hook(GLenum target, GLenum pname, GLvoid* *params)
        {
            if(glGetBufferPointerv_Impl)
                glGetBufferPointerv_Impl(target, pname, params);
            PostHook();
        }


        // GL_VERSION_2_0
        void OVR::GLEContext::glBlendEquationSeparate_Hook(GLenum modeRGB, GLenum modeAlpha)
        {
            if(glBlendEquationSeparate_Impl)
                glBlendEquationSeparate_Impl(modeRGB, modeAlpha);
            PostHook();
        }

        void OVR::GLEContext::glDrawBuffers_Hook(GLsizei n, const GLenum *bufs)
        {
            if(glDrawBuffers_Impl)
                glDrawBuffers_Impl(n, bufs);
            PostHook();
        }

        void OVR::GLEContext::glStencilOpSeparate_Hook(GLenum face, GLenum sfail, GLenum dpfail, GLenum dppass)
        {
            if(glStencilOpSeparate_Impl)
                glStencilOpSeparate_Impl(face, sfail, dpfail, dppass);
            PostHook();
        }

        void OVR::GLEContext::glStencilFuncSeparate_Hook(GLenum face, GLenum func, GLint ref, GLuint mask)
        {
            if(glStencilFuncSeparate_Impl)
                glStencilFuncSeparate_Impl(face, func, ref, mask);
            PostHook();
        }

        void OVR::GLEContext::glStencilMaskSeparate_Hook(GLenum face, GLuint mask)
        {
            if(glStencilMaskSeparate_Impl)
                glStencilMaskSeparate_Impl(face, mask);
            PostHook();
        }

        void OVR::GLEContext::glAttachShader_Hook(GLuint program, GLuint shader)
        {
            if(glAttachShader_Impl)
                glAttachShader_Impl(program, shader);
            PostHook();
        }

        void OVR::GLEContext::glBindAttribLocation_Hook(GLuint program, GLuint index, const GLchar *name)
        {
            if(glBindAttribLocation_Impl)
                glBindAttribLocation_Impl(program, index, name);
            PostHook();
        }

        void OVR::GLEContext::glCompileShader_Hook(GLuint shader)
        {
            if(glCompileShader_Impl)
                glCompileShader_Impl(shader);
            PostHook();
        }

        GLuint OVR::GLEContext::glCreateProgram_Hook()
        {
            GLuint u = 0;
            if(glCreateProgram_Impl)
                u = glCreateProgram_Impl();
            PostHook();
            return u;
        }

        GLuint OVR::GLEContext::glCreateShader_Hook(GLenum type)
        {
            GLuint u = 0;
            if(glCreateShader_Impl)
                u = glCreateShader_Impl(type);
            PostHook();
            return u;
        }

        void OVR::GLEContext::glDeleteProgram_Hook(GLuint program)
        {
            if(glDeleteProgram_Impl)
                glDeleteProgram_Impl(program);
            PostHook();
        }

        void OVR::GLEContext::glDeleteShader_Hook(GLuint shader)
        {
            if(glDeleteShader_Impl)
                glDeleteShader_Impl(shader);
            PostHook();
        }

        void OVR::GLEContext::glDetachShader_Hook(GLuint program, GLuint shader)
        {
            if(glDetachShader_Impl)
                glDetachShader_Impl(program, shader);
            PostHook();
        }

        void OVR::GLEContext::glDisableVertexAttribArray_Hook(GLuint index)
        {
            if(glDisableVertexAttribArray_Impl)
                glDisableVertexAttribArray_Impl(index);
            PostHook();
        }

        void OVR::GLEContext::glEnableVertexAttribArray_Hook(GLuint index)
        {
            if(glEnableVertexAttribArray_Impl)
                glEnableVertexAttribArray_Impl(index);
            PostHook();
        }

        void OVR::GLEContext::glGetActiveAttrib_Hook(GLuint program, GLuint index, GLsizei bufSize, GLsizei *length, GLint *size, GLenum *type, GLchar *name)
        {
            if(glGetActiveAttrib_Impl)
                glGetActiveAttrib_Impl(program, index, bufSize, length, size, type, name);
            PostHook();
        }

        void OVR::GLEContext::glGetActiveUniform_Hook(GLuint program, GLuint index, GLsizei bufSize, GLsizei *length, GLint *size, GLenum *type, GLchar *name)
        {
            if(glGetActiveUniform_Impl)
                glGetActiveUniform_Impl(program, index, bufSize, length, size, type, name);
            PostHook();
        }

        void OVR::GLEContext::glGetAttachedShaders_Hook(GLuint program, GLsizei maxCount, GLsizei *count, GLuint *obj)
        {
            if(glGetAttachedShaders_Impl)
                glGetAttachedShaders_Impl(program, maxCount, count, obj);
            PostHook();
        }

        GLint OVR::GLEContext::glGetAttribLocation_Hook(GLuint program, const GLchar *name)
        {
            GLint i = 0;
            if(glGetAttribLocation_Impl)
                i = glGetAttribLocation_Impl(program, name);
            PostHook();
            return i;
        }

        void OVR::GLEContext::glGetProgramiv_Hook(GLuint program, GLenum pname, GLint *params)
        {
            if(glGetProgramiv_Impl)
                glGetProgramiv_Impl(program, pname, params);
            PostHook();
        }

        void OVR::GLEContext::glGetProgramInfoLog_Hook(GLuint program, GLsizei bufSize, GLsizei *length, GLchar *infoLog)
        {
            if(glGetProgramInfoLog_Impl)
                glGetProgramInfoLog_Impl(program, bufSize, length, infoLog);
            PostHook();
        }

        void OVR::GLEContext::glGetShaderiv_Hook(GLuint shader, GLenum pname, GLint *params)
        {
            if(glGetShaderiv_Impl)
                glGetShaderiv_Impl(shader, pname, params);
            PostHook();
        }

        void OVR::GLEContext::glGetShaderInfoLog_Hook(GLuint shader, GLsizei bufSize, GLsizei *length, GLchar *infoLog)
        {
            if(glGetShaderInfoLog_Impl)
                glGetShaderInfoLog_Impl(shader, bufSize, length, infoLog);
            PostHook();
        }

        void OVR::GLEContext::glGetShaderSource_Hook(GLuint shader, GLsizei bufSize, GLsizei *length, GLchar *source)
        {
            if(glGetShaderSource_Impl)
                glGetShaderSource_Impl(shader, bufSize, length, source);
            PostHook();
        }

        GLint OVR::GLEContext::glGetUniformLocation_Hook(GLuint program, const GLchar *name)
        {
            GLint i = 0;
            if(glGetUniformLocation_Impl)
                i = glGetUniformLocation_Impl(program, name);
            PostHook();
            return i;
        }

        void OVR::GLEContext::glGetUniformfv_Hook(GLuint program, GLint location, GLfloat *params)
        {
            if(glGetUniformfv_Impl)
                glGetUniformfv_Impl(program, location, params);
            PostHook();
        }

        void OVR::GLEContext::glGetUniformiv_Hook(GLuint program, GLint location, GLint *params)
        {
            if(glGetUniformiv_Impl)
                glGetUniformiv_Impl(program, location, params);
            PostHook();
        }

        void OVR::GLEContext::glGetVertexAttribdv_Hook(GLuint index, GLenum pname, GLdouble *params)
        {
            if(glGetVertexAttribdv_Impl)
                glGetVertexAttribdv_Impl(index, pname, params);
            PostHook();
        }

        void OVR::GLEContext::glGetVertexAttribfv_Hook(GLuint index, GLenum pname, GLfloat *params)
        {
            if(glGetVertexAttribfv_Impl)
                glGetVertexAttribfv_Impl(index, pname, params);
            PostHook();
        }

        void OVR::GLEContext::glGetVertexAttribiv_Hook(GLuint index, GLenum pname, GLint *params)
        {
            if(glGetVertexAttribiv_Impl)
                glGetVertexAttribiv_Impl(index, pname, params);
            PostHook();
        }

        void OVR::GLEContext::glGetVertexAttribPointerv_Hook(GLuint index, GLenum pname, GLvoid* *pointer)
        {
            if(glGetVertexAttribPointerv_Impl)
                glGetVertexAttribPointerv_Impl(index, pname, pointer);
            PostHook();
        }

        GLboolean OVR::GLEContext::glIsProgram_Hook(GLuint program)
        {
            GLboolean b = GL_FALSE;
            if(glIsProgram_Impl)
                b = glIsProgram_Impl(program);
            PostHook();
            return b;
        }

        GLboolean OVR::GLEContext::glIsShader_Hook(GLuint shader)
        {
            GLboolean b = GL_FALSE;
            if(glIsShader_Impl)
                b = glIsShader_Impl(shader);
            PostHook();
            return b;
        }

        void OVR::GLEContext::glLinkProgram_Hook(GLuint program)
        {
            if(glLinkProgram_Impl)
                glLinkProgram_Impl(program);
            PostHook();
        }

        void OVR::GLEContext::glShaderSource_Hook(GLuint shader, GLsizei count, const GLchar* *string, const GLint *length)
        {
            if(glShaderSource_Impl)
                glShaderSource_Impl(shader, count, string, length);
            PostHook();
        }

        void OVR::GLEContext::glUseProgram_Hook(GLuint program)
        {
            if(glUseProgram_Impl)
                glUseProgram_Impl(program);
            PostHook();
        }

        void OVR::GLEContext::glUniform1f_Hook(GLint location, GLfloat v0)
        {
            if(glUniform1f_Impl)
                glUniform1f_Impl(location, v0);
            PostHook();
        }

        void OVR::GLEContext::glUniform2f_Hook(GLint location, GLfloat v0, GLfloat v1)
        {
            if(glUniform2f_Impl)
                glUniform2f_Impl(location, v0, v1);
            PostHook();
        }

        void OVR::GLEContext::glUniform3f_Hook(GLint location, GLfloat v0, GLfloat v1, GLfloat v2)
        {
            if(glUniform3f_Impl)
                glUniform3f_Impl(location, v0, v1, v2);
            PostHook();
        }

        void OVR::GLEContext::glUniform4f_Hook(GLint location, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3)
        {
            if(glUniform4f_Impl)
                glUniform4f_Impl(location, v0, v1, v2, v3);
            PostHook();
        }

        void OVR::GLEContext::glUniform1i_Hook(GLint location, GLint v0)
        {
            if(glUniform1i_Impl)
                glUniform1i_Impl(location, v0);
            PostHook();
        }

        void OVR::GLEContext::glUniform2i_Hook(GLint location, GLint v0, GLint v1)
        {
            if(glUniform2i_Impl)
                glUniform2i_Impl(location, v0, v1);
            PostHook();
        }

        void OVR::GLEContext::glUniform3i_Hook(GLint location, GLint v0, GLint v1, GLint v2)
        {
            if(glUniform3i_Impl)
                glUniform3i_Impl(location, v0, v1, v2);
            PostHook();
        }

        void OVR::GLEContext::glUniform4i_Hook(GLint location, GLint v0, GLint v1, GLint v2, GLint v3)
        {
            if(glUniform4i_Impl)
                glUniform4i_Impl(location, v0, v1, v2, v3);
            PostHook();
        }

        void OVR::GLEContext::glUniform1fv_Hook(GLint location, GLsizei count, const GLfloat *value)
        {
            if(glUniform1fv_Impl)
                glUniform1fv_Impl(location, count, value);
            PostHook();
        }

        void OVR::GLEContext::glUniform2fv_Hook(GLint location, GLsizei count, const GLfloat *value)
        {
            if(glUniform2fv_Impl)
                glUniform2fv_Impl(location, count, value);
            PostHook();
        }

        void OVR::GLEContext::glUniform3fv_Hook(GLint location, GLsizei count, const GLfloat *value)
        {
            if(glUniform3fv_Impl)
                glUniform3fv_Impl(location, count, value);
            PostHook();
        }

        void OVR::GLEContext::glUniform4fv_Hook(GLint location, GLsizei count, const GLfloat *value)
        {
            if(glUniform4fv_Impl)
                glUniform4fv_Impl(location, count, value);
            PostHook();
        }

        void OVR::GLEContext::glUniform1iv_Hook(GLint location, GLsizei count, const GLint *value)
        {
            if(glUniform1iv_Impl)
                glUniform1iv_Impl(location, count, value);
            PostHook();
        }

        void OVR::GLEContext::glUniform2iv_Hook(GLint location, GLsizei count, const GLint *value)
        {
            if(glUniform2iv_Impl)
                glUniform2iv_Impl(location, count, value);
            PostHook();
        }

        void OVR::GLEContext::glUniform3iv_Hook(GLint location, GLsizei count, const GLint *value)
        {
            if(glUniform3iv_Impl)
                glUniform3iv_Impl(location, count, value);
            PostHook();
        }

        void OVR::GLEContext::glUniform4iv_Hook(GLint location, GLsizei count, const GLint *value)
        {
            if(glUniform4iv_Impl)
                glUniform4iv_Impl(location, count, value);
            PostHook();
        }

        void OVR::GLEContext::glUniformMatrix2fv_Hook(GLint location, GLsizei count, GLboolean transpose, const GLfloat *value)
        {
            if(glUniformMatrix2fv_Impl)
                glUniformMatrix2fv_Impl(location, count, transpose, value);
            PostHook();
        }

        void OVR::GLEContext::glUniformMatrix3fv_Hook(GLint location, GLsizei count, GLboolean transpose, const GLfloat *value)
        {
            if(glUniformMatrix3fv_Impl)
                glUniformMatrix3fv_Impl(location, count, transpose, value);
            PostHook();
        }

        void OVR::GLEContext::glUniformMatrix4fv_Hook(GLint location, GLsizei count, GLboolean transpose, const GLfloat *value)
        {
            if(glUniformMatrix4fv_Impl)
                glUniformMatrix4fv_Impl(location, count, transpose, value);
            PostHook();
        }

        void OVR::GLEContext::glValidateProgram_Hook(GLuint program)
        {
            if(glValidateProgram_Impl)
                glValidateProgram_Impl(program);
            PostHook();
        }

        void OVR::GLEContext::glVertexAttrib1d_Hook(GLuint index, GLdouble x)
        {
            if(glVertexAttrib1d_Impl)
                glVertexAttrib1d_Impl(index, x);
            PostHook();
        }

        void OVR::GLEContext::glVertexAttrib1dv_Hook(GLuint index, const GLdouble *v)
        {
            if(glVertexAttrib1dv_Impl)
                glVertexAttrib1dv_Impl(index, v);
            PostHook();
        }

        void OVR::GLEContext::glVertexAttrib1f_Hook(GLuint index, GLfloat x)
        {
            if(glVertexAttrib1f_Impl)
                glVertexAttrib1f_Impl(index, x);
            PostHook();
        }

        void OVR::GLEContext::glVertexAttrib1fv_Hook(GLuint index, const GLfloat *v)
        {
            if(glVertexAttrib1fv_Impl)
                glVertexAttrib1fv_Impl(index, v);
            PostHook();
        }

        void OVR::GLEContext::glVertexAttrib1s_Hook(GLuint index, GLshort x)
        {
            if(glVertexAttrib1s_Impl)
                glVertexAttrib1s_Impl(index, x);
            PostHook();
        }

        void OVR::GLEContext::glVertexAttrib1sv_Hook(GLuint index, const GLshort *v)
        {
            if(glVertexAttrib1sv_Impl)
                glVertexAttrib1sv_Impl(index, v);
            PostHook();
        }

        void OVR::GLEContext::glVertexAttrib2d_Hook(GLuint index, GLdouble x, GLdouble y)
        {
            if(glVertexAttrib2d_Impl)
                glVertexAttrib2d_Impl(index, x, y);
            PostHook();
        }

        void OVR::GLEContext::glVertexAttrib2dv_Hook(GLuint index, const GLdouble *v)
        {
            if(glVertexAttrib2dv_Impl)
                glVertexAttrib2dv_Impl(index, v);
            PostHook();
        }

        void OVR::GLEContext::glVertexAttrib2f_Hook(GLuint index, GLfloat x, GLfloat y)
        {
            if(glVertexAttrib2f_Impl)
                glVertexAttrib2f_Impl(index, x, y);
            PostHook();
        }

        void OVR::GLEContext::glVertexAttrib2fv_Hook(GLuint index, const GLfloat *v)
        {
            if(glVertexAttrib2fv_Impl)
                glVertexAttrib2fv_Impl(index, v);
            PostHook();
        }

        void OVR::GLEContext::glVertexAttrib2s_Hook(GLuint index, GLshort x, GLshort y)
        {
            if(glVertexAttrib2s_Impl)
                glVertexAttrib2s_Impl(index, x, y);
            PostHook();
        }

        void OVR::GLEContext::glVertexAttrib2sv_Hook(GLuint index, const GLshort *v)
        {
            if(glVertexAttrib2sv_Impl)
                glVertexAttrib2sv_Impl(index, v);
            PostHook();
        }

        void OVR::GLEContext::glVertexAttrib3d_Hook(GLuint index, GLdouble x, GLdouble y, GLdouble z)
        {
            if(glVertexAttrib3d_Impl)
                glVertexAttrib3d_Impl(index, x, y, z);
            PostHook();
        }

        void OVR::GLEContext::glVertexAttrib3dv_Hook(GLuint index, const GLdouble *v)
        {
            if(glVertexAttrib3dv_Impl)
                glVertexAttrib3dv_Impl(index, v);
            PostHook();
        }

        void OVR::GLEContext::glVertexAttrib3f_Hook(GLuint index, GLfloat x, GLfloat y, GLfloat z)
        {
            if(glVertexAttrib3f_Impl)
                glVertexAttrib3f_Impl(index, x, y, z);
            PostHook();
        }

        void OVR::GLEContext::glVertexAttrib3fv_Hook(GLuint index, const GLfloat *v)
        {
            if(glVertexAttrib3fv_Impl)
                glVertexAttrib3fv_Impl(index, v);
            PostHook();
        }

        void OVR::GLEContext::glVertexAttrib3s_Hook(GLuint index, GLshort x, GLshort y, GLshort z)
        {
            if(glVertexAttrib3s_Impl)
                glVertexAttrib3s_Impl(index, x, y, z);
            PostHook();
        }

        void OVR::GLEContext::glVertexAttrib3sv_Hook(GLuint index, const GLshort *v)
        {
            if(glVertexAttrib3sv_Impl)
                glVertexAttrib3sv_Impl(index, v);
            PostHook();
        }

        void OVR::GLEContext::glVertexAttrib4Nbv_Hook(GLuint index, const GLbyte *v)
        {
            if(glVertexAttrib4Nbv_Impl)
                glVertexAttrib4Nbv_Impl(index, v);
            PostHook();
        }

        void OVR::GLEContext::glVertexAttrib4Niv_Hook(GLuint index, const GLint *v)
        {
            if(glVertexAttrib4Niv_Impl)
                glVertexAttrib4Niv_Impl(index, v);
            PostHook();
        }

        void OVR::GLEContext::glVertexAttrib4Nsv_Hook(GLuint index, const GLshort *v)
        {
            if(glVertexAttrib4Nsv_Impl)
                glVertexAttrib4Nsv_Impl(index, v);
            PostHook();
        }

        void OVR::GLEContext::glVertexAttrib4Nub_Hook(GLuint index, GLubyte x, GLubyte y, GLubyte z, GLubyte w)
        {
            if(glVertexAttrib4Nub_Impl)
                glVertexAttrib4Nub_Impl(index, x, y, z, w);
            PostHook();
        }

        void OVR::GLEContext::glVertexAttrib4Nubv_Hook(GLuint index, const GLubyte *v)
        {
            if(glVertexAttrib4Nubv_Impl)
                glVertexAttrib4Nubv_Impl(index, v);
            PostHook();
        }

        void OVR::GLEContext::glVertexAttrib4Nuiv_Hook(GLuint index, const GLuint *v)
        {
            if(glVertexAttrib4Nuiv_Impl)
                glVertexAttrib4Nuiv_Impl(index, v);
            PostHook();
        }

        void OVR::GLEContext::glVertexAttrib4Nusv_Hook(GLuint index, const GLushort *v)
        {
            if(glVertexAttrib4Nusv_Impl)
                glVertexAttrib4Nusv_Impl(index, v);
            PostHook();
        }

        void OVR::GLEContext::glVertexAttrib4bv_Hook(GLuint index, const GLbyte *v)
        {
            if(glVertexAttrib4bv_Impl)
                glVertexAttrib4bv_Impl(index, v);
            PostHook();
        }

        void OVR::GLEContext::glVertexAttrib4d_Hook(GLuint index, GLdouble x, GLdouble y, GLdouble z, GLdouble w)
        {
            if(glVertexAttrib4d_Impl)
                glVertexAttrib4d_Impl(index, x, y, z, w);
            PostHook();
        }

        void OVR::GLEContext::glVertexAttrib4dv_Hook(GLuint index, const GLdouble *v)
        {
            if(glVertexAttrib4dv_Impl)
                glVertexAttrib4dv_Impl(index, v);
            PostHook();
        }

        void OVR::GLEContext::glVertexAttrib4f_Hook(GLuint index, GLfloat x, GLfloat y, GLfloat z, GLfloat w)
        {
            if(glVertexAttrib4f_Impl)
                glVertexAttrib4f_Impl(index, x, y, z, w);
            PostHook();
        }

        void OVR::GLEContext::glVertexAttrib4fv_Hook(GLuint index, const GLfloat *v)
        {
            if(glVertexAttrib4fv_Impl)
                glVertexAttrib4fv_Impl(index, v);
            PostHook();
        }

        void OVR::GLEContext::glVertexAttrib4iv_Hook(GLuint index, const GLint *v)
        {
            if(glVertexAttrib4iv_Impl)
                glVertexAttrib4iv_Impl(index, v);
            PostHook();
        }

        void OVR::GLEContext::glVertexAttrib4s_Hook(GLuint index, GLshort x, GLshort y, GLshort z, GLshort w)
        {
            if(glVertexAttrib4s_Impl)
                glVertexAttrib4s_Impl(index, x, y, z, w);
            PostHook();
        }

        void OVR::GLEContext::glVertexAttrib4sv_Hook(GLuint index, const GLshort *v)
        {
            if(glVertexAttrib4sv_Impl)
                glVertexAttrib4sv_Impl(index, v);
            PostHook();
        }

        void OVR::GLEContext::glVertexAttrib4ubv_Hook(GLuint index, const GLubyte *v)
        {
            if(glVertexAttrib4ubv_Impl)
                glVertexAttrib4ubv_Impl(index, v);
            PostHook();
        }

        void OVR::GLEContext::glVertexAttrib4uiv_Hook(GLuint index, const GLuint *v)
        {
            if(glVertexAttrib4uiv_Impl)
                glVertexAttrib4uiv_Impl(index, v);
            PostHook();
        }

        void OVR::GLEContext::glVertexAttrib4usv_Hook(GLuint index, const GLushort *v)
        {
            if(glVertexAttrib4usv_Impl)
                glVertexAttrib4usv_Impl(index, v);
            PostHook();
        }

        void OVR::GLEContext::glVertexAttribPointer_Hook(GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const GLvoid *pointer)
        {
            if(glVertexAttribPointer_Impl)
                glVertexAttribPointer_Impl(index, size, type, normalized, stride, pointer);
            PostHook();
        }


        // GL_VERSION_2_1
        void OVR::GLEContext::glUniformMatrix2x3fv_Hook(GLint location, GLsizei count, GLboolean transpose, const GLfloat *value)
        {
            if(glUniformMatrix2x3fv_Impl)
                glUniformMatrix2x3fv_Impl(location, count, transpose, value);
            PostHook();
        }

        void OVR::GLEContext::glUniformMatrix3x2fv_Hook(GLint location, GLsizei count, GLboolean transpose, const GLfloat *value)
        {
            if(glUniformMatrix3x2fv_Impl)
                glUniformMatrix3x2fv_Impl(location, count, transpose, value);
            PostHook();
        }

        void OVR::GLEContext::glUniformMatrix2x4fv_Hook(GLint location, GLsizei count, GLboolean transpose, const GLfloat *value)
        {
            if(glUniformMatrix2x4fv_Impl)
                glUniformMatrix2x4fv_Impl(location, count, transpose, value);
            PostHook();
        }

        void OVR::GLEContext::glUniformMatrix4x2fv_Hook(GLint location, GLsizei count, GLboolean transpose, const GLfloat *value)
        {
            if(glUniformMatrix4x2fv_Impl)
                glUniformMatrix4x2fv_Impl(location, count, transpose, value);
            PostHook();
        }

        void OVR::GLEContext::glUniformMatrix3x4fv_Hook(GLint location, GLsizei count, GLboolean transpose, const GLfloat *value)
        {
            if(glUniformMatrix3x4fv_Impl)
                glUniformMatrix3x4fv_Impl(location, count, transpose, value);
            PostHook();
        }

        void OVR::GLEContext::glUniformMatrix4x3fv_Hook(GLint location, GLsizei count, GLboolean transpose, const GLfloat *value)
        {
            if(glUniformMatrix4x3fv_Impl)
                glUniformMatrix4x3fv_Impl(location, count, transpose, value);
            PostHook();
        }


        // GL_VERSION_3_0
        void OVR::GLEContext::glColorMaski_Hook(GLuint index, GLboolean r, GLboolean g, GLboolean b, GLboolean a)
        {
            if(glColorMaski_Impl)
                glColorMaski_Impl(index, r, g, b, a);
            PostHook();
        }

        void OVR::GLEContext::glGetBooleani_v_Hook(GLenum target, GLuint index, GLboolean *data)
        {
            if(glGetBooleani_v_Impl)
                glGetBooleani_v_Impl(target, index, data);
            PostHook();
        }

        void OVR::GLEContext::glGetIntegeri_v_Hook(GLenum target, GLuint index, GLint *data)
        {
            if(glGetIntegeri_v_Impl)
                glGetIntegeri_v_Impl(target, index, data);
            PostHook();
        }

        void OVR::GLEContext::glEnablei_Hook(GLenum target, GLuint index)
        {
            if(glEnablei_Impl)
                glEnablei_Impl(target, index);
            PostHook();
        }

        void OVR::GLEContext::glDisablei_Hook(GLenum target, GLuint index)
        {
            if(glDisablei_Impl)
                glDisablei_Impl(target, index);
            PostHook();
        }

        GLboolean OVR::GLEContext::glIsEnabledi_Hook(GLenum target, GLuint index)
        {
            GLboolean b = GL_FALSE;
            if(glIsEnabledi_Impl)
                b = glIsEnabledi_Impl(target, index);
            PostHook();
            return b;
        }

        void OVR::GLEContext::glBeginTransformFeedback_Hook(GLenum primitiveMode)
        {
            if(glBeginTransformFeedback_Impl)
                glBeginTransformFeedback_Impl(primitiveMode);
            PostHook();
        }

        void OVR::GLEContext::glEndTransformFeedback_Hook()
        {
            if(glEndTransformFeedback_Impl)
                glEndTransformFeedback_Impl();
            PostHook();
        }

        void OVR::GLEContext::glBindBufferRange_Hook(GLenum target, GLuint index, GLuint buffer, GLintptr offset, GLsizeiptr size)
        {
            if(glBindBufferRange_Impl)
                glBindBufferRange_Impl(target, index, buffer, offset, size);
            PostHook();
        }

        void OVR::GLEContext::glBindBufferBase_Hook(GLenum target, GLuint index, GLuint buffer)
        {
            if(glBindBufferBase_Impl)
                glBindBufferBase_Impl(target, index, buffer);
            PostHook();
        }

        void OVR::GLEContext::glTransformFeedbackVaryings_Hook(GLuint program, GLsizei count, const GLchar* *varyings, GLenum bufferMode)
        {
            if(glTransformFeedbackVaryings_Impl)
                glTransformFeedbackVaryings_Impl(program, count, varyings, bufferMode);
            PostHook();
        }

        void OVR::GLEContext::glGetTransformFeedbackVarying_Hook(GLuint program, GLuint index, GLsizei bufSize, GLsizei *length, GLsizei *size, GLenum *type, GLchar *name)
        {
            if(glGetTransformFeedbackVarying_Impl)
                glGetTransformFeedbackVarying_Impl(program, index, bufSize, length, size, type, name);
            PostHook();
        }

        void OVR::GLEContext::glClampColor_Hook(GLenum target, GLenum clamp)
        {
            if(glClampColor_Impl)
                glClampColor_Impl(target, clamp);
            PostHook();
        }

        void OVR::GLEContext::glBeginConditionalRender_Hook(GLuint id, GLenum mode)
        {
            if(glBeginConditionalRender_Impl)
                glBeginConditionalRender_Impl(id, mode);
            PostHook();
        }

        void OVR::GLEContext::glEndConditionalRender_Hook()
        {
            if(glEndConditionalRender_Impl)
                glEndConditionalRender_Impl();
            PostHook();
        }

        void OVR::GLEContext::glVertexAttribIPointer_Hook(GLuint index, GLint size, GLenum type, GLsizei stride, const GLvoid *pointer)
        {
            if(glVertexAttribIPointer_Impl)
                glVertexAttribIPointer_Impl(index, size, type,  stride, pointer);
            PostHook();
        }

        void OVR::GLEContext::glGetVertexAttribIiv_Hook(GLuint index, GLenum pname, GLint *params)
        {
            if(glGetVertexAttribIiv_Impl)
                glGetVertexAttribIiv_Impl(index, pname, params);
            PostHook();
        }

        void OVR::GLEContext::glGetVertexAttribIuiv_Hook(GLuint index, GLenum pname, GLuint *params)
        {
            if(glGetVertexAttribIuiv_Impl)
                glGetVertexAttribIuiv_Impl(index, pname, params);
            PostHook();
        }

        void OVR::GLEContext::glVertexAttribI1i_Hook(GLuint index, GLint x)
        {
            if(glVertexAttribI1i_Impl)
                glVertexAttribI1i_Impl(index, x);
            PostHook();
        }

        void OVR::GLEContext::glVertexAttribI2i_Hook(GLuint index, GLint x, GLint y)
        {
            if(glVertexAttribI2i_Impl)
                glVertexAttribI2i_Impl(index, x, y);
            PostHook();
        }

        void OVR::GLEContext::glVertexAttribI3i_Hook(GLuint index, GLint x, GLint y, GLint z)
        {
            if(glVertexAttribI3i_Impl)
                glVertexAttribI3i_Impl(index, x, y, z);
            PostHook();
        }

        void OVR::GLEContext::glVertexAttribI4i_Hook(GLuint index, GLint x, GLint y, GLint z, GLint w)
        {
            if(glVertexAttribI4i_Impl)
                glVertexAttribI4i_Impl(index, x, y, z, w);
            PostHook();
        }

        void OVR::GLEContext::glVertexAttribI1ui_Hook(GLuint index, GLuint x)
        {
            if(glVertexAttribI1ui_Impl)
                glVertexAttribI1ui_Impl(index, x);
            PostHook();
        }

        void OVR::GLEContext::glVertexAttribI2ui_Hook(GLuint index, GLuint x, GLuint y)
        {
            if(glVertexAttribI2ui_Impl)
                glVertexAttribI2ui_Impl(index, x, y);
            PostHook();
        }

        void OVR::GLEContext::glVertexAttribI3ui_Hook(GLuint index, GLuint x, GLuint y, GLuint z)
        {
            if(glVertexAttribI3ui_Impl)
                glVertexAttribI3ui_Impl(index, x, y, z);
            PostHook();
        }

        void OVR::GLEContext::glVertexAttribI4ui_Hook(GLuint index, GLuint x, GLuint y, GLuint z, GLuint w)
        {
            if(glVertexAttribI4ui_Impl)
                glVertexAttribI4ui_Impl(index, x, y, z, w);
            PostHook();
        }

        void OVR::GLEContext::glVertexAttribI1iv_Hook(GLuint index, const GLint *v)
        {
            if(glVertexAttribI1iv_Impl)
                glVertexAttribI1iv_Impl(index, v);
            PostHook();
        }

        void OVR::GLEContext::glVertexAttribI2iv_Hook(GLuint index, const GLint *v)
        {
            if(glVertexAttribI2iv_Impl)
                glVertexAttribI2iv_Impl(index, v);
            PostHook();
        }

        void OVR::GLEContext::glVertexAttribI3iv_Hook(GLuint index, const GLint *v)
        {
            if(glVertexAttribI3iv_Impl)
                glVertexAttribI3iv_Impl(index, v);
            PostHook();
        }

        void OVR::GLEContext::glVertexAttribI4iv_Hook(GLuint index, const GLint *v)
        {
            if(glVertexAttribI4iv_Impl)
                glVertexAttribI4iv_Impl(index, v);
            PostHook();
        }

        void OVR::GLEContext::glVertexAttribI1uiv_Hook(GLuint index, const GLuint *v)
        {
            if(glVertexAttribI1uiv_Impl)
                glVertexAttribI1uiv_Impl(index, v);
            PostHook();
        }

        void OVR::GLEContext::glVertexAttribI2uiv_Hook(GLuint index, const GLuint *v)
        {
            if(glVertexAttribI2uiv_Impl)
                glVertexAttribI2uiv_Impl(index, v);
            PostHook();
        }

        void OVR::GLEContext::glVertexAttribI3uiv_Hook(GLuint index, const GLuint *v)
        {
            if(glVertexAttribI3uiv_Impl)
                glVertexAttribI3uiv_Impl(index, v);
            PostHook();
        }

        void OVR::GLEContext::glVertexAttribI4uiv_Hook(GLuint index, const GLuint *v)
        {
            if(glVertexAttribI4uiv_Impl)
                glVertexAttribI4uiv_Impl(index, v);
            PostHook();
        }

        void OVR::GLEContext::glVertexAttribI4bv_Hook(GLuint index, const GLbyte *v)
        {
            if(glVertexAttribI4bv_Impl)
                glVertexAttribI4bv_Impl(index, v);
            PostHook();
        }

        void OVR::GLEContext::glVertexAttribI4sv_Hook(GLuint index, const GLshort *v)
        {
            if(glVertexAttribI4sv_Impl)
                glVertexAttribI4sv_Impl(index, v);
            PostHook();
        }

        void OVR::GLEContext::glVertexAttribI4ubv_Hook(GLuint index, const GLubyte *v)
        {
            if(glVertexAttribI4ubv_Impl)
                glVertexAttribI4ubv_Impl(index, v);
            PostHook();
        }

        void OVR::GLEContext::glVertexAttribI4usv_Hook(GLuint index, const GLushort *v)
        {
            if(glVertexAttribI4usv_Impl)
                glVertexAttribI4usv_Impl(index, v);
            PostHook();
        }

        void OVR::GLEContext::glGetUniformuiv_Hook(GLuint program, GLint location, GLuint *params)
        {
            if(glGetUniformuiv_Impl)
                glGetUniformuiv_Impl(program, location, params);
            PostHook();
        }

        void OVR::GLEContext::glBindFragDataLocation_Hook(GLuint program, GLuint color, const GLchar *name)
        {
            if(glBindFragDataLocation_Impl)
                glBindFragDataLocation_Impl(program, color, name);
            PostHook();
        }

        GLint OVR::GLEContext::glGetFragDataLocation_Hook(GLuint program, const GLchar *name)
        {
            GLint i = 0;
            if(glGetFragDataLocation_Impl)
                i = glGetFragDataLocation_Impl(program, name);
            PostHook();
            return i;
        }

        void OVR::GLEContext::glUniform1ui_Hook(GLint location, GLuint v0)
        {
            if(glUniform1ui_Impl)
                glUniform1ui_Impl(location, v0);
            PostHook();
        }

        void OVR::GLEContext::glUniform2ui_Hook(GLint location, GLuint v0, GLuint v1)
        {
            if(glUniform2ui_Impl)
                glUniform2ui_Impl(location, v0, v1);
            PostHook();
        }

        void OVR::GLEContext::glUniform3ui_Hook(GLint location, GLuint v0, GLuint v1, GLuint v2)
        {
            if(glUniform3ui_Impl)
                glUniform3ui_Impl(location, v0, v1, v2);
            PostHook();
        }

        void OVR::GLEContext::glUniform4ui_Hook(GLint location, GLuint v0, GLuint v1, GLuint v2, GLuint v3)
        {
            if(glUniform4ui_Impl)
                glUniform4ui_Impl(location, v0, v1, v2, v3);
            PostHook();
        }

        void OVR::GLEContext::glUniform1uiv_Hook(GLint location, GLsizei count, const GLuint *value)
        {
            if(glUniform1uiv_Impl)
                glUniform1uiv_Impl(location, count, value);
            PostHook();
        }

        void OVR::GLEContext::glUniform2uiv_Hook(GLint location, GLsizei count, const GLuint *value)
        {
            if(glUniform2uiv_Impl)
                glUniform2uiv_Impl(location, count, value);
            PostHook();
        }

        void OVR::GLEContext::glUniform3uiv_Hook(GLint location, GLsizei count, const GLuint *value)
        {
            if(glUniform3uiv_Impl)
                glUniform3uiv_Impl(location, count, value);
            PostHook();
        }

        void OVR::GLEContext::glUniform4uiv_Hook(GLint location, GLsizei count, const GLuint *value)
        {
            if(glUniform4uiv_Impl)
                glUniform4uiv_Impl(location, count, value);
            PostHook();
        }

        void OVR::GLEContext::glTexParameterIiv_Hook(GLenum target, GLenum pname, const GLint *params)
        {
            if(glTexParameterIiv_Impl)
                glTexParameterIiv_Impl(target, pname, params);
            PostHook();
        }

        void OVR::GLEContext::glTexParameterIuiv_Hook(GLenum target, GLenum pname, const GLuint *params)
        {
            if(glTexParameterIuiv_Impl)
                glTexParameterIuiv_Impl(target, pname, params);
            PostHook();
        }

        void OVR::GLEContext::glGetTexParameterIiv_Hook(GLenum target, GLenum pname, GLint *params)
        {
            if(glGetTexParameterIiv_Impl)
                glGetTexParameterIiv_Impl(target, pname, params);
            PostHook();
        }

        void OVR::GLEContext::glGetTexParameterIuiv_Hook(GLenum target, GLenum pname, GLuint *params)
        {
            if(glGetTexParameterIuiv_Impl)
                glGetTexParameterIuiv_Impl(target, pname, params);
            PostHook();
        }

        void OVR::GLEContext::glClearBufferiv_Hook(GLenum buffer, GLint drawbuffer, const GLint *value)
        {
            if(glClearBufferiv_Impl)
                glClearBufferiv_Impl(buffer, drawbuffer, value);
            PostHook();
        }

        void OVR::GLEContext::glClearBufferuiv_Hook(GLenum buffer, GLint drawbuffer, const GLuint *value)
        {
            if(glClearBufferuiv_Impl)
                glClearBufferuiv_Impl(buffer, drawbuffer, value);
            PostHook();
        }

        void OVR::GLEContext::glClearBufferfv_Hook(GLenum buffer, GLint drawbuffer, const GLfloat *value)
        {
            if(glClearBufferfv_Impl)
                glClearBufferfv_Impl(buffer, drawbuffer, value);
            PostHook();
        }

        void OVR::GLEContext::glClearBufferfi_Hook(GLenum buffer, GLint drawbuffer, GLfloat depth, GLint stencil)
        {
            if(glClearBufferfi_Impl)
                glClearBufferfi_Impl(buffer, drawbuffer, depth, stencil);
            PostHook();
        }

        const GLubyte* OVR::GLEContext::glGetStringi_Hook(GLenum name, GLuint index)
        {
            const GLubyte* p = NULL;
            if(glGetStringi_Impl)
                p = glGetStringi_Impl(name, index);
            PostHook();
            return p;
        }


        // GL_VERSION_3_1
        void OVR::GLEContext::glDrawArraysInstanced_Hook(GLenum mode, GLint first, GLsizei count, GLsizei primcount)
        {
            if(glDrawArraysInstanced_Impl)
                glDrawArraysInstanced_Impl(mode, first, count, primcount);
            PostHook();
        }

        void OVR::GLEContext::glDrawElementsInstanced_Hook(GLenum mode, GLsizei count, GLenum type, const GLvoid *indices, GLsizei primcount)
        {
            if(glDrawElementsInstanced_Impl)
                glDrawElementsInstanced_Impl(mode, count, type, indices, primcount);
            PostHook();
        }

        void OVR::GLEContext::glTexBuffer_Hook(GLenum target, GLenum internalformat, GLuint buffer)
        {
            if(glTexBuffer_Impl)
                glTexBuffer_Impl(target, internalformat, buffer);
            PostHook();
        }

        void OVR::GLEContext::glPrimitiveRestartIndex_Hook(GLuint index)
        {
            if(glPrimitiveRestartIndex_Impl)
                glPrimitiveRestartIndex_Impl(index);
            PostHook();
        }


        // GL_VERSION_3_2
        void OVR::GLEContext::glGetInteger64i_v_Hook(GLenum target, GLuint index, GLint64 *data)
        {
            if(glGetInteger64i_v_Impl)
                glGetInteger64i_v_Impl(target, index, data);
            PostHook();
        }

        void OVR::GLEContext::glGetBufferParameteri64v_Hook(GLenum target, GLenum pname, GLint64 *params)
        {
            if(glGetBufferParameteri64v_Impl)
                glGetBufferParameteri64v_Impl(target, pname, params);
            PostHook();
        }

        void OVR::GLEContext::glFramebufferTexture_Hook(GLenum target, GLenum attachment, GLuint texture, GLint level)
        {
            if(glFramebufferTexture_Impl)
                glFramebufferTexture_Impl(target, attachment, texture, level);
            PostHook();
        }


        // GL_VERSION_3_3
        void OVR::GLEContext::glVertexAttribDivisor_Hook(GLuint index, GLuint divisor)
        {
            if(glVertexAttribDivisor_Impl)
                glVertexAttribDivisor_Impl(index, divisor);
            PostHook();
        }


        // GL_VERSION_4_0
        void OVR::GLEContext::glMinSampleShading_Hook(GLclampf value)
        {
            if(glMinSampleShading_Impl)
                glMinSampleShading_Impl(value);
            PostHook();
        }

        void OVR::GLEContext::glBlendEquationi_Hook(GLuint buf, GLenum mode)
        {
            if(glBlendEquationi_Impl)
                glBlendEquationi_Impl(buf, mode);
            PostHook();
        }

        void OVR::GLEContext::glBlendEquationSeparatei_Hook(GLuint buf, GLenum modeRGB, GLenum modeAlpha)
        {
            if(glBlendEquationSeparatei_Impl)
                glBlendEquationSeparatei_Impl(buf, modeRGB, modeAlpha);
            PostHook();
        }

        void OVR::GLEContext::glBlendFunci_Hook(GLuint buf, GLenum src, GLenum dst)
        {
            if(glBlendFunci_Impl)
                glBlendFunci_Impl(buf, src, dst);
            PostHook();
        }

        void OVR::GLEContext::glBlendFuncSeparatei_Hook(GLuint buf, GLenum srcRGB, GLenum dstRGB, GLenum srcAlpha, GLenum dstAlpha)
        {
            if(glBlendFuncSeparatei_Impl)
                glBlendFuncSeparatei_Impl(buf, srcRGB, dstRGB, srcAlpha, dstAlpha);
            PostHook();
        }


        // GL_AMD_debug_output
        void OVR::GLEContext::glDebugMessageEnableAMD_Hook(GLenum category, GLenum severity, GLsizei count, const GLuint *ids, GLboolean enabled)
        {
            if(glDebugMessageEnableAMD_Impl)
                glDebugMessageEnableAMD_Impl(category, severity, count, ids, enabled);
            PostHook();
        }

        void OVR::GLEContext::glDebugMessageInsertAMD_Hook(GLenum category, GLenum severity, GLuint id, GLsizei length, const GLchar *buf)
        {
            if(glDebugMessageInsertAMD_Impl)
                glDebugMessageInsertAMD_Impl(category, severity, id, length, buf);
            PostHook();
        }

        void OVR::GLEContext::glDebugMessageCallbackAMD_Hook(GLDEBUGPROCAMD callback, GLvoid *userParam)
        {
            if(glDebugMessageCallbackAMD_Impl)
                glDebugMessageCallbackAMD_Impl(callback, userParam);
            PostHook();
        }

        GLuint OVR::GLEContext::glGetDebugMessageLogAMD_Hook(GLuint count, GLsizei bufsize, GLenum *categories, GLuint *severities, GLuint *ids, GLsizei *lengths, GLchar *message)
        {
            GLuint u = 0;
            if(glGetDebugMessageLogAMD_Impl)
                u = glGetDebugMessageLogAMD_Impl(count, bufsize, categories, severities, ids, lengths, message);
            PostHook();
            return u;
        }


    #if defined(GLE_APPLE_ENABLED)
        // GL_APPLE_element_array
        void OVR::GLEContext::glElementPointerAPPLE_Hook(GLenum type, const GLvoid *pointer)
        {
            if(glElementPointerAPPLE_Impl)
                glElementPointerAPPLE_Impl(type, pointer);
            PostHook();
        }

        void OVR::GLEContext::glDrawElementArrayAPPLE_Hook(GLenum mode, GLint first, GLsizei count)
        {
            if(glDrawElementArrayAPPLE_Impl)
                glDrawElementArrayAPPLE_Impl(mode, first, count);
            PostHook();
        }

        void OVR::GLEContext::glDrawRangeElementArrayAPPLE_Hook(GLenum mode, GLuint start, GLuint end, GLint first, GLsizei count)
        {
            if(glDrawRangeElementArrayAPPLE_Impl)
                glDrawRangeElementArrayAPPLE_Impl(mode, start, end, first, count);
            PostHook();
        }

        void OVR::GLEContext::glMultiDrawElementArrayAPPLE_Hook(GLenum mode, const GLint *first, const GLsizei *count, GLsizei primcount)
        {
            if(glMultiDrawElementArrayAPPLE_Impl)
                glMultiDrawElementArrayAPPLE_Impl(mode, first, count, primcount);
            PostHook();
        }

        void OVR::GLEContext::glMultiDrawRangeElementArrayAPPLE_Hook(GLenum mode, GLuint start, GLuint end, const GLint *first, const GLsizei *count, GLsizei primcount)
        {
            if(glMultiDrawRangeElementArrayAPPLE_Impl)
                glMultiDrawRangeElementArrayAPPLE_Impl(mode, start, end, first, count, primcount);
            PostHook();
        }


        // GL_APPLE_fence
        void OVR::GLEContext::glGenFencesAPPLE_Hook(GLsizei n, GLuint *fences)
        {
            if(glGenFencesAPPLE_Impl)
                glGenFencesAPPLE_Impl(n, fences);
            PostHook();
        }

        void OVR::GLEContext::glDeleteFencesAPPLE_Hook(GLsizei n, const GLuint *fences)
        {
            if(glDeleteFencesAPPLE_Impl)
                glDeleteFencesAPPLE_Impl(n, fences);
            PostHook();
        }

        void OVR::GLEContext::glSetFenceAPPLE_Hook(GLuint fence)
        {
            if(glSetFenceAPPLE_Impl)
                glSetFenceAPPLE_Impl(fence);
            PostHook();
        }

        GLboolean OVR::GLEContext::glIsFenceAPPLE_Hook(GLuint fence)
        {
            GLboolean b = GL_FALSE;
            if(glIsFenceAPPLE_Impl)
                b = glIsFenceAPPLE_Impl(fence);
            PostHook();
            return b;
        }

        GLboolean OVR::GLEContext::glTestFenceAPPLE_Hook(GLuint fence)
        {
            GLboolean b = GL_FALSE;
            if(glTestFenceAPPLE_Impl)
                b = glTestFenceAPPLE_Impl(fence);
            PostHook();
            return b;
        }

        void OVR::GLEContext::glFinishFenceAPPLE_Hook(GLuint fence)
        {
            if(glFinishFenceAPPLE_Impl)
                glFinishFenceAPPLE_Impl(fence);
            PostHook();
        }

        GLboolean OVR::GLEContext::glTestObjectAPPLE_Hook(GLenum object, GLuint name)
        {
            GLboolean b = GL_FALSE;
            if(glTestObjectAPPLE_Impl)
                b = glTestObjectAPPLE_Impl(object, name);
            PostHook();
            return b;
        }

        void OVR::GLEContext::glFinishObjectAPPLE_Hook(GLenum object, GLint name)
        {
            if(glFinishObjectAPPLE_Impl)
                glFinishObjectAPPLE_Impl(object, name);
            PostHook();
        }


        // GL_APPLE_flush_buffer_range
        void OVR::GLEContext::glBufferParameteriAPPLE_Hook(GLenum target, GLenum pname, GLint param)
        {
            if(glBufferParameteriAPPLE_Impl)
                glBufferParameteriAPPLE_Impl(target, pname, param);
            PostHook();
        }

        void OVR::GLEContext::glFlushMappedBufferRangeAPPLE_Hook(GLenum target, GLintptr offset, GLsizeiptr size)
        {
            if(glFlushMappedBufferRangeAPPLE_Impl)
                glFlushMappedBufferRangeAPPLE_Impl(target, offset, size);
            PostHook();
        }


        // GL_APPLE_object_purgeable
        GLenum OVR::GLEContext::glObjectPurgeableAPPLE_Hook(GLenum objectType, GLuint name, GLenum option)
        {
            GLenum e = 0;
            if(glObjectPurgeableAPPLE_Impl)
                e = glObjectPurgeableAPPLE_Impl(objectType, name, option);
            PostHook();
            return e;
        }

        GLenum OVR::GLEContext::glObjectUnpurgeableAPPLE_Hook(GLenum objectType, GLuint name, GLenum option)
        {
            GLenum e = 0;
            if(glObjectUnpurgeableAPPLE_Impl)
                e =glObjectUnpurgeableAPPLE_Impl(objectType, name, option);
            PostHook();
            return e;
        }

        void OVR::GLEContext::glGetObjectParameterivAPPLE_Hook(GLenum objectType, GLuint name, GLenum pname, GLint *params)
        {
            if(glGetObjectParameterivAPPLE_Impl)
                glGetObjectParameterivAPPLE_Impl(objectType, name, pname, params);
            PostHook();
        }


        // GL_APPLE_texture_range
        void OVR::GLEContext::glTextureRangeAPPLE_Hook(GLenum target, GLsizei length, const GLvoid *pointer)
        {
            if(glTextureRangeAPPLE_Impl)
                glTextureRangeAPPLE_Impl(target, length, pointer);
            PostHook();
        }

        void OVR::GLEContext::glGetTexParameterPointervAPPLE_Hook(GLenum target, GLenum pname, GLvoid **params)
        {
            if(glGetTexParameterPointervAPPLE_Impl)
                glGetTexParameterPointervAPPLE_Impl(target, pname, params);
            PostHook();
        }


        // GL_APPLE_vertex_array_object
        void OVR::GLEContext::glBindVertexArrayAPPLE_Hook(GLuint array)
        {
            if(glBindVertexArrayAPPLE_Impl)
                glBindVertexArrayAPPLE_Impl(array);
            PostHook();
        }

        void OVR::GLEContext::glDeleteVertexArraysAPPLE_Hook(GLsizei n, const GLuint *arrays)
        {
            if(glDeleteVertexArraysAPPLE_Impl)
                glDeleteVertexArraysAPPLE_Impl(n, arrays);
            PostHook();
        }

        void OVR::GLEContext::glGenVertexArraysAPPLE_Hook(GLsizei n, GLuint *arrays)
        {
            if(glGenVertexArraysAPPLE_Impl)
                glGenVertexArraysAPPLE_Impl(n, arrays);
            PostHook();
        }

        GLboolean OVR::GLEContext::glIsVertexArrayAPPLE_Hook(GLuint array)
        {
            GLboolean b = GL_FALSE;
            if(glIsVertexArrayAPPLE_Impl)
                b = glIsVertexArrayAPPLE_Impl(array);
            PostHook();
            return b;
        }


        // GL_APPLE_vertex_array_range
        void OVR::GLEContext::glVertexArrayRangeAPPLE_Hook(GLsizei length, GLvoid *pointer)
        {
            if(glVertexArrayRangeAPPLE_Impl)
                glVertexArrayRangeAPPLE_Impl(length, pointer);
            PostHook();
        }

        void OVR::GLEContext::glFlushVertexArrayRangeAPPLE_Hook(GLsizei length, GLvoid *pointer)
        {
            if(glFlushVertexArrayRangeAPPLE_Impl)
                glFlushVertexArrayRangeAPPLE_Impl(length, pointer);
            PostHook();
        }

        void OVR::GLEContext::glVertexArrayParameteriAPPLE_Hook(GLenum pname, GLint param)
        {
            if(glVertexArrayParameteriAPPLE_Impl)
                glVertexArrayParameteriAPPLE_Impl(pname, param);
            PostHook();
        }


        // GL_APPLE_vertex_program_evaluators
        void OVR::GLEContext::glEnableVertexAttribAPPLE_Hook(GLuint index, GLenum pname)
        {
            if(glEnableVertexAttribAPPLE_Impl)
                glEnableVertexAttribAPPLE_Impl(index, pname);
            PostHook();
        }

        void OVR::GLEContext::glDisableVertexAttribAPPLE_Hook(GLuint index, GLenum pname)
        {
            if(glDisableVertexAttribAPPLE_Impl)
                glDisableVertexAttribAPPLE_Impl(index, pname);
            PostHook();
        }

        GLboolean OVR::GLEContext::glIsVertexAttribEnabledAPPLE_Hook(GLuint index, GLenum pname)
        {
            GLboolean b = GL_FALSE;
            if(glIsVertexAttribEnabledAPPLE_Impl)
                b = glIsVertexAttribEnabledAPPLE_Impl(index, pname);
            PostHook();
            return b;
        }

        void OVR::GLEContext::glMapVertexAttrib1dAPPLE_Hook(GLuint index, GLuint size, GLdouble u1, GLdouble u2, GLint stride, GLint order, const GLdouble *points)
        {
            if(glMapVertexAttrib1dAPPLE_Impl)
                glMapVertexAttrib1dAPPLE_Impl(index, size, u1, u2, stride, order, points);
            PostHook();
        }

        void OVR::GLEContext::glMapVertexAttrib1fAPPLE_Hook(GLuint index, GLuint size, GLfloat u1, GLfloat u2, GLint stride, GLint order, const GLfloat *points)
        {
            if(glMapVertexAttrib1fAPPLE_Impl)
                glMapVertexAttrib1fAPPLE_Impl(index, size, u1, u2, stride, order, points);
            PostHook();
        }

        void OVR::GLEContext::glMapVertexAttrib2dAPPLE_Hook(GLuint index, GLuint size, GLdouble u1, GLdouble u2, GLint ustride, GLint uorder, GLdouble v1, GLdouble v2, GLint vstride, GLint vorder, const GLdouble *points)
        {
            if(glMapVertexAttrib2dAPPLE_Impl)
                glMapVertexAttrib2dAPPLE_Impl(index, size, u1, u2, ustride, uorder, v1, v2, vstride, vorder, points);
            PostHook();
        }

        void OVR::GLEContext::glMapVertexAttrib2fAPPLE_Hook(GLuint index, GLuint size, GLfloat u1, GLfloat u2, GLint ustride, GLint uorder, GLfloat v1, GLfloat v2, GLint vstride, GLint vorder, const GLfloat *points)
        {
            if(glMapVertexAttrib2fAPPLE_Impl)
                glMapVertexAttrib2fAPPLE_Impl(index, size, u1, u2, ustride, uorder, v1, v2, vstride, vorder, points);
            PostHook();
        }
    #endif // GLE_APPLE_ENABLED


        // GL_ARB_debug_output
        void OVR::GLEContext::glDebugMessageControlARB_Hook(GLenum source, GLenum type, GLenum severity, GLsizei count, const GLuint *ids, GLboolean enabled)
        {
            if(glDebugMessageControlARB_Impl)
                glDebugMessageControlARB_Impl(source, type, severity, count, ids, enabled);
            PostHook();
        }

        void OVR::GLEContext::glDebugMessageInsertARB_Hook(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar *buf)
        {
            if(glDebugMessageInsertARB_Impl)
                glDebugMessageInsertARB_Impl(source, type, id, severity, length, buf);
            PostHook();
        }

        void OVR::GLEContext::glDebugMessageCallbackARB_Hook(GLDEBUGPROCARB callback, const GLvoid *userParam)
        {
            if(glDebugMessageCallbackARB_Impl)
                glDebugMessageCallbackARB_Impl(callback, userParam);
            PostHook();
        }

        GLuint OVR::GLEContext::glGetDebugMessageLogARB_Hook(GLuint count, GLsizei bufsize, GLenum *sources, GLenum *types, GLuint *ids, GLenum *severities, GLsizei *lengths, GLchar *messageLog)
        {
            GLuint u = 0;
            if(glGetDebugMessageLogARB_Impl)
                u = glGetDebugMessageLogARB_Impl(count, bufsize, sources, types, ids, severities, lengths, messageLog);
            PostHook();
            return u;
        }


        // GL_ARB_ES2_compatibility
        void OVR::GLEContext::glReleaseShaderCompiler_Hook()
        {
            if(glReleaseShaderCompiler_Impl)
                glReleaseShaderCompiler_Impl();
            PostHook();
        }

        void OVR::GLEContext::glShaderBinary_Hook(GLsizei count, const GLuint *shaders, GLenum binaryformat, const GLvoid *binary, GLsizei length)
        {
            if(glShaderBinary_Impl)
                glShaderBinary_Impl(count, shaders, binaryformat, binary, length);
            PostHook();
        }

        void OVR::GLEContext::glGetShaderPrecisionFormat_Hook(GLenum shadertype, GLenum precisiontype, GLint *range, GLint *precision)
        {
            if(glGetShaderPrecisionFormat_Impl)
                glGetShaderPrecisionFormat_Impl(shadertype, precisiontype, range, precision);
            PostHook();
        }

        void OVR::GLEContext::glDepthRangef_Hook(GLclampf n, GLclampf f)
        {
            if(glDepthRangef_Impl)
                glDepthRangef_Impl(n, f);
            PostHook();
        }

        void OVR::GLEContext::glClearDepthf_Hook(GLclampf d)
        {
            if(glClearDepthf_Impl)
                glClearDepthf_Impl(d);
            PostHook();
        }


        // GL_ARB_framebuffer_object
        GLboolean OVR::GLEContext::glIsRenderbuffer_Hook(GLuint renderbuffer)
        {
            GLboolean b = GL_FALSE;
            if(glIsRenderbuffer_Impl)
                b = glIsRenderbuffer_Impl(renderbuffer);
            PostHook();
            return b;
        }

        void OVR::GLEContext::glBindRenderbuffer_Hook(GLenum target, GLuint renderbuffer)
        {
            if(glBindRenderbuffer_Impl)
                glBindRenderbuffer_Impl(target, renderbuffer);
            PostHook();
        }

        void OVR::GLEContext::glDeleteRenderbuffers_Hook(GLsizei n, const GLuint *renderbuffers)
        {
            if(glDeleteRenderbuffers_Impl)
                glDeleteRenderbuffers_Impl(n, renderbuffers);
            PostHook();
        }

        void OVR::GLEContext::glGenRenderbuffers_Hook(GLsizei n, GLuint *renderbuffers)
        {
            if(glGenRenderbuffers_Impl)
                glGenRenderbuffers_Impl(n, renderbuffers);
            PostHook();
        }

        void OVR::GLEContext::glRenderbufferStorage_Hook(GLenum target, GLenum internalformat, GLsizei width, GLsizei height)
        {
            if(glRenderbufferStorage_Impl)
                glRenderbufferStorage_Impl(target, internalformat, width, height);
            PostHook();
        }

        void OVR::GLEContext::glGetRenderbufferParameteriv_Hook(GLenum target, GLenum pname, GLint *params)
        {
            if(glGetRenderbufferParameteriv_Impl)
                glGetRenderbufferParameteriv_Impl(target, pname, params);
            PostHook();
        }

        GLboolean OVR::GLEContext::glIsFramebuffer_Hook(GLuint framebuffer)
        {
            GLboolean b = GL_FALSE;
            if(glIsFramebuffer_Impl)
                b = glIsFramebuffer_Impl(framebuffer);
            PostHook();
            return b;
        }

        void OVR::GLEContext::glBindFramebuffer_Hook(GLenum target, GLuint framebuffer)
        {
            if(glBindFramebuffer_Impl)
                glBindFramebuffer_Impl(target, framebuffer);
            PostHook();
        }

        void OVR::GLEContext::glDeleteFramebuffers_Hook(GLsizei n, const GLuint *framebuffers)
        {
            if(glDeleteFramebuffers_Impl)
                glDeleteFramebuffers_Impl(n, framebuffers);
            PostHook();
        }

        void OVR::GLEContext::glGenFramebuffers_Hook(GLsizei n, GLuint *framebuffers)
        {
            if(glGenFramebuffers_Impl)
                glGenFramebuffers_Impl(n, framebuffers);
            PostHook();
        }

        GLenum OVR::GLEContext::glCheckFramebufferStatus_Hook(GLenum target)
        {
            GLenum e = 0;
            if(glCheckFramebufferStatus_Impl)
                e = glCheckFramebufferStatus_Impl(target);
            PostHook();
            return e;
        }

        void OVR::GLEContext::glFramebufferTexture1D_Hook(GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level)
        {
            if(glFramebufferTexture1D_Impl)
                glFramebufferTexture1D_Impl(target, attachment, textarget, texture, level);
            PostHook();
        }

        void OVR::GLEContext::glFramebufferTexture2D_Hook(GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level)
        {
            if(glFramebufferTexture2D_Impl)
                glFramebufferTexture2D_Impl(target, attachment, textarget, texture, level);
            PostHook();
        }

        void OVR::GLEContext::glFramebufferTexture3D_Hook(GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level, GLint zoffset)
        {
            if(glFramebufferTexture3D_Impl)
                glFramebufferTexture3D_Impl(target, attachment, textarget, texture, level, zoffset);
            PostHook();
        }

        void OVR::GLEContext::glFramebufferRenderbuffer_Hook(GLenum target, GLenum attachment, GLenum renderbuffertarget, GLuint renderbuffer)
        {
            if(glFramebufferRenderbuffer_Impl)
                glFramebufferRenderbuffer_Impl(target, attachment, renderbuffertarget, renderbuffer);
            PostHook();
        }

        void OVR::GLEContext::glGetFramebufferAttachmentParameteriv_Hook(GLenum target, GLenum attachment, GLenum pname, GLint *params)
        {
            if(glGetFramebufferAttachmentParameteriv_Impl)
                glGetFramebufferAttachmentParameteriv_Impl(target, attachment, pname, params);
            PostHook();
        }

        void OVR::GLEContext::glGenerateMipmap_Hook(GLenum target)
        {
            if(glGenerateMipmap_Impl)
                glGenerateMipmap_Impl(target);
            PostHook();
        }

        void OVR::GLEContext::glBlitFramebuffer_Hook(GLint srcX0, GLint srcY0, GLint srcX1, GLint srcY1, GLint dstX0, GLint dstY0, GLint dstX1, GLint dstY1, GLbitfield mask, GLenum filter)
        {
            if(glBlitFramebuffer_Impl)
                glBlitFramebuffer_Impl(srcX0, srcY0, srcX1, srcY1, dstX0, dstY0, dstX1, dstY1, mask, filter);
            PostHook();
        }

        void OVR::GLEContext::glRenderbufferStorageMultisample_Hook(GLenum target, GLsizei samples, GLenum internalformat, GLsizei width, GLsizei height)
        {
            if(glRenderbufferStorageMultisample_Impl)
                glRenderbufferStorageMultisample_Impl(target, samples, internalformat, width, height);
            PostHook();
        }

        void OVR::GLEContext::glFramebufferTextureLayer_Hook(GLenum target, GLenum attachment, GLuint texture, GLint level, GLint layer)
        {
            if(glFramebufferTextureLayer_Impl)
                glFramebufferTextureLayer_Impl(target, attachment, texture, level, layer);
            PostHook();
        }


        // GL_ARB_texture_multisample
        void OVR::GLEContext::glTexImage2DMultisample_Hook(GLenum target, GLsizei samples, GLint internalformat, GLsizei width, GLsizei height, GLboolean fixedsamplelocations)
        {
            if(glTexImage2DMultisample_Impl)
                glTexImage2DMultisample_Impl(target, samples, internalformat, width, height, fixedsamplelocations);
            PostHook();
        }

        void OVR::GLEContext::glTexImage3DMultisample_Hook(GLenum target, GLsizei samples, GLint internalformat, GLsizei width, GLsizei height, GLsizei depth, GLboolean fixedsamplelocations)
        {
            if(glTexImage3DMultisample_Impl)
                glTexImage3DMultisample_Impl(target, samples, internalformat, width, height, depth, fixedsamplelocations);
            PostHook();
        }

        void OVR::GLEContext::glGetMultisamplefv_Hook(GLenum pname, GLuint index, GLfloat *val)
        {
            if(glGetMultisamplefv_Impl)
                glGetMultisamplefv_Impl(pname, index, val);
            PostHook();
        }

        void OVR::GLEContext::glSampleMaski_Hook(GLuint index, GLbitfield mask)
        {
            if(glSampleMaski_Impl)
                glSampleMaski_Impl(index, mask);
            PostHook();
        }


        // GL_ARB_timer_query
        void OVR::GLEContext::glQueryCounter_Hook(GLuint id, GLenum target)
        {
            if(glQueryCounter_Impl)
                glQueryCounter_Impl(id, target);
            PostHook();
        }

        void OVR::GLEContext::glGetQueryObjecti64v_Hook(GLuint id, GLenum pname, GLint64 *params)
        {
            if(glGetQueryObjecti64v_Impl)
                glGetQueryObjecti64v_Impl(id, pname, params);
            PostHook();
        }

        void OVR::GLEContext::glGetQueryObjectui64v_Hook(GLuint id, GLenum pname, GLuint64 *params)
        {
            if(glGetQueryObjectui64v_Impl)
                glGetQueryObjectui64v_Impl(id, pname, params);
            PostHook();
        }


        // GL_ARB_vertex_array_object
        void OVR::GLEContext::glBindVertexArray_Hook(GLuint array)
        {
            if(glBindVertexArray_Impl)
                glBindVertexArray_Impl(array);
            PostHook();
        }

        void OVR::GLEContext::glDeleteVertexArrays_Hook(GLsizei n, const GLuint *arrays)
        {
            if(glDeleteVertexArrays_Impl)
                glDeleteVertexArrays_Impl(n, arrays);
            PostHook();
        }

        void OVR::GLEContext::glGenVertexArrays_Hook(GLsizei n, GLuint *arrays)
        {
            if(glGenVertexArrays_Impl)
                glGenVertexArrays_Impl(n, arrays);
            PostHook();
        }

        GLboolean OVR::GLEContext::glIsVertexArray_Hook(GLuint array)
        {
            GLboolean b = GL_FALSE;
            if(glIsVertexArray_Impl)
                b = glIsVertexArray_Impl(array);
            PostHook();
            return b;
        }


        // GL_EXT_draw_buffers2
        void OVR::GLEContext::glColorMaskIndexedEXT_Hook(GLuint index, GLboolean r, GLboolean g, GLboolean b, GLboolean a)
        {
            if(glColorMaskIndexedEXT_Impl)
                glColorMaskIndexedEXT_Impl(index, r, g, b, a);
            PostHook();
        }

        void OVR::GLEContext::glGetBooleanIndexedvEXT_Hook(GLenum target, GLuint index, GLboolean *data)
        {
            if(glGetBooleanIndexedvEXT_Impl)
                glGetBooleanIndexedvEXT_Impl(target, index, data);
            PostHook();
        }

        void OVR::GLEContext::glGetIntegerIndexedvEXT_Hook(GLenum target, GLuint index, GLint *data)
        {
            if(glGetIntegerIndexedvEXT_Impl)
                glGetIntegerIndexedvEXT_Impl(target, index, data);
            PostHook();
        }

        void OVR::GLEContext::glEnableIndexedEXT_Hook(GLenum target, GLuint index)
        {
            if(glEnableIndexedEXT_Impl)
                glEnableIndexedEXT_Impl(target, index);
            PostHook();
        }

        void OVR::GLEContext::glDisableIndexedEXT_Hook(GLenum target, GLuint index)
        {
            if(glDisableIndexedEXT_Impl)
                glDisableIndexedEXT_Impl(target, index);
            PostHook();
        }

        GLboolean OVR::GLEContext::glIsEnabledIndexedEXT_Hook(GLenum target, GLuint index)
        {
            GLboolean b = GL_FALSE;
            if(glIsEnabledIndexedEXT_Impl)
                b = glIsEnabledIndexedEXT_Impl(target, index);
            PostHook();
            return b;
        }


        // GL_KHR_debug
        void OVR::GLEContext::glDebugMessageControl_Hook(GLenum source, GLenum type, GLenum severity, GLsizei count, const GLuint* ids, GLboolean enabled)
        {
            if(glDebugMessageControl_Impl)
                glDebugMessageControl_Impl(source, type, severity, count, ids, enabled);
            PostHook();
        }

        void OVR::GLEContext::glDebugMessageInsert_Hook(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const char* buf)
        {
            if(glDebugMessageInsert_Impl)
                glDebugMessageInsert_Impl(source, type, id, severity, length, buf);
            PostHook();
        }

        void OVR::GLEContext::glDebugMessageCallback_Hook(GLDEBUGPROC callback, const void* userParam)
        {
            if(glDebugMessageCallback_Impl)
                glDebugMessageCallback_Impl(callback, userParam);
            PostHook();
        }

        GLuint OVR::GLEContext::glGetDebugMessageLog_Hook(GLuint count, GLsizei bufSize, GLenum* sources, GLenum* types, GLuint* ids, GLenum* severities, GLsizei* lengths,  char* messageLog)
        {
            GLuint u = 0;
            if(glGetDebugMessageLog_Impl)
                u = glGetDebugMessageLog_Impl(count, bufSize, sources, types, ids, severities, lengths, messageLog);
            PostHook();
            return u;
        }

        void OVR::GLEContext::glPushDebugGroup_Hook(GLenum source, GLuint id, GLsizei length, const char * message)
        {
            if(glPushDebugGroup_Impl)
                glPushDebugGroup_Impl(source, id, length,  message);
            PostHook();
        }

        void OVR::GLEContext::glPopDebugGroup_Hook()
        {
            if(glPopDebugGroup_Impl)
                glPopDebugGroup_Impl();
            PostHook();
        }

        void OVR::GLEContext::glObjectLabel_Hook(GLenum identifier, GLuint name, GLsizei length, const char *label)
        {
            if(glObjectLabel_Impl)
                glObjectLabel_Impl(identifier, name, length, label);
            PostHook();
        }

        void OVR::GLEContext::glGetObjectLabel_Hook(GLenum identifier, GLuint name, GLsizei bufSize, GLsizei *length, char *label)
        {
            if(glGetObjectLabel_Impl)
                glGetObjectLabel_Impl(identifier, name, bufSize, length, label);
            PostHook();
        }

        void OVR::GLEContext::glObjectPtrLabel_Hook(void* ptr, GLsizei length, const char *label)
        {
            if(glObjectPtrLabel_Impl)
                glObjectPtrLabel_Impl(ptr, length, label);
            PostHook();
        }

        void OVR::GLEContext::glGetObjectPtrLabel_Hook(void* ptr, GLsizei bufSize, GLsizei *length, char *label)
        {
            if(glGetObjectPtrLabel_Impl)
                glGetObjectPtrLabel_Impl(ptr, bufSize, length, label);
            PostHook();
        }


        // GL_WIN_swap_hint
        void OVR::GLEContext::glAddSwapHintRectWIN_Hook(GLint x, GLint y, GLsizei width, GLsizei height)
        {
            if(glAddSwapHintRectWIN_Impl)
                glAddSwapHintRectWIN_Impl(x, y, width, height);
            PostHook();
        }


        #if defined(GLE_WINDOWS_ENABLED)
			// WGL
			void OVR::GLEContext::PostWGLHook()
			{
				// Empty for now.
			}

			#undef wglCopyContext
			extern "C" { GLAPI BOOL GLAPIENTRY wglCopyContext(HGLRC hglrcSrc, HGLRC hglrcDst, UINT mask); }
			BOOL OVR::GLEContext::wglCopyContext_Hook(HGLRC hglrcSrc, HGLRC hglrcDst, UINT mask)
			{
				BOOL b = wglCopyContext(hglrcSrc, hglrcDst, mask);
				PostWGLHook();
				return b;
			}

			#undef wglCreateContext
			extern "C" { GLAPI HGLRC GLAPIENTRY wglCreateContext(HDC hdc); }
			HGLRC OVR::GLEContext::wglCreateContext_Hook(HDC hdc)
			{
				HGLRC h = wglCreateContext(hdc);
				PostWGLHook();
				return h;
			}

			#undef wglCreateLayerContext
			extern "C" { GLAPI HGLRC GLAPIENTRY wglCreateLayerContext(HDC hdc, int iLayerPlane); }
			HGLRC OVR::GLEContext::wglCreateLayerContext_Hook(HDC hdc, int iLayerPlane)
			{
				HGLRC h = wglCreateLayerContext(hdc, iLayerPlane);
				PostWGLHook();
				return h;
			}

			#undef wglDeleteContext
			extern "C" { GLAPI BOOL GLAPIENTRY wglDeleteContext(HGLRC hglrc); }
			BOOL OVR::GLEContext::wglDeleteContext_Hook(HGLRC hglrc)
			{
				BOOL b = wglDeleteContext(hglrc);
				PostWGLHook();
				return b;
			}

			#undef wglGetCurrentContext
			extern "C" { GLAPI HGLRC GLAPIENTRY wglGetCurrentContext(); }
			HGLRC OVR::GLEContext::wglGetCurrentContext_Hook()
			{
				HGLRC h = wglGetCurrentContext();
				PostWGLHook();
				return h;
			}

			#undef wglGetCurrentDC
			extern "C" { GLAPI HDC GLAPIENTRY wglGetCurrentDC(); }
			HDC OVR::GLEContext::wglGetCurrentDC_Hook()
			{
				HDC h = wglGetCurrentDC();
				PostWGLHook();
				return h;
			}

			//#undef wglGetProcAddress Not needed because we happen to do it above already.
			//extern "C" { GLAPI PROC GLAPIENTRY wglGetProcAddress(LPCSTR lpszProc); }
			PROC OVR::GLEContext::wglGetProcAddress_Hook(LPCSTR lpszProc)
			{
				PROC p = wglGetProcAddress(lpszProc);
				PostWGLHook();
				return p;
			}

			#undef wglMakeCurrent
			extern "C" { GLAPI BOOL GLAPIENTRY wglMakeCurrent(HDC hdc, HGLRC hglrc); }
			BOOL OVR::GLEContext::wglMakeCurrent_Hook(HDC hdc, HGLRC hglrc)
			{
				BOOL b = wglMakeCurrent(hdc, hglrc);
				PostWGLHook();
				return b;
			}

			#undef wglShareLists
			extern "C" { GLAPI BOOL GLAPIENTRY wglShareLists(HGLRC hglrc1, HGLRC hglrc2); }
			BOOL OVR::GLEContext::wglShareLists_Hook(HGLRC hglrc1, HGLRC hglrc2)
			{
				BOOL b = wglShareLists(hglrc1, hglrc2);
				PostWGLHook();
				return b;
			}

			#undef wglUseFontBitmapsA
			extern "C" { GLAPI BOOL GLAPIENTRY wglUseFontBitmapsA(HDC hdc, DWORD first, DWORD count, DWORD listBase); }
			BOOL OVR::GLEContext::wglUseFontBitmapsA_Hook(HDC hdc, DWORD first, DWORD count, DWORD listBase)
			{
				BOOL b = wglUseFontBitmapsA(hdc, first, count, listBase);
				PostWGLHook();
				return b;
			}

			#undef wglUseFontBitmapsW
			extern "C" { GLAPI BOOL GLAPIENTRY wglUseFontBitmapsW(HDC hdc, DWORD first, DWORD count, DWORD listBase); }
			BOOL OVR::GLEContext::wglUseFontBitmapsW_Hook(HDC hdc, DWORD first, DWORD count, DWORD listBase)
			{
				BOOL b = wglUseFontBitmapsW(hdc, first, count, listBase);
				PostWGLHook();
				return b;
			}

			#undef wglUseFontOutlinesA
			extern "C" { GLAPI BOOL GLAPIENTRY wglUseFontOutlinesA(HDC hdc, DWORD first, DWORD count, DWORD listBase, FLOAT deviation, FLOAT extrusion, int format, LPGLYPHMETRICSFLOAT lpgmf); }
			BOOL OVR::GLEContext::wglUseFontOutlinesA_Hook(HDC hdc, DWORD first, DWORD count, DWORD listBase, FLOAT deviation, FLOAT extrusion, int format, LPGLYPHMETRICSFLOAT lpgmf)
			{
				BOOL b = wglUseFontOutlinesA(hdc, first, count, listBase, deviation, extrusion, format, lpgmf);
				PostWGLHook();
				return b;
			}

			#undef wglUseFontOutlinesW
			extern "C" { GLAPI BOOL GLAPIENTRY wglUseFontOutlinesW(HDC hdc, DWORD first, DWORD count, DWORD listBase, FLOAT deviation, FLOAT extrusion, int format, LPGLYPHMETRICSFLOAT lpgmf); }
			BOOL OVR::GLEContext::wglUseFontOutlinesW_Hook(HDC hdc, DWORD first, DWORD count, DWORD listBase, FLOAT deviation, FLOAT extrusion, int format, LPGLYPHMETRICSFLOAT lpgmf)
			{
				BOOL b = wglUseFontOutlinesW(hdc, first, count, listBase, deviation, extrusion, format, lpgmf);
				PostWGLHook();
				return b;
			}

			#undef wglDescribeLayerPlane
			extern "C" { GLAPI BOOL GLAPIENTRY wglDescribeLayerPlane(HDC hdc, int iPixelFormat, int iLayerPlane, UINT nBytes, LPLAYERPLANEDESCRIPTOR plpd); }
			BOOL OVR::GLEContext::wglDescribeLayerPlane_Hook(HDC hdc, int iPixelFormat, int iLayerPlane, UINT nBytes, LPLAYERPLANEDESCRIPTOR plpd)
			{
				BOOL b = wglDescribeLayerPlane(hdc, iPixelFormat, iLayerPlane, nBytes, plpd);
				PostWGLHook();
				return b;
			}

			#undef wglSetLayerPaletteEntries
			extern "C" { GLAPI int GLAPIENTRY wglSetLayerPaletteEntries(HDC hdc, int iLayerPlane, int iStart, int cEntries, const COLORREF *pcr); }
			int OVR::GLEContext::wglSetLayerPaletteEntries_Hook(HDC hdc, int iLayerPlane, int iStart, int cEntries, const COLORREF *pcr)
			{
				int i = wglSetLayerPaletteEntries(hdc, iLayerPlane, iStart, cEntries, pcr);
				PostWGLHook();
				return i;
			}

			#undef wglGetLayerPaletteEntries
			extern "C" { GLAPI int GLAPIENTRY wglGetLayerPaletteEntries(HDC hdc, int iLayerPlane, int iStart, int cEntries, COLORREF *pcr); }
			int OVR::GLEContext::wglGetLayerPaletteEntries_Hook(HDC hdc, int iLayerPlane, int iStart, int cEntries, COLORREF *pcr)
			{
				int i = wglGetLayerPaletteEntries(hdc, iLayerPlane, iStart, cEntries, pcr);
				PostWGLHook();
				return i;
			}

			#undef wglRealizeLayerPalette
			extern "C" { GLAPI BOOL GLAPIENTRY wglRealizeLayerPalette(HDC hdc, int iLayerPlane, BOOL bRealize); }
			BOOL OVR::GLEContext::wglRealizeLayerPalette_Hook(HDC hdc, int iLayerPlane, BOOL bRealize)
			{
				BOOL b = wglRealizeLayerPalette(hdc, iLayerPlane, bRealize);
				PostWGLHook();
				return b;
			}

			#undef wglSwapLayerBuffers
			extern "C" { GLAPI BOOL GLAPIENTRY wglSwapLayerBuffers(HDC hdc, UINT fuPlanes); }
			BOOL OVR::GLEContext::wglSwapLayerBuffers_Hook(HDC hdc, UINT fuPlanes)
			{
				BOOL b = wglSwapLayerBuffers(hdc, fuPlanes);
				PostWGLHook();
				return b;
			}

			#undef wglSwapMultipleBuffers
			extern "C" { GLAPI DWORD GLAPIENTRY wglSwapMultipleBuffers(UINT i, CONST WGLSWAP* p); }
			DWORD OVR::GLEContext::wglSwapMultipleBuffers_Hook(UINT i, CONST WGLSWAP* p)
			{
				DWORD dw = wglSwapMultipleBuffers(i, p);
				PostWGLHook();
				return dw;
			}

			// The rest of the functions are pointer-based.

			// WGL_ARB_buffer_region
			HANDLE OVR::GLEContext::wglCreateBufferRegionARB_Hook(HDC hDC, int iLayerPlane, UINT uType)
			{
				HANDLE h = NULL;
				if(wglCreateBufferRegionARB_Impl)
					h = wglCreateBufferRegionARB_Impl(hDC, iLayerPlane, uType);
				PostWGLHook();
				return h;
			}

			VOID OVR::GLEContext::wglDeleteBufferRegionARB_Hook(HANDLE hRegion)
			{
				if(wglDeleteBufferRegionARB_Impl)
					wglDeleteBufferRegionARB_Impl(hRegion);
				PostWGLHook();
			}

			BOOL OVR::GLEContext::wglSaveBufferRegionARB_Hook(HANDLE hRegion, int x, int y, int width, int height)
			{
				BOOL b = FALSE;
				if(wglSaveBufferRegionARB_Impl)
					b = wglSaveBufferRegionARB_Impl(hRegion, x, y, width, height);
				PostWGLHook();
				return b;
			}

			BOOL OVR::GLEContext::wglRestoreBufferRegionARB_Hook(HANDLE hRegion, int x, int y, int width, int height, int xSrc, int ySrc)
			{
				BOOL b = FALSE;
				if(wglRestoreBufferRegionARB_Impl)
					b = wglRestoreBufferRegionARB_Impl(hRegion, x, y, width, height, xSrc, ySrc);
				PostWGLHook();
				return b;
			}

			// WGL_ARB_extensions_string
			const char * OVR::GLEContext::wglGetExtensionsStringARB_Hook(HDC hdc)
			{
				const char * p = NULL;
				if(wglGetExtensionsStringARB_Impl)
					p = wglGetExtensionsStringARB_Impl(hdc);
				PostWGLHook();
				return p;
			}

			// WGL_ARB_pixel_format
			BOOL OVR::GLEContext::wglGetPixelFormatAttribivARB_Hook(HDC hdc, int iPixelFormat, int iLayerPlane, UINT nAttributes, const int *piAttributes, int *piValues)
			{
				BOOL b = FALSE;
				if(wglGetPixelFormatAttribivARB_Impl)
					b = wglGetPixelFormatAttribivARB_Impl(hdc, iPixelFormat, iLayerPlane, nAttributes, piAttributes, piValues);
				PostWGLHook();
				return b;
			}

			BOOL OVR::GLEContext::wglGetPixelFormatAttribfvARB_Hook(HDC hdc, int iPixelFormat, int iLayerPlane, UINT nAttributes, const int *piAttributes, FLOAT *pfValues)
			{
				BOOL b = FALSE;
				if(wglGetPixelFormatAttribfvARB_Impl)
					b = wglGetPixelFormatAttribfvARB_Impl(hdc, iPixelFormat, iLayerPlane, nAttributes, piAttributes, pfValues);
				PostWGLHook();
				return b;
			}

			BOOL OVR::GLEContext::wglChoosePixelFormatARB_Hook(HDC hdc, const int *piAttribIList, const FLOAT *pfAttribFList, UINT nMaxFormats, int *piFormats, UINT *nNumFormats)
			{
				BOOL b = FALSE;
				if(wglChoosePixelFormatARB_Impl)
					b = wglChoosePixelFormatARB_Impl(hdc, piAttribIList, pfAttribFList, nMaxFormats, piFormats, nNumFormats);
				PostWGLHook();
				return b;
			}

			// WGL_ARB_make_current_read
			BOOL OVR::GLEContext::wglMakeContextCurrentARB_Hook(HDC hDrawDC, HDC hReadDC, HGLRC hglrc)
			{
				BOOL b = FALSE;
				if(wglMakeContextCurrentARB_Impl)
					b = wglMakeContextCurrentARB_Impl(hDrawDC, hReadDC, hglrc);
				PostWGLHook();
				return b;
			}

			HDC OVR::GLEContext::wglGetCurrentReadDCARB_Hook()
			{
				HDC h = NULL;
				if(wglGetCurrentReadDCARB_Impl)
					h = wglGetCurrentReadDCARB_Impl();
				PostWGLHook();
				return h;
			}

			// WGL_ARB_pbuffer
			HPBUFFERARB OVR::GLEContext::wglCreatePbufferARB_Hook(HDC hDC, int iPixelFormat, int iWidth, int iHeight, const int *piAttribList)
			{
				HPBUFFERARB h = NULL;
				if(wglCreatePbufferARB_Impl)
					h = wglCreatePbufferARB_Impl(hDC, iPixelFormat, iWidth, iHeight, piAttribList);
				PostWGLHook();
				return h;
			}

			HDC OVR::GLEContext::wglGetPbufferDCARB_Hook(HPBUFFERARB hPbuffer)
			{
				HDC h = NULL;
				if(wglGetPbufferDCARB_Impl)
					h = wglGetPbufferDCARB_Impl(hPbuffer);
				PostWGLHook();
				return h;
			}

			int OVR::GLEContext::wglReleasePbufferDCARB_Hook(HPBUFFERARB hPbuffer, HDC hDC)
			{
				int i = 0;
				if(wglReleasePbufferDCARB_Impl)
					i = wglReleasePbufferDCARB_Impl(hPbuffer, hDC);
				PostWGLHook();
				return i;
			}

			BOOL OVR::GLEContext::wglDestroyPbufferARB_Hook(HPBUFFERARB hPbuffer)
			{
				BOOL b = FALSE;
				if(wglDestroyPbufferARB_Impl)
					b = wglDestroyPbufferARB_Impl(hPbuffer);
				PostWGLHook();
				return b;
			}

			BOOL OVR::GLEContext::wglQueryPbufferARB_Hook(HPBUFFERARB hPbuffer, int iAttribute, int *piValue)
			{
				BOOL b = FALSE;
				if(wglQueryPbufferARB_Impl)
					b = wglQueryPbufferARB_Impl(hPbuffer, iAttribute, piValue);
				PostWGLHook();
				return b;
			}

			// WGL_ARB_render_texture
			BOOL OVR::GLEContext::wglBindTexImageARB_Hook(HPBUFFERARB hPbuffer, int iBuffer)
			{
				BOOL b = FALSE;
				if(wglBindTexImageARB_Impl)
					b = wglBindTexImageARB_Impl(hPbuffer, iBuffer);
				PostWGLHook();
				return b;
			}

			BOOL OVR::GLEContext::wglReleaseTexImageARB_Hook(HPBUFFERARB hPbuffer, int iBuffer)
			{
				BOOL b = FALSE;
				if(wglReleaseTexImageARB_Impl)
					b = wglReleaseTexImageARB_Impl(hPbuffer, iBuffer);
				PostWGLHook();
				return b;
			}

			BOOL OVR::GLEContext::wglSetPbufferAttribARB_Hook(HPBUFFERARB hPbuffer, const int *piAttribList)
			{
				BOOL b = FALSE;
				if(wglSetPbufferAttribARB_Impl)
					b = wglSetPbufferAttribARB_Impl(hPbuffer, piAttribList);
				PostWGLHook();
				return b;
			}

			// WGL_NV_present_video
			int OVR::GLEContext::wglEnumerateVideoDevicesNV_Hook(HDC hDC, HVIDEOOUTPUTDEVICENV *phDeviceList)
			{
				int i = 0;
				if(wglEnumerateVideoDevicesNV_Impl)
					i = wglEnumerateVideoDevicesNV_Impl(hDC, phDeviceList);
				PostWGLHook();
				return i;
			}

			BOOL OVR::GLEContext::wglBindVideoDeviceNV_Hook(HDC hDC, unsigned int uVideoSlot, HVIDEOOUTPUTDEVICENV hVideoDevice, const int *piAttribList)
			{
				BOOL b = FALSE;
				if(wglBindVideoDeviceNV_Impl)
					b = wglBindVideoDeviceNV_Impl(hDC, uVideoSlot, hVideoDevice, piAttribList);
				PostWGLHook();
				return b;
			}

			BOOL OVR::GLEContext::wglQueryCurrentContextNV_Hook(int iAttribute, int *piValue)
			{
				BOOL b = FALSE;
				if(wglQueryCurrentContextNV_Impl)
					b = wglQueryCurrentContextNV_Impl(iAttribute, piValue);
				PostWGLHook();
				return b;
			}

			// WGL_ARB_create_context
			HGLRC OVR::GLEContext::wglCreateContextAttribsARB_Hook(HDC hDC, HGLRC hShareContext, const int *attribList)
			{
				HGLRC h = NULL;
				if(wglCreateContextAttribsARB_Impl)
					h = wglCreateContextAttribsARB_Impl(hDC, hShareContext, attribList);
				PostWGLHook();
				return h;
			}

			// WGL_EXT_extensions_string
			const char * OVR::GLEContext::wglGetExtensionsStringEXT_Hook()
			{
				const char * p = NULL;
				if(wglGetExtensionsStringEXT_Impl)
					p = wglGetExtensionsStringEXT_Impl();
				PostWGLHook();
				return p;
			}

			// WGL_EXT_swap_control
			BOOL OVR::GLEContext::wglSwapIntervalEXT_Hook(int interval)
			{
				BOOL b = FALSE;
				if(wglSwapIntervalEXT_Impl)
					b = wglSwapIntervalEXT_Impl(interval);
				PostWGLHook();
				return b;
			}

			int OVR::GLEContext::wglGetSwapIntervalEXT_Hook()
			{
				int i = 0;
				if(wglGetSwapIntervalEXT_Impl)
					i = wglGetSwapIntervalEXT_Impl();
				PostWGLHook();
				return i;
			}

			// WGL_OML_sync_control
			BOOL  OVR::GLEContext::wglGetSyncValuesOML_Hook(HDC hdc, INT64 *ust, INT64 *msc, INT64 *sbc)
			{
				BOOL b = FALSE;
				if(wglGetSyncValuesOML_Impl)
					b = wglGetSyncValuesOML_Impl(hdc, ust, msc, sbc);
				PostWGLHook();
				return b;
			}

			BOOL  OVR::GLEContext::wglGetMscRateOML_Hook(HDC hdc, INT32 *numerator, INT32 *denominator)
			{
				BOOL b = FALSE;
				if(wglGetMscRateOML_Impl)
					b = wglGetMscRateOML_Impl(hdc, numerator, denominator);
				PostWGLHook();
				return b;
			}

			INT64 OVR::GLEContext::wglSwapBuffersMscOML_Hook(HDC hdc, INT64 target_msc, INT64 divisor, INT64 remainder)
			{
				INT64 i = 0;
				if(wglSwapBuffersMscOML_Impl)
					i = wglSwapBuffersMscOML_Impl(hdc, target_msc, divisor, remainder);
				PostWGLHook();
				return i;
			}

			INT64 OVR::GLEContext::wglSwapLayerBuffersMscOML_Hook(HDC hdc, int fuPlanes, INT64 target_msc, INT64 divisor, INT64 remainder)
			{
				INT64 i = 0;
				if(wglSwapLayerBuffersMscOML_Impl)
					i = wglSwapLayerBuffersMscOML_Impl(hdc, fuPlanes, target_msc, divisor, remainder);
				PostWGLHook();
				return i;
			}

			BOOL  OVR::GLEContext::wglWaitForMscOML_Hook(HDC hdc, INT64 target_msc, INT64 divisor, INT64 remainder, INT64 *ust, INT64 *msc, INT64 *sbc)
			{
				BOOL b = FALSE;
				if(wglWaitForMscOML_Impl)
					b = wglWaitForMscOML_Impl(hdc, target_msc, divisor, remainder, ust, msc, sbc);
				PostWGLHook();
				return b;
			}

			BOOL  OVR::GLEContext::wglWaitForSbcOML_Hook(HDC hdc, INT64 target_sbc, INT64 *ust, INT64 *msc, INT64 *sbc)
			{
				BOOL b = FALSE;
				if(wglWaitForSbcOML_Impl)
					b = wglWaitForSbcOML_Impl(hdc, target_sbc, ust, msc, sbc);
				PostWGLHook();
				return b;
			}

			// WGL_NV_video_output
			BOOL OVR::GLEContext::wglGetVideoDeviceNV_Hook(HDC hDC, int numDevices, HPVIDEODEV *hVideoDevice)
			{
				BOOL b = FALSE;
				if(wglGetVideoDeviceNV_Impl)
					b = wglGetVideoDeviceNV_Impl(hDC, numDevices, hVideoDevice);
				PostWGLHook();
				return b;
			}

			BOOL OVR::GLEContext::wglReleaseVideoDeviceNV_Hook(HPVIDEODEV hVideoDevice)
			{
				BOOL b = FALSE;
				if(wglReleaseVideoDeviceNV_Impl)
					b = wglReleaseVideoDeviceNV_Impl(hVideoDevice);
				PostWGLHook();
				return b;
			}

			BOOL OVR::GLEContext::wglBindVideoImageNV_Hook(HPVIDEODEV hVideoDevice, HPBUFFERARB hPbuffer, int iVideoBuffer)
			{
				BOOL b = FALSE;
				if(wglBindVideoImageNV_Impl)
					b = wglBindVideoImageNV_Impl(hVideoDevice, hPbuffer, iVideoBuffer);
				PostWGLHook();
				return b;
			}

			BOOL OVR::GLEContext::wglReleaseVideoImageNV_Hook(HPBUFFERARB hPbuffer, int iVideoBuffer)
			{
				BOOL b = FALSE;
				if(wglReleaseVideoImageNV_Impl)
					b = wglReleaseVideoImageNV_Impl(hPbuffer, iVideoBuffer);
				PostWGLHook();
				return b;
			}

			BOOL OVR::GLEContext::wglSendPbufferToVideoNV_Hook(HPBUFFERARB hPbuffer, int iBufferType, unsigned long *pulCounterPbuffer, BOOL bBlock)
			{
				BOOL b = FALSE;
				if(wglSendPbufferToVideoNV_Impl)
					b = wglSendPbufferToVideoNV_Impl(hPbuffer, iBufferType, pulCounterPbuffer, bBlock);
				PostWGLHook();
				return b;
			}

			BOOL OVR::GLEContext::wglGetVideoInfoNV_Hook(HPVIDEODEV hpVideoDevice, unsigned long *pulCounterOutputPbuffer, unsigned long *pulCounterOutputVideo)
			{
				BOOL b = FALSE;
				if(wglGetVideoInfoNV_Impl)
					b = wglGetVideoInfoNV_Impl(hpVideoDevice, pulCounterOutputPbuffer, pulCounterOutputVideo);
				PostWGLHook();
				return b;
			}

			// WGL_NV_swap_group
			BOOL OVR::GLEContext::wglJoinSwapGroupNV_Hook(HDC hDC, GLuint group)
			{
				BOOL b = FALSE;
				if(wglJoinSwapGroupNV_Impl)
					b = wglJoinSwapGroupNV_Impl(hDC, group);
				PostWGLHook();
				return b;
			}

			BOOL OVR::GLEContext::wglBindSwapBarrierNV_Hook(GLuint group, GLuint barrier)
			{
				BOOL b = FALSE;
				if(wglBindSwapBarrierNV_Impl)
					b = wglBindSwapBarrierNV_Impl(group, barrier);
				PostWGLHook();
				return b;
			}

			BOOL OVR::GLEContext::wglQuerySwapGroupNV_Hook(HDC hDC, GLuint *group, GLuint *barrier)
			{
				BOOL b = FALSE;
				if(wglQuerySwapGroupNV_Impl)
					b = wglQuerySwapGroupNV_Impl(hDC, group, barrier);
				PostWGLHook();
				return b;
		   }

			BOOL OVR::GLEContext::wglQueryMaxSwapGroupsNV_Hook(HDC hDC, GLuint *maxGroups, GLuint *maxBarriers)
			{
				BOOL b = FALSE;
				if(wglQueryMaxSwapGroupsNV_Impl)
					b = wglQueryMaxSwapGroupsNV_Impl(hDC, maxGroups, maxBarriers);
				PostWGLHook();
				return b;
			}

			BOOL OVR::GLEContext::wglQueryFrameCountNV_Hook(HDC hDC, GLuint *count)
			{
				BOOL b = FALSE;
				if(wglQueryFrameCountNV_Impl)
					b = wglQueryFrameCountNV_Impl(hDC, count);
				PostWGLHook();
				return b;
			}

			BOOL OVR::GLEContext::wglResetFrameCountNV_Hook(HDC hDC)
			{
				BOOL b = FALSE;
				if(wglResetFrameCountNV_Impl)
					b = wglResetFrameCountNV_Impl(hDC);
				PostHook();
				return b;
		   }

			// WGL_NV_video_capture
			BOOL OVR::GLEContext::wglBindVideoCaptureDeviceNV_Hook(UINT uVideoSlot, HVIDEOINPUTDEVICENV hDevice)
			{
				BOOL b = FALSE;
				if(wglBindVideoCaptureDeviceNV_Impl)
					b = wglBindVideoCaptureDeviceNV_Impl(uVideoSlot, hDevice);
				PostWGLHook();
				return b;
			}

			UINT OVR::GLEContext::wglEnumerateVideoCaptureDevicesNV_Hook(HDC hDc, HVIDEOINPUTDEVICENV *phDeviceList)
			{
				UINT u = 0;
				if(wglEnumerateVideoCaptureDevicesNV_Impl)
					u = wglEnumerateVideoCaptureDevicesNV_Impl(hDc, phDeviceList);
				PostWGLHook();
				return u;
			}

			BOOL OVR::GLEContext::wglLockVideoCaptureDeviceNV_Hook(HDC hDc, HVIDEOINPUTDEVICENV hDevice)
			{
				BOOL b = FALSE;
				if(wglLockVideoCaptureDeviceNV_Impl)
					b = wglLockVideoCaptureDeviceNV_Impl(hDc, hDevice);
				PostWGLHook();
				return b;
			}

			BOOL OVR::GLEContext::wglQueryVideoCaptureDeviceNV_Hook(HDC hDc, HVIDEOINPUTDEVICENV hDevice, int iAttribute, int *piValue)
			{
				BOOL b = FALSE;
				if(wglQueryVideoCaptureDeviceNV_Impl)
					b = wglQueryVideoCaptureDeviceNV_Impl(hDc, hDevice, iAttribute, piValue);
				PostWGLHook();
				return b;
			}

			BOOL OVR::GLEContext::wglReleaseVideoCaptureDeviceNV_Hook(HDC hDc, HVIDEOINPUTDEVICENV hDevice)
			{
				BOOL b = FALSE;
				if(wglReleaseVideoCaptureDeviceNV_Impl)
					b = wglReleaseVideoCaptureDeviceNV_Impl(hDc, hDevice);
				PostWGLHook();
				return b;
			}

			// WGL_NV_copy_image
			BOOL OVR::GLEContext::wglCopyImageSubDataNV_Hook(HGLRC hSrcRC, GLuint srcName, GLenum srcTarget, GLint srcLevel, GLint srcX, GLint srcY, GLint srcZ, HGLRC hDstRC,
															 GLuint dstName, GLenum dstTarget, GLint dstLevel, GLint dstX, GLint dstY, GLint dstZ, GLsizei width, GLsizei height, GLsizei depth)
			{
				BOOL b = FALSE;
				if(wglCopyImageSubDataNV_Impl)
					b = wglCopyImageSubDataNV_Impl(hSrcRC, srcName, srcTarget, srcLevel, srcX, srcY, srcZ, hDstRC, dstName, dstTarget, dstLevel, dstX, dstY, dstZ, width, height, depth);
				PostWGLHook();
				return b;
			}

			// WGL_NV_DX_interop
			BOOL OVR::GLEContext::wglDXSetResourceShareHandleNV_Hook(void *dxObject, HANDLE shareHandle)
			{
				BOOL b = FALSE;
				if(wglDXSetResourceShareHandleNV_Impl)
					b = wglDXSetResourceShareHandleNV_Impl(dxObject, shareHandle);
				PostWGLHook();
				return b;
			}

			HANDLE OVR::GLEContext::wglDXOpenDeviceNV_Hook(void *dxDevice)
			{
				HANDLE h = NULL;
				if(wglDXOpenDeviceNV_Impl)
					h = wglDXOpenDeviceNV_Impl(dxDevice);
				PostWGLHook();
				return h;
			}

			BOOL OVR::GLEContext::wglDXCloseDeviceNV_Hook(HANDLE hDevice)
			{
				BOOL b = FALSE;
				if(wglDXCloseDeviceNV_Impl)
					b = wglDXCloseDeviceNV_Impl(hDevice);
				PostWGLHook();
				return b;
			}

			HANDLE OVR::GLEContext::wglDXRegisterObjectNV_Hook(HANDLE hDevice, void *dxObject, GLuint name, GLenum type, GLenum access)
			{
				HANDLE h = NULL;
				if(wglDXRegisterObjectNV_Impl)
					h = wglDXRegisterObjectNV_Impl(hDevice, dxObject, name, type, access);
				PostWGLHook();
				return h;
			}

			BOOL OVR::GLEContext::wglDXUnregisterObjectNV_Hook(HANDLE hDevice, HANDLE hObject)
			{
				BOOL b = FALSE;
				if(wglDXUnregisterObjectNV_Impl)
					b = wglDXUnregisterObjectNV_Impl(hDevice, hObject);
				PostWGLHook();
				return b;
			}

			BOOL OVR::GLEContext::wglDXObjectAccessNV_Hook(HANDLE hObject, GLenum access)
			{
				BOOL b = FALSE;
				if(wglDXObjectAccessNV_Impl)
					b = wglDXObjectAccessNV_Impl(hObject, access);
				PostWGLHook();
				return b;
			}

			BOOL OVR::GLEContext::wglDXLockObjectsNV_Hook(HANDLE hDevice, GLint count, HANDLE *hObjects)
			{
				BOOL b = FALSE;
				if(wglDXLockObjectsNV_Impl)
					b = wglDXLockObjectsNV_Impl(hDevice, count, hObjects);
				PostWGLHook();
				return b;
			}

			BOOL OVR::GLEContext::wglDXUnlockObjectsNV_Hook(HANDLE hDevice, GLint count, HANDLE *hObjects)
			{
				BOOL b = FALSE;
				if(wglDXUnlockObjectsNV_Impl)
					b = wglDXUnlockObjectsNV_Impl(hDevice, count, hObjects);
				PostWGLHook();
				return b;
			}

        #endif // defined(GLE_WINDOWS_ENABLED)

        #if defined(GLE_UNIX_ENABLED)
			void OVR::GLEContext::PostGLXHook()
			{
				// Empty for now.
			}

			// GLX_VERSION_1_0
			// GLX_VERSION_1_1
			// We don't currently implement hooking of these.

			// GLX_VERSION_1_2
			::Display* OVR::GLEContext::glXGetCurrentDisplay_Hook(void)
			{
				::Display* p = NULL;
				if(glXGetCurrentDisplay_Impl)
	                p = glXGetCurrentDisplay_Impl();
	            PostGLXHook();
	            return p;
			}

			// GLX_VERSION_1_3
		    GLXFBConfig* OVR::GLEContext::glXChooseFBConfig_Hook(Display *dpy, int screen, const int *attrib_list, int *nelements)
			{
				GLXFBConfig* p = NULL;
				if(glXChooseFBConfig_Impl)
	                p = glXChooseFBConfig_Impl(dpy, screen, attrib_list, nelements);
	            PostGLXHook();
	            return p;
			}

		    GLXContext OVR::GLEContext::glXCreateNewContext_Hook(Display *dpy, GLXFBConfig config, int render_type, GLXContext share_list, Bool direct)
			{
		    	GLXContext c = 0;
				if(glXCreateNewContext_Impl)
	                c = glXCreateNewContext_Impl(dpy, config, render_type, share_list, direct);
	            PostGLXHook();
	            return c;
			}

		    GLXPbuffer OVR::GLEContext::glXCreatePbuffer_Hook(Display *dpy, GLXFBConfig config, const int *attrib_list)
			{
		    	GLXPbuffer b = 0;
	            if(glXCreatePbuffer_Impl)
	            	b = glXCreatePbuffer_Impl(dpy, config, attrib_list);
	            PostGLXHook();
	            return b;
			}

		    GLXPixmap OVR::GLEContext::glXCreatePixmap_Hook(Display *dpy, GLXFBConfig config, Pixmap pixmap, const int *attrib_list)
			{
		    	GLXPixmap m = 0;
	            if(glXCreatePixmap_Impl)
	            	m = glXCreatePixmap_Impl(dpy, config, pixmap, attrib_list);
	            PostGLXHook();
	            return m;
			}

		    GLXWindow OVR::GLEContext::glXCreateWindow_Hook(Display *dpy, GLXFBConfig config, Window win, const int *attrib_list)
			{
		    	GLXWindow w = 0;
	            if(glXCreateWindow_Impl)
	            	w = glXCreateWindow_Impl(dpy, config, win, attrib_list);
	            PostGLXHook();
	            return w;
			}

		    void OVR::GLEContext::glXDestroyPbuffer_Hook(Display *dpy, GLXPbuffer pbuf)
		    {
	            if(glXDestroyPbuffer_Impl)
	                glXDestroyPbuffer_Impl(dpy, pbuf);
	            PostGLXHook();
		    }

		    void OVR::GLEContext::glXDestroyPixmap_Hook(Display *dpy, GLXPixmap pixmap)
			{
	            if(glXDestroyPixmap_Impl)
	            	glXDestroyPixmap_Impl(dpy, pixmap);
	            PostGLXHook();
			}

		    void OVR::GLEContext::glXDestroyWindow_Hook(Display *dpy, GLXWindow win)
			{
	            if(glXDestroyWindow_Impl)
	            	glXDestroyWindow_Impl(dpy, win);
	            PostGLXHook();
			}

		    GLXDrawable OVR::GLEContext::glXGetCurrentReadDrawable_Hook(void)
			{
		    	GLXDrawable d;
	            if(glXGetCurrentReadDrawable_Impl)
	            	d = glXGetCurrentReadDrawable_Impl();
	            PostGLXHook();
	            return d;
			}

		    int OVR::GLEContext::glXGetFBConfigAttrib_Hook(Display *dpy, GLXFBConfig config, int attribute, int *value)
			{
	            int i = -1;
	            if(glXGetFBConfigAttrib_Impl)
	            	i = glXGetFBConfigAttrib_Impl(dpy, config, attribute, value);
	            PostGLXHook();
	            return i;
			}

		    GLXFBConfig* OVR::GLEContext::glXGetFBConfigs_Hook(Display *dpy, int screen, int *nelements)
		    {
		    	GLXFBConfig* p = NULL;
	            if(glXGetFBConfigs_Impl)
	            	p = glXGetFBConfigs_Impl(dpy, screen, nelements);
	            PostGLXHook();
	            return p;
		    }

		    void OVR::GLEContext::glXGetSelectedEvent_Hook(Display *dpy, GLXDrawable draw, unsigned long *event_mask)
			{
	            if(glXGetSelectedEvent_Impl)
	            	glXGetSelectedEvent_Impl(dpy, draw, event_mask);
	            PostGLXHook();
			}

		    XVisualInfo* OVR::GLEContext::glXGetVisualFromFBConfig_Hook(Display *dpy, GLXFBConfig config)
			{
		    	XVisualInfo* p = NULL;
	            if(glXGetVisualFromFBConfig_Impl)
	            	p = glXGetVisualFromFBConfig_Impl(dpy, config);
	            PostGLXHook();
	            return p;
			}

		    Bool OVR::GLEContext::glXMakeContextCurrent_Hook(Display *dpy, GLXDrawable draw, GLXDrawable read, GLXContext ctx)
			{
	            Bool b = False;
	            if(glXMakeContextCurrent_Impl)
	            	b = glXMakeContextCurrent_Impl(dpy, draw, read, ctx);
	            PostGLXHook();
	            return b;
			}

		    int OVR::GLEContext::glXQueryContext_Hook(Display *dpy, GLXContext ctx, int attribute, int *value)
		    {
	            int i = GLX_BAD_ATTRIBUTE;
	            if(glXQueryContext_Impl)
	            	i = glXQueryContext_Impl(dpy, ctx, attribute, value);
	            PostGLXHook();
	            return i;
		    }

		    void OVR::GLEContext::glXQueryDrawable_Hook(Display *dpy, GLXDrawable draw, int attribute, unsigned int *value)
			{
	            if(glXQueryDrawable_Impl)
	            	glXQueryDrawable_Impl(dpy, draw, attribute, value);
	            PostGLXHook();
			}

		    void OVR::GLEContext::glXSelectEvent_Hook(Display *dpy, GLXDrawable draw, unsigned long event_mask)
			{
	            if(glXSelectEvent_Impl)
	            	glXSelectEvent_Impl(dpy, draw, event_mask);
	            PostGLXHook();
			}

		    // GLX_VERSION_1_4
		    // We don't do hooking of this.

		    // GLX_EXT_swap_control
		    void OVR::GLEContext::glXSwapIntervalEXT_Hook(Display* dpy, GLXDrawable drawable, int interval)
		    {
	            if(glXSwapIntervalEXT_Impl)
	            	glXSwapIntervalEXT_Impl(dpy, drawable, interval);
	            PostGLXHook();
		    }

            // GLX_OML_sync_control
   			Bool OVR::GLEContext::glXGetMscRateOML_Hook(Display* dpy, GLXDrawable drawable, int32_t* numerator, int32_t* denominator)
			{
	            Bool b = False;
	            if(glXGetMscRateOML_Impl)
	            	b = glXGetMscRateOML_Impl(dpy, drawable, numerator, denominator);
	            PostGLXHook();
	            return b;
			}

   			Bool OVR::GLEContext::glXGetSyncValuesOML_Hook(Display* dpy, GLXDrawable drawable, int64_t* ust, int64_t* msc, int64_t* sbc)
			{
	            Bool b = False;
	            if(glXGetSyncValuesOML_Impl)
	            	b = glXGetSyncValuesOML_Impl(dpy, drawable, ust, msc, sbc);
	            PostGLXHook();
	            return b;
			}

   			int64_t OVR::GLEContext::glXSwapBuffersMscOML_Hook(Display* dpy, GLXDrawable drawable, int64_t target_msc, int64_t divisor, int64_t remainder)
			{
   				int64_t i = 0;
	            if(glXSwapBuffersMscOML_Impl)
	            	i = glXSwapBuffersMscOML_Impl(dpy, drawable, target_msc, divisor, remainder);
	            PostGLXHook();
	            return i;
			}

   			Bool OVR::GLEContext::glXWaitForMscOML_Hook(Display* dpy, GLXDrawable drawable, int64_t target_msc, int64_t divisor, int64_t remainder, int64_t* ust, int64_t* msc, int64_t* sbc)
			{
	            Bool b = False;
	            if(glXWaitForMscOML_Impl)
	            	b = glXWaitForMscOML_Impl(dpy, drawable, target_msc, divisor, remainder, ust, msc, sbc);
	            PostGLXHook();
	            return b;
			}

   			Bool OVR::GLEContext::glXWaitForSbcOML_Hook(Display* dpy, GLXDrawable drawable, int64_t target_sbc, int64_t* ust, int64_t* msc, int64_t* sbc)
			{
	            Bool b = False;
	            if(glXWaitForSbcOML_Impl)
	            	b = glXWaitForSbcOML_Impl(dpy, drawable, target_sbc, ust, msc, sbc);
	            PostGLXHook();
	            return b;
			}

        #endif // defined(GLE_UNIX_ENABLED)

    #endif // GLE_HOOKING_ENABLED

//} // namespace OVR



