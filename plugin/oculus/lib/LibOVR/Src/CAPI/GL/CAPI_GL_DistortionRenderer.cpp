/************************************************************************************

Filename    :   CAPI_GL_DistortionRenderer.h
Content     :   Distortion renderer header for GL
Created     :   November 11, 2013
Authors     :   David Borel, Lee Cooper

Copyright   :   Copyright 2013 Oculus VR, Inc. All Rights reserved.

Use of this software is subject to the terms of the Oculus Inc license
agreement provided at the time of installation or download, or which
otherwise accompanies this software in either electronic or hard copy form.

************************************************************************************/

#include "CAPI_GL_DistortionRenderer.h"

#include "CAPI_GL_DistortionShaders.h"

#include "../../OVR_CAPI_GL.h"
#include "../../Kernel/OVR_Color.h"

namespace OVR { namespace CAPI { namespace GL {


// Distortion pixel shader lookup.
//  Bit 0: Chroma Correction
//  Bit 1: Timewarp

enum {
    DistortionVertexShaderBitMask = 3,
    DistortionVertexShaderCount   = DistortionVertexShaderBitMask + 1,
    DistortionPixelShaderBitMask  = 1,
    DistortionPixelShaderCount    = DistortionPixelShaderBitMask + 1
};

struct ShaderInfo
{
    const char* ShaderData;
    size_t ShaderSize;
    const ShaderBase::Uniform* ReflectionData;
    size_t ReflectionSize;
};

// Do add a new distortion shader use these macros (with or w/o reflection)
#define SI_NOREFL(shader) { shader, sizeof(shader), NULL, 0 }
#define SI_REFL__(shader) { shader, sizeof(shader), shader ## _refl, sizeof( shader ## _refl )/sizeof(*(shader ## _refl)) }


static ShaderInfo DistortionVertexShaderLookup[DistortionVertexShaderCount] =
{
    SI_REFL__(Distortion_vs),
    SI_REFL__(DistortionChroma_vs),
    SI_REFL__(DistortionTimewarp_vs),
    SI_REFL__(DistortionTimewarpChroma_vs)
};

static ShaderInfo DistortionPixelShaderLookup[DistortionPixelShaderCount] =
{
    SI_NOREFL(Distortion_fs),
    SI_NOREFL(DistortionChroma_fs)
};

void DistortionShaderBitIndexCheck()
{
    OVR_COMPILER_ASSERT(ovrDistortionCap_Chromatic == 1);
    OVR_COMPILER_ASSERT(ovrDistortionCap_TimeWarp  == 2);
}



struct DistortionVertex
{
    Vector2f ScreenPosNDC;
    Vector2f TanEyeAnglesR;
    Vector2f TanEyeAnglesG;
    Vector2f TanEyeAnglesB;
    Color    Col;
};


// Vertex type; same format is used for all shapes for simplicity.
// Shapes are built by adding vertices to Model.
struct LatencyVertex
{
    Vector3f  Pos;
    LatencyVertex (const Vector3f& p) : Pos(p) {}
};


//----------------------------------------------------------------------------
// ***** GL::DistortionRenderer

DistortionRenderer::DistortionRenderer(ovrHmd hmd, FrameTimeManager& timeManager,
                                       const HMDRenderState& renderState)
    : CAPI::DistortionRenderer(ovrRenderAPI_OpenGL, hmd, timeManager, renderState)
	, LatencyVAO(0)
{
	DistortionMeshVAOs[0] = 0;
	DistortionMeshVAOs[1] = 0;

    // Initialize render params.
    memset(&RParams, 0, sizeof(RParams));
}

DistortionRenderer::~DistortionRenderer()
{
    destroy();
}

// static
CAPI::DistortionRenderer* DistortionRenderer::Create(ovrHmd hmd,
                                                     FrameTimeManager& timeManager,
                                                     const HMDRenderState& renderState)
{
#if !defined(OVR_OS_MAC)
    InitGLExtensions();
#endif
    return new DistortionRenderer(hmd, timeManager, renderState);
}


bool DistortionRenderer::Initialize(const ovrRenderAPIConfig* apiConfig,
									unsigned distortionCaps)
{
	GfxState = *new GraphicsState();

	SaveGraphicsState();

    const ovrGLConfig* config = (const ovrGLConfig*)apiConfig;

    if (!config)
    {
        // Cleanup
        pEyeTextures[0].Clear();
        pEyeTextures[1].Clear();
        memset(&RParams, 0, sizeof(RParams));
        return true;
    }

	RParams.Multisample = config->OGL.Header.Multisample;
	RParams.RTSize      = config->OGL.Header.RTSize;
#if defined(OVR_OS_WIN32)
	RParams.Window      = (config->OGL.Window) ? config->OGL.Window : GetActiveWindow();
    RParams.DC          = config->OGL.DC;
#elif defined(OVR_OS_LINUX)
    if (config->OGL.Disp)
        RParams.Disp = config->OGL.Disp;
    if (!RParams.Disp)
        RParams.Disp = XOpenDisplay(NULL);
    if (!RParams.Disp)
    {
        OVR_DEBUG_LOG(("XOpenDisplay failed."));
        return false;
    }

    if (config->OGL.Win)
        RParams.Win         = config->OGL.Win;
    if (!RParams.Win)
    {
        RParams.Win = glXGetCurrentDrawable();
    }
    if (!RParams.Win)
    {
        OVR_DEBUG_LOG(("XGetInputFocus failed."));
        return false;
    }
#endif
	
    DistortionCaps = distortionCaps;
	
    DistortionMeshVAOs[0] = 0;
    DistortionMeshVAOs[1] = 0;

    LatencyVAO = 0;

    //DistortionWarper.SetVsync((hmdCaps & ovrHmdCap_NoVSync) ? false : true);

    pEyeTextures[0] = *new Texture(&RParams, 0, 0);
    pEyeTextures[1] = *new Texture(&RParams, 0, 0);

    initBuffersAndShaders();

	RestoreGraphicsState();

    return true;
}


void DistortionRenderer::SubmitEye(int eyeId, const ovrTexture* eyeTexture)
{
    // Doesn't do a lot in here??
	const ovrGLTexture* tex = (const ovrGLTexture*)eyeTexture;

	// Write in values
    eachEye[eyeId].texture = tex->OGL.TexId;

	if (tex)
	{
        // Its only at this point we discover what the viewport of the texture is.
	    // because presumably we allow users to realtime adjust the resolution.
        eachEye[eyeId].TextureSize    = tex->OGL.Header.TextureSize;
        eachEye[eyeId].RenderViewport = tex->OGL.Header.RenderViewport;

        const ovrEyeRenderDesc& erd = RState.EyeRenderDesc[eyeId];
    
        ovrHmd_GetRenderScaleAndOffset( erd.Fov,
                                        eachEye[eyeId].TextureSize, eachEye[eyeId].RenderViewport,
                                        eachEye[eyeId].UVScaleOffset );

		if (!(RState.DistortionCaps & ovrDistortionCap_FlipInput))
		{
			eachEye[eyeId].UVScaleOffset[0].y = -eachEye[eyeId].UVScaleOffset[0].y;
			eachEye[eyeId].UVScaleOffset[1].y = 1.0f - eachEye[eyeId].UVScaleOffset[1].y;
		}

        pEyeTextures[eyeId]->UpdatePlaceholderTexture(tex->OGL.TexId,
                                                      tex->OGL.Header.TextureSize);
	}
}

void DistortionRenderer::renderEndFrame()
{
    renderDistortion(pEyeTextures[0], pEyeTextures[1]);

    // TODO: Add rendering context to callback.
    if(RegisteredPostDistortionCallback)
       RegisteredPostDistortionCallback(NULL);

    if(LatencyTest2Active)
    {
        renderLatencyPixel(LatencyTest2DrawColor);
    }
}

void DistortionRenderer::EndFrame(bool swapBuffers)
{
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

    if(LatencyTestActive)
    {
        renderLatencyQuad(LatencyTestDrawColor);
    }

    if (swapBuffers)
    {
		bool useVsync = ((RState.EnabledHmdCaps & ovrHmdCap_NoVSync) == 0);
        int swapInterval = (useVsync) ? 1 : 0;
#if defined(OVR_OS_WIN32)
#ifndef NO_SCREEN_TEAR_HEALING
        if (TimeManager.ScreenTearingReaction())
        {
            swapInterval = 0;
            useVsync = false;
        }
#endif // NO_SCREEN_TEAR_HEALING
        if (wglGetSwapIntervalEXT() != swapInterval)
            wglSwapIntervalEXT(swapInterval);

        HDC dc = (RParams.DC != NULL) ? RParams.DC : GetDC(RParams.Window);
		BOOL success = SwapBuffers(dc);
        OVR_ASSERT(success);
        OVR_UNUSED(success);

        if (RParams.DC == NULL)
            ReleaseDC(RParams.Window, dc);
#elif defined(OVR_OS_MAC)
        CGLContextObj context = CGLGetCurrentContext();
        GLint currentSwapInterval = 0;
        CGLGetParameter(context, kCGLCPSwapInterval, &currentSwapInterval);
        if (currentSwapInterval != swapInterval)
            CGLSetParameter(context, kCGLCPSwapInterval, &swapInterval);
        
        CGLFlushDrawable(context);
#elif defined(OVR_OS_LINUX)
        static const char* extensions = glXQueryExtensionsString(RParams.Disp, 0);
        static bool supportsVSync = (extensions != NULL && strstr(extensions, "GLX_EXT_swap_control"));
        if (supportsVSync)
        {
            GLuint currentSwapInterval = 0;
            glXQueryDrawable(RParams.Disp, RParams.Win, GLX_SWAP_INTERVAL_EXT, &currentSwapInterval);
            if (currentSwapInterval != (GLuint)swapInterval)
                glXSwapIntervalEXT(RParams.Disp, RParams.Win, swapInterval);
        }

        glXSwapBuffers(RParams.Disp, RParams.Win);
#endif
        // Force GPU to flush the scene, resulting in the lowest possible latency.
        // It's critical that this flush is *after* present.
        // With the display driver this flush is obsolete and theoretically should
        // be a no-op.
        // Doesn't need to be done if running through the Oculus driver.
        if (RState.OurHMDInfo.InCompatibilityMode &&
            !(RState.DistortionCaps & ovrDistortionCap_ProfileNoTimewarpSpinWaits))
            WaitUntilGpuIdle();
    }
}

void DistortionRenderer::WaitUntilGpuIdle()
{
	glFinish();
}

double DistortionRenderer::FlushGpuAndWaitTillTime(double absTime)
{
    // because glFlush() is not strict enough certain GL drivers
    // we do a glFinish(), but before doing so, we make sure we're not
    // running late
    double initialTime = ovr_GetTimeInSeconds();
    if (initialTime >= absTime)
        return 0.0;

    glFinish();

    return WaitTillTime(absTime);
}


DistortionRenderer::GraphicsState::GraphicsState()
{
    GetGLVersionAndExtensions(GLVersionInfo);
}


void DistortionRenderer::GraphicsState::ApplyBool(GLenum Name, GLint Value, GLint index)
{
    if (GLVersionInfo.SupportsDrawBuffers && index != -1)
	{
		if (Value != 0)
			glEnablei(Name, index);
		else
			glDisablei(Name, index);
	}
	else
	{
		if (Value != 0)
			glEnable(Name);
		else
			glDisable(Name);
	}
}
    
    
void DistortionRenderer::GraphicsState::Save()
{
    glGetIntegerv(GL_VIEWPORT, Viewport);
    glGetFloatv(GL_COLOR_CLEAR_VALUE, ClearColor);
    glGetIntegerv(GL_DEPTH_TEST, &DepthTest);
    glGetIntegerv(GL_CULL_FACE, &CullFace);
	glGetIntegerv(GL_FRAMEBUFFER_SRGB, &SRGB);
    glGetIntegerv(GL_CURRENT_PROGRAM, &Program);
    glGetIntegerv(GL_ACTIVE_TEXTURE, &ActiveTexture);
    glGetIntegerv(GL_TEXTURE_BINDING_2D, &TextureBinding);
    if (GLVersionInfo.SupportsVAO)
    {
        glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &VertexArrayBinding);
    }
    else
    {
        glGetIntegerv(GL_ELEMENT_ARRAY_BUFFER_BINDING, &ElementArrayBufferBinding);
        glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &ArrayBufferBinding);
    }
    glGetIntegerv(GL_FRAMEBUFFER_BINDING, &FrameBufferBinding);
    if (GLVersionInfo.SupportsDrawBuffers)
    {
        glGetIntegeri_v(GL_BLEND, 0, &Blend);
        glGetIntegeri_v(GL_COLOR_WRITEMASK, 0, ColorWritemask);
    }
    else
    {
        glGetIntegerv(GL_BLEND, &Blend);
        glGetIntegerv(GL_COLOR_WRITEMASK, ColorWritemask);
    }
    glGetIntegerv(GL_DITHER, &Dither);
    glGetIntegerv(GL_RASTERIZER_DISCARD, &RasterizerDiscard);
    if ((GLVersionInfo.MajorVersion == 3 && GLVersionInfo.MinorVersion >= 2) || GLVersionInfo.MajorVersion >= 4)
    {
        glGetIntegerv(GL_SAMPLE_MASK, &SampleMask);
    }

