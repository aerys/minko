/************************************************************************************

Filename    :   CAPI_GL_Util.cpp
Content     :   RenderDevice implementation for OpenGL
Created     :   September 10, 2012
Authors     :   David Borel, Andrew Reisse

Copyright   :   Copyright 2014 Oculus VR, LLC All Rights reserved.

Licensed under the Oculus VR Rift SDK License Version 3.2 (the "License");
you may not use the Oculus VR Rift SDK except in compliance with the License,
which is provided at the time of installation or download, or which
otherwise accompanies this software in either electronic or hard copy form.

You may obtain a copy of the License at

http://www.oculusvr.com/licenses/LICENSE-3.2

Unless required by applicable law or agreed to in writing, the Oculus VR SDK
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.

************************************************************************************/

#include "CAPI_GL_Util.h"
#include "../../Kernel/OVR_Log.h"
#include <string.h>

#if defined(OVR_OS_LINUX)
 #include "../../Displays/OVR_Linux_SDKWindow.h"
#endif

#if defined(OVR_OS_MAC)

typedef void *CGSConnectionID;
typedef int32_t CGSWindowID;
typedef int32_t CGSSurfaceID;

extern "C" CGLError CGLGetSurface(CGLContextObj ctx, CGSConnectionID *cid, CGSWindowID *wid, CGSSurfaceID *sid);
extern "C" CGLError CGLSetSurface(CGLContextObj ctx, CGSConnectionID cid, CGSWindowID wid, CGSSurfaceID sid);

#endif

