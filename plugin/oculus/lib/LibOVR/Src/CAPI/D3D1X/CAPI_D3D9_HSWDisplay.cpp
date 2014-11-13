/************************************************************************************

Filename    :   CAPI_D3D9_HSWDisplay.cpp
Content     :   Implements Health and Safety Warning system.
Created     :   July 7, 2014
Authors     :   Paul Pedriana

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

#define OVR_D3D_VERSION 9
#include "CAPI_D3D9_HSWDisplay.h"
#include "../../OVR_CAPI_D3D.h"
#undef  OVR_D3D_VERSION

#include <d3d9.h>
#include "../../Kernel/OVR_File.h"
#include "../../Kernel/OVR_SysFile.h"
#include "../../Kernel/OVR_Math.h"
#include "../../Kernel/OVR_Allocator.h"
#include "../../Kernel/OVR_Color.h"


namespace OVR { namespace CAPI { 


// To do Need to move LoadTextureTgaData to a shared location.
uint8_t* LoadTextureTgaData(OVR::File* f, uint8_t alpha, int& width, int& height);


namespace D3D9 {

// This is a temporary function implementation, and it functionality needs to be implemented in a more generic way.
IDirect3DTexture9* LoadTextureTga(HSWRenderParams& rParams, OVR::File* f, uint8_t alpha)
{
    IDirect3DTexture9* pTexture = NULL;

    int width, height;
    const uint8_t* pRGBA = LoadTextureTgaData(f, alpha, width, height);

    if (pRGBA)
    {
        // We don't have access to D3DX9 and so we currently have to do this manually instead of calling a D3DX9 utility function.
        Ptr<IDirect3DTexture9> pTextureSysmem;
        HRESULT hResult = rParams.Device->CreateTexture((UINT)width, (UINT)height, 1, 0, D3DFMT_A8R8G8B8, D3DPOOL_SYSTEMMEM, &pTextureSysmem.GetRawRef(), NULL);

        if(FAILED(hResult))
            { HSWDISPLAY_LOG(("CreateTexture(D3DPOOL_SYSTEMMEM) failed. %d (%x)", hResult, hResult)); }
        else
        {
            // Lock the texture so we can write this frame's texel data
            D3DLOCKED_RECT lock;
            hResult = pTextureSysmem->LockRect(0, &lock, NULL, D3DLOCK_NOSYSLOCK | D3DLOCK_NO_DIRTY_UPDATE);
            if(FAILED(hResult))
                { HSWDISPLAY_LOG(("LockRect failed. %d (%x)", hResult, hResult)); }
            else
            {
                // Four bytes per pixel. Pitch bytes per row (will be >= w * 4).
                uint8_t*       pRow = (uint8_t*)lock.pBits;
                const uint8_t* pSource = pRGBA;

                for(int y = 0; y < height; y++, pRow += lock.Pitch, pSource += (width * 4))
                {
                    uint8_t* pDest = pRow;

                    for(int x = 0, xEnd = width * 4; x < xEnd; x += 4)
                    {
                        pDest[x + 0] = pSource[x + 2];
                        pDest[x + 1] = pSource[x + 1];
                        pDest[x + 2] = pSource[x + 0];
                        pDest[x + 3] = pSource[x + 3];
                    }
                }

                pTextureSysmem->UnlockRect(0);

                hResult = rParams.Device->CreateTexture((UINT)width, (UINT)height, 1, 0, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &pTexture, NULL);
                if(FAILED(hResult))
                    { HSWDISPLAY_LOG(("CreateTexture(D3DPOOL_DEFAULT) failed. %d (%x)", hResult, hResult)); }
                else
                {
                    hResult = rParams.Device->UpdateTexture(pTextureSysmem, pTexture);
                    if(FAILED(hResult))
                    {
                        HSWDISPLAY_LOG(("UpdateTexture failed. %d (%x)", hResult, hResult));
                        pTexture->Release();
                        pTexture = NULL;
                    }
                }
            }
        }

        OVR_FREE(const_cast<uint8_t*>(pRGBA));
    }

    return pTexture;
}


// Loads a texture from a memory image of a TGA file.
IDirect3DTexture9* LoadTextureTga(HSWRenderParams& rParams, const uint8_t* pData, int dataSize, uint8_t alpha)
{
    MemoryFile memoryFile("", pData, dataSize);

    return LoadTextureTga(rParams, &memoryFile, alpha);
}


// Loads a texture from a disk TGA file.
IDirect3DTexture9* LoadTextureTga(HSWRenderParams& rParams, const char* pFilePath, uint8_t alpha)
{
    SysFile sysFile;

    if(sysFile.Open(pFilePath, FileConstants::Open_Read | FileConstants::Open_Buffered))
        return LoadTextureTga(rParams, &sysFile, alpha);

    return NULL;
}



// To do: This needs to be promoted to a central version, possibly in CAPI_HSWDisplay.h
struct HASWVertex
{
    Vector3f  Pos;
    Color     C;
    float     U, V;	

    HASWVertex(const Vector3f& p, const Color& c = Color(64,0,0,255), float u = 0, float v = 0)
        : Pos(p), C(c), U(u), V(v)
    {}

    HASWVertex(float x, float y, float z, const Color& c = Color(64,0,0,255), float u = 0, float v = 0) 
        : Pos(x,y,z), C(c), U(u), V(v)
    {}

    bool operator==(const HASWVertex& b) const
    {
        return (Pos == b.Pos) && (C == b.C) && (U == b.U) && (V == b.V);
    }
};

#define HASWVertexD3D9Format (D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1)


// The texture below may conceivably be shared between HSWDisplay instances. However,  
// beware that sharing may not be possible if two HMDs are using different locales  
// simultaneously. As of this writing it's not clear if that can occur in practice.

HSWDisplay::HSWDisplay(ovrRenderAPIType api, ovrHmd hmd, const HMDRenderState& renderState)
  : OVR::CAPI::HSWDisplay(api, hmd, renderState)
  , RenderParams()
{
}
    
bool HSWDisplay::Initialize(const ovrRenderAPIConfig* apiConfig)
{
    const ovrD3D9Config* config = reinterpret_cast<const ovrD3D9Config*>(apiConfig);

    if(config)
    {
	    RenderParams.Device     = config->D3D9.pDevice;
        RenderParams.SwapChain  = config->D3D9.pSwapChain;
	    RenderParams.ScreenSize = config->D3D9.Header.RTSize;
    }
    else
    {
        UnloadGraphics();
    }

    return true;
}

void HSWDisplay::Shutdown()
{
    UnloadGraphics();
}

void HSWDisplay::DisplayInternal()
{
    HSWDISPLAY_LOG(("[HSWDisplay D3D9] DisplayInternal()"));
    // We may want to call LoadGraphics here instead of within Render.
}

void HSWDisplay::DismissInternal()
{
    HSWDISPLAY_LOG(("[HSWDisplay D3D9] DismissInternal()"));
    UnloadGraphics();
}


void HSWDisplay::UnloadGraphics()
{
    // RenderParams: No need to clear.
    pTexture.Clear();
    pVB.Clear();
    // OrthoProjection: No need to clear.
}


void HSWDisplay::LoadGraphics()
{
    // As of this writing, we don't yet have an abstraction for Textures, Buffers, and Shaders like we do for D3D11, D3D11, and OpenGL.
    #if defined(OVR_BUILD_DEBUG)
        if(!pTexture)
            pTexture = *LoadTextureTga(RenderParams, "C:\\TestPath\\TestFile.tga", 255);
    #endif

    if(!pTexture)
    {
        D3DCAPS9 caps;
        RenderParams.Device->GetDeviceCaps(&caps);

        if(caps.TextureCaps & (D3DPTEXTURECAPS_SQUAREONLY | D3DPTEXTURECAPS_POW2))
            { HSWDISPLAY_LOG(("[HSWDisplay D3D9] Square textures allowed only.")); }

        size_t textureSize;
        const uint8_t* TextureData = GetDefaultTexture(textureSize);
        pTexture = *LoadTextureTga(RenderParams, TextureData, (int)textureSize, 255);
        OVR_ASSERT(pTexture);
    }

    if(!pVB)
    {
        HRESULT hResult = RenderParams.Device->CreateVertexBuffer(4 * sizeof(HASWVertex), NULL, HASWVertexD3D9Format, D3DPOOL_MANAGED, &pVB.GetRawRef(), NULL);

        if(FAILED(hResult))
            { HSWDISPLAY_LOG(("[HSWDisplay D3D9] CreateVertexBuffer failed. %d (%x)", hResult, hResult)); }
        else
        {
            void* pVerticesVoid;
            hResult = pVB->Lock(0, 0, (void**)&pVerticesVoid, 0);

            if(FAILED(hResult))
                { HSWDISPLAY_LOG(("[HSWDisplay D3D9] Lock failed. %d (%x)", hResult, hResult)); }
            else
            {
                HASWVertex* pVertices = reinterpret_cast<HASWVertex*>(pVerticesVoid);

                const bool  flip   = ((RenderState.DistortionCaps & ovrDistortionCap_FlipInput) != 0);
                const float left   = -1.0f;
                const float top    = -1.1f;
                const float right  = +1.0f;
                const float bottom = +0.9f;

                pVertices[0] = HASWVertex(left,  top,    0.f, Color(255, 255, 255, 255), 0.f, flip ? 1.f : 0.f); // To do: Make this branchless 
                pVertices[1] = HASWVertex(left,  bottom, 0.f, Color(255, 255, 255, 255), 0.f, flip ? 0.f : 1.f);
                pVertices[2] = HASWVertex(right, top,    0.f, Color(255, 255, 255, 255), 1.f, flip ? 1.f : 0.f); 
                pVertices[3] = HASWVertex(right, bottom, 0.f, Color(255, 255, 255, 255), 1.f, flip ? 0.f : 1.f);

                pVB->Unlock();
            }
        }
    }
}


void HSWDisplay::RenderInternal(ovrEyeType eye, const ovrTexture* eyeTexture)
{
    if(RenderEnabled && eyeTexture)
    {
        // Note: The D3D9 implementation below is entirely fixed-function and isn't yet using shaders.
        // For the time being this is sufficient, but future designs will likely necessitate moving
        // to a system that uses programmable shaders.

        // We need to render to the eyeTexture with the texture viewport.
        // Setup rendering to the texture.
        ovrD3D9Texture* eyeTextureD3D9 = const_cast<ovrD3D9Texture*>(reinterpret_cast<const ovrD3D9Texture*>(eyeTexture));
        OVR_ASSERT(eyeTextureD3D9->Texture.Header.API == ovrRenderAPI_D3D9);


        // Save previous state.
        // To do: Merge this saved state with that done by DistortionRenderer::GraphicsState::Save(), and put them in a shared location.
        DWORD fvfSaved;
        RenderParams.Device->GetFVF(&fvfSaved);

        Ptr<IDirect3DVertexBuffer9> pVBDSaved;
        UINT vbOffsetSaved;
        UINT vbStrideSaved;
        RenderParams.Device->GetStreamSource(0, &pVBDSaved.GetRawRef(), &vbOffsetSaved, &vbStrideSaved);

        Ptr<IDirect3DBaseTexture9> pTexture0Saved;
        RenderParams.Device->GetTexture(0, &pTexture0Saved.GetRawRef());
        Ptr<IDirect3DBaseTexture9> pTexture1Saved;
        RenderParams.Device->GetTexture(1, &pTexture1Saved.GetRawRef());

        D3DMATRIX worldMatrixSaved, viewMatrixSaved, projectionMatrixSaved, texture0MatrixSaved;
        RenderParams.Device->GetTransform(D3DTS_WORLD, &worldMatrixSaved);
        RenderParams.Device->GetTransform(D3DTS_VIEW, &viewMatrixSaved);
        RenderParams.Device->GetTransform(D3DTS_PROJECTION, &projectionMatrixSaved);
        RenderParams.Device->GetTransform(D3DTS_TEXTURE0, &texture0MatrixSaved);

        Ptr<IDirect3DVertexShader9> pVertexShaderSaved;
        RenderParams.Device->GetVertexShader(&pVertexShaderSaved.GetRawRef());

        Ptr<IDirect3DPixelShader9> pPixelShaderSaved;
        RenderParams.Device->GetPixelShader(&pPixelShaderSaved.GetRawRef());

        D3DVIEWPORT9 viewportSaved;
        RenderParams.Device->GetViewport(&viewportSaved);

        Ptr<IDirect3DSurface9> pRenderTargetSaved;
        RenderParams.Device->GetRenderTarget(0, &pRenderTargetSaved.GetRawRef());


        // Load the graphics if not loaded already.
        if(!pTexture)
            LoadGraphics();

        // Calculate ortho projection.
        GetOrthoProjection(RenderState, OrthoProjection);

        HRESULT hResult = RenderParams.Device->BeginScene();
        if(FAILED(hResult))
            { HSWDISPLAY_LOG(("[HSWDisplay D3D9] BeginScene failed. %d (%x)", hResult, hResult)); }

        Ptr<IDirect3DSurface9> pDestSurface;
        hResult = eyeTextureD3D9->D3D9.pTexture->GetSurfaceLevel(0, &pDestSurface.GetRawRef());
        if(FAILED(hResult))
            { HSWDISPLAY_LOG(("[HSWDisplay D3D9] GetSurfaceLevel failed. %d (%x)", hResult, hResult)); }

        hResult = RenderParams.Device->SetRenderTarget(0, pDestSurface);
        if(FAILED(hResult))
            { HSWDISPLAY_LOG(("[HSWDisplay D3D9] SetRenderTarget failed. %d (%x)", hResult, hResult)); }

        D3DVIEWPORT9 D3DViewport;
        D3DViewport.X        = eyeTextureD3D9->Texture.Header.RenderViewport.Pos.x;
        D3DViewport.Y        = eyeTextureD3D9->Texture.Header.RenderViewport.Pos.y;    
        D3DViewport.Width    = eyeTextureD3D9->Texture.Header.RenderViewport.Size.w;
        D3DViewport.Height   = eyeTextureD3D9->Texture.Header.RenderViewport.Size.h;
        D3DViewport.MinZ     = 0;
        D3DViewport.MaxZ     = 1;
        hResult = RenderParams.Device->SetViewport(&D3DViewport);
        if(FAILED(hResult))
            { HSWDISPLAY_LOG(("[HSWDisplay D3D9] SetViewport failed. %d (%x)", hResult, hResult)); }

        hResult = RenderParams.Device->SetTexture(0, pTexture);
        if(FAILED(hResult))
            { HSWDISPLAY_LOG(("[HSWDisplay D3D9] SetTexture failed. %d (%x)", hResult, hResult)); }

        RenderParams.Device->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
        RenderParams.Device->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
        RenderParams.Device->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);

        RenderParams.Device->SetVertexShader(NULL);
        RenderParams.Device->SetPixelShader(NULL);

        hResult = RenderParams.Device->SetStreamSource(0, pVB, 0, sizeof(HASWVertex));
        if(FAILED(hResult))
            { HSWDISPLAY_LOG(("[HSWDisplay D3D9] SetStreamSource failed. %d (%x)", hResult, hResult)); }

        RenderParams.Device->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
        RenderParams.Device->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);
		RenderParams.Device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
        RenderParams.Device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
		RenderParams.Device->SetRenderState(D3DRS_LIGHTING, FALSE);
        RenderParams.Device->SetRenderState(D3DRS_ZENABLE, FALSE);
        RenderParams.Device->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
        RenderParams.Device->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
        RenderParams.Device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
        RenderParams.Device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

        const float scale  = HSWDISPLAY_SCALE * ((RenderState.OurHMDInfo.HmdType == HmdType_DK1) ? 0.70f : 1.f);
        Matrix4f identityMatrix = Matrix4f::Identity();
        Vector3f translation = OrthoProjection[eye].GetTranslation();
        Matrix4f orthoStereoMatrix(
            scale, 0, 0, 0,
            0, scale / 2, 0, 0,
            0, 0, HSWDISPLAY_DISTANCE, 0,
            translation.x, translation.y, translation.z, 1
            );
        RenderParams.Device->SetTransform(D3DTS_WORLD,      reinterpret_cast<const D3DMATRIX*>(&identityMatrix));
        RenderParams.Device->SetTransform(D3DTS_VIEW,       reinterpret_cast<const D3DMATRIX*>(&identityMatrix));
        RenderParams.Device->SetTransform(D3DTS_PROJECTION, reinterpret_cast<const D3DMATRIX*>(&orthoStereoMatrix));
        RenderParams.Device->SetTransform(D3DTS_TEXTURE0,   reinterpret_cast<const D3DMATRIX*>(&identityMatrix));

        hResult = RenderParams.Device->SetFVF(HASWVertexD3D9Format);
        if(FAILED(hResult))
            { HSWDISPLAY_LOG(("[HSWDisplay D3D9] SetFVF failed. %d (%x)", hResult, hResult)); }

        hResult = RenderParams.Device->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);
        if(FAILED(hResult))
            { HSWDISPLAY_LOG(("[HSWDisplay D3D9] DrawPrimitive failed. %d (%x)", hResult, hResult)); }

        hResult = RenderParams.Device->EndScene();
        if(FAILED(hResult))
            { HSWDISPLAY_LOG(("[HSWDisplay D3D9] EndScene failed. %d (%x)", hResult, hResult)); }


        // Restore previous state.
        RenderParams.Device->SetRenderTarget(0, pRenderTargetSaved);
        RenderParams.Device->SetViewport(&viewportSaved);
        RenderParams.Device->SetPixelShader(pPixelShaderSaved);
        RenderParams.Device->SetVertexShader(pVertexShaderSaved);
        RenderParams.Device->SetTransform(D3DTS_TEXTURE0, &texture0MatrixSaved);
        RenderParams.Device->SetTransform(D3DTS_PROJECTION, &projectionMatrixSaved);
        RenderParams.Device->SetTransform(D3DTS_VIEW, &viewMatrixSaved);
        RenderParams.Device->SetTransform(D3DTS_WORLD, &worldMatrixSaved);
        RenderParams.Device->SetTexture(0, pTexture0Saved);
        RenderParams.Device->SetTexture(1, pTexture1Saved);
        RenderParams.Device->SetStreamSource(0, pVBDSaved, vbOffsetSaved, vbStrideSaved);
        RenderParams.Device->SetFVF(fvfSaved);
    }
}


}}} // namespace OVR::CAPI::D3D9







