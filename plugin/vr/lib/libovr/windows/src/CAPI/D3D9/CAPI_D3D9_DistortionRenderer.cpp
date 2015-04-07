/************************************************************************************

Filename    :   CAPI_D3D1X_DistortionRenderer.cpp
Content     :   Experimental distortion renderer
Created     :   March 7th, 2014
Authors     :   Tom Heath

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

#include "CAPI_D3D9_DistortionRenderer.h"
#define OVR_D3D_VERSION 9
#include "../../OVR_CAPI_D3D.h"

namespace OVR { namespace CAPI { namespace D3D9 {


///QUESTION : Why not just a normal constructor?
CAPI::DistortionRenderer* DistortionRenderer::Create(ovrHmd hmd,
                                                     FrameTimeManager& timeManager,
                                                     const HMDRenderState& renderState)
{
    return new DistortionRenderer(hmd, timeManager, renderState);
}

DistortionRenderer::DistortionRenderer(ovrHmd hmd, FrameTimeManager& timeManager,
                                       const HMDRenderState& renderState)
  : CAPI::DistortionRenderer(ovrRenderAPI_D3D9, hmd, timeManager, renderState),
    Device(NULL),
    SwapChain(NULL),
    VertexDecl(NULL),
    PixelShader(NULL),
    VertexShader(NULL),
    VertexShaderTimewarp(NULL),
   //screenSize(),
    ResolutionInPixels(0,0)
  //eachEye[]
{
    ScreenSize.w = 0;
    ScreenSize.h = 0;
    InitLatencyTester(renderState);
}


/**********************************************/
DistortionRenderer::~DistortionRenderer()
{
	//Release any memory 
	eachEye[0].dxIndices->Release();
	eachEye[0].dxVerts->Release();
	eachEye[1].dxIndices->Release();
	eachEye[1].dxVerts->Release();
}


/******************************************************************************/
bool DistortionRenderer::Initialize(const ovrRenderAPIConfig* apiConfig)
{
	///QUESTION - what is returned bool for???  Are we happy with this true, if not config.
    const ovrD3D9Config * config = (const ovrD3D9Config*)apiConfig;
    if (!config)                return true; 
    if (!config->D3D9.pDevice)  return false;

	//Glean all the required variables from the input structures
	Device         = config->D3D9.pDevice;
    SwapChain      = config->D3D9.pSwapChain;
	ScreenSize     = config->D3D9.Header.RTSize;

	GfxState = *new GraphicsState(Device, RState.DistortionCaps);

	CreateVertexDeclaration();
	CreateDistortionShaders();
	CreateDistortionModels();

    return true;
}

void DistortionRenderer::InitLatencyTester(const HMDRenderState& RenderState)
{
    ResolutionInPixels = RenderState.OurHMDInfo.ResolutionInPixels;
}


/**************************************************************/
void DistortionRenderer::SubmitEye(int eyeId, const ovrTexture* eyeTexture)
{
	//Doesn't do a lot in here??
	const ovrD3D9Texture* tex = (const ovrD3D9Texture*)eyeTexture;

	//Write in values
    eachEye[eyeId].texture = tex->D3D9.pTexture;

	// Its only at this point we discover what the viewport of the texture is.
	// because presumably we allow users to realtime adjust the resolution.
    eachEye[eyeId].TextureSize    = tex->D3D9.Header.TextureSize;
    eachEye[eyeId].RenderViewport = tex->D3D9.Header.RenderViewport;

    const ovrEyeRenderDesc& erd = RState.EyeRenderDesc[eyeId];
    
    ovrHmd_GetRenderScaleAndOffset( erd.Fov,
                                    eachEye[eyeId].TextureSize, eachEye[eyeId].RenderViewport,
                                    eachEye[eyeId].UVScaleOffset );

	if (RState.DistortionCaps & ovrDistortionCap_FlipInput)
	{
		eachEye[eyeId].UVScaleOffset[0].y = -eachEye[eyeId].UVScaleOffset[0].y;
		eachEye[eyeId].UVScaleOffset[1].y = 1.0f - eachEye[eyeId].UVScaleOffset[1].y;
	}
}

void DistortionRenderer::renderEndFrame()
{
    RenderBothDistortionMeshes();

    if(RegisteredPostDistortionCallback)
        RegisteredPostDistortionCallback(Device);

    if (LatencyTest2Active)
    {
        renderLatencyPixel(LatencyTest2DrawColor);
    }
}