namespace OVR { namespace CAPI { namespace GL {


// GL Hooks for non-Mac.
#if !defined(OVR_OS_MAC)

#if defined(OVR_OS_WIN32)

PFNWGLGETPROCADDRESS                     wglGetProcAddress;

PFNGLGETERRORPROC                        glGetError;
PFNGLENABLEPROC                          glEnable;
PFNGLDISABLEPROC                         glDisable;
PFNGLGETFLOATVPROC                       glGetFloatv;
PFNGLGETINTEGERVPROC                     glGetIntegerv;
PFNGLGETDOUBLEVPROC                      glGetDoublev;
PFNGLGETSTRINGPROC                       glGetString;
PFNGLCOLORMASKPROC                       glColorMask;
PFNGLCLEARPROC                           glClear;
PFNGLCLEARCOLORPROC                      glClearColor;
PFNGLCLEARDEPTHPROC                      glClearDepth;
PFNGLDEPTHMASKPROC                       glDepthMask;
PFNGLDEPTHRANGEPROC                      glDepthRange;
PFNGLDEPTHRANGEFPROC                     glDepthRangef;
PFNGLVIEWPORTPROC                        glViewport;
PFNGLDRAWELEMENTSPROC                    glDrawElements;
PFNGLTEXPARAMETERIPROC                   glTexParameteri;
PFNGLFLUSHPROC                           glFlush;
PFNGLFINISHPROC                          glFinish;
PFNGLDRAWARRAYSPROC                      glDrawArrays;
PFNGLGENTEXTURESPROC                     glGenTextures;
PFNGLDELETETEXTURESPROC                  glDeleteTextures;
PFNGLBINDTEXTUREPROC                     glBindTexture;
PFNGLTEXIMAGE2DPROC                      glTexImage2D;
PFNGLBLENDFUNCPROC                       glBlendFunc;
PFNGLFRONTFACEPROC                       glFrontFace;
PFNGLRENDERMODEPROC                      glRenderMode;
PFNGLPOLYGONMODEPROC                     glPolygonMode;

PFNWGLGETSWAPINTERVALEXTPROC             wglGetSwapIntervalEXT;
PFNWGLSWAPINTERVALEXTPROC                wglSwapIntervalEXT;
PFNWGLGETCURRENTDCPROC                   wglGetCurrentDC;
PFNWGLGETCURRENTCONTEXTPROC              wglGetCurrentContext;
PFNWGLCREATECONTEXTPROC                  wglCreateContext;
PFNWGLDELETECONTEXTPROC                  wglDeleteContext;
PFNWGLSHARELISTSPROC                     wglShareLists;
PFNWGLMAKECURRENTPROC                    wglMakeCurrent;

#elif defined(OVR_OS_LINUX)

PFNGLXSWAPINTERVALEXTPROC                glXSwapIntervalEXT;

#endif

PFNGLENABLEIPROC                         glEnablei;
PFNGLDISABLEIPROC                        glDisablei;
PFNGLCOLORMASKIPROC                      glColorMaski;
PFNGLGETINTEGERI_VPROC                   glGetIntegeri_v;
PFNGLGETSTRINGIPROC                      glGetStringi;
PFNGLGENFRAMEBUFFERSPROC                 glGenFramebuffers;
PFNGLDELETEFRAMEBUFFERSPROC              glDeleteFramebuffers;
PFNGLDELETESHADERPROC                    glDeleteShader;
PFNGLCHECKFRAMEBUFFERSTATUSPROC          glCheckFramebufferStatus;
PFNGLFRAMEBUFFERRENDERBUFFERPROC         glFramebufferRenderbuffer;
PFNGLFRAMEBUFFERTEXTURE2DPROC            glFramebufferTexture2D;
PFNGLBINDFRAMEBUFFERPROC                 glBindFramebuffer;
PFNGLACTIVETEXTUREPROC                   glActiveTexture;
PFNGLGETVERTEXATTRIBIVPROC               glGetVertexAttribiv;
PFNGLGETVERTEXATTRIBPOINTERVPROC         glGetVertexAttribPointerv;
PFNGLDISABLEVERTEXATTRIBARRAYPROC        glDisableVertexAttribArray;
PFNGLVERTEXATTRIBPOINTERPROC             glVertexAttribPointer;
PFNGLENABLEVERTEXATTRIBARRAYPROC         glEnableVertexAttribArray;
PFNGLBINDBUFFERPROC                      glBindBuffer;
PFNGLUNIFORMMATRIX3FVPROC                glUniformMatrix3fv;
PFNGLUNIFORMMATRIX4FVPROC                glUniformMatrix4fv;
PFNGLDELETEBUFFERSPROC                   glDeleteBuffers;
PFNGLBUFFERDATAPROC                      glBufferData;
PFNGLGENBUFFERSPROC                      glGenBuffers;
PFNGLMAPBUFFERPROC                       glMapBuffer;
PFNGLUNMAPBUFFERPROC                     glUnmapBuffer;
PFNGLGETSHADERINFOLOGPROC                glGetShaderInfoLog;
PFNGLGETSHADERIVPROC                     glGetShaderiv;
PFNGLCOMPILESHADERPROC                   glCompileShader;
PFNGLSHADERSOURCEPROC                    glShaderSource;
PFNGLCREATESHADERPROC                    glCreateShader;
PFNGLCREATEPROGRAMPROC                   glCreateProgram;
PFNGLATTACHSHADERPROC                    glAttachShader;
PFNGLDETACHSHADERPROC                    glDetachShader;
PFNGLDELETEPROGRAMPROC                   glDeleteProgram;
PFNGLUNIFORM1IPROC                       glUniform1i;
PFNGLGETUNIFORMLOCATIONPROC              glGetUniformLocation;
PFNGLGETACTIVEUNIFORMPROC                glGetActiveUniform;
PFNGLUSEPROGRAMPROC                      glUseProgram;
PFNGLGETPROGRAMINFOLOGPROC               glGetProgramInfoLog;
PFNGLGETPROGRAMIVPROC                    glGetProgramiv;
PFNGLLINKPROGRAMPROC                     glLinkProgram;
PFNGLBINDATTRIBLOCATIONPROC              glBindAttribLocation;
PFNGLGETATTRIBLOCATIONPROC               glGetAttribLocation;
PFNGLUNIFORM4FVPROC                      glUniform4fv;
PFNGLUNIFORM3FVPROC                      glUniform3fv;
PFNGLUNIFORM2FVPROC                      glUniform2fv;
PFNGLUNIFORM1FVPROC                      glUniform1fv;
PFNGLGENVERTEXARRAYSPROC                 glGenVertexArrays;
PFNGLDELETEVERTEXARRAYSPROC              glDeleteVertexArrays;
PFNGLBINDVERTEXARRAYPROC                 glBindVertexArray;
PFNGLBLENDFUNCSEPARATEPROC               glBlendFuncSeparate;
PFNGLBLITFRAMEBUFFEREXTPROC              glBlitFramebuffer;
PFNGLDRAWBUFFERSPROC                     glDrawBuffers;


#if defined(OVR_OS_WIN32)

void* GetFunction(const char* functionName)
{
    return wglGetProcAddress(functionName);
}

#else

void (*GetFunction(const char *functionName))( void )
{
    return glXGetProcAddress((GLubyte*)functionName);
}

#endif

void InitGLExtensions()
{
    if (glGenVertexArrays)
        return;

#if defined(OVR_OS_WIN32)
	HINSTANCE hInst = LoadLibrary(L"Opengl32.dll");
	if (!hInst)
		return;

	glGetError =                        (PFNGLGETERRORPROC)                        GetProcAddress(hInst, "glGetError");
	glGetFloatv =                       (PFNGLGETFLOATVPROC)                       GetProcAddress(hInst, "glGetFloatv");
	glGetIntegerv =                     (PFNGLGETINTEGERVPROC)                     GetProcAddress(hInst, "glGetIntegerv");
	glGetDoublev =                      (PFNGLGETDOUBLEVPROC)                      GetProcAddress(hInst, "glGetDoublev");
	glGetString =                       (PFNGLGETSTRINGPROC)                       GetProcAddress(hInst, "glGetString");
	glEnable =                          (PFNGLENABLEPROC)                          GetProcAddress(hInst, "glEnable");
	glDisable =                         (PFNGLDISABLEPROC)                         GetProcAddress(hInst, "glDisable");
	glColorMask =                       (PFNGLCOLORMASKPROC)                       GetProcAddress(hInst, "glColorMask");
	glClear =                           (PFNGLCLEARPROC)                           GetProcAddress(hInst, "glClear" );
	glClearColor =                      (PFNGLCLEARCOLORPROC)                      GetProcAddress(hInst, "glClearColor");
	glClearDepth =                      (PFNGLCLEARDEPTHPROC)                      GetProcAddress(hInst, "glClearDepth");
	glDepthMask =                       (PFNGLDEPTHMASKPROC)                       GetProcAddress(hInst, "glDepthMask");
	glDepthRange =                      (PFNGLDEPTHRANGEPROC)                      GetProcAddress(hInst, "glDepthRange");
	glDepthRangef =                     (PFNGLDEPTHRANGEFPROC)                     GetProcAddress(hInst, "glDepthRangef");
	glViewport =                        (PFNGLVIEWPORTPROC)                        GetProcAddress(hInst, "glViewport");
	glFlush =                           (PFNGLFLUSHPROC)                           GetProcAddress(hInst, "glFlush");
	glFinish =                          (PFNGLFINISHPROC)                          GetProcAddress(hInst, "glFinish");
    glDrawArrays =                      (PFNGLDRAWARRAYSPROC)                      GetProcAddress(hInst, "glDrawArrays");
	glDrawElements =                    (PFNGLDRAWELEMENTSPROC)                    GetProcAddress(hInst, "glDrawElements");
    glGenTextures =                     (PFNGLGENTEXTURESPROC)                     GetProcAddress(hInst, "glGenTextures");
    glDeleteTextures =                  (PFNGLDELETETEXTURESPROC)                  GetProcAddress(hInst, "glDeleteTextures");
    glBindTexture =                     (PFNGLBINDTEXTUREPROC)                     GetProcAddress(hInst, "glBindTexture");
    glTexImage2D =                      (PFNGLTEXIMAGE2DPROC)                      GetProcAddress(hInst, "glTexImage2D");
	glTexParameteri =                   (PFNGLTEXPARAMETERIPROC)                   GetProcAddress(hInst, "glTexParameteri");
	glBlendFunc =                       (PFNGLBLENDFUNCPROC)                       GetProcAddress(hInst, "glBlendFunc");
	glFrontFace =                       (PFNGLFRONTFACEPROC)                       GetProcAddress(hInst, "glFrontFace");
	glRenderMode =                      (PFNGLRENDERMODEPROC)                      GetProcAddress(hInst, "glRenderMode");
	glPolygonMode =                     (PFNGLPOLYGONMODEPROC)                     GetProcAddress(hInst, "glPolygonMode");

    wglGetProcAddress =                 (PFNWGLGETPROCADDRESS)                     GetProcAddress(hInst, "wglGetProcAddress");
	wglGetCurrentDC =                   (PFNWGLGETCURRENTDCPROC)                   GetProcAddress(hInst, "wglGetCurrentDC");
	wglGetCurrentContext =              (PFNWGLGETCURRENTCONTEXTPROC)              GetProcAddress(hInst, "wglGetCurrentContext");
	wglCreateContext =                  (PFNWGLCREATECONTEXTPROC)                  GetProcAddress(hInst, "wglCreateContext");
	wglDeleteContext =                  (PFNWGLDELETECONTEXTPROC)                  GetProcAddress(hInst, "wglDeleteContext");
	wglShareLists =                     (PFNWGLSHARELISTSPROC)                     GetProcAddress(hInst, "wglShareLists");
	wglMakeCurrent =                    (PFNWGLMAKECURRENTPROC)                    GetProcAddress(hInst, "wglMakeCurrent");

    wglGetSwapIntervalEXT =             (PFNWGLGETSWAPINTERVALEXTPROC)             GetFunction("wglGetSwapIntervalEXT");
    wglSwapIntervalEXT =                (PFNWGLSWAPINTERVALEXTPROC)                GetFunction("wglSwapIntervalEXT");
#elif defined(OVR_OS_LINUX)

    glXSwapIntervalEXT =                (PFNGLXSWAPINTERVALEXTPROC)                GetFunction("glXSwapIntervalEXT");
#endif
    
	glGetStringi =                      (PFNGLGETSTRINGIPROC)                      GetFunction("glGetStringi");
    glGenFramebuffers =                 (PFNGLGENFRAMEBUFFERSPROC)                 GetFunction("glGenFramebuffersEXT");
    glDeleteFramebuffers =              (PFNGLDELETEFRAMEBUFFERSPROC)              GetFunction("glDeleteFramebuffersEXT");
	glEnablei =                         (PFNGLENABLEIPROC)                         GetFunction("glEnableIndexedEXT");
	glDisablei =                        (PFNGLDISABLEIPROC)                        GetFunction("glDisableIndexedEXT");
	glColorMaski =                      (PFNGLCOLORMASKIPROC)                      GetFunction("glColorMaskIndexedEXT");
	glGetIntegeri_v =                   (PFNGLGETINTEGERI_VPROC)                   GetFunction("glGetIntegerIndexedvEXT");
    glCheckFramebufferStatus =          (PFNGLCHECKFRAMEBUFFERSTATUSPROC)          GetFunction("glCheckFramebufferStatusEXT");
    glFramebufferRenderbuffer =         (PFNGLFRAMEBUFFERRENDERBUFFERPROC)         GetFunction("glFramebufferRenderbufferEXT");
    glFramebufferTexture2D =            (PFNGLFRAMEBUFFERTEXTURE2DPROC)            GetFunction("glFramebufferTexture2DEXT");
    glBindFramebuffer =                 (PFNGLBINDFRAMEBUFFERPROC)                 GetFunction("glBindFramebufferEXT");
    glGenVertexArrays =                 (PFNGLGENVERTEXARRAYSPROC)                 GetFunction("glGenVertexArrays");
    glDeleteVertexArrays =              (PFNGLDELETEVERTEXARRAYSPROC)              GetFunction("glDeleteVertexArrays");
    glBindVertexArray =                 (PFNGLBINDVERTEXARRAYPROC)                 GetFunction("glBindVertexArray");
    glGenBuffers =                      (PFNGLGENBUFFERSPROC)                      GetFunction("glGenBuffers");
    glDeleteBuffers =                   (PFNGLDELETEBUFFERSPROC)                   GetFunction("glDeleteBuffers");
    glBindBuffer =                      (PFNGLBINDBUFFERPROC)                      GetFunction("glBindBuffer");	
    glBufferData =                      (PFNGLBUFFERDATAPROC)                      GetFunction("glBufferData");
    glMapBuffer =                       (PFNGLMAPBUFFERPROC)                       GetFunction("glMapBuffer");
    glUnmapBuffer =                     (PFNGLUNMAPBUFFERPROC)                     GetFunction("glUnmapBuffer");
    glGetVertexAttribiv =               (PFNGLGETVERTEXATTRIBIVPROC)               GetFunction("glGetVertexAttribiv");
    glGetVertexAttribPointerv =         (PFNGLGETVERTEXATTRIBPOINTERVPROC)         GetFunction("glGetVertexAttribPointerv");
    glDisableVertexAttribArray =        (PFNGLDISABLEVERTEXATTRIBARRAYPROC)        GetFunction("glDisableVertexAttribArray");
    glVertexAttribPointer =             (PFNGLVERTEXATTRIBPOINTERPROC)             GetFunction("glVertexAttribPointer");
    glEnableVertexAttribArray =         (PFNGLENABLEVERTEXATTRIBARRAYPROC)         GetFunction("glEnableVertexAttribArray");
    glActiveTexture =                   (PFNGLACTIVETEXTUREPROC)                   GetFunction("glActiveTexture");
    glUniformMatrix3fv =                (PFNGLUNIFORMMATRIX3FVPROC)                GetFunction("glUniformMatrix3fv");
    glUniformMatrix4fv =                (PFNGLUNIFORMMATRIX4FVPROC)                GetFunction("glUniformMatrix4fv");
    glUniform1i =                       (PFNGLUNIFORM1IPROC)                       GetFunction("glUniform1i");
    glUniform1fv =                      (PFNGLUNIFORM1FVPROC)                      GetFunction("glUniform1fv");
    glUniform2fv =                      (PFNGLUNIFORM2FVPROC)                      GetFunction("glUniform2fv");
    glUniform3fv =                      (PFNGLUNIFORM3FVPROC)                      GetFunction("glUniform3fv");
    glUniform2fv =                      (PFNGLUNIFORM2FVPROC)                      GetFunction("glUniform2fv");
    glUniform4fv =                      (PFNGLUNIFORM4FVPROC)                      GetFunction("glUniform4fv");
    glGetUniformLocation =              (PFNGLGETUNIFORMLOCATIONPROC)              GetFunction("glGetUniformLocation");
    glGetActiveUniform =                (PFNGLGETACTIVEUNIFORMPROC)                GetFunction("glGetActiveUniform");
    glGetShaderInfoLog =                (PFNGLGETSHADERINFOLOGPROC)                GetFunction("glGetShaderInfoLog");
    glGetShaderiv =                     (PFNGLGETSHADERIVPROC)                     GetFunction("glGetShaderiv");
    glCompileShader =                   (PFNGLCOMPILESHADERPROC)                   GetFunction("glCompileShader");
    glShaderSource =                    (PFNGLSHADERSOURCEPROC)                    GetFunction("glShaderSource");
    glCreateShader =                    (PFNGLCREATESHADERPROC)                    GetFunction("glCreateShader");
    glDeleteShader =                    (PFNGLDELETESHADERPROC)                    GetFunction("glDeleteShader");
    glCreateProgram =                   (PFNGLCREATEPROGRAMPROC)                   GetFunction("glCreateProgram");
    glDeleteProgram =                   (PFNGLDELETEPROGRAMPROC)                   GetFunction("glDeleteProgram");
    glUseProgram =                      (PFNGLUSEPROGRAMPROC)                      GetFunction("glUseProgram");
    glGetProgramInfoLog =               (PFNGLGETPROGRAMINFOLOGPROC)               GetFunction("glGetProgramInfoLog");
    glGetProgramiv =                    (PFNGLGETPROGRAMIVPROC)                    GetFunction("glGetProgramiv");
    glLinkProgram =                     (PFNGLLINKPROGRAMPROC)                     GetFunction("glLinkProgram");
    glAttachShader =                    (PFNGLATTACHSHADERPROC)                    GetFunction("glAttachShader");
    glDetachShader =                    (PFNGLDETACHSHADERPROC)                    GetFunction("glDetachShader");
    glBindAttribLocation =              (PFNGLBINDATTRIBLOCATIONPROC)              GetFunction("glBindAttribLocation");
    glGetAttribLocation =               (PFNGLGETATTRIBLOCATIONPROC)               GetFunction("glGetAttribLocation");
    glBlendFuncSeparate =               (PFNGLBLENDFUNCSEPARATEPROC)               GetFunction("glBlendFuncSeparate");
    glBlitFramebuffer =                 (PFNGLBLITFRAMEBUFFEREXTPROC)              GetFunction("glBlitFramebufferEXT");
    glDrawBuffers =                     (PFNGLDRAWBUFFERSPROC)                     GetFunction("glDrawBuffers");
}
    
#endif
    
Buffer::Buffer(RenderParams* rp) : pParams(rp), Size(0), Use(0), GLBuffer(0)
{
}

Buffer::~Buffer()
{
    if (GLBuffer)
        glDeleteBuffers(1, &GLBuffer);
}

bool Buffer::Data(int use, const void* buffer, size_t size)
{
	Size = size;

    switch (use & Buffer_TypeMask)
    {
    case Buffer_Index:     Use = GL_ELEMENT_ARRAY_BUFFER; break;
    default:               Use = GL_ARRAY_BUFFER; break;
    }

    if (!GLBuffer)
        glGenBuffers(1, &GLBuffer);

    int mode = GL_DYNAMIC_DRAW;
    if (use & Buffer_ReadOnly)
        mode = GL_STATIC_DRAW;

    glBindBuffer(Use, GLBuffer);
    glBufferData(Use, size, buffer, mode);
    return 1;
}

void* Buffer::Map(size_t, size_t, int)
{
    int mode = GL_WRITE_ONLY;
    //if (flags & Map_Unsynchronized)
    //    mode |= GL_MAP_UNSYNCHRONIZED;
    
    glBindBuffer(Use, GLBuffer);
    void* v = glMapBuffer(Use, mode);
    return v;
}

bool Buffer::Unmap(void*)
{
    glBindBuffer(Use, GLBuffer);
    int r = glUnmapBuffer(Use);
    return r != 0;
}

ShaderSet::ShaderSet()
{
    Prog = glCreateProgram();
}
ShaderSet::~ShaderSet()
{
    glDeleteProgram(Prog);
}

GLint ShaderSet::GetGLShader(Shader* s)
{
	switch (s->Stage)
	{
	case Shader_Vertex: {
		ShaderImpl<Shader_Vertex, GL_VERTEX_SHADER>* gls = (ShaderImpl<Shader_Vertex, GL_VERTEX_SHADER>*)s;
		return gls->GLShader;
	} break;
	case Shader_Fragment: {
		ShaderImpl<Shader_Fragment, GL_FRAGMENT_SHADER>* gls = (ShaderImpl<Shader_Fragment, GL_FRAGMENT_SHADER>*)s;
		return gls->GLShader;
	} break;
    default: break;
	}

	return -1;
}

void ShaderSet::SetShader(Shader *s)
{
    Shaders[s->Stage] = s;
	GLint GLShader = GetGLShader(s);
    glAttachShader(Prog, GLShader);
    if (Shaders[Shader_Vertex] && Shaders[Shader_Fragment])
        Link();
}

void ShaderSet::UnsetShader(int stage)
{
    if (Shaders[stage] == NULL)
		return;

	GLint GLShader = GetGLShader(Shaders[stage]);
    glDetachShader(Prog, GLShader);

    Shaders[stage] = NULL;
}

bool ShaderSet::SetUniform(const char* name, int n, const float* v)
{
    for (unsigned int i = 0; i < UniformInfo.GetSize(); i++)
        if (!strcmp(UniformInfo[i].Name.ToCStr(), name))
        {
            OVR_ASSERT(UniformInfo[i].Location >= 0);
            glUseProgram(Prog);
            switch (UniformInfo[i].Type)
            {
            case 1:   glUniform1fv(UniformInfo[i].Location, n, v); break;
            case 2:   glUniform2fv(UniformInfo[i].Location, n/2, v); break;
            case 3:   glUniform3fv(UniformInfo[i].Location, n/3, v); break;
            case 4:   glUniform4fv(UniformInfo[i].Location, n/4, v); break;
            case 12:  glUniformMatrix3fv(UniformInfo[i].Location, 1, 1, v); break;
            case 16:  glUniformMatrix4fv(UniformInfo[i].Location, 1, 1, v); break;
            default: OVR_ASSERT(0);
            }
            return 1;
        }

    OVR_DEBUG_LOG(("Warning: uniform %s not present in selected shader", name));
    return 0;
}

bool ShaderSet::Link()
{
    glLinkProgram(Prog);
    GLint r;
    glGetProgramiv(Prog, GL_LINK_STATUS, &r);
    if (!r)
    {
        GLchar msg[1024];
        glGetProgramInfoLog(Prog, sizeof(msg), 0, msg);
        OVR_DEBUG_LOG(("Linking shaders failed: %s\n", msg));
        if (!r)
            return 0;
    }
    glUseProgram(Prog);

    UniformInfo.Clear();
    LightingVer = 0;
    UsesLighting = 0;

	GLint uniformCount = 0;
	glGetProgramiv(Prog, GL_ACTIVE_UNIFORMS, &uniformCount);
	OVR_ASSERT(uniformCount >= 0);

    for(GLuint i = 0; i < (GLuint)uniformCount; i++)
    {
        GLsizei namelen;
        GLint size = 0;
        GLenum type;
        GLchar name[32];
        glGetActiveUniform(Prog, i, sizeof(name), &namelen, &size, &type, name);

        if (size)
        {
            int l = glGetUniformLocation(Prog, name);
            char *np = name;
            while (*np)
            {
                if (*np == '[')
                    *np = 0;
                np++;
            }
            Uniform u;
            u.Name = name;
            u.Location = l;
            u.Size = size;
            switch (type)
            {
            case GL_FLOAT:      u.Type = 1; break;
            case GL_FLOAT_VEC2: u.Type = 2; break;
            case GL_FLOAT_VEC3: u.Type = 3; break;
            case GL_FLOAT_VEC4: u.Type = 4; break;
            case GL_FLOAT_MAT3: u.Type = 12; break;
            case GL_FLOAT_MAT4: u.Type = 16; break;
            default:
                continue;
            }
            UniformInfo.PushBack(u);
            if (!strcmp(name, "LightCount"))
                UsesLighting = 1;
        }
        else
            break;
    }

    ProjLoc = glGetUniformLocation(Prog, "Proj");
    ViewLoc = glGetUniformLocation(Prog, "View");
    for (int i = 0; i < 8; i++)
    {
        char texv[32];
        OVR_sprintf(texv, 10, "Texture%d", i);
        TexLoc[i] = glGetUniformLocation(Prog, texv);
        if (TexLoc[i] < 0)
            break;

        glUniform1i(TexLoc[i], i);
    }
    if (UsesLighting)
        OVR_ASSERT(ProjLoc >= 0 && ViewLoc >= 0);
    return 1;
}

bool ShaderBase::SetUniform(const char* name, int n, const float* v)
{
    for(unsigned i = 0; i < UniformReflSize; i++)
    {
        if (!strcmp(UniformRefl[i].Name, name))
        {
            memcpy(UniformData + UniformRefl[i].Offset, v, n * sizeof(float));
            return 1;
        }
    }
    return 0;
}

bool ShaderBase::SetUniformBool(const char* name, int n, const bool* v) 
{
    OVR_UNUSED(n);
    for(unsigned i = 0; i < UniformReflSize; i++)
    {
        if (!strcmp(UniformRefl[i].Name, name))
        {
            memcpy(UniformData + UniformRefl[i].Offset, v, UniformRefl[i].Size);
            return 1;
        }
    }
    return 0;
}

void ShaderBase::InitUniforms(const Uniform* refl, size_t reflSize)
{
    if(!refl)
    {
        UniformRefl = NULL;
        UniformReflSize = 0;

        UniformsSize = 0;
        if (UniformData)
        {
            OVR_FREE(UniformData);
            UniformData = 0;
        }
        return; // no reflection data
    }

    UniformRefl = refl;
    UniformReflSize = reflSize;
    
    UniformsSize = UniformRefl[UniformReflSize-1].Offset + UniformRefl[UniformReflSize-1].Size;
    UniformData = (unsigned char*)OVR_ALLOC(UniformsSize);
}

Texture::Texture(RenderParams* rp, int w, int h) : IsUserAllocated(false), pParams(rp), TexId(0), Width(w), Height(h)
{
	if (w && h)
		glGenTextures(1, &TexId);
}

Texture::~Texture()
{
    if (TexId && !IsUserAllocated)
        glDeleteTextures(1, &TexId);
}

void Texture::Set(int slot, ShaderStage) const
{
	glActiveTexture(GL_TEXTURE0 + slot);
	glBindTexture(GL_TEXTURE_2D, TexId);
}

void Texture::SetSampleMode(int sm)
{
    glBindTexture(GL_TEXTURE_2D, TexId);
    switch (sm & Sample_FilterMask)
    {
    case Sample_Linear:
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, 1);
        break;

    case Sample_Anisotropic:
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, 8);
        break;

