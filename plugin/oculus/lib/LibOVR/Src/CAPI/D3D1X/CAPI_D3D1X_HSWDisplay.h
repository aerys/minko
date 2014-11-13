/************************************************************************************

Filename    :   CAPI_D3D1X_HSWDisplay.h
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

// Do not use include guards, as this file is #included separately by both 
// CAPI_D3D10_HSWDisplay.h and CAPI_D3D11_HSWDisplay.h with OVR_D3D_VERSION defined
// to either 10 or 11. Only those two headers should #include this one.
//#ifndef OVR_CAPI_D3D1X_HSWDisplay_h
//#define OVR_CAPI_D3D1X_HSWDisplay_h

//#if !defined(OVR_CAPI_D3D10_HSWDisplay_h) && !defined(OVR_CAPI_D3D11_HSWDisplay_h)
//    #error This header is expected to be compiled only by these two headers.
//#endif
#if !defined(OVR_D3D_VERSION) || ((OVR_D3D_VERSION != 10) && (OVR_D3D_VERSION != 11))
    #error This header expects OVR_D3D_VERSION to be defined, to 10 or 11.
#endif

#include "../CAPI_HSWDisplay.h"
#include "CAPI_D3D1X_Util.h"


namespace OVR { namespace CAPI { namespace D3D_NS {

    class HSWDisplay : public CAPI::HSWDisplay
    {
    public:
        HSWDisplay(ovrRenderAPIType api, ovrHmd hmd, const HMDRenderState& renderState);

        // Must be called before use. apiConfig is such that:
        //   const ovrD3D11Config* config = (const ovrD3D11Config*)apiConfig; or
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

        OVR::CAPI::D3D_NS::RenderParams   RenderParams;
        Ptr<ID3D1xSamplerState>           pSamplerState;
        Ptr<OVR::CAPI::D3D_NS::Texture>   pTexture;
        Ptr<Buffer>                       pVB;
        Ptr<Buffer>                       UniformBufferArray[OVR::CAPI::D3D_NS::Shader_Count];
        Ptr<ShaderSet>                    pShaderSet;
        Ptr<ID3D1xInputLayout>            pVertexInputLayout;
        Ptr<ID3D1xBlendState>             pBlendState;
        Ptr<ID3D1xRasterizerState>        pRasterizerState;
        Matrix4f                          OrthoProjection[ovrEye_Count];

    private:
        OVR_NON_COPYABLE(HSWDisplay)
    };

}}} // namespace OVR::CAPI::D3D_NS