	IsValid = true;
}

#ifdef OVR_OS_MAC
bool DistortionRenderer::GraphicsState::isAtLeastOpenGL3()
{
    return (((GLVersionInfo.MajorVersion * 100) + GLVersionInfo.MinorVersion) >= 302); // OpenGL 3.2 or later
}
#endif

void DistortionRenderer::GraphicsState::Restore()
{
	// Don't allow restore-before-save.
	if (!IsValid)
		return;

    glViewport(Viewport[0], Viewport[1], Viewport[2], Viewport[3]);
    glClearColor(ClearColor[0], ClearColor[1], ClearColor[2], ClearColor[3]);
    
    ApplyBool(GL_DEPTH_TEST, DepthTest);
    ApplyBool(GL_CULL_FACE, CullFace);
    ApplyBool(GL_FRAMEBUFFER_SRGB, SRGB);	
    
    glUseProgram(Program);
    glActiveTexture(ActiveTexture);
    glBindTexture(GL_TEXTURE_2D, TextureBinding);
    if (GLVersionInfo.SupportsVAO)
    {
#ifdef OVR_OS_MAC
        if (isAtLeastOpenGL3())
        {
            glBindVertexArray(VertexArrayBinding);
        }
        else
        {
            glBindVertexArrayAPPLE(VertexArrayBinding);
        }
#else
		glBindVertexArray(VertexArrayBinding);
#endif
    }
    else
    {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ElementArrayBufferBinding);
        glBindBuffer(GL_ARRAY_BUFFER, ArrayBufferBinding);
    }
    glBindFramebuffer(GL_FRAMEBUFFER, FrameBufferBinding);
    