    case Sample_Nearest:
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, 1);
        break;
    }

    switch (sm & Sample_AddressMask)
    {
    case Sample_Repeat:
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        break;

    case Sample_Clamp:
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        break;

    case Sample_ClampBorder:
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
        break;
    }
}

void Texture::UpdatePlaceholderTexture(GLuint texId, const Sizei& textureSize)
{
	if (!IsUserAllocated && TexId && texId != TexId)
		glDeleteTextures(1, &TexId);

    TexId = texId;
	Width = textureSize.w;
	Height = textureSize.h;

	IsUserAllocated = true;
}


//// GLVersion

void GLVersionAndExtensions::ParseGLVersion()
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

    // Write version data
    MajorVersion  = major;
    MinorVersion  = minor;
    WholeVersion  = (major * 100) + minor;
    IsGLES        = isGLES;
    IsCoreProfile = (MajorVersion >= 3); // Until we get a better way to detect core profiles, we err on the conservative side and set to true if the version is >= 3.
}


bool GLVersionAndExtensions::HasGLExtension(const char* searchKey) const
{
    if(Extensions && Extensions[0]) // If we have an extension string to search for individual extensions...
    {
        const int searchKeyLen = (int)strlen(searchKey);
        const char* p = Extensions;

        for (;;)
        {
            p = strstr(p, searchKey);

            // If not found,
            if (p == NULL)
            {
                break;
            }

            // Only match full string
            if ((p == Extensions || p[-1] == ' ') &&
                (p[searchKeyLen] == '\0' || p[searchKeyLen] == ' '))
            {
                return true;
            }

            // Skip ahead
            p += searchKeyLen;
        }
    }
    else
    {
        if(MajorVersion >= 3) // If glGetIntegerv(GL_NUM_EXTENSIONS, ...) is supported...
        {
            GLint extensionCount = 0;
            glGetIntegerv(GL_NUM_EXTENSIONS, &extensionCount);
            GLenum err = glGetError();
            
            if(err == 0)
            {
                for(GLint i = 0; i != extensionCount; ++i)
                {
                    const char* extension = (const char*)glGetStringi(GL_EXTENSIONS, (GLuint)i);
                    
                    if(extension) // glGetStringi returns NULL upon error.
                    {
                        if(strcmp(extension, searchKey) == 0)
                            return true;
                    }
                    else
                        break;
                }
            }
        }
    }
    
    return false;
}

