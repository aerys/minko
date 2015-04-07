/************************************************************************************

Filename    :   CAPI_D3D1X_DistortionRenderer.h
Content     :   Experimental distortion renderer
Created     :   March 7, 2014
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

#include "../../Kernel/OVR_Types.h"

#if defined (OVR_OS_WIN32)
#define WIN32_LEAN_AND_MEAN
#include <d3d9.h>
#endif

#if defined(OVR_DEFINE_NEW)
#define new OVR_DEFINE_NEW
#endif 

#include "../CAPI_DistortionRenderer.h"


namespace OVR { namespace CAPI { namespace D3D9 {


//Implementation of DistortionRenderer for D3D9.
/***************************************************/
class DistortionRenderer : public CAPI::DistortionRenderer
{
public:    
    DistortionRenderer(ovrHmd hmd, FrameTimeManager& timeManager, const HMDRenderState& renderState);
    ~DistortionRenderer();

    // Creation function for the device.    
    static CAPI::DistortionRenderer* Create(ovrHmd hmd,
		                                    FrameTimeManager& timeManager,
                                            const HMDRenderState& renderState);
	
    // ***** Public DistortionRenderer interface
    virtual bool Initialize(const ovrRenderAPIConfig* apiConfig) OVR_OVERRIDE;

    virtual void SubmitEye(int eyeId, const ovrTexture* eyeTexture);

    virtual void EndFrame(bool swapBuffers);

    // TBD: Make public?
    void         WaitUntilGpuIdle();

	// Similar to ovr_WaitTillTime but it also flushes GPU.
	// Note, it exits when time expires, even if GPU is not in idle state yet.
	double       FlushGpuAndWaitTillTime(double absTime);

protected:
	
	class GraphicsState : public CAPI::DistortionRenderer::GraphicsState
	{
	public:
		GraphicsState(IDirect3DDevice9* d, unsigned distortionCaps);
		virtual void Save();
		virtual void Restore();

	protected:
		void RecordAndSetState(int which, int type, DWORD newValue);

		//Structure to store our state changes
		static const int MAX_SAVED_STATES=100;
		struct SavedStateType
		{
			int which;  //0 for samplerstate, 1 for renderstate
			int type;
			DWORD valueToRevertTo;
		} SavedState[MAX_SAVED_STATES];

		//Keep track of how many we've done, for reverting
		int NumSavedStates;
		IDirect3DDevice9* Device;
        unsigned DistortionCaps;
	};

private:

	//Functions
	void         CreateDistortionShaders(void);
	void         CreateDistortionModels(void);
	void         CreateVertexDeclaration(void);
	void         RenderBothDistortionMeshes();
	void         RecordAndSetState(int which, int type, DWORD newValue);
	void         RevertAllStates(void);

    void         renderEndFrame();

    // Latency tester
    void InitLatencyTester(const HMDRenderState& renderState);
    void renderLatencyQuad(unsigned char* latencyTesterDrawColor);
    void renderLatencyPixel(unsigned char* latencyTesterPixelColor);

	//Data, structures and pointers
	IDirect3DDevice9            * Device;
	IDirect3DSwapChain9         * SwapChain;
	IDirect3DVertexDeclaration9 * VertexDecl;
	IDirect3DPixelShader9       * PixelShader;
	IDirect3DVertexShader9      * VertexShader;
	IDirect3DVertexShader9      * VertexShaderTimewarp;
	ovrSizei                      ScreenSize;

    // Latency tester
    Size<int>                     ResolutionInPixels;

	struct FOR_EACH_EYE
	{
        FOR_EACH_EYE() : TextureSize(0), RenderViewport(Sizei(0)) { }

		IDirect3DVertexBuffer9  * dxVerts;
		IDirect3DIndexBuffer9   * dxIndices;
		int                       numVerts;
		int                       numIndices;
		IDirect3DTexture9       * texture;
		ovrVector2f			 	  UVScaleOffset[2]; 
        Sizei                     TextureSize;
        Recti                     RenderViewport;
	} eachEye[2];
};

}}} // OVR::CAPI::D3D9