	ApplyBool(GL_BLEND, Blend, 0);
    
    if (GLVersionInfo.SupportsDrawBuffers)
    {
        glColorMaski(0, (GLboolean)ColorWritemask[0], (GLboolean)ColorWritemask[1], (GLboolean)ColorWritemask[2], (GLboolean)ColorWritemask[3]);
    }
    else
    {
        glColorMask((GLboolean)ColorWritemask[0], (GLboolean)ColorWritemask[1], (GLboolean)ColorWritemask[2], (GLboolean)ColorWritemask[3]);
    }
    
    ApplyBool(GL_DITHER, Dither);
    ApplyBool(GL_RASTERIZER_DISCARD, RasterizerDiscard);
    if ((GLVersionInfo.MajorVersion == 3 && GLVersionInfo.MinorVersion >= 2) ||
        GLVersionInfo.MajorVersion >= 4)
    {
        ApplyBool(GL_SAMPLE_MASK, SampleMask);
    }
}


void DistortionRenderer::initBuffersAndShaders()
{
    for ( int eyeNum = 0; eyeNum < 2; eyeNum++ )
    {
        // Allocate & generate distortion mesh vertices.
        ovrDistortionMesh meshData;

//        double startT = ovr_GetTimeInSeconds();

        if (!ovrHmd_CreateDistortionMesh( HMD,
                                          RState.EyeRenderDesc[eyeNum].Eye,
                                          RState.EyeRenderDesc[eyeNum].Fov,
                                          RState.DistortionCaps,
                                          &meshData) )
        {
            OVR_ASSERT(false);
            continue;
        }

        // Now parse the vertex data and create a render ready vertex buffer from it
        DistortionVertex *   pVBVerts    = (DistortionVertex*)OVR_ALLOC ( sizeof(DistortionVertex) * meshData.VertexCount );
        DistortionVertex *   pCurVBVert  = pVBVerts;
        ovrDistortionVertex* pCurOvrVert = meshData.pVertexData;

        for ( unsigned vertNum = 0; vertNum < meshData.VertexCount; vertNum++ )
        {
            pCurVBVert->ScreenPosNDC.x = pCurOvrVert->ScreenPosNDC.x;
            pCurVBVert->ScreenPosNDC.y = pCurOvrVert->ScreenPosNDC.y;

            // Previous code here did this: pCurVBVert->TanEyeAnglesR = (*(Vector2f*)&pCurOvrVert->TanEyeAnglesR); However that's an usafe
            // cast of unrelated types which can result in undefined behavior by a conforming compiler. A safe equivalent is simply memcpy.
            static_assert(sizeof(OVR::Vector2f) == sizeof(ovrVector2f), "Mismatch of structs that are presumed binary equivalents.");
            memcpy(&pCurVBVert->TanEyeAnglesR, &pCurOvrVert->TanEyeAnglesR, sizeof(pCurVBVert->TanEyeAnglesR));
            memcpy(&pCurVBVert->TanEyeAnglesG, &pCurOvrVert->TanEyeAnglesG, sizeof(pCurVBVert->TanEyeAnglesG));
            memcpy(&pCurVBVert->TanEyeAnglesB, &pCurOvrVert->TanEyeAnglesB, sizeof(pCurVBVert->TanEyeAnglesB));

            // Convert [0.0f,1.0f] to [0,255]
			if (DistortionCaps & ovrDistortionCap_Vignette)
				pCurVBVert->Col.R = (uint8_t)( pCurOvrVert->VignetteFactor * 255.99f );
			else
				pCurVBVert->Col.R = 255;

            pCurVBVert->Col.G = pCurVBVert->Col.R;
            pCurVBVert->Col.B = pCurVBVert->Col.R;
            pCurVBVert->Col.A = (uint8_t)( pCurOvrVert->TimeWarpFactor * 255.99f );;
            pCurOvrVert++;
            pCurVBVert++;
        }

        DistortionMeshVBs[eyeNum] = *new Buffer(&RParams);
        DistortionMeshVBs[eyeNum]->Data ( Buffer_Vertex | Buffer_ReadOnly, pVBVerts, sizeof(DistortionVertex) * meshData.VertexCount );
        DistortionMeshIBs[eyeNum] = *new Buffer(&RParams);
        DistortionMeshIBs[eyeNum]->Data ( Buffer_Index | Buffer_ReadOnly, meshData.pIndexData, ( sizeof(int16_t) * meshData.IndexCount ) );

        OVR_FREE ( pVBVerts );
        ovrHmd_DestroyDistortionMesh( &meshData );
    }

    initShaders();
}

