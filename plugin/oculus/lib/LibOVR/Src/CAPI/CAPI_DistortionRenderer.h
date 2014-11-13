/************************************************************************************

Filename    :   CAPI_DistortionRenderer.h
Content     :   Abstract interface for platform-specific rendering of distortion
Created     :   February 2, 2014
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

#ifndef OVR_CAPI_DistortionRenderer_h
#define OVR_CAPI_DistortionRenderer_h

#include "CAPI_HMDRenderState.h"
#include "CAPI_FrameTimeManager.h"

typedef void (*PostDistortionCallback)(void* pRenderContext);

namespace OVR { namespace CAPI {

//-------------------------------------------------------------------------------------
// ***** CAPI::DistortionRenderer

// DistortionRenderer implements rendering of distortion and other overlay elements
// in platform-independent way.
// Platform-specific renderer back ends for CAPI are derived from this class.

class  DistortionRenderer : public RefCountBase<DistortionRenderer>
{
    // Quiet assignment compiler warning.
    void operator = (const DistortionRenderer&) { }
public:
    
    DistortionRenderer(ovrRenderAPIType api, ovrHmd hmd,
                       FrameTimeManager& timeManager,              
                       const HMDRenderState& renderState) :
		LastUsedOverdriveTextureIndex(-1),
        LatencyTestActive(false),
        LatencyTest2Active(false),
        RenderAPI(api),
        HMD(hmd),
        TimeManager(timeManager),
        RState(renderState),
        GfxState(),
        RegisteredPostDistortionCallback(NULL)
    {
#ifdef OVR_OS_WIN32
        timer = CreateWaitableTimer(NULL, TRUE, NULL);
        OVR_ASSERT(timer != NULL);
#endif
    }
    virtual ~DistortionRenderer()
    {
    }
    

    // Configures the Renderer based on externally passed API settings. Must be
    // called before use.
    // Under D3D, apiConfig includes D3D Device pointer, back buffer and other
    // needed structures.
    virtual bool Initialize(const ovrRenderAPIConfig* apiConfig,
                            unsigned distortionCaps) = 0;

    // Submits one eye texture for rendering. This is in the separate method to
    // allow "submit as you render" scenarios on horizontal screens where one
    // eye can be scanned out before the other.
    virtual void SubmitEye(int eyeId, const ovrTexture* eyeTexture) = 0;

    // Finish the frame, optionally swapping buffers.
    // Many implementations may actually apply the distortion here.
    virtual void EndFrame(bool swapBuffers) = 0;
    
    void RegisterPostDistortionCallback(PostDistortionCallback postDistortionCallback)
    {
        RegisteredPostDistortionCallback = postDistortionCallback;
    }

	// Stores the current graphics pipeline state so it can be restored later.
	void SaveGraphicsState() { if (!(RState.DistortionCaps & ovrDistortionCap_NoRestore)) GfxState->Save(); }

	// Restores the saved graphics pipeline state.
	void RestoreGraphicsState() { if (!(RState.DistortionCaps & ovrDistortionCap_NoRestore)) GfxState->Restore(); }

    // *** Creation Factory logic
    
    ovrRenderAPIType GetRenderAPI() const { return RenderAPI; }

    // Creation function for this interface, registered for API.
    typedef DistortionRenderer* (*CreateFunc)(ovrHmd hmd,
                                              FrameTimeManager &timeManager,
                                              const HMDRenderState& renderState);

    static CreateFunc APICreateRegistry[ovrRenderAPI_Count];

    // Color is expected to be 3 byte RGB
    void SetLatencyTestColor(unsigned char* color);
    void SetLatencyTest2Color(unsigned char* color);
    
protected:
	// Used for pixel luminance overdrive on DK2 displays
	// A copy of back buffer images will be ping ponged
	// TODO: figure out 0 dynamically based on DK2 latency?
	static const int	NumOverdriveTextures = 2;
	int					LastUsedOverdriveTextureIndex;

    bool                LatencyTestActive;
    unsigned char       LatencyTestDrawColor[3];
    bool                LatencyTest2Active;
    unsigned char       LatencyTest2DrawColor[3];

    bool IsOverdriveActive()
	{
		// doesn't make sense to use overdrive when vsync is disabled as we cannot guarantee
		// when the rendered frame will be displayed
		return LastUsedOverdriveTextureIndex >= 0 && !((RState.EnabledHmdCaps & ovrHmdCap_NoVSync) > 0);
	}

    double WaitTillTime(double absTime);

#ifdef OVR_OS_WIN32
    HANDLE timer;
    LARGE_INTEGER waitableTimerInterval;
#endif

    class GraphicsState : public RefCountBase<GraphicsState>
    {
    public:
        GraphicsState() : IsValid(false) {}
        virtual ~GraphicsState() {}
        virtual void Save() = 0;
        virtual void Restore() = 0;
        
    protected:
        bool IsValid;
    };
    
    const ovrRenderAPIType  RenderAPI;
    const ovrHmd            HMD;
    FrameTimeManager&       TimeManager;
    const HMDRenderState&   RState;
    Ptr<GraphicsState>      GfxState;
    PostDistortionCallback  RegisteredPostDistortionCallback;
};

}} // namespace OVR::CAPI


#endif // OVR_CAPI_DistortionRenderer_h
