/************************************************************************************

Filename    :   CAPI_D3D1X_Util.h
Content     :   D3DX 10/11 utility classes for rendering
Created     :   September 10, 2012
Authors     :   Andrew Reisse

Copyright   :   Copyright 2014 Oculus VR, Inc. All Rights reserved.

Licensed under the Oculus VR Rift SDK License Version 3.1 (the "License"); 
you may not use the Oculus VR Rift SDK except in compliance with the License, 
which is provided at the time of installation or download, or which 
otherwise accompanies this software in either electronic or hard copy form.

You may obtain a copy of the License at

http://www.oculusvr.com/licenses/LICENSE-3.1 

Unless required by applicable law or agreed to in writing, the Oculus VR SDK 
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.

************************************************************************************/

// ***** IMPORTANT:
// This file can be included twice, once with OVR_D3D_VERSION=10 and
// once with OVR_D3D_VERSION=11.


#ifndef OVR_D3D_VERSION
#error define OVR_D3D_VERSION to 10 or 11
#endif

// Custom include guard, allowing one of each D3D10/11.
#if (OVR_D3D_VERSION == 10 && !defined(INC_OVR_CAPI_D3D10_Util_h)) || \
    (OVR_D3D_VERSION == 11 && !defined(INC_OVR_CAPI_D3D11_Util_h))

#include "../../Kernel/OVR_String.h"
#include "../../Kernel/OVR_Array.h"
#include "../../Kernel/OVR_Math.h"

#if defined(OVR_OS_WIN32)
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <comdef.h> // for _COM_SMARTPTR_TYPEDEF()

#undef D3D_NS           // namespace
#undef D3D1X_       
#undef ID3D1X           // interface prefix
#undef ovrD3D1X         // ovrD3D10Config, etc.
#undef D3D11_COMMA_0    // Injects on ", 0" for D3D11 only
#undef D3DSELECT_10_11
#undef IID_ID3D1xShaderReflection

#if (OVR_D3D_VERSION == 10)

    #define INC_OVR_CAPI_D3D10_Util_h 
    #define D3D_NS                      D3D10
    #define D3D1X_(x)                   D3D10_##x
    #define ID3D1X(x)                   ID3D10##x
    #define ovrD3D1X(x)                 ovrD3D10##x
    #define D3DSELECT_10_11(a10, a11)   a10
    #define D3D11_COMMA_0    
    #define IID_ID3D1xShaderReflection  IID_ID3D10ShaderReflection
    #include <d3d10_1.h> // avoids warning?
    #include <d3d10.h>

#else // (OVR_D3D_VERSION == 11)

    #define INC_OVR_CAPI_D3D11_Util_h
    #define D3D_NS                      D3D11        
    #define D3D1X_(x)                   D3D11_##x
    #define ID3D1X(x)                   ID3D11##x
    #define ovrD3D1X(x)                 ovrD3D11##x
    #define D3DSELECT_10_11(a10, a11)   a11
    #define D3D11_COMMA_0               , 0
    #define IID_ID3D1xShaderReflection  IID_ID3D11ShaderReflection
    #include <d3d11.h>
    #include <D3D11Shader.h>
#endif
#endif