void DistortionRenderer::renderDistortion(Texture* leftEyeTexture, Texture* rightEyeTexture)
{
    GraphicsState* glState = (GraphicsState*)GfxState.GetPtr();

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    setViewport( Recti(0,0, RParams.RTSize.w, RParams.RTSize.h) );

	if (DistortionCaps & ovrDistortionCap_SRGB)
		glEnable(GL_FRAMEBUFFER_SRGB);
    else
        glDisable(GL_FRAMEBUFFER_SRGB);

	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);
    
    if (glState->GLVersionInfo.SupportsDrawBuffers)
    {
        glDisablei(GL_BLEND, 0);
        glColorMaski(0, GL_TRUE, GL_TRUE, GL_TRUE, GL_FALSE);
    }
    else
    {
        glDisable(GL_BLEND);
        glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_FALSE);
    }
    
    glDisable(GL_DITHER);
    glDisable(GL_RASTERIZER_DISCARD);
    if ((glState->GLVersionInfo.MajorVersion >= 3 && glState->GLVersionInfo.MinorVersion >= 2) || glState->GLVersionInfo.MajorVersion >= 4)
    {
        glDisable(GL_SAMPLE_MASK);
    }
        
	glClearColor(
		RState.ClearColor[0],
		RState.ClearColor[1],
		RState.ClearColor[2],
		RState.ClearColor[3] );

    glClear(GL_COLOR_BUFFER_BIT);

    for (int eyeNum = 0; eyeNum < 2; eyeNum++)
    {        
		ShaderFill distortionShaderFill(DistortionShader);
        distortionShaderFill.SetTexture(0, eyeNum == 0 ? leftEyeTexture : rightEyeTexture);

		DistortionShader->SetUniform2f("EyeToSourceUVScale",  eachEye[eyeNum].UVScaleOffset[0].x, eachEye[eyeNum].UVScaleOffset[0].y);
		DistortionShader->SetUniform2f("EyeToSourceUVOffset", eachEye[eyeNum].UVScaleOffset[1].x, eachEye[eyeNum].UVScaleOffset[1].y);
        
		if (DistortionCaps & ovrDistortionCap_TimeWarp)
		{                       
            ovrMatrix4f timeWarpMatrices[2];            
            ovrHmd_GetEyeTimewarpMatrices(HMD, (ovrEyeType)eyeNum,
                                          RState.EyeRenderPoses[eyeNum], timeWarpMatrices);

            // Feed identity like matrices in until we get proper timewarp calculation going on
			DistortionShader->SetUniform4x4f("EyeRotationStart", Matrix4f(timeWarpMatrices[0]).Transposed());
			DistortionShader->SetUniform4x4f("EyeRotationEnd",   Matrix4f(timeWarpMatrices[1]).Transposed());

            renderPrimitives(&distortionShaderFill, DistortionMeshVBs[eyeNum], DistortionMeshIBs[eyeNum],
                            0, (int)DistortionMeshIBs[eyeNum]->GetSize()/2, Prim_Triangles, &DistortionMeshVAOs[eyeNum], true);
		}
        else
        {
            renderPrimitives(&distortionShaderFill, DistortionMeshVBs[eyeNum], DistortionMeshIBs[eyeNum],
                            0, (int)DistortionMeshIBs[eyeNum]->GetSize()/2, Prim_Triangles, &DistortionMeshVAOs[eyeNum], true);
        }
    }
}