void GLVersionAndExtensions::ParseGLExtensions()
{
    if(MajorVersion >= 3)
    {
        // Set to empty because we need to use glGetStringi to read extensions on recent OpenGL.
        Extensions = "";
    }
    else
    {
        const char* extensions = (const char*)glGetString(GL_EXTENSIONS);

        OVR_ASSERT(extensions);
        if (!extensions)
        {
            extensions = ""; // Note: glGetString() can return null
            LogText("Warning: GL_EXTENSIONS was NULL\n");
        }
        else
        {
            // Cannot print this to debug log: It's too long!
            //OVR_DEBUG_LOG(("GL_EXTENSIONS: %s", (const char*)extensions));
        }

        Extensions = extensions;
    }

    // To do: revise the code below to loop through calls to glGetStringi(GL_EXTENSIONS, ...) so that all extensions below 
    // can be searched with a single pass over the extensions instead of a full loop per HasGLExtensionCall. 

    if (MajorVersion >= 3)
    {
        SupportsVAO = true;
    }
    else
    {
        SupportsVAO =
            HasGLExtension("GL_ARB_vertex_array_object") ||
            HasGLExtension("GL_APPLE_vertex_array_object");
    }

    SupportsDrawBuffers = HasGLExtension("GL_EXT_draw_buffers2");

    // Add more extension checks here...
}

