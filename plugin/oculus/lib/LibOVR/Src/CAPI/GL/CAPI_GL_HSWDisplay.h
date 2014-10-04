/************************************************************************************

Filename    :   CAPI_GL_HSWDisplay.h
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

#ifndef OVR_CAPI_GL_HSWDisplay_h
#define OVR_CAPI_GL_HSWDisplay_h


#include "../CAPI_HSWDisplay.h"
#include "CAPI_GL_Util.h"


namespace OVR { namespace CAPI { namespace GL {

    class HSWDisplay : public CAPI::HSWDisplay
    {
    public:
        HSWDisplay(ovrRenderAPIType api, ovrHmd hmd, const HMDRenderState& renderState);

        // Must be called before use. apiConfig is such that:
        //   const ovrGLConfig* config = (const ovrGLConfig*)apiConfig; or
        bool Initialize(const ovrRenderAPIConfig* apiConfig);
        void Shutdown();
        void DisplayInternal();
        void DismissInternal();

        // Draws the warning to the eye texture(s). This must be done at the end of a 
        // frame but prior to executing the distortion rendering of the eye textures. 
        void RenderInternal(ovrEyeType eye, const ovrTexture* eyeTexture);

    protected:
        void UnloadGraphics();
        void LoadGraphics();

        OVR::CAPI::GL::RenderParams        RenderParams;
        GLVersionAndExtensions             GLVersionInfo;
        GLuint                             FrameBuffer;         // This is a container for a texture, depth buffer, stencil buffer to be rendered to. To consider: Make a wrapper class, like the OculusWorldDemo RBuffer class. 
        Ptr<OVR::CAPI::GL::Texture>        pTexture;
        Ptr<OVR::CAPI::GL::ShaderSet>      pShaderSet;
        Ptr<OVR::CAPI::GL::VertexShader>   pVertexShader;
        Ptr<OVR::CAPI::GL::FragmentShader> pFragmentShader;
        Ptr<OVR::CAPI::GL::Buffer>         pVB;
        GLuint                             VAO;                 // Vertex Array Object.
        bool                               VAOInitialized;      // True if the VAO was initialized with vertex buffer data.
        Matrix4f                           OrthoProjection[2];  // Projection for 2D.

    private:
        OVR_NON_COPYABLE(HSWDisplay)
    };

}}} // namespace OVR::CAPI::GL


#endif // OVR_CAPI_GL_HSWDisplay_h