void DistortionRenderer::createDrawQuad()
{
    const int numQuadVerts = 4;
    LatencyTesterQuadVB = *new Buffer(&RParams);
    if(!LatencyTesterQuadVB)
    {
        return;
    }

    LatencyTesterQuadVB->Data(Buffer_Vertex, NULL, numQuadVerts * sizeof(LatencyVertex));
    LatencyVertex* vertices = (LatencyVertex*)LatencyTesterQuadVB->Map(0, numQuadVerts * sizeof(LatencyVertex), Map_Discard);
    if(!vertices)
    {
        OVR_ASSERT(false); // failed to lock vertex buffer
        return;
    }

    const float left   = -1.0f;
    const float top    = -1.0f;
    const float right  =  1.0f;
    const float bottom =  1.0f;

    vertices[0] = LatencyVertex(Vector3f(left,  top,    0.0f));
    vertices[1] = LatencyVertex(Vector3f(left,  bottom, 0.0f));
    vertices[2] = LatencyVertex(Vector3f(right, top,    0.0f));
    vertices[3] = LatencyVertex(Vector3f(right, bottom, 0.0f));

    LatencyTesterQuadVB->Unmap(vertices);
}

void DistortionRenderer::renderLatencyQuad(unsigned char* latencyTesterDrawColor)
{
    const int numQuadVerts = 4;

    if(!LatencyTesterQuadVB)
    {
        createDrawQuad();
    }
       
    Ptr<ShaderSet> quadShader = (DistortionCaps & ovrDistortionCap_SRGB) ? SimpleQuadGammaShader : SimpleQuadShader;
    ShaderFill quadFill(quadShader);
    //quadFill.SetInputLayout(SimpleQuadVertexIL);

    setViewport(Recti(0,0, RParams.RTSize.w, RParams.RTSize.h));

    quadShader->SetUniform2f("Scale", 0.3f, 0.3f);
    quadShader->SetUniform4f("Color", (float)latencyTesterDrawColor[0] / 255.99f,
                                      (float)latencyTesterDrawColor[0] / 255.99f,
                                      (float)latencyTesterDrawColor[0] / 255.99f,
                                      1.0f);

    for(int eyeNum = 0; eyeNum < 2; eyeNum++)
    {
        quadShader->SetUniform2f("PositionOffset", eyeNum == 0 ? -0.5f : 0.5f, 0.0f);    
        renderPrimitives(&quadFill, LatencyTesterQuadVB, NULL, 0, numQuadVerts, Prim_TriangleStrip, &LatencyVAO, false);
    }
}