void GetGLVersionAndExtensions(GLVersionAndExtensions& versionInfo)
{
    versionInfo.ParseGLVersion();
    // GL Version must be parsed before parsing extensions:
    versionInfo.ParseGLExtensions();
    // To consider: Call to glGetStringi(GL_SHADING_LANGUAGE_VERSION, ...) check/validate the GLSL support.
}


Context::Context() : initialized(false), ownsContext(true), incarnation(0)
{
#if defined(OVR_OS_MAC)
    systemContext = 0;
#elif defined(OVR_OS_WIN32)
    hdc = 0;
    systemContext = 0;
#elif defined(OVR_OS_LINUX)
    x11Display = 0;
    x11Drawable = 0;
    systemContext = 0;
#endif

}

void Context::InitFromCurrent()
{
    Destroy();

    initialized = true;
    ownsContext = false;
    incarnation++;
    
#if defined(OVR_OS_MAC)
    systemContext = CGLGetCurrentContext();
        {
        CGSConnectionID cid;
        CGSWindowID wid;
        CGSSurfaceID sid;
        CGLError e  = kCGLNoError;
        e = CGLGetSurface(systemContext, &cid, &wid, &sid);
        OVR_ASSERT(e == kCGLNoError); OVR_UNUSED(e);
    }

#elif defined(OVR_OS_WIN32)
    hdc = wglGetCurrentDC();
    systemContext = wglGetCurrentContext();
#elif defined(OVR_OS_LINUX)
    x11Display = glXGetCurrentDisplay();
    x11Drawable = glXGetCurrentDrawable();
    systemContext = glXGetCurrentContext();
    if (!SDKWindow::getVisualFromDrawable(x11Drawable, &x11Visual))
    {
        OVR::LogError("[Context] Unable to obtain x11 visual from context");
    }
#endif
}