namespace OVR { namespace CAPI { namespace D3D_NS {

// D3D Namespace-local types.
typedef ID3D1X(Device)                          ID3D1xDevice;
typedef ID3D1X(RenderTargetView)                ID3D1xRenderTargetView;
typedef ID3D1X(Texture2D)                       ID3D1xTexture2D;
typedef ID3D1X(ShaderResourceView)              ID3D1xShaderResourceView;
typedef ID3D1X(DepthStencilView)                ID3D1xDepthStencilView;
typedef ID3D1X(DepthStencilState)               ID3D1xDepthStencilState;
typedef ID3D1X(InputLayout)                     ID3D1xInputLayout;
typedef ID3D1X(Buffer)                          ID3D1xBuffer;
typedef ID3D1X(VertexShader)                    ID3D1xVertexShader;
typedef ID3D1X(PixelShader)                     ID3D1xPixelShader;
typedef ID3D1X(GeometryShader)                  ID3D1xGeometryShader;
typedef ID3D1X(BlendState)                      ID3D1xBlendState;
typedef ID3D1X(RasterizerState)                 ID3D1xRasterizerState;
typedef ID3D1X(SamplerState)                    ID3D1xSamplerState;
typedef ID3D1X(Query)                           ID3D1xQuery;
typedef ID3D1X(ShaderReflection)                ID3D1xShaderReflection;
typedef ID3D1X(ShaderReflectionVariable)        ID3D1xShaderReflectionVariable;
typedef ID3D1X(ShaderReflectionConstantBuffer)  ID3D1xShaderReflectionConstantBuffer;
typedef D3D1X_(VIEWPORT)                        D3D1x_VIEWPORT;
typedef D3D1X_(QUERY_DESC)                      D3D1x_QUERY_DESC;
typedef D3D1X_(SHADER_BUFFER_DESC)              D3D1x_SHADER_BUFFER_DESC;
typedef D3D1X_(SHADER_VARIABLE_DESC)            D3D1x_SHADER_VARIABLE_DESC;
typedef D3D1X_(PRIMITIVE_TOPOLOGY)            D3D1x_PRIMITIVE_TOPOLOGY;
static const int D3D1x_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT = D3D1X_(COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT);
static const int D3D1x_COMMONSHADER_SAMPLER_SLOT_COUNT = D3D1X_(COMMONSHADER_SAMPLER_SLOT_COUNT);
static const int D3D1x_SIMULTANEOUS_RENDER_TARGET_COUNT = D3D1X_(SIMULTANEOUS_RENDER_TARGET_COUNT);
static const int D3D1x_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT = D3D1X_(IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT);
static const int D3D1x_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT = D3D1X_(COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT);
// Blob is the same
typedef ID3D10Blob                 ID3D1xBlob;

#if (OVR_D3D_VERSION == 10)
    typedef ID3D10Device		        ID3D1xDeviceContext;
#else
    typedef ID3D11DeviceContext         ID3D1xDeviceContext;
#endif


// Assert on HRESULT failure
inline void VERIFY_HRESULT(HRESULT hr)
{
    if (FAILED(hr))
        OVR_ASSERT(false);
}

class Buffer;

// Rendering parameters/pointers describing D3DX rendering setup.
struct RenderParams
{
    ID3D1xDevice*			pDevice;
    ID3D1xDeviceContext*    pContext;
    ID3D1xRenderTargetView* pBackBufferRT;
    IDXGISwapChain*         pSwapChain;
    Sizei                   RTSize;
    int                     Multisample;
};


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
// A ShaderSet is applied to a RenderDevice for rendering with a given fill.
class ShaderSet : public RefCountBase<ShaderSet>
{
protected:
    Ptr<Shader> Shaders[Shader_Count];

public:
    ShaderSet() { }
    ~ShaderSet() { }

    virtual void SetShader(Shader *s)
    {
        Shaders[s->GetStage()] = s;
    }
    virtual void UnsetShader(int stage)
    {
        Shaders[stage] = NULL;
    }
    Shader* GetShader(int stage) { return Shaders[stage]; }

    virtual void Set(PrimitiveType prim) const
    {
        for (int i = 0; i < Shader_Count; i++)
            if (Shaders[i])
                Shaders[i]->Set(prim);
    }

    // Set a uniform (other than the standard matrices). It is undefined whether the
    // uniforms from one shader occupy the same space as those in other shaders
    // (unless a buffer is used, then each buffer is independent).     
    virtual bool SetUniform(const char* name, int n, const float* v)
    {
        bool result = 0;
        for (int i = 0; i < Shader_Count; i++)
            if (Shaders[i])
                result |= Shaders[i]->SetUniform(name, n, v);

        return result;
    }
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

    virtual void Set(PrimitiveType prim = Prim_Unknown) const;   
    virtual void SetTexture(int i, class Texture* tex) { if (i < 8) Textures[i] = tex; }
    void SetInputLayout(void* newIL) { InputLayout = (void*)newIL; }
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

	ShaderBase(RenderParams* rp, ShaderStage stage);
	~ShaderBase();

    ShaderStage GetStage() const { return Stage; }

    void InitUniforms(const Uniform* refl, size_t reflSize);
	bool SetUniform(const char* name, int n, const float* v);
	bool SetUniformBool(const char* name, int n, const bool* v);
 
    void UpdateBuffer(Buffer* b);
};


template<ShaderStage SStage, class D3DShaderType>
class ShaderImpl : public ShaderBase
{
public:
    D3DShaderType*  D3DShader;