void DistortionRenderer::renderLatencyPixel(unsigned char* latencyTesterPixelColor)
{
    const int numQuadVerts = 4;

    if(!LatencyTesterQuadVB)
    {
        createDrawQuad();
    }

    Ptr<ShaderSet> quadShader = (DistortionCaps & ovrDistortionCap_SRGB) ? SimpleQuadGammaShader : SimpleQuadShader;
    ShaderFill quadFill(quadShader);

    setViewport(Recti(0,0, RParams.RTSize.w, RParams.RTSize.h));

#ifdef OVR_BUILD_DEBUG
    quadShader->SetUniform4f("Color", (float)latencyTesterPixelColor[0] / 255.99f,
                                      (float)latencyTesterPixelColor[1] / 255.99f,
                                      (float)latencyTesterPixelColor[2] / 255.99f,
                                      1.0f);

    Vector2f scale(20.0f / RParams.RTSize.w, 20.0f / RParams.RTSize.h); 
#else
    quadShader->SetUniform4f("Color", (float)latencyTesterPixelColor[0] / 255.99f,
                                      (float)latencyTesterPixelColor[0] / 255.99f,
                                      (float)latencyTesterPixelColor[0] / 255.99f,
                                      1.0f);

    Vector2f scale(1.0f / RParams.RTSize.w, 1.0f / RParams.RTSize.h); 
#endif
    quadShader->SetUniform2f("Scale", scale.x, scale.y);
    quadShader->SetUniform2f("PositionOffset", 1.0f-scale.x, 1.0f-scale.y);
	renderPrimitives(&quadFill, LatencyTesterQuadVB, NULL, 0, numQuadVerts, Prim_TriangleStrip, &LatencyVAO, false);
}