void Context::CreateShared( Context & ctx )
{
    Destroy();
    OVR_ASSERT( ctx.initialized == true );
    if( ctx.initialized == false )
    {
        return;
    }

    initialized = true;
    ownsContext = true;
    incarnation++;
    
#if defined(OVR_OS_MAC)
    CGLPixelFormatObj pixelFormat = CGLGetPixelFormat( ctx.systemContext );
    CGLError e = CGLCreateContext( pixelFormat, ctx.systemContext, &systemContext );
    OVR_ASSERT(e == kCGLNoError); OVR_UNUSED(e);
    SetSurface(ctx);
#elif defined(OVR_OS_WIN32)
    hdc = ctx.hdc;
    systemContext = wglCreateContext( ctx.hdc );
    BOOL success = wglShareLists(ctx.systemContext, systemContext );
    OVR_ASSERT( success == TRUE );
    OVR_UNUSED(success);
#elif defined(OVR_OS_LINUX)
    x11Display = ctx.x11Display;
    x11Drawable = ctx.x11Drawable;
    x11Visual = ctx.x11Visual;
    systemContext = glXCreateContext( ctx.x11Display, &x11Visual, ctx.systemContext, True );
    OVR_ASSERT( systemContext != NULL );
#endif
}

#if defined(OVR_OS_MAC)
void Context::SetSurface( Context & ctx ) {
    CGLError e = kCGLNoError;
    CGSConnectionID cid, cid2;
    CGSWindowID wid, wid2;
    CGSSurfaceID sid, sid2;
    

    
    e = CGLGetSurface(ctx.systemContext, &cid, &wid, &sid);
    OVR_ASSERT(e == kCGLNoError); OVR_UNUSED(e);
    e = CGLGetSurface(systemContext, &cid2, &wid2, &sid2);
    OVR_ASSERT(e == kCGLNoError); OVR_UNUSED(e);
    if( sid && sid != sid2 ) {
        e = CGLSetSurface(systemContext, cid, wid, sid);
        OVR_ASSERT(e == kCGLNoError); OVR_UNUSED(e);
    }
}
#endif