/******************************************************************/
void DistortionRenderer::EndFrame(bool swapBuffers)
{
	///QUESTION : Clear the screen? 
	///QUESTION : Ensure the screen is the render target

    // Don't spin if we are explicitly asked not to
    if (RState.DistortionCaps & ovrDistortionCap_TimeWarp &&
        !(RState.DistortionCaps & ovrDistortionCap_ProfileNoTimewarpSpinWaits))
    {
        if (!TimeManager.NeedDistortionTimeMeasurement())
        {
            // Wait for timewarp distortion if it is time and Gpu idle
            FlushGpuAndWaitTillTime(TimeManager.GetFrameTiming().TimewarpPointTime);

            renderEndFrame();
        }
        else
        {
            // If needed, measure distortion time so that TimeManager can better estimate
            // latency-reducing time-warp wait timing.
            WaitUntilGpuIdle();
            double  distortionStartTime = ovr_GetTimeInSeconds();

            renderEndFrame();

            WaitUntilGpuIdle();
            TimeManager.AddDistortionTimeMeasurement(ovr_GetTimeInSeconds() - distortionStartTime);
        }
    }
    else
    {
        renderEndFrame();
    }

    if (LatencyTestActive)
    {
        renderLatencyQuad(LatencyTestDrawColor);
    }

    if (swapBuffers)
    {
        if (SwapChain)
        {
            SwapChain->Present(NULL, NULL, NULL, NULL, 0);
        }
        else
        {
		    Device->Present( NULL, NULL, NULL, NULL );
        }

        // Force GPU to flush the scene, resulting in the lowest possible latency.
        // It's critical that this flush is *after* present.
        // Doesn't need to be done if running through the Oculus driver.
        if (RState.OurHMDInfo.InCompatibilityMode &&
            !(RState.DistortionCaps & ovrDistortionCap_ProfileNoTimewarpSpinWaits))
        {
            WaitUntilGpuIdle();
        }
    }
}


void DistortionRenderer::WaitUntilGpuIdle()
{
	if(Device)
    {
         IDirect3DQuery9* pEventQuery=NULL ;
         Device->CreateQuery(D3DQUERYTYPE_EVENT, &pEventQuery) ;

         if(pEventQuery!=NULL)
         {
            pEventQuery->Issue(D3DISSUE_END) ;
            while(S_FALSE == pEventQuery->GetData(NULL, 0, D3DGETDATA_FLUSH)){}
            pEventQuery->Release();
         }
     }
}

double DistortionRenderer::FlushGpuAndWaitTillTime(double absTime)
{
	double initialTime = ovr_GetTimeInSeconds();
	if (initialTime >= absTime)
		return 0.0;

	WaitUntilGpuIdle();

    return WaitTillTime(absTime);
}


//-----------------------------------------------------------------------------
// Latency Tester Quad

static void ConvertSRGB(unsigned char c[3])
{
    for (int i = 0; i < 3; ++i)
    {
        double d = (double)c[i];
        double ds = d / 255.;

        if (ds <= 0.04045)
        {
            d /= 12.92;
        }
        else
        {
            d = 255. * pow((ds + 0.055) / 1.055, 2.4);
        }

        int color = (int)d;
        if (color < 0)
        {
            color = 0;
        }
        else if (color > 255)
        {
            color = 255;
        }

        c[i] = (unsigned char)color;
    }
}

void DistortionRenderer::renderLatencyQuad(unsigned char* color)
{
    D3DRECT rect = { ResolutionInPixels.w / 4, ResolutionInPixels.h / 4, ResolutionInPixels.w * 3 / 4, ResolutionInPixels.h * 3 / 4 };
    unsigned char c[3] = { color[0], color[1], color[2] };

    if (RState.DistortionCaps & ovrDistortionCap_SRGB)
    {
        ConvertSRGB(c);
    }

    Device->Clear(1, &rect, D3DCLEAR_TARGET, D3DCOLOR_RGBA(c[0], c[1], c[2], 255), 1, 0);
}

#ifdef OVR_BUILD_DEBUG
#define OVR_LATENCY_PIXEL_SIZE 20
#else
#define OVR_LATENCY_PIXEL_SIZE 5
#endif

void DistortionRenderer::renderLatencyPixel(unsigned char* color)
{
    D3DRECT rect = { ResolutionInPixels.w - OVR_LATENCY_PIXEL_SIZE, 0, ResolutionInPixels.w, OVR_LATENCY_PIXEL_SIZE };
    unsigned char c[3] = { color[0], color[1], color[2] };

    if (RState.DistortionCaps & ovrDistortionCap_SRGB)
    {
        ConvertSRGB(c);
    }

    Device->Clear(1, &rect, D3DCLEAR_TARGET, D3DCOLOR_RGBA(c[0], c[1], c[2], 255), 1, 0);
}


