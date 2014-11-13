/************************************************************************************

Filename    :   CAPI_D3D1X_Util.cpp
Content     :   D3DX10 utility classes for rendering
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

#include "CAPI_D3D1X_Util.h"

#include <d3dcompiler.h>

namespace OVR { namespace CAPI { namespace D3D_NS {


//-------------------------------------------------------------------------------------
// ***** ShaderFill

void ShaderFill::Set(PrimitiveType prim) const
{
    Shaders->Set(prim);
    for(int i = 0; i < 8; i++)
    {
        if(Textures[i])
        {
            Textures[i]->Set(i);
        }
    }
}


//-------------------------------------------------------------------------------------
// ***** Buffer

Buffer::~Buffer()
{
}

bool Buffer::Data(int use, const void *buffer, size_t size)
{
    if (D3DBuffer && Size >= size)
    {
        if (Dynamic)
        {
            if (!buffer)
                return true;

            void* v = Map(0, size, Map_Discard);
            if (v)
            {
                memcpy(v, buffer, size);
                Unmap(v);
                return true;
            }
        }
        else
        {
            pParams->pContext->UpdateSubresource(D3DBuffer, 0, NULL, buffer, 0, 0);
            return true;
        }
    }
    if (D3DBuffer)
    {
        D3DBuffer = NULL;
        Size = 0;
        Use = 0;
        Dynamic = 0;
    }

    D3D1X_(BUFFER_DESC) desc;
    memset(&desc, 0, sizeof(desc));
    if (use & Buffer_ReadOnly)
    {
        desc.Usage = D3D1X_(USAGE_IMMUTABLE);
        desc.CPUAccessFlags = 0;
    }
    else
    {
        desc.Usage = D3D1X_(USAGE_DYNAMIC);
        desc.CPUAccessFlags = D3D1X_(CPU_ACCESS_WRITE);
        Dynamic = 1;
    }

    switch(use & Buffer_TypeMask)
    {
    case Buffer_Vertex:  desc.BindFlags = D3D1X_(BIND_VERTEX_BUFFER); break;
    case Buffer_Index:   desc.BindFlags = D3D1X_(BIND_INDEX_BUFFER);  break;
    case Buffer_Uniform:
        desc.BindFlags = D3D1X_(BIND_CONSTANT_BUFFER);
        size += ((size + 15) & ~15) - size;
        break;
    }

    desc.ByteWidth = (unsigned)size;

    D3D1X_(SUBRESOURCE_DATA) sr;
    sr.pSysMem = buffer;
    sr.SysMemPitch = 0;
    sr.SysMemSlicePitch = 0;

    D3DBuffer = NULL;
    HRESULT hr = pParams->pDevice->CreateBuffer(&desc, buffer ? &sr : NULL, &D3DBuffer.GetRawRef());
    if (SUCCEEDED(hr))
    {
        Use = use;
        Size = desc.ByteWidth;
        return 1;
    }
    return 0;
}

void*  Buffer::Map(size_t start, size_t size, int flags)
{
    OVR_UNUSED(size);

    D3D1X_(MAP) mapFlags = D3D1X_(MAP_WRITE);
    if (flags & Map_Discard)    
        mapFlags = D3D1X_(MAP_WRITE_DISCARD);    
    if (flags & Map_Unsynchronized)    
        mapFlags = D3D1X_(MAP_WRITE_NO_OVERWRITE);

#if (OVR_D3D_VERSION == 10)
    void* map;
    if (SUCCEEDED(D3DBuffer->Map(mapFlags, 0, &map)))    
        return ((char*)map) + start;    
#else
    D3D11_MAPPED_SUBRESOURCE map;
    if (SUCCEEDED(pParams->pContext->Map(D3DBuffer, 0, mapFlags, 0, &map)))
        return ((char*)map.pData) + start;
#endif

    return NULL;
}

bool   Buffer::Unmap(void *m)
{
    OVR_UNUSED(m);

    D3DSELECT_10_11( D3DBuffer->Unmap(),
                     pParams->pContext->Unmap(D3DBuffer, 0) );
    return true;
}


//-------------------------------------------------------------------------------------
// Shaders

template<> bool ShaderImpl<Shader_Vertex, ID3D1xVertexShader>::Load(void* shader, size_t size)
{
    return SUCCEEDED(pParams->pDevice->CreateVertexShader(shader, size  D3D11_COMMA_0, &D3DShader));
}
template<> bool ShaderImpl<Shader_Pixel, ID3D1xPixelShader>::Load(void* shader, size_t size)
{
    return SUCCEEDED(pParams->pDevice->CreatePixelShader(shader, size  D3D11_COMMA_0, &D3DShader));
}

template<> void ShaderImpl<Shader_Vertex, ID3D1xVertexShader>::Set(PrimitiveType) const
{
    pParams->pContext->VSSetShader(D3DShader D3D11_COMMA_0 D3D11_COMMA_0 );
}
template<> void ShaderImpl<Shader_Pixel, ID3D1xPixelShader>::Set(PrimitiveType) const
{
    pParams->pContext->PSSetShader(D3DShader D3D11_COMMA_0 D3D11_COMMA_0 ) ;
}

template<> void ShaderImpl<Shader_Vertex, ID3D1xVertexShader>::SetUniformBuffer(Buffer* buffer, int i)
{
    pParams->pContext->VSSetConstantBuffers(i, 1, &((Buffer*)buffer)->D3DBuffer.GetRawRef());
}
template<> void ShaderImpl<Shader_Pixel, ID3D1xPixelShader>::SetUniformBuffer(Buffer* buffer, int i)
{
    pParams->pContext->PSSetConstantBuffers(i, 1, &((Buffer*)buffer)->D3DBuffer.GetRawRef());
}


//-------------------------------------------------------------------------------------
// ***** Shader Base

ShaderBase::ShaderBase(RenderParams* rp, ShaderStage stage)
    : Shader(stage), pParams(rp), UniformData(0)
{
}
ShaderBase::~ShaderBase()
{
    if (UniformData)    
        OVR_FREE(UniformData);    
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

void ShaderBase::UpdateBuffer(Buffer* buf)
{
    if (UniformsSize)
    {
        buf->Data(Buffer_Uniform, UniformData, UniformsSize);
    }
}


//-------------------------------------------------------------------------------------
// ***** Texture
// 
Texture::Texture(RenderParams* rp, int fmt, const Sizei texSize,
                 ID3D1xSamplerState* sampler, int samples)
    : pParams(rp), Tex(NULL), TexSv(NULL), TexRtv(NULL), TexDsv(NULL),
    TextureSize(texSize),
    Sampler(sampler),
    Samples(samples)
{
    OVR_UNUSED(fmt);    
}

Texture::~Texture()
{
}

void Texture::Set(int slot, ShaderStage stage) const
{    
    ID3D1xShaderResourceView* texSv = TexSv.GetPtr();

    switch(stage)
    {
    case Shader_Fragment:
        pParams->pContext->PSSetShaderResources(slot, 1, &texSv);
        pParams->pContext->PSSetSamplers(slot, 1, &Sampler.GetRawRef());        
        break;

    case Shader_Vertex:
        pParams->pContext->VSSetShaderResources(slot, 1, &texSv);
        break;
    }
}


//-------------------------------------------------------------------------------------
// ***** GpuTimer
// 
#if (OVR_D3D_VERSION == 11)
#define D3DQUERY_EXEC(_context_, _query_, _command_, ...)  _context_->_command_(_query_, __VA_ARGS__)
#else
#define D3DQUERY_EXEC(_context_, _query_, _command_, ...)  _query_->_command_(__VA_ARGS__)
#endif


void GpuTimer::Init(ID3D1xDevice* device, ID3D1xDeviceContext* content)
{
    D3dDevice = device;
    Context = content;    
}

void GpuTimer::BeginQuery()
{
    if(GotoNextFrame(LastQueuedFrame) == LastTimedFrame)
    {
        OVR_ASSERT(false); // too many queries queued
        return;
    }

    LastQueuedFrame = GotoNextFrame(LastQueuedFrame);

    GpuQuerySets& newQuerySet = QuerySets[LastQueuedFrame];
    if(newQuerySet.DisjointQuery == NULL)
    {
        // Create the queries
        D3D1x_QUERY_DESC desc;
        desc.Query = D3D1X_(QUERY_TIMESTAMP_DISJOINT);
        desc.MiscFlags = 0;
        VERIFY_HRESULT(D3dDevice->CreateQuery(&desc, &newQuerySet.DisjointQuery));

        desc.Query = D3D1X_(QUERY_TIMESTAMP);
        VERIFY_HRESULT(D3dDevice->CreateQuery(&desc, &newQuerySet.TimeStartQuery));
        VERIFY_HRESULT(D3dDevice->CreateQuery(&desc, &newQuerySet.TimeEndQuery));
    }

    OVR_ASSERT(!newQuerySet.QueryStarted);
    OVR_ASSERT(!newQuerySet.QueryAwaitingTiming);

    
    D3DQUERY_EXEC(Context, QuerySets[LastQueuedFrame].DisjointQuery, Begin, );  // First start a disjoint query
    D3DQUERY_EXEC(Context, QuerySets[LastQueuedFrame].TimeStartQuery, End, );   // Insert start timestamp
    
    newQuerySet.QueryStarted = true;
    newQuerySet.QueryAwaitingTiming = false;
    //newQuerySet.QueryTimed = false;
}

void GpuTimer::EndQuery()
{
    if(LastQueuedFrame > 0 && !QuerySets[LastQueuedFrame].QueryStarted)
        return;

    GpuQuerySets& doneQuerySet = QuerySets[LastQueuedFrame];
    OVR_ASSERT(doneQuerySet.QueryStarted);
    OVR_ASSERT(!doneQuerySet.QueryAwaitingTiming);

    // Insert the end timestamp
    D3DQUERY_EXEC(Context, doneQuerySet.TimeEndQuery, End, );

    // End the disjoint query
    D3DQUERY_EXEC(Context, doneQuerySet.DisjointQuery, End, );

    doneQuerySet.QueryStarted = false;
    doneQuerySet.QueryAwaitingTiming = true;
}

float GpuTimer::GetTiming(bool blockUntilValid)
{
    float time = -1.0f;

    // loop until we hit a query that is not ready yet, or we have read all queued queries
    while(LastTimedFrame != LastQueuedFrame)
    {
        int timeTestFrame = GotoNextFrame(LastTimedFrame);

        GpuQuerySets& querySet = QuerySets[timeTestFrame];

        OVR_ASSERT(!querySet.QueryStarted && querySet.QueryAwaitingTiming);

        UINT64 startTime = 0;
        UINT64 endTime = 0;
        D3D1X_(QUERY_DATA_TIMESTAMP_DISJOINT) disjointData;

        if(blockUntilValid)
        {
            while(D3DQUERY_EXEC(Context, querySet.TimeStartQuery, GetData, &startTime, sizeof(startTime), 0) != S_OK);
            while(D3DQUERY_EXEC(Context, querySet.TimeEndQuery, GetData, &endTime, sizeof(endTime), 0) != S_OK);
            while(D3DQUERY_EXEC(Context, querySet.DisjointQuery, GetData, &disjointData, sizeof(disjointData), 0) != S_OK);
        }
        else
        {
// Early return if we fail to get data for any of these
            if(D3DQUERY_EXEC(Context, querySet.TimeStartQuery, GetData, &startTime, sizeof(startTime), 0) != S_OK)    return time;
            if(D3DQUERY_EXEC(Context, querySet.TimeEndQuery, GetData, &endTime, sizeof(endTime), 0) != S_OK)          return time;
            if(D3DQUERY_EXEC(Context, querySet.DisjointQuery, GetData, &disjointData, sizeof(disjointData), 0) != S_OK)    return time;
        }

        querySet.QueryAwaitingTiming = false;
        LastTimedFrame = timeTestFrame; // successfully retrieved the timing data

        if(disjointData.Disjoint == false)
        {
            UINT64 delta = endTime - startTime;
            float frequency = (float)(disjointData.Frequency);
            time = (delta / frequency);
        }
    }
    
    return time;
}

}}} // OVR::CAPI::D3DX