void DistortionRenderer::renderPrimitives(
                          const ShaderFill* fill,
                          Buffer* vertices, Buffer* indices,
                          int offset, int count,
                          PrimitiveType rprim, GLuint* vao, bool isDistortionMesh)
{
    GraphicsState* glState = (GraphicsState*)GfxState.GetPtr();

    GLenum prim;
    switch (rprim)
    {
    case Prim_Triangles:
        prim = GL_TRIANGLES;
        break;
    case Prim_Lines:
        prim = GL_LINES;
        break;
    case Prim_TriangleStrip:
        prim = GL_TRIANGLE_STRIP;
        break;
    default:
        OVR_ASSERT(false);
        return;
    }

    fill->Set();
    
    GLuint prog = fill->GetShaders()->Prog;

	if (vao != NULL)
	{
		if (*vao != 0)
		{
#ifdef OVR_OS_MAC
            if (glState->isAtLeastOpenGL3())
            {
                glBindVertexArray(*vao);
            }
            else
            {
                glBindVertexArrayAPPLE(*vao);
            }
#else
			glBindVertexArray(*vao);
#endif

			if (isDistortionMesh)
				glDrawElements(prim, count, GL_UNSIGNED_SHORT, NULL);
			else
				glDrawArrays(prim, 0, count);

#ifdef OVR_OS_MAC
            if (glState->isAtLeastOpenGL3())
            {
                glBindVertexArray(*vao);
            }
            else
            {
                glBindVertexArrayAPPLE(0);
            }
#else
            glBindVertexArray(0);
#endif
		}
		else
		{
            if (glState->GLVersionInfo.SupportsVAO)
            {
#ifdef OVR_OS_MAC
                if (glState->isAtLeastOpenGL3())
                {
                    glGenVertexArrays(1, vao);
                    glBindVertexArray(*vao);
                }
                else
                {
                    glGenVertexArraysAPPLE(1, vao);
                    glBindVertexArrayAPPLE(*vao);
                }
#else
                glGenVertexArrays(1, vao);
                glBindVertexArray(*vao);
#endif
			}

			int attributeCount = (isDistortionMesh) ? 5 : 1;
			int* locs = new int[attributeCount];

			glBindBuffer(GL_ARRAY_BUFFER, ((Buffer*)vertices)->GLBuffer);

			if (isDistortionMesh)
			{
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ((Buffer*)indices)->GLBuffer);

				locs[0] = glGetAttribLocation(prog, "Position");
				locs[1] = glGetAttribLocation(prog, "Color");
				locs[2] = glGetAttribLocation(prog, "TexCoord0");
				locs[3] = glGetAttribLocation(prog, "TexCoord1");
				locs[4] = glGetAttribLocation(prog, "TexCoord2");

				glVertexAttribPointer(locs[0], 2, GL_FLOAT, false, sizeof(DistortionVertex), reinterpret_cast<char*>(offset)+offsetof(DistortionVertex, ScreenPosNDC));
				glVertexAttribPointer(locs[1], 4, GL_UNSIGNED_BYTE, true, sizeof(DistortionVertex), reinterpret_cast<char*>(offset)+offsetof(DistortionVertex, Col));
				glVertexAttribPointer(locs[2], 2, GL_FLOAT, false, sizeof(DistortionVertex), reinterpret_cast<char*>(offset)+offsetof(DistortionVertex, TanEyeAnglesR));
				glVertexAttribPointer(locs[3], 2, GL_FLOAT, false, sizeof(DistortionVertex), reinterpret_cast<char*>(offset)+offsetof(DistortionVertex, TanEyeAnglesG));
				glVertexAttribPointer(locs[4], 2, GL_FLOAT, false, sizeof(DistortionVertex), reinterpret_cast<char*>(offset)+offsetof(DistortionVertex, TanEyeAnglesB));
			}
			else
			{
				locs[0] = glGetAttribLocation(prog, "Position");

				glVertexAttribPointer(locs[0], 3, GL_FLOAT, false, sizeof(LatencyVertex), reinterpret_cast<char*>(offset)+offsetof(LatencyVertex, Pos));
			}

            for (int i = 0; i < attributeCount; ++i)
                glEnableVertexAttribArray(locs[i]);
            
			if (isDistortionMesh)
				glDrawElements(prim, count, GL_UNSIGNED_SHORT, NULL);
			else
				glDrawArrays(prim, 0, count);


            if (!glState->GLVersionInfo.SupportsVAO)
            {
				for (int i = 0; i < attributeCount; ++i)
                    glDisableVertexAttribArray(locs[i]);
            }

			delete[] locs;

            if (glState->GLVersionInfo.SupportsVAO)
            {
#ifdef OVR_OS_MAC
                if (glState->isAtLeastOpenGL3())
                {
                    glBindVertexArray(0);
                }
                else
                {
                    glBindVertexArrayAPPLE(0);
                }
#else
                glBindVertexArray(0);
#endif
            }
		}
	}
}

void DistortionRenderer::setViewport(const Recti& vp)
{
    glViewport(vp.x, vp.y, vp.w, vp.h);
}


