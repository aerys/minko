/************************************************************************************

Filename    :   CAPI_D3D1X_DistortionRenderer.h
Content     :   Experimental distortion renderer
Created     :   November 11, 2013
Authors     :   Volga Aksoy

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

// No include guard, since this fill will be multiply-included. 
//#ifndef OVR_CAPI_D3D1X_DistortionRenderer_h

#include "CAPI_D3D1X_Util.h"
#include "../CAPI_DistortionRenderer.h"

#include "../../Kernel/OVR_Log.h"

namespace OVR { namespace CAPI { namespace D3D_NS {


// ***** D3D1X::DistortionRenderer

// Implementation of DistortionRenderer for D3D10/11.

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

    virtual bool Initialize(const ovrRenderAPIConfig* apiConfig,
                            unsigned distortionCaps);

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
		GraphicsState(ID3D1xDeviceContext* context);
		virtual ~GraphicsState();
		virtual void clearMemory();
		virtual void Save();
		virtual void Restore();

	protected:
		ID3D1xDeviceContext* context;
		BOOL memoryCleared;

		ID3D1xRasterizerState* rasterizerState;
		ID3D1xSamplerState* samplerStates[D3D1x_COMMONSHADER_SAMPLER_SLOT_COUNT];
		ID3D1xInputLayout* inputLayoutState;

		ID3D1xShaderResourceView* psShaderResourceState[D3D1x_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT];
		ID3D1xShaderResourceView* vsShaderResourceState[D3D1x_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT];

		ID3D1xBuffer* psConstantBuffersState[D3D1x_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT];
		ID3D1xBuffer* vsConstantBuffersState[D3D1x_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT];

		ID3D1xRenderTargetView* renderTargetViewState[D3D1x_SIMULTANEOUS_RENDER_TARGET_COUNT];
		ID3D1xDepthStencilView* depthStencilViewState;

		ID3D1xBlendState* omBlendState;
		FLOAT omBlendFactorState[4];
		UINT omSampleMaskState;

		D3D1x_PRIMITIVE_TOPOLOGY primitiveTopologyState;

		ID3D1xBuffer* iaIndexBufferPointerState;
		DXGI_FORMAT iaIndexBufferFormatState;
		UINT iaIndexBufferOffsetState;

		ID3D1xBuffer* iaVertexBufferPointersState[D3D1x_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT];
		UINT iaVertexBufferStridesState[D3D1x_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT];
		UINT iaVertexBufferOffsetsState[D3D1x_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT];

		ID3D1xPixelShader* currentPixelShader;
		ID3D1xVertexShader* currentVertexShader;
		ID3D1xGeometryShader* currentGeometryShader;
#if (OVR_D3D_VERSION == 11)
		ID3D11HullShader* currentHullShader;
		ID3D11DomainShader* currentDomainShader;
#endif

	};

private:
    // Helpers
    void initBuffersAndShaders();
    void initShaders();
    void initFullscreenQuad();
	void initOverdrive();
    void destroy();

    void setViewport(const Recti& vp);

    void renderDistortion(Texture* leftEyeTexture, Texture* rightEyeTexture);

    void renderPrimitives(const ShaderFill* fill, Buffer* vertices, Buffer* indices,
                          Matrix4f* viewMatrix, int offset, int count,
                          PrimitiveType rprim);

    void renderEndFrame();

    void createDrawQuad();
    void renderLatencyQuad(unsigned char* latencyTesterDrawColor);
    void renderLatencyPixel(unsigned char* latencyTesterPixelColor);

    // Create or get cached D3D sampler based on flags.
    ID3D1xSamplerState* getSamplerState(int sm);

    
    // TBD: Should we be using oe from RState instead?
    unsigned            DistortionCaps;

    // D3DX device and utility variables.
    RenderParams        RParams;    
    Ptr<Texture>        pEyeTextures[2];

    // U,V scale and offset needed for timewarp.
    ovrVector2f         UVScaleOffset[2][2];
    ovrSizei            EyeTextureSize[2];
    ovrRecti            EyeRenderViewport[2];

	Ptr<Texture>        pOverdriveTextures[NumOverdriveTextures];

    //Ptr<Buffer>         mpFullScreenVertexBuffer;

	Ptr<Buffer>         DistortionMeshVBs[2];    // one per-eye
	Ptr<Buffer>         DistortionMeshIBs[2];    // one per-eye

	Ptr<ShaderSet>      DistortionShader;
	Ptr<ID3D1xInputLayout> DistortionVertexIL;

    struct StandardUniformData
    {
        Matrix4f  Proj;
        Matrix4f  View;
    }                   StdUniforms;
    Ptr<Buffer>         UniformBuffers[Shader_Count];
    
    Ptr<ID3D1xSamplerState>     SamplerStates[Sample_Count];
    Ptr<ID3D1xRasterizerState>  Rasterizer;

    Ptr<Buffer>         LatencyTesterQuadVB;
    Ptr<ShaderSet>      SimpleQuadShader;
    Ptr<ID3D1xInputLayout> SimpleQuadVertexIL;

    GpuTimer GpuProfiler;
};

}}} // OVR::CAPI::D3D1X
