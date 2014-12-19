/************************************************************************************

Filename    :   CAPI_GL_DistortionRenderer.h
Content     :   Distortion renderer header for GL
Created     :   November 11, 2013
Authors     :   David Borel, Lee Cooper

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

#ifndef OVR_CAPI_GL_DistortionRenderer_h
#define OVR_CAPI_GL_DistortionRenderer_h

#include "../CAPI_DistortionRenderer.h"

#include "../../Kernel/OVR_Log.h"
#include "CAPI_GL_Util.h"

namespace OVR { namespace CAPI { namespace GL {

// ***** GL::DistortionRenderer

// Implementation of DistortionRenderer for GL.

class DistortionRenderer : public CAPI::DistortionRenderer
{
public:    
    DistortionRenderer(ovrHmd hmd,
                       FrameTimeManager& timeManager,
                       const HMDRenderState& renderState);
    ~DistortionRenderer();

    
    // Creation function for the device.    
    static CAPI::DistortionRenderer* Create(ovrHmd hmd,
                                            FrameTimeManager& timeManager,
                                            const HMDRenderState& renderState);


    // ***** Public DistortionRenderer interface
	
    virtual bool Initialize(const ovrRenderAPIConfig* apiConfig) OVR_OVERRIDE;

    virtual void SubmitEye(int eyeId, const ovrTexture* eyeTexture);

    virtual void EndFrame(bool swapBuffers);

    void         WaitUntilGpuIdle();

	// Similar to ovr_WaitTillTime but it also flushes GPU.
	// Note, it exits when time expires, even if GPU is not in idle state yet.
	double       FlushGpuAndWaitTillTime(double absTime);

protected:
    
    
    class GraphicsState : public CAPI::DistortionRenderer::GraphicsState
    {
    public:
        GraphicsState();
        virtual void Save();
        virtual void Restore();
        
#ifdef OVR_OS_MAC
        // Asking for Core Profile is equivalent to asking whether we have a
        // 3.2+ context on mac.
        bool isAtLeastOpenGL3();
#endif

    protected:
        void ApplyBool(GLenum Name, GLint Value, GLint index = -1);
        
    public:
        GLVersionAndExtensions GLVersionInfo;
        
        GLint Viewport[4];
        GLfloat ClearColor[4];
        GLint DepthTest;
        GLint CullFace;
        GLint SRGB;
        GLint Program;
        GLint ActiveTexture;
        GLint TextureBinding;
        GLint VertexArrayBinding;
        GLint ElementArrayBufferBinding;
        GLint ArrayBufferBinding;
        GLint FrameBufferBinding;
        
        GLint Blend;
        GLint ColorWritemask[4];
        GLint Dither;
        GLint Fog;
        GLint Lighting;
        GLint RasterizerDiscard;
        GLint RenderMode;
        GLint SampleMask;
        GLint ScissorTest;
        GLfloat ZoomX;
        GLfloat ZoomY;
    };

	struct FOR_EACH_EYE
	{
        FOR_EACH_EYE() : TextureSize(0), RenderViewport(Sizei(0)) { }

#if 0
		IDirect3DVertexBuffer9  * dxVerts;
		IDirect3DIndexBuffer9   * dxIndices;
#endif
		int                       numVerts;
		int                       numIndices;

		GLuint                    texture;

		ovrVector2f			 	  UVScaleOffset[2];
        Sizei                     TextureSize;
        Recti                     RenderViewport;
	} eachEye[2];

    Ptr<Texture>    pOverdriveTextures[NumOverdriveTextures];
    Ptr<Texture>    OverdriveBackBufferTexture;

    // GL context and utility variables.
    RenderParams        RParams;
    Context             distortionContext;

	// Helpers
    void initOverdrive();
    void initBuffersAndShaders();
    void initShaders();
    void initFullscreenQuad();
    void destroy();
	
    void setViewport(const Recti& vp);

    void renderDistortion(Texture* leftEyeTexture, Texture* rightEyeTexture);

    void renderPrimitives(const ShaderFill* fill, Buffer* vertices, Buffer* indices,
                          int offset, int count,
						  PrimitiveType rprim, GLuint* vao, bool isDistortionMesh);

	void createDrawQuad();
    void renderLatencyQuad(unsigned char* latencyTesterDrawColor);
    void renderLatencyPixel(unsigned char* latencyTesterPixelColor);
	
    void renderEndFrame();

    Ptr<Texture>        pEyeTextures[2];

	Ptr<Buffer>         DistortionMeshVBs[2];    // one per-eye
	Ptr<Buffer>         DistortionMeshIBs[2];    // one per-eye
	GLuint              DistortionMeshVAOs[2];   // one per-eye

	Ptr<ShaderSet>      DistortionShader;

    bool                RotateCCW90;

    struct StandardUniformData
    {
        Matrix4f  Proj;
        Matrix4f  View;
    }                   StdUniforms;
	
	GLuint              LatencyVAO;
    Ptr<Buffer>         LatencyTesterQuadVB;
    Ptr<ShaderSet>      SimpleQuadShader;
    Ptr<ShaderSet>      SimpleQuadGammaShader;

    GLuint              OverdriveFbo;

	GLint SavedViewport[4];
	GLfloat SavedClearColor[4];
	GLint SavedDepthTest;
	GLint SavedCullFace;
	GLint SavedProgram;
	GLint SavedActiveTexture;
	GLint SavedBoundTexture;
	GLint SavedVertexArray;
    GLint SavedBoundFrameBuffer;
};

}}} // OVR::CAPI::GL

#endif // OVR_CAPI_GL_DistortionRenderer_h