void DistortionRenderer::initShaders()
{
    GraphicsState* glState = (GraphicsState*)GfxState.GetPtr();

    const char* shaderPrefix =
        (glState->GLVersionInfo.MajorVersion < 3 ||
         (glState->GLVersionInfo.MajorVersion == 3 && glState->GLVersionInfo.MinorVersion < 2)) ?
            glsl2Prefix : glsl3Prefix;

    {
		ShaderInfo vsInfo = DistortionVertexShaderLookup[DistortionVertexShaderBitMask & DistortionCaps];

		size_t vsSize = strlen(shaderPrefix)+vsInfo.ShaderSize;
		char* vsSource = new char[vsSize];
		OVR_strcpy(vsSource, vsSize, shaderPrefix);
		OVR_strcat(vsSource, vsSize, vsInfo.ShaderData);

        Ptr<GL::VertexShader> vs = *new GL::VertexShader(
            &RParams,
			(void*)vsSource, vsSize,
			vsInfo.ReflectionData, vsInfo.ReflectionSize);

        DistortionShader = *new ShaderSet;
        DistortionShader->SetShader(vs);

		delete[](vsSource);

		ShaderInfo psInfo = DistortionPixelShaderLookup[DistortionPixelShaderBitMask & DistortionCaps];

		size_t psSize = strlen(shaderPrefix)+psInfo.ShaderSize;
		char* psSource = new char[psSize];
		OVR_strcpy(psSource, psSize, shaderPrefix);
		OVR_strcat(psSource, psSize, psInfo.ShaderData);

        Ptr<GL::FragmentShader> ps  = *new GL::FragmentShader(
            &RParams,
			(void*)psSource, psSize,
			psInfo.ReflectionData, psInfo.ReflectionSize);

        DistortionShader->SetShader(ps);

		delete[](psSource);
    }
	{
		size_t vsSize = strlen(shaderPrefix)+sizeof(SimpleQuad_vs);
		char* vsSource = new char[vsSize];
		OVR_strcpy(vsSource, vsSize, shaderPrefix);
		OVR_strcat(vsSource, vsSize, SimpleQuad_vs);

        Ptr<GL::VertexShader> vs = *new GL::VertexShader(
            &RParams,
            (void*)vsSource, vsSize,
			SimpleQuad_vs_refl, sizeof(SimpleQuad_vs_refl) / sizeof(SimpleQuad_vs_refl[0]));

        SimpleQuadShader = *new ShaderSet;
		SimpleQuadShader->SetShader(vs);

		delete[](vsSource);

		size_t psSize = strlen(shaderPrefix)+sizeof(SimpleQuad_fs);
		char* psSource = new char[psSize];
		OVR_strcpy(psSource, psSize, shaderPrefix);
		OVR_strcat(psSource, psSize, SimpleQuad_fs);

        Ptr<GL::FragmentShader> ps  = *new GL::FragmentShader(
            &RParams,
            (void*)psSource, psSize,
            SimpleQuad_fs_refl, sizeof(SimpleQuad_fs_refl) / sizeof(SimpleQuad_fs_refl[0]));

		SimpleQuadShader->SetShader(ps);

		delete[](psSource);
    }
    {
        size_t vsSize = strlen(shaderPrefix)+sizeof(SimpleQuad_vs);
        char* vsSource = new char[vsSize];
        OVR_strcpy(vsSource, vsSize, shaderPrefix);
        OVR_strcat(vsSource, vsSize, SimpleQuad_vs);

        Ptr<GL::VertexShader> vs = *new GL::VertexShader(
            &RParams,
            (void*)vsSource, vsSize,
            SimpleQuad_vs_refl, sizeof(SimpleQuad_vs_refl) / sizeof(SimpleQuad_vs_refl[0]));

        SimpleQuadGammaShader = *new ShaderSet;
        SimpleQuadGammaShader->SetShader(vs);

        delete[](vsSource);

        size_t psSize = strlen(shaderPrefix)+sizeof(SimpleQuadGamma_fs);
        char* psSource = new char[psSize];
        OVR_strcpy(psSource, psSize, shaderPrefix);
        OVR_strcat(psSource, psSize, SimpleQuadGamma_fs);

        Ptr<GL::FragmentShader> ps  = *new GL::FragmentShader(
            &RParams,
            (void*)psSource, psSize,
            SimpleQuadGamma_fs_refl, sizeof(SimpleQuadGamma_fs_refl) / sizeof(SimpleQuadGamma_fs_refl[0]));

        SimpleQuadGammaShader->SetShader(ps);

        delete[](psSource);
    }
}


void DistortionRenderer::destroy()
{
	SaveGraphicsState();

    GraphicsState* glState = (GraphicsState*)GfxState.GetPtr();
    
	for(int eyeNum = 0; eyeNum < 2; eyeNum++)
	{
        if (glState->GLVersionInfo.SupportsVAO)
        {
            glDeleteVertexArrays(1, &DistortionMeshVAOs[eyeNum]);
        }

		DistortionMeshVAOs[eyeNum] = 0;

		DistortionMeshVBs[eyeNum].Clear();
		DistortionMeshIBs[eyeNum].Clear();
	}

	if (DistortionShader)
    {
        DistortionShader->UnsetShader(Shader_Vertex);
	    DistortionShader->UnsetShader(Shader_Pixel);
	    DistortionShader.Clear();
    }

    LatencyTesterQuadVB.Clear();

    if(LatencyVAO != 0)
    {
        glDeleteVertexArrays(1, &LatencyVAO);
	    LatencyVAO = 0;
    }

	RestoreGraphicsState();
}


}}} // OVR::CAPI::GL
