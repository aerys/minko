/************************************************************************************

Filename    :   OVR_CAPI_D3D.h
Content     :   D3D specific structures used by the CAPI interface.
Created     :   November 7, 2013
Authors     :   Michael Antonov

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
#ifndef OVR_CAPI_D3D_h
#define OVR_CAPI_D3D_h

/// @file OVR_CAPI_D3D.h
/// D3D rendering support.

#include "OVR_CAPI.h"

#ifndef OVR_D3D_VERSION
#error Please define OVR_D3D_VERSION to 9 or 10 or 11 before including OVR_CAPI_D3D.h
#endif


#if defined(OVR_D3D_VERSION) && (OVR_D3D_VERSION == 11)

//-----------------------------------------------------------------------------------
// ***** D3D11 Specific

#include <d3d11.h>

/// Used to configure slave D3D rendering (i.e. for devices created externally).
struct ovrD3D11ConfigData
{
    /// General device settings.
    ovrRenderAPIConfigHeader Header;
    /// The D3D device to use for rendering.
	ID3D11Device*            pDevice;
    /// The D3D device context to use for rendering.
    ID3D11DeviceContext*     pDeviceContext;
    /// A render target view for the backbuffer.
    ID3D11RenderTargetView*  pBackBufferRT;
    /// The swapchain that will present rendered frames.
    IDXGISwapChain*          pSwapChain;
};

/// Contains D3D11-specific rendering information.
union ovrD3D11Config
{
    /// General device settings.
    ovrRenderAPIConfig Config;
    /// D3D11-specific settings.
    ovrD3D11ConfigData D3D11;
};

/// Used to pass D3D11 eye texture data to ovrHmd_EndFrame.
struct ovrD3D11TextureData
{
    /// General device settings.
    ovrTextureHeader          Header;
    /// The D3D11 texture containing the undistorted eye image.
    ID3D11Texture2D*          pTexture;
    /// The D3D11 shader resource view for this texture.
    ID3D11ShaderResourceView* pSRView;
};

/// Contains OpenGL-specific texture information.
union ovrD3D11Texture
{
    /// General device settings.
    ovrTexture          Texture;
    /// D3D11-specific settings.
    ovrD3D11TextureData D3D11;
};



#elif defined(OVR_D3D_VERSION) && (OVR_D3D_VERSION == 10)

#include <d3d10_1.h>
#include <d3d10.h>

//-----------------------------------------------------------------------------------
// ***** D3D10 Specific

/// Used to configure slave D3D rendering (i.e. for devices created externally).
struct ovrD3D10ConfigData
{
    /// General device settings.
    ovrRenderAPIConfigHeader Header;
    ID3D10Device*            pDevice;
    void*                    Unused;
    ID3D10RenderTargetView*  pBackBufferRT;
    IDXGISwapChain*          pSwapChain;
};

union ovrD3D10Config
{
    ovrRenderAPIConfig Config;
    ovrD3D10ConfigData D3D10;
};

/// Used to pass D3D10 eye texture data to ovrHmd_EndFrame.
struct ovrD3D10TextureData
{
    /// General device settings.
    ovrTextureHeader          Header;
    ID3D10Texture2D*          pTexture;
    ID3D10ShaderResourceView* pSRView;
};

union ovrD3D10Texture
{
    ovrTexture          Texture;
    ovrD3D10TextureData D3D10;
};

#elif defined(OVR_D3D_VERSION) && (OVR_D3D_VERSION == 9)

#include <d3d9.h>

//-----------------------------------------------------------------------------------
// ***** D3D9 Specific

// Used to configure D3D9 rendering 
struct ovrD3D9ConfigData
{
    // General device settings.
    ovrRenderAPIConfigHeader Header;
    
	IDirect3DDevice9*           pDevice;
    IDirect3DSwapChain9*        pSwapChain;
};

union ovrD3D9Config
{
    ovrRenderAPIConfig Config;
    ovrD3D9ConfigData D3D9;
};

// Used to pass D3D9 eye texture data to ovrHmd_EndFrame.
struct ovrD3D9TextureData
{
    // General device settings.
    ovrTextureHeader            Header;
    IDirect3DTexture9*          pTexture;  
};

union ovrD3D9Texture
{
    ovrTexture         Texture;
    ovrD3D9TextureData D3D9;
};

#endif

#endif	// OVR_CAPI_h
