/************************************************************************************

Filename    :   CAPI_D3D9_HSWDisplay.h
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

#ifndef OVR_CAPI_D3D9_HSWDisplay_h
#define OVR_CAPI_D3D9_HSWDisplay_h

#if !defined(OVR_D3D_VERSION) || (OVR_D3D_VERSION != 9)
    #error This header expects OVR_D3D_VERSION to be defined, to 9.
#endif

#include "../CAPI_HSWDisplay.h"
#include "CAPI_D3D1X_Util.h"
#include <d3d9.h>


namespace OVR { namespace CAPI { namespace D3D9 {

    // There currently isn't a D3D9::RenderParams, as D3D9 support is currently only very basic.
    struct HSWRenderParams
    {
	    IDirect3DDevice9*    Device;
	    IDirect3DSwapChain9* SwapChain;
	    ovrSizei             ScreenSize; 
    };

    class HSWDisplay : public CAPI::HSWDisplay
    {
    public:
        HSWDisplay(ovrRenderAPIType api, ovrHmd hmd, const HMDRenderState& renderState);

        // Must be called before use. apiConfig is such that:
        //   const ovrD3D9Config* config = (const ovrD3D9Config*)apiConfig; or
        bool Initialize(const ovrRenderAPIConfig* apiConfig);
        void Shutdown();
        void DisplayInternal();
        void DismissInternal();

        // Draws the warning to the eye texture(s). This must be done at the end of a 
        // frame but prior to executing the distortion rendering of the eye textures. 
        void RenderInternal(ovrEyeType eye, const ovrTexture* eyeTexture);

    protected:
        void LoadGraphics();
        void UnloadGraphics();

        D3D9::HSWRenderParams       RenderParams;
        Ptr<IDirect3DTexture9>      pTexture;
        Ptr<IDirect3DVertexBuffer9> pVB;
        Matrix4f                    OrthoProjection[2];     // Projection for 2D.

    private:
        OVR_NON_COPYABLE(HSWDisplay)
    };

}}} // namespace OVR::CAPI::D3D9


#endif // OVR_CAPI_D3D9_HSWDisplay_h