//-----------------------------------------------------------------------------
// GraphicsState

DistortionRenderer::GraphicsState::GraphicsState(IDirect3DDevice9* d, unsigned distortionCaps)
: Device(d)
, NumSavedStates(0)
, DistortionCaps(distortionCaps)
{
    #if defined(OVR_BUILD_DEBUG)
        memset(SavedState, 0, sizeof(SavedState));
    #endif
}

void DistortionRenderer::GraphicsState::RecordAndSetState(int which, int type, DWORD newValue)
{
	SavedStateType * sst = &SavedState[NumSavedStates++];
	sst->which = which;
	sst->type = type;
	if (which == 0)
	{
		Device->GetSamplerState(0, (D3DSAMPLERSTATETYPE)type, &sst->valueToRevertTo);
		Device->SetSamplerState(0, (D3DSAMPLERSTATETYPE)type, newValue);
	}
	else
	{
		Device->GetRenderState((D3DRENDERSTATETYPE)type, &sst->valueToRevertTo);
		Device->SetRenderState((D3DRENDERSTATETYPE)type, newValue);
	}
}

void DistortionRenderer::GraphicsState::Save()
{
	//Record and set rasterizer and sampler states.

	NumSavedStates=0;

    RecordAndSetState(0, D3DSAMP_MINFILTER,          D3DTEXF_LINEAR );
    RecordAndSetState(0, D3DSAMP_MAGFILTER,          D3DTEXF_LINEAR );
    RecordAndSetState(0, D3DSAMP_MIPFILTER,          D3DTEXF_LINEAR );
    RecordAndSetState(0, D3DSAMP_BORDERCOLOR,        0x000000 );
    RecordAndSetState(0, D3DSAMP_ADDRESSU,           D3DTADDRESS_BORDER );
    RecordAndSetState(0, D3DSAMP_ADDRESSV,           D3DTADDRESS_BORDER );
    RecordAndSetState(0, D3DSAMP_SRGBTEXTURE,        (DistortionCaps & ovrDistortionCap_SRGB) ? TRUE : FALSE );

	RecordAndSetState(1, D3DRS_MULTISAMPLEANTIALIAS, FALSE );
	RecordAndSetState(1, D3DRS_DITHERENABLE,         FALSE );
	RecordAndSetState(1, D3DRS_ZENABLE,              FALSE );
    RecordAndSetState(1, D3DRS_ZWRITEENABLE,         TRUE   );
    RecordAndSetState(1, D3DRS_ZFUNC,                D3DCMP_LESSEQUAL   );
    RecordAndSetState(1, D3DRS_CULLMODE ,            D3DCULL_CCW  );
   	RecordAndSetState(1, D3DRS_ALPHABLENDENABLE ,    FALSE );
   	RecordAndSetState(1, D3DRS_DEPTHBIAS ,           0 );
    RecordAndSetState(1, D3DRS_SRCBLEND ,            D3DBLEND_SRCALPHA );
    RecordAndSetState(1, D3DRS_DESTBLEND ,           D3DBLEND_INVSRCALPHA   );
   	RecordAndSetState(1, D3DRS_FILLMODE,             D3DFILL_SOLID );
    RecordAndSetState(1, D3DRS_ALPHATESTENABLE,      FALSE);
 	RecordAndSetState(1, D3DRS_DEPTHBIAS ,           0 );
    RecordAndSetState(1, D3DRS_LIGHTING,             FALSE );
   	RecordAndSetState(1, D3DRS_FOGENABLE,            FALSE );
    RecordAndSetState(1, D3DRS_SRGBWRITEENABLE,      (DistortionCaps & ovrDistortionCap_SRGB) ? TRUE : FALSE );
}


void DistortionRenderer::GraphicsState::Restore()
{
	for (int i = 0; i < NumSavedStates; i++)
	{
		SavedStateType * sst = &SavedState[i];
		if (sst->which == 0)
		{
			Device->SetSamplerState(0, (D3DSAMPLERSTATETYPE)sst->type, sst->valueToRevertTo);
		}
		else
		{
			Device->SetRenderState((D3DRENDERSTATETYPE)sst->type, sst->valueToRevertTo);
		}
	}
}


}}} // OVR::CAPI::D3D1X


