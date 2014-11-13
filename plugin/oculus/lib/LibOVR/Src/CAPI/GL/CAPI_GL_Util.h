/************************************************************************************

Filename    :   CAPI_GL_Util.h
Content     :   Utility header for OpenGL
Created     :   March 27, 2014
Authors     :   Andrew Reisse, David Borel

Copyright   :   Copyright 2012 Oculus VR, Inc. All Rights reserved.

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

#ifndef INC_OVR_CAPI_GL_Util_h
#define INC_OVR_CAPI_GL_Util_h

#include "../../OVR_CAPI.h"  
#include "../../Kernel/OVR_Array.h"
#include "../../Kernel/OVR_Math.h"
#include "../../Kernel/OVR_RefCount.h"
#include "../../Kernel/OVR_String.h"
#include "../../Kernel/OVR_Types.h"
#include "../../Kernel/OVR_Log.h"

#if defined(OVR_OS_WIN32)
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#endif

#if defined(OVR_OS_MAC)
#define GL_DO_NOT_WARN_IF_MULTI_GL_VERSION_HEADERS_INCLUDED
#include <OpenGL/gl.h>
#include <OpenGL/glext.h>
#include <OpenGL/gl3.h>
#include <OpenGL/gl3ext.h>
#else
#ifndef GL_GLEXT_PROTOTYPES
#define GL_GLEXT_PROTOTYPES
#endif
#include <GL/gl.h>
#include <GL/glext.h>
#if defined(OVR_OS_WIN32)
#include <GL/wglext.h>
#elif defined(OVR_OS_LINUX)
#include <GL/glx.h>
#endif
#endif

namespace OVR { namespace CAPI { namespace GL {


// GL extension Hooks for Non-Mac.
#if !defined(OVR_OS_MAC)

// Let Windows apps build without linking GL.
#if defined(OVR_OS_WIN32)

typedef GLenum (__stdcall *PFNGLGETERRORPROC) ();
typedef void (__stdcall *PFNGLENABLEPROC) (GLenum);
typedef void (__stdcall *PFNGLDISABLEPROC) (GLenum);
typedef void (__stdcall *PFNGLGETFLOATVPROC) (GLenum, GLfloat*);
typedef const GLubyte * (__stdcall *PFNGLGETSTRINGPROC) (GLenum);
typedef const GLubyte * (__stdcall *PFNGLGETSTRINGIPROC) (GLenum, GLuint);
typedef void(__stdcall *PFNGLGETINTEGERVPROC) (GLenum, GLint*);
typedef void (__stdcall *PFNGLGETDOUBLEVPROC) (GLenum, GLdouble*);
typedef PROC (__stdcall *PFNWGLGETPROCADDRESS) (LPCSTR);
typedef void (__stdcall *PFNGLFLUSHPROC) ();
typedef void (__stdcall *PFNGLFINISHPROC) ();
typedef void (__stdcall *PFNGLDRAWARRAYSPROC) (GLenum mode, GLint first, GLsizei count);
typedef void (__stdcall *PFNGLCLEARPROC) (GLbitfield);
typedef void (__stdcall *PFNGLCOLORMASKPROC) (GLboolean red, GLboolean green, GLboolean blue, GLboolean alpha);
typedef void (__stdcall *PFNGLDRAWELEMENTSPROC) (GLenum mode, GLsizei count, GLenum type, const GLvoid *indices);
typedef void (__stdcall *PFNGLGENTEXTURESPROC) (GLsizei n, GLuint *textures);
typedef void (__stdcall *PFNGLDELETETEXTURESPROC) (GLsizei n, GLuint *textures);
typedef void (__stdcall *PFNGLBINDTEXTUREPROC) (GLenum target, GLuint texture);
typedef void (__stdcall *PFNGLTEXIMAGE2DPROC) (GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLint border, GLint format, GLenum type, const GLvoid *pixels);
typedef void (__stdcall *PFNGLCLEARCOLORPROC) (GLfloat r, GLfloat g, GLfloat b, GLfloat a);
typedef void (__stdcall *PFNGLCLEARDEPTHPROC) (GLclampd depth);
typedef void (__stdcall *PFNGLDEPTHMASKPROC) (GLboolean flag);
typedef void (__stdcall *PFNGLDEPTHRANGEPROC) (GLclampd nearVal,  GLclampd farVal);
typedef void (__stdcall *PFNGLDEPTHRANGEFPROC) (GLclampf nearVal,  GLclampf farVal);
typedef void (__stdcall *PFNGLTEXPARAMETERIPROC) (GLenum target, GLenum pname, GLint param);
typedef void (__stdcall *PFNGLVIEWPORTPROC) (GLint x, GLint y, GLsizei width, GLsizei height);
typedef void (__stdcall *PFNGLBLENDFUNCPROC) (GLenum sfactor, GLenum dfactor);
typedef void (__stdcall *PFNGLFRONTFACEPROC) (GLenum mode);
typedef GLint (__stdcall *PFNGLRENDERMODEPROC) (GLenum mode);
typedef void (__stdcall *PFNGLPOLYGONMODEPROC) (GLenum face, GLenum mode);

extern PFNWGLGETPROCADDRESS                     wglGetProcAddress;
extern PFNWGLGETSWAPINTERVALEXTPROC             wglGetSwapIntervalEXT;
extern PFNWGLSWAPINTERVALEXTPROC                wglSwapIntervalEXT;

extern PFNGLGETERRORPROC                        glGetError;
extern PFNGLENABLEPROC                          glEnable;
extern PFNGLENABLEIPROC                         glEnablei;
extern PFNGLDISABLEPROC                         glDisable;
extern PFNGLDISABLEIPROC                        glDisablei;
extern PFNGLCOLORMASKPROC                       glColorMask;
extern PFNGLCOLORMASKIPROC                      glColorMaski;
extern PFNGLGETFLOATVPROC                       glGetFloatv;
extern PFNGLGETSTRINGPROC                       glGetString;
extern PFNGLGETINTEGERVPROC                     glGetIntegerv;
extern PFNGLGETINTEGERI_VPROC                   glGetIntegeri_v;
extern PFNGLGETDOUBLEVPROC                      glGetDoublev;
extern PFNGLCLEARPROC                           glClear;
extern PFNGLCLEARCOLORPROC                      glClearColor;
extern PFNGLCLEARDEPTHPROC                      glClearDepth;
extern PFNGLDEPTHMASKPROC                       glDepthMask;
extern PFNGLDEPTHRANGEPROC                      glDepthRange;
extern PFNGLDEPTHRANGEFPROC                     glDepthRangef;
extern PFNGLVIEWPORTPROC                        glViewport;
extern PFNGLDRAWARRAYSPROC                      glDrawArrays;
extern PFNGLDRAWELEMENTSPROC                    glDrawElements;
extern PFNGLGENTEXTURESPROC                     glGenTextures;
extern PFNGLDELETETEXTURESPROC                  glDeleteTextures;
extern PFNGLBINDTEXTUREPROC                     glBindTexture;
extern PFNGLTEXIMAGE2DPROC                      glTexImage2D;
extern PFNGLTEXPARAMETERIPROC                   glTexParameteri;
extern PFNGLFLUSHPROC                           glFlush;
extern PFNGLFINISHPROC                          glFinish;
extern PFNGLBLENDFUNCPROC                       glBlendFunc;
extern PFNGLFRONTFACEPROC                       glFrontFace;
extern PFNGLRENDERMODEPROC                      glRenderMode;
extern PFNGLPOLYGONMODEPROC                     glPolygonMode;

#elif defined(OVR_OS_LINUX)

extern PFNGLXSWAPINTERVALEXTPROC                glXSwapIntervalEXT;

#endif // defined(OVR_OS_WIN32)

extern PFNGLGETSTRINGIPROC                      glGetStringi;
extern PFNGLGENFRAMEBUFFERSPROC                 glGenFramebuffers;
extern PFNGLDELETEFRAMEBUFFERSPROC              glDeleteFramebuffers;
extern PFNGLDELETESHADERPROC                    glDeleteShader;
extern PFNGLCHECKFRAMEBUFFERSTATUSPROC          glCheckFramebufferStatus;
extern PFNGLFRAMEBUFFERRENDERBUFFERPROC         glFramebufferRenderbuffer;
extern PFNGLFRAMEBUFFERTEXTURE2DPROC            glFramebufferTexture2D;
extern PFNGLBINDFRAMEBUFFERPROC                 glBindFramebuffer;
extern PFNGLACTIVETEXTUREPROC                   glActiveTexture;
extern PFNGLGETVERTEXATTRIBIVPROC               glGetVertexAttribiv;
extern PFNGLGETVERTEXATTRIBPOINTERVPROC         glGetVertexAttribPointerv;
extern PFNGLDISABLEVERTEXATTRIBARRAYPROC        glDisableVertexAttribArray;
extern PFNGLVERTEXATTRIBPOINTERPROC             glVertexAttribPointer;
extern PFNGLENABLEVERTEXATTRIBARRAYPROC         glEnableVertexAttribArray;
extern PFNGLBINDBUFFERPROC                      glBindBuffer;
extern PFNGLUNIFORMMATRIX4FVPROC                glUniformMatrix4fv;
extern PFNGLDELETEBUFFERSPROC                   glDeleteBuffers;
extern PFNGLBUFFERDATAPROC                      glBufferData;
extern PFNGLGENBUFFERSPROC                      glGenBuffers;
extern PFNGLMAPBUFFERPROC                       glMapBuffer;
extern PFNGLUNMAPBUFFERPROC                     glUnmapBuffer;
extern PFNGLGETSHADERINFOLOGPROC                glGetShaderInfoLog;
extern PFNGLGETSHADERIVPROC                     glGetShaderiv;
extern PFNGLCOMPILESHADERPROC                   glCompileShader;
extern PFNGLSHADERSOURCEPROC                    glShaderSource;
extern PFNGLCREATESHADERPROC                    glCreateShader;
extern PFNGLCREATEPROGRAMPROC                   glCreateProgram;
extern PFNGLATTACHSHADERPROC                    glAttachShader;
extern PFNGLDETACHSHADERPROC                    glDetachShader;
extern PFNGLDELETEPROGRAMPROC                   glDeleteProgram;
extern PFNGLUNIFORM1IPROC                       glUniform1i;
extern PFNGLGETUNIFORMLOCATIONPROC              glGetUniformLocation;
extern PFNGLGETACTIVEUNIFORMPROC                glGetActiveUniform;
extern PFNGLUSEPROGRAMPROC                      glUseProgram;
extern PFNGLGETPROGRAMINFOLOGPROC               glGetProgramInfoLog;
extern PFNGLGETPROGRAMIVPROC                    glGetProgramiv;
extern PFNGLLINKPROGRAMPROC                     glLinkProgram;
extern PFNGLBINDATTRIBLOCATIONPROC              glBindAttribLocation;
extern PFNGLGETATTRIBLOCATIONPROC               glGetAttribLocation;
extern PFNGLUNIFORM4FVPROC                      glUniform4fv;
extern PFNGLUNIFORM3FVPROC                      glUniform3fv;
extern PFNGLUNIFORM2FVPROC                      glUniform2fv;
extern PFNGLUNIFORM1FVPROC                      glUniform1fv;
extern PFNGLGENVERTEXARRAYSPROC                 glGenVertexArrays;
extern PFNGLDELETEVERTEXARRAYSPROC              glDeleteVertexArrays;
extern PFNGLBINDVERTEXARRAYPROC                 glBindVertexArray;
extern PFNGLBLENDFUNCSEPARATEPROC               glBlendFuncSeparate;

extern void InitGLExtensions();

#endif // !defined(OVR_OS_MAC)


// Rendering primitive type used to render Model.
enum PrimitiveType
{
    Prim_Triangles,
    Prim_Lines,
    Prim_TriangleStrip,
    Prim_Unknown,
    Prim_Count
};

// Types of shaders that can be stored together in a ShaderSet.
enum ShaderStage
{
    Shader_Vertex   = 0,
    Shader_Fragment = 2,
    Shader_Pixel    = 2,
    Shader_Count    = 3,
};

enum MapFlags
{
    Map_Discard        = 1,
    Map_Read           = 2, // do not use
    Map_Unsynchronized = 4, // like D3D11_MAP_NO_OVERWRITE
};


// Buffer types used for uploading geometry & constants.
enum BufferUsage
{
    Buffer_Unknown  = 0,
    Buffer_Vertex   = 1,
    Buffer_Index    = 2,
    Buffer_Uniform  = 4,
    Buffer_TypeMask = 0xff,
    Buffer_ReadOnly = 0x100, // Buffer must be created with Data().
};

enum TextureFormat
{
    Texture_RGBA            = 0x0100,
    Texture_Depth           = 0x8000,
    Texture_TypeMask        = 0xff00,
    Texture_SamplesMask     = 0x00ff,
    Texture_RenderTarget    = 0x10000,
    Texture_GenMipmaps      = 0x20000,
};

// Texture sampling modes.
enum SampleMode
{
    Sample_Linear       = 0,
    Sample_Nearest      = 1,
    Sample_Anisotropic  = 2,
    Sample_FilterMask   = 3,

    Sample_Repeat       = 0,
    Sample_Clamp        = 4,
    Sample_ClampBorder  = 8, // If unsupported Clamp is used instead.
    Sample_AddressMask  =12,

    Sample_Count        =13,
};


// Rendering parameters/pointers describing GL rendering setup.
struct RenderParams
{
#if defined(OVR_OS_WIN32)
    HWND   Window;
    HDC    DC;
#elif defined(OVR_OS_LINUX)
    _XDisplay*  Disp;
    Window      Win;
#endif

    ovrSizei  RTSize;
    int    Multisample;
};


class Buffer : public RefCountBase<Buffer>
{
public:
    RenderParams* pParams;
    size_t        Size;
    GLenum        Use;
    GLuint        GLBuffer;

public:
    Buffer(RenderParams* r);
    ~Buffer();

    GLuint         GetBuffer() { return GLBuffer; }

    virtual size_t GetSize() { return Size; }
    virtual void*  Map(size_t start, size_t size, int flags = 0);
    virtual bool   Unmap(void *m);
    virtual bool   Data(int use, const void* buffer, size_t size);
};

class Texture : public RefCountBase<Texture>
{
	bool IsUserAllocated;

public:
    RenderParams* pParams;
    GLuint        TexId;
    int           Width, Height;

    Texture(RenderParams* rp, int w, int h);
    ~Texture();

    virtual int GetWidth() const { return Width; }
    virtual int GetHeight() const { return Height; }

    virtual void SetSampleMode(int sm);

    // Updates texture to point to specified resources
    //  - used for slave rendering.
    void UpdatePlaceholderTexture(GLuint texId,
                                  const Sizei& textureSize);

    virtual void Set(int slot, ShaderStage stage = Shader_Fragment) const;
};

// Base class for vertex and pixel shaders. Stored in ShaderSet.
class Shader : public RefCountBase<Shader>
{
    friend class ShaderSet;

protected:
    ShaderStage Stage;

public:
    Shader(ShaderStage s) : Stage(s) {}
    virtual ~Shader() {}

    ShaderStage GetStage() const { return Stage; }

    virtual void Set(PrimitiveType) const { }
    virtual void SetUniformBuffer(class Buffer* buffers, int i = 0) { OVR_UNUSED2(buffers, i); }

protected:
    virtual bool SetUniform(const char* name, int n, const float* v) { OVR_UNUSED3(name, n, v); return false; }
    virtual bool SetUniformBool(const char* name, int n, const bool* v) { OVR_UNUSED3(name, n, v); return false; }
};



// A group of shaders, one per stage.
// A ShaderSet is applied for rendering with a given fill.
class ShaderSet : public RefCountBase<ShaderSet>
{
protected:
    Ptr<Shader> Shaders[Shader_Count];

    struct Uniform
    {
        String Name;
        int    Location, Size;
        int    Type; // currently number of floats in vector
    };
    Array<Uniform> UniformInfo;
	
public:
	GLuint    Prog;
    GLint     ProjLoc, ViewLoc;
    GLint     TexLoc[8];
    bool      UsesLighting;
    int       LightingVer;

    ShaderSet();
    ~ShaderSet();

    virtual void SetShader(Shader *s);
    virtual void UnsetShader(int stage);
    Shader* GetShader(int stage) { return Shaders[stage]; }

    virtual void Set(PrimitiveType prim) const
    {
		glUseProgram(Prog);

        for (int i = 0; i < Shader_Count; i++)
            if (Shaders[i])
                Shaders[i]->Set(prim);
    }

    // Set a uniform (other than the standard matrices). It is undefined whether the
    // uniforms from one shader occupy the same space as those in other shaders
    // (unless a buffer is used, then each buffer is independent).     
    virtual bool SetUniform(const char* name, int n, const float* v);
    bool SetUniform1f(const char* name, float x)
    {
        const float v[] = {x};
        return SetUniform(name, 1, v);
    }
    bool SetUniform2f(const char* name, float x, float y)
    {
        const float v[] = {x,y};
        return SetUniform(name, 2, v);
    }
    bool SetUniform3f(const char* name, float x, float y, float z)
    {
        const float v[] = {x,y,z};
        return SetUniform(name, 3, v);
    }
    bool SetUniform4f(const char* name, float x, float y, float z, float w = 1)
    {
        const float v[] = {x,y,z,w};
        return SetUniform(name, 4, v);
    }

    bool SetUniformv(const char* name, const Vector3f& v)
    {
        const float a[] = {v.x,v.y,v.z,1};
        return SetUniform(name, 4, a);
    }
 
    virtual bool SetUniform4x4f(const char* name, const Matrix4f& m)
    {
        Matrix4f mt = m.Transposed();
        return SetUniform(name, 16, &mt.M[0][0]);
    }

protected:
	GLint GetGLShader(Shader* s);
    bool Link();
};


// Fill combines a ShaderSet (vertex, pixel) with textures, if any.
// Every model has a fill.
class ShaderFill : public RefCountBase<ShaderFill>
{
    Ptr<ShaderSet>     Shaders;
    Ptr<class Texture> Textures[8];
    void*              InputLayout; // HACK this should be abstracted

public:
    ShaderFill(ShaderSet* sh) : Shaders(sh) { InputLayout = NULL; }
    ShaderFill(ShaderSet& sh) : Shaders(sh) { InputLayout = NULL; }    

    ShaderSet*  GetShaders() const      { return Shaders; }
    void*       GetInputLayout() const  { return InputLayout; }

    virtual void Set(PrimitiveType prim = Prim_Unknown) const {
		Shaders->Set(prim);
		for(int i = 0; i < 8; i++)
		{
			if(Textures[i])
			{
				Textures[i]->Set(i);
			}
		}
	}

    virtual void SetTexture(int i, class Texture* tex) { if (i < 8) Textures[i] = tex; }
};

    
struct DisplayId
{
    // Windows
    String MonitorName; // Monitor name for fullscreen mode
    
    // MacOS
    long   CgDisplayId; // CGDirectDisplayID
    
    DisplayId() : CgDisplayId(0) {}
    DisplayId(long id) : CgDisplayId(id) {}
    DisplayId(String m, long id=0) : MonitorName(m), CgDisplayId(id) {}
    
    operator bool () const
    {
        return MonitorName.GetLength() || CgDisplayId;
    }
    
    bool operator== (const DisplayId& b) const
    {
        return CgDisplayId == b.CgDisplayId &&
            (strstr(MonitorName.ToCStr(), b.MonitorName.ToCStr()) ||
             strstr(b.MonitorName.ToCStr(), MonitorName.ToCStr()));
    }
};


class ShaderBase : public Shader
{
public:    
    RenderParams*   pParams;
    unsigned char*  UniformData;
    int             UniformsSize;

	enum VarType
	{
		VARTYPE_FLOAT,
		VARTYPE_INT,
		VARTYPE_BOOL,
	};

	struct Uniform
	{
		const char* Name;
		VarType Type;
		int     Offset, Size;
	};
    const Uniform* UniformRefl;
    size_t UniformReflSize;

	ShaderBase(RenderParams* rp, ShaderStage stage) : Shader(stage), pParams(rp), UniformData(0), UniformsSize(0) {}
	~ShaderBase()
	{
		if (UniformData)    
			OVR_FREE(UniformData);
	}

    void InitUniforms(const Uniform* refl, size_t reflSize);
	bool SetUniform(const char* name, int n, const float* v);
	bool SetUniformBool(const char* name, int n, const bool* v);
};


template<ShaderStage SStage, GLenum SType>
class ShaderImpl : public ShaderBase
{
    friend class ShaderSet;

public:
    ShaderImpl(RenderParams* rp, void* s, size_t size, const Uniform* refl, size_t reflSize)
		: ShaderBase(rp, SStage)
		, GLShader(0)
    {
		bool success;
        OVR_UNUSED(size);
        success = Compile((const char*) s);
        OVR_ASSERT(success);
        OVR_UNUSED(success);
		InitUniforms(refl, reflSize);
    }
    ~ShaderImpl()
    {      
		if (GLShader)
		{
			glDeleteShader(GLShader);
			GLShader = 0;
		}
    }
    bool Compile(const char* src)
	{
		if (!GLShader)
			GLShader = glCreateShader(GLStage());

		glShaderSource(GLShader, 1, &src, 0);
		glCompileShader(GLShader);
		GLint r;
		glGetShaderiv(GLShader, GL_COMPILE_STATUS, &r);
		if (!r)
		{
			GLchar msg[1024];
			glGetShaderInfoLog(GLShader, sizeof(msg), 0, msg);
			if (msg[0])
				OVR_DEBUG_LOG(("Compiling shader\n%s\nfailed: %s\n", src, msg));

			return 0;
		}
		return 1;
	}
	
    GLenum GLStage() const
    {
		return SType;
	}

private:
	GLuint GLShader;
};

typedef ShaderImpl<Shader_Vertex,  GL_VERTEX_SHADER> VertexShader;
typedef ShaderImpl<Shader_Fragment, GL_FRAGMENT_SHADER> FragmentShader;


//// GLVersionAndExtensions
//
// FIXME: CODE DUPLICATION WARNING
// Right now we have this same code in CommonSrc and in CAPI::GL.
// At some point we need to consolidate these, in Kernel or Util.
// Be sure to update both locations for now!
//
// This class needs to be initialized at runtime with GetGLVersionAndExtensions,
// after an OpenGL context has been created. It must be re-initialized any time
// a new OpenGL context is created, as the new context may differ in version or
// supported functionality.
struct GLVersionAndExtensions
{
    // Version information
    int         MajorVersion;        // Best guess at major version
    int         MinorVersion;        // Best guess at minor version
    int         WholeVersion;        // Equals ((MajorVersion * 100) + MinorVersion). Example usage: if(glv.WholeVersion >= 302) // If OpenGL v3.02+ ...
    bool        IsGLES;              // Open GL ES?
    bool        IsCoreProfile;       // Is the current OpenGL context a core profile context? Its trueness may be a false positive but will never be a false negative.

    // Extension information
    bool        SupportsVAO;         // Supports Vertex Array Objects?
    bool        SupportsDrawBuffers; // Supports Draw Buffers?
    const char* Extensions;          // Other extensions string (will not be null)

    GLVersionAndExtensions()
      : MajorVersion(0),
        MinorVersion(0),
        WholeVersion(0),
        IsGLES(false),
        IsCoreProfile(false),
        SupportsDrawBuffers(false),
        SupportsVAO(false),
        Extensions("")
    {
    }
    
    bool HasGLExtension(const char* searchKey) const;
    
protected:
    friend void GetGLVersionAndExtensions(GLVersionAndExtensions& versionInfo);
    
    void ParseGLVersion();
    void ParseGLExtensions();
};

void GetGLVersionAndExtensions(GLVersionAndExtensions& versionInfo);


}}} // namespace OVR::CAPI::GL


#endif // INC_OVR_CAPI_GL_Util_h