    ShaderImpl(RenderParams* rp, void* s, size_t size, const Uniform* refl, size_t reflSize) : ShaderBase(rp, SStage)
    {
        Load(s, size);
        InitUniforms(refl, reflSize);
    }
    ~ShaderImpl()
    {
        if (D3DShader)        
            D3DShader->Release();        
    }

    // These functions have specializations.
    bool Load(void* shader, size_t size);
    void Set(PrimitiveType prim) const;
    void SetUniformBuffer(Buffer* buffers, int i = 0);
};

typedef ShaderImpl<Shader_Vertex,  ID3D1xVertexShader> VertexShader;
typedef ShaderImpl<Shader_Fragment, ID3D1xPixelShader> PixelShader;


class Buffer : public RefCountBase<Buffer>
{
public:
    RenderParams*     pParams;
    Ptr<ID3D1xBuffer> D3DBuffer;
    size_t            Size;
    int               Use;
    bool              Dynamic;

public:
    Buffer(RenderParams* rp) : pParams(rp), Size(0), Use(0) {}
    ~Buffer();

    ID3D1xBuffer* GetBuffer() const { return D3DBuffer; }

    virtual size_t GetSize()        { return Size; }
    virtual void*  Map(size_t start, size_t size, int flags = 0);
    virtual bool   Unmap(void *m);
    virtual bool   Data(int use, const void* buffer, size_t size);
};


class Texture : public RefCountBase<Texture>
{
public:
    RenderParams*                   pParams;
    Ptr<ID3D1xTexture2D>            Tex;
    Ptr<ID3D1xShaderResourceView>   TexSv;
    Ptr<ID3D1xRenderTargetView>     TexRtv;
    Ptr<ID3D1xDepthStencilView>     TexDsv;
    mutable Ptr<ID3D1xSamplerState> Sampler;
    Sizei                           TextureSize;
    int                             Samples;

    Texture(RenderParams* rp, int fmt, const Sizei texSize,
            ID3D1xSamplerState* sampler, int samples = 1);
    ~Texture();

    virtual Sizei GetSize() const     { return TextureSize; }    
    virtual int   GetSamples() const  { return Samples; }

  //  virtual void SetSampleMode(int sm);

    // Updates texture to point to specified resources
    //  - used for slave rendering.
    void UpdatePlaceholderTexture(ID3D1xTexture2D* texture,
                                  ID3D1xShaderResourceView* psrv,
                                  const Sizei& textureSize)
    {
        Tex     = texture;
        TexSv   = psrv;
        TexRtv.Clear();
        TexDsv.Clear();

        TextureSize = textureSize;

#ifdef OVR_BUILD_DEBUG
        D3D1X_(TEXTURE2D_DESC) desc;
        texture->GetDesc(&desc);
        OVR_ASSERT(TextureSize == Sizei(desc.Width, desc.Height));
#endif
    }


    virtual void Set(int slot, ShaderStage stage = Shader_Fragment) const;

};


class GpuTimer : public RefCountBase<GpuTimer>
{
public:
    GpuTimer()
        : QuerySets(MaxNumQueryFrames)
        , D3dDevice(NULL)
        , Context(NULL)
        , LastQueuedFrame(-1)
        , LastTimedFrame(-1)
    { }

    void Init(ID3D1xDevice* device, ID3D1xDeviceContext* content);

    void BeginQuery();
    void EndQuery();

    // Returns -1 if timing is invalid
    float GetTiming(bool blockUntilValid);

protected:
    static const unsigned MaxNumQueryFrames = 10;
    
    int GotoNextFrame(int frame)
    {
        return (frame + 1) % MaxNumQueryFrames;
    }
    
    _COM_SMARTPTR_TYPEDEF(ID3D1xQuery, __uuidof(ID3D1xQuery));

    struct GpuQuerySets
    {
        ID3D1xQueryPtr DisjointQuery;
        ID3D1xQueryPtr TimeStartQuery;
        ID3D1xQueryPtr TimeEndQuery;
        bool QueryStarted;
        bool QueryAwaitingTiming;

        GpuQuerySets() : QueryStarted(false), QueryAwaitingTiming(false) {}
    };
    Array<GpuQuerySets> QuerySets;
    
    int LastQueuedFrame;
    int LastTimedFrame;

    Ptr<ID3D1xDevice> D3dDevice;
    Ptr<ID3D1xDeviceContext> Context;
};

}}} // OVR::CAPI::D3D1X

#endif // INC_OVR_CAPI_D3D10/11_Util_h