void Context::Destroy()
{
    if( initialized == false )
    {
        return;
    }
  
    if( ownsContext )
    {
#if defined(OVR_OS_MAC)
        CGLDestroyContext( systemContext );
#elif defined(OVR_OS_WIN32)
        BOOL success = wglDeleteContext( systemContext );
		OVR_ASSERT( success == TRUE );
        OVR_UNUSED( success );
#elif defined(OVR_OS_LINUX)
        glXDestroyContext( x11Display, systemContext );
#endif
        systemContext = NULL;
    }
  
  initialized = false;
  ownsContext = true;
  
  
}

void Context::Bind()
{
#if defined(OVR_OS_MAC)
    glFlush(); //Apple doesn't automatically flush within CGLSetCurrentContext, unlike other platforms.
    CGLSetCurrentContext( systemContext );
#elif defined(OVR_OS_WIN32)
    wglMakeCurrent( hdc, systemContext );
#elif defined(OVR_OS_LINUX)
    glXMakeCurrent( x11Display, x11Drawable, systemContext );
#endif
}

void Context::Unbind()
{
#if defined(OVR_OS_MAC)
    glFlush(); //Apple doesn't automatically flush within CGLSetCurrentContext, unlike other platforms.
    CGLSetCurrentContext( NULL );
#elif defined(OVR_OS_WIN32)
    wglMakeCurrent( hdc, NULL );
#elif defined(OVR_OS_LINUX)
    glXMakeCurrent( x11Display, None, NULL );
#endif
}

}}} // namespace OVR::CAPI::GL
