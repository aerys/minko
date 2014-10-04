/************************************************************************************

Filename    :   CAPI_GL_HSWDisplay.cpp
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


#include "CAPI_GL_HSWDisplay.h"
#include "CAPI_GL_DistortionShaders.h"
#include "../../OVR_CAPI_GL.h"
#include "../../Kernel/OVR_File.h"
#include "../../Kernel/OVR_Math.h"
#include "../../Kernel/OVR_Allocator.h"
#include "../../Kernel/OVR_Color.h"


OVR_DISABLE_MSVC_WARNING(4996) // "This function or variable may be unsafe..."


namespace OVR { namespace CAPI { 


// Loads the TGA data from the File as an array of width * height 32 bit Texture_RGBA values.
// Returned pointer must be freed with OVR_FREE.
uint8_t* LoadTextureTgaData(OVR::File* f, uint8_t alpha, int& width, int& height)
{
    // See http://www.fileformat.info/format/tga/egff.htm for format details.
    // TGA files are stored with little-endian data.
    uint8_t* pRGBA  = NULL;

    f->SeekToBegin();
    
    const int desclen = f->ReadUByte();
    const int palette = f->ReadUByte();
    OVR_UNUSED(palette);
    const int imgtype = f->ReadUByte();
    f->ReadUInt16(); // Skip bytes
    int palCount = f->ReadUInt16();
    int palSize = f->ReadUByte();
    f->ReadUInt16();
    f->ReadUInt16();
    width = f->ReadUInt16();
    height = f->ReadUInt16();
    int bpp = f->ReadUByte();
    f->ReadUByte();

    const int ImgTypeBGRAUncompressed = 2;
    const int ImgTypeBGRARLECompressed = 10;

    OVR_ASSERT(((imgtype == ImgTypeBGRAUncompressed) || (imgtype == ImgTypeBGRARLECompressed)) && ((bpp == 24) || (bpp == 32)));

    // imgType 2 is uncompressed true-color image.
    // imgType 10 is run-length encoded true-color image.
    if(((imgtype == ImgTypeBGRAUncompressed) || (imgtype == ImgTypeBGRARLECompressed)) && ((bpp == 24) || (bpp == 32)))
    {
        int imgsize = width * height * 4;
        pRGBA = (uint8_t*) OVR_ALLOC(imgsize);
        f->Skip(desclen);
        f->Skip(palCount * (palSize + 7) >> 3);
        int strideBytes = width * 4; // This is the number of bytes between successive rows.

        unsigned char buf[4] = { 0, 0, 0, alpha }; // If bpp is 24 then this alpha will be unmodified below.

        switch (imgtype)
        {
        case ImgTypeBGRAUncompressed:
            switch (bpp)
            {
            case 24:
            case 32:
                for (int y = 0; y < height; y++)
                {
                    for (int x = 0; x < width; x++)
                    {
                        f->Read(buf, bpp / 8); // Data is stored as B, G, R
                        pRGBA[y*strideBytes + x*4 + 0] = buf[2];
                        pRGBA[y*strideBytes + x*4 + 1] = buf[1];
                        pRGBA[y*strideBytes + x*4 + 2] = buf[0];
                        pRGBA[y*strideBytes + x*4 + 3] = buf[3];
                    }
                }
                break;
            }
            break;

        case ImgTypeBGRARLECompressed:
            switch (bpp)
            {
            case 24:
            case 32:
                for (int y = 0; y < height; y++) // RLE spans don't cross successive rows.
                {
                    int x = 0;

                    while(x < width)
                    {
                        uint8_t rleByte;
                        f->Read(&rleByte, 1);

                        if(rleByte & 0x80) // If the high byte is set then what follows are RLE bytes.
                        {
                            size_t rleCount = ((rleByte & 0x7f) + 1);
                            f->Read(buf, bpp / 8); // Data is stored as B, G, R, A

                            for (; rleCount; --rleCount, ++x)
                            {
                                pRGBA[y*strideBytes + x*4 + 0] = buf[2];
                                pRGBA[y*strideBytes + x*4 + 1] = buf[1];
                                pRGBA[y*strideBytes + x*4 + 2] = buf[0];
                                pRGBA[y*strideBytes + x*4 + 3] = buf[3];
                            }   
                        }
                        else // Else what follows are regular bytes of a count indicated by rleByte
                        {
                            for (size_t rleCount = (rleByte + 1); rleCount; --rleCount, ++x)
                            {
                                f->Read(buf, bpp / 8); // Data is stored as B, G, R, A
                                pRGBA[y*strideBytes + x*4 + 0] = buf[2];
                                pRGBA[y*strideBytes + x*4 + 1] = buf[1];
                                pRGBA[y*strideBytes + x*4 + 2] = buf[0];
                                pRGBA[y*strideBytes + x*4 + 3] = buf[3];
                            }
                        }
                    }
                }
                break;
            }
            break;
        }
    }

    return pRGBA;
} // LoadTextureTgaData



namespace GL {


// To do: This needs to be promoted to a central version, possibly in CAPI_HSWDisplay.h
struct HASWVertex
{
    Vector3f  Pos;
    Color     C;
    float     U, V;    

    HASWVertex(const Vector3f& p, const Color& c = Color(64,0,0,255), float u = 0, float v = 0)
        : Pos(p), C(c), U(u), V(v)
    {}

    HASWVertex(float x, float y, float z, const Color& c = Color(64,0,0,255), float u = 0, float v = 0) 
        : Pos(x,y,z), C(c), U(u), V(v)
    {}

    bool operator==(const HASWVertex& b) const
    {
        return (Pos == b.Pos) && (C == b.C) && (U == b.U) && (V == b.V);
    }
};



// This is a temporary function implementation, and it functionality needs to be implemented in a more generic way.
Texture* LoadTextureTga(RenderParams& rParams, int samplerMode, OVR::File* f, uint8_t alpha)
{
    OVR::CAPI::GL::Texture* pTexture = NULL;

    int width, height;
    const uint8_t* pRGBA = LoadTextureTgaData(f, alpha, width, height);

    if (pRGBA)
    {
        pTexture = new OVR::CAPI::GL::Texture(&rParams, width, height);

        // SetSampleMode forces the use of mipmaps through GL_LINEAR_MIPMAP_LINEAR.
        pTexture->SetSampleMode(samplerMode); // Calls glBindTexture internally.

        // We are intentionally not using mipmaps. We need to use this because Texture::SetSampleMode unilaterally uses GL_LINEAR_MIPMAP_LINEAR.
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);
        OVR_ASSERT(glGetError() == 0);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pRGBA);
        OVR_ASSERT(glGetError() == 0);

        // With OpenGL 4.2+ we can use this instead of glTexImage2D:
        // glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, width, height);
        // glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, pRGBA);

        OVR_FREE(const_cast<uint8_t*>(pRGBA));
    }

    return pTexture;
}


// Loads a texture from a memory image of a TGA file.
Texture* LoadTextureTga(RenderParams& rParams, int samplerMode, const uint8_t* pData, int dataSize, uint8_t alpha)
{
    MemoryFile memoryFile("", pData, dataSize);

    return LoadTextureTga(rParams, samplerMode, &memoryFile, alpha);
}




// The texture below may conceivably be shared between HSWDisplay instances. However,  
// beware that sharing may not be possible if two HMDs are using different locales  
// simultaneously. As of this writing it's not clear if that can occur in practice.

HSWDisplay::HSWDisplay(ovrRenderAPIType api, ovrHmd hmd, const HMDRenderState& renderState)
  : OVR::CAPI::HSWDisplay(api, hmd, renderState)
  , RenderParams()
  , FrameBuffer(0)
  , pTexture()
  , pShaderSet()
  , pVertexShader()
  , pFragmentShader()
  , pVB()
  , VAO(0)
  , VAOInitialized(false)
  , OrthoProjection()
{
}


bool HSWDisplay::Initialize(const ovrRenderAPIConfig* apiConfig)
{
    const ovrGLConfig* config = (const ovrGLConfig*)apiConfig;

    if(config)
    {
        // The following is essentially copied from CAPI_GL_DistortionRender.cpp's 
        // Initialize function. To do: Merge this to a central location.
        RenderParams.Multisample = config->OGL.Header.Multisample;
        RenderParams.RTSize      = config->OGL.Header.RTSize;

        #if defined(OVR_OS_WIN32)
            RenderParams.Window = (config->OGL.Window) ? config->OGL.Window : GetActiveWindow();
            RenderParams.DC     = config->OGL.DC;
        #elif defined(OVR_OS_LINUX)
            if (config->OGL.Disp)
                RenderParams.Disp = config->OGL.Disp;
            if (!RenderParams.Disp)
                RenderParams.Disp = XOpenDisplay(NULL);
            if (!RenderParams.Disp)
            {
                OVR_DEBUG_LOG(("XOpenDisplay failed."));
                return false;
            }

            if (config->OGL.Win)
                RenderParams.Win= config->OGL.Win;
            if (!RenderParams.Win)
                RenderParams.Win = glXGetCurrentDrawable();

            if (!RenderParams.Win)
            {
                OVR_DEBUG_LOG(("XGetInputFocus failed."));
                return false;
            }
        #endif
    }
    else
    {
        UnloadGraphics();
    }

    return true;
}


void HSWDisplay::Shutdown()
{
    UnloadGraphics();
}


void HSWDisplay::DisplayInternal()
{
    HSWDISPLAY_LOG(("[HSWDisplay GL] DisplayInternal()"));
    // We may want to call LoadGraphics here instead of within Render.
}


void HSWDisplay::DismissInternal()
{
    HSWDISPLAY_LOG(("[HSWDisplay GL] DismissInternal()"));
    UnloadGraphics();
}


void HSWDisplay::UnloadGraphics()
{
    // RenderParams: No need to clear.
    if(FrameBuffer != 0)
    {
        glDeleteFramebuffers(1, &FrameBuffer);
        FrameBuffer = 0;
    }
    pTexture.Clear();
    pShaderSet.Clear();
    pVertexShader.Clear();
    pFragmentShader.Clear();
    pVB.Clear();
    if(VAO)
    {
        #ifdef OVR_OS_MAC
            if(GLVersionInfo.WholeVersion >= 302)
                glDeleteVertexArrays(1, &VAO);
            else
                glDeleteVertexArraysAPPLE(1, &VAO);
        #else
            glDeleteVertexArrays(1, &VAO);
        #endif
        
        VAO = 0;
        VAOInitialized = false;
    }
    // OrthoProjection: No need to clear.
}


void HSWDisplay::LoadGraphics()
{
    if (FrameBuffer == 0)
    {
        glGenFramebuffers(1, &FrameBuffer);
    }

    if (!pTexture) // To do: Add support for .dds files, which would be significantly smaller than the size of the tga.
    {
        size_t textureSize;
        const uint8_t* TextureData = GetDefaultTexture(textureSize);
        pTexture = *LoadTextureTga(RenderParams, Sample_Linear | Sample_Clamp, TextureData, (int)textureSize, 255);
    }

    if (!pShaderSet)
    {
        pShaderSet = *new ShaderSet();
    }

    if(!pVertexShader)
    {
        OVR::String strShader((GLVersionInfo.MajorVersion >= 3) ? glsl3Prefix : glsl2Prefix);
        strShader += SimpleTexturedQuad_vs;

        pVertexShader = *new VertexShader(&RenderParams, const_cast<char*>(strShader.ToCStr()), strShader.GetLength(), SimpleTexturedQuad_vs_refl, OVR_ARRAY_COUNT(SimpleTexturedQuad_vs_refl));
        pShaderSet->SetShader(pVertexShader);
    }

    if(!pFragmentShader)
    {
        OVR::String strShader((GLVersionInfo.MajorVersion >= 3) ? glsl3Prefix : glsl2Prefix);
        strShader += SimpleTexturedQuad_ps;

        pFragmentShader = *new FragmentShader(&RenderParams, const_cast<char*>(strShader.ToCStr()), strShader.GetLength(), SimpleTexturedQuad_ps_refl, OVR_ARRAY_COUNT(SimpleTexturedQuad_ps_refl));
        pShaderSet->SetShader(pFragmentShader);
    }

    if(!pVB)
    {
        pVB = *new Buffer(&RenderParams);

        pVB->Data(Buffer_Vertex, NULL, 4 * sizeof(HASWVertex));
        HASWVertex* pVertices = (HASWVertex*)pVB->Map(0, 4 * sizeof(HASWVertex), Map_Discard);
        OVR_ASSERT(pVertices);

        if(pVertices)
        {
            const bool  flip   = ((RenderState.DistortionCaps & ovrDistortionCap_FlipInput) != 0);
            const float left   = -1.0f; // We currently draw this in normalized device coordinates with an stereo translation
            const float top    = -1.1f; // applied as a vertex shader uniform. In the future when we have a more formal graphics
            const float right  =  1.0f; // API abstraction we may move this draw to an overlay layer or to a more formal 
            const float bottom =  0.9f; // model/mesh scheme with a perspective projection.

            pVertices[0] = HASWVertex(left,  top,    0.f, Color(255, 255, 255, 255), 0.f, flip ? 1.f : 0.f);
            pVertices[1] = HASWVertex(left,  bottom, 0.f, Color(255, 255, 255, 255), 0.f, flip ? 0.f : 1.f);
            pVertices[2] = HASWVertex(right, top,    0.f, Color(255, 255, 255, 255), 1.f, flip ? 1.f : 0.f); 
            pVertices[3] = HASWVertex(right, bottom, 0.f, Color(255, 255, 255, 255), 1.f, flip ? 0.f : 1.f);

            pVB->Unmap(pVertices);
        }
    }

    // We don't generate the vertex arrays here
    if (!VAO && GLVersionInfo.SupportsVAO)
    {
        OVR_ASSERT(!VAOInitialized);
        
        #ifdef OVR_OS_MAC
            if(GLVersionInfo.WholeVersion >= 302)
                glGenVertexArrays(1, &VAO);
            else
                glGenVertexArraysAPPLE(1, &VAO);
        #else
            glGenVertexArrays(1, &VAO);
        #endif
    }
}


void HSWDisplay::RenderInternal(ovrEyeType eye, const ovrTexture* eyeTexture)
{
    if(RenderEnabled && eyeTexture)
    {
        // glGetError clears any previous error state. We call it here in order to start with
        // a clean slate, as we are asserting below that our calls do not generate errors.
        glGetError();
        
        if(GLVersionInfo.MajorVersion == 0) // If not yet initialized...
            GetGLVersionAndExtensions(GLVersionInfo);

        // We need to render to the eyeTexture with the texture viewport.
        // Setup rendering to the texture.
        ovrGLTexture* eyeTextureGL = const_cast<ovrGLTexture*>(reinterpret_cast<const ovrGLTexture*>(eyeTexture));
        OVR_ASSERT(eyeTextureGL->Texture.Header.API == ovrRenderAPI_OpenGL);

        const GLuint kVertexAttribCount = 3;
        const GLuint kSavedVertexAttribCount = 8;

        // Save state
        // To do: Converge this with the state setting/restoring functionality present in the distortion renderer.
        // Consider usage of the EXT_direct_state_access (http://www.opengl.org/registry/specs/EXT/direct_state_access.txt) extension.
        // Note that the glGet functions below will block until command buffer has completed.
        // glPushAttrib is deprecated, so we use glGet* to save/restore fixed-function settings.
        // https://www.opengl.org/sdk/docs/man/docbook4/xhtml/glGet.xml
        //
        GLint RenderModeSaved = 0;
        if(!GLVersionInfo.IsCoreProfile) // glGetIntegerv can fail if an OpenGL 3.x+ core profile is enabled due to GL_RENDER_MODE being no longer supported.
        {
            glGetIntegerv(GL_RENDER_MODE, &RenderModeSaved);
            OVR_ASSERT(glGetError() == 0);
            OVR_ASSERT(RenderModeSaved == GL_RENDER); // Make sure it's not GL_SELECT or GL_FEEDBACK.
        }
        
        GLint FrameBufferBindingSaved; // OpenGL renamed GL_FRAMEBUFFER_BINDING to GL_DRAW_FRAMEBUFFER_BINDING and adds GL_READ_FRAMEBUFFER_BINDING.
        glGetIntegerv(GL_FRAMEBUFFER_BINDING, &FrameBufferBindingSaved);
        OVR_ASSERT(glGetError() == 0);

        GLint TextureBinding2DSaved;
        glGetIntegerv(GL_TEXTURE_BINDING_2D, &TextureBinding2DSaved);
        OVR_ASSERT(glGetError() == 0);

        GLint ViewportSaved[4];
        glGetIntegerv(GL_VIEWPORT, ViewportSaved);
        OVR_ASSERT(glGetError() == 0);

        GLint PolygonModeSaved[2]; // Will be two (e.g. GL_FILL) values, one for the front mode and one for the mode.
        glGetIntegerv(GL_POLYGON_MODE, PolygonModeSaved);
        OVR_ASSERT(glGetError() == 0);

        GLdouble DepthRangeSaved[2];
        #if defined(OVR_OS_MAC) || defined(OVR_OS_LINUX)
            // Using glDepthRange as a conditional will always evaluate to true on Mac/Linux.
            glGetDoublev(GL_DEPTH_RANGE, DepthRangeSaved);
        #else
            GLfloat DepthRangefSaved[2];
            if(glDepthRange) // If we can use the double version (glDepthRangef may not be available)...
                glGetDoublev(GL_DEPTH_RANGE, DepthRangeSaved);
            else
                glGetFloatv(GL_DEPTH_RANGE, DepthRangefSaved);
        #endif
        OVR_ASSERT(glGetError() == 0);

        GLint DepthWriteMaskSaved;
        glGetIntegerv(GL_DEPTH_WRITEMASK, &DepthWriteMaskSaved);
        OVR_ASSERT(glGetError() == 0);

        GLint DepthTestSaved;
        glGetIntegerv(GL_DEPTH_TEST, &DepthTestSaved);
        OVR_ASSERT(glGetError() == 0);

        // No need to save/restore depth offset because we are neither testing nor writing depth.

        GLint CullFaceSaved;
        glGetIntegerv(GL_CULL_FACE, &CullFaceSaved);
        OVR_ASSERT(glGetError() == 0);

        GLint FrontFaceSaved;
        glGetIntegerv(GL_FRONT_FACE, &FrontFaceSaved);
        OVR_ASSERT(glGetError() == 0);

        GLint BlendSaved;
        glGetIntegerv(GL_BLEND, &BlendSaved);
        OVR_ASSERT(glGetError() == 0);

        GLint BlendSrcRGBSaved, BlendSrcAlphaSaved, BlendDstRGBSaved, BlendDstAlphaSaved;
        glGetIntegerv(GL_BLEND_SRC_RGB,   &BlendSrcRGBSaved);
        glGetIntegerv(GL_BLEND_SRC_ALPHA, &BlendSrcAlphaSaved);
        glGetIntegerv(GL_BLEND_DST_RGB,   &BlendDstRGBSaved);
        glGetIntegerv(GL_BLEND_DST_ALPHA, &BlendDstAlphaSaved);

        GLint DitherSaved;
        glGetIntegerv(GL_DITHER, &DitherSaved);
        OVR_ASSERT(glGetError() == 0);

        GLint RasterizerDiscardSaved;
        glGetIntegerv(GL_RASTERIZER_DISCARD, &RasterizerDiscardSaved);
        OVR_ASSERT(glGetError() == 0);

        GLint ScissorTestSaved;
        glGetIntegerv(GL_SCISSOR_TEST, &ScissorTestSaved);
        OVR_ASSERT(glGetError() == 0);

        GLint SampleMaskSaved = 0;
        if (GLVersionInfo.WholeVersion >= 302) // OpenGL 3.2 or later
        {
            glGetIntegerv(GL_SAMPLE_MASK, &SampleMaskSaved);
            OVR_ASSERT(glGetError() == 0);
        }

        GLint ColorWriteMaskSaved[4];
        glGetIntegerv(GL_COLOR_WRITEMASK, ColorWriteMaskSaved);

        GLint ArrayBufferBindingSaved;
        glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &ArrayBufferBindingSaved);
        OVR_ASSERT(glGetError() == 0);

        GLint ProgramSaved;
        glGetIntegerv(GL_CURRENT_PROGRAM, &ProgramSaved);
        OVR_ASSERT(glGetError() == 0);

        GLint ActiveTextureSaved;
        glGetIntegerv(GL_ACTIVE_TEXTURE, &ActiveTextureSaved);
        OVR_ASSERT(glGetError() == 0);

        GLint TextureBindingSaved;
        glGetIntegerv(GL_TEXTURE_BINDING_2D, &TextureBindingSaved);
        OVR_ASSERT(glGetError() == 0);

        GLint VertexArrayBindingSaved = 0;
        if (GLVersionInfo.SupportsVAO)
            glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &VertexArrayBindingSaved);

        // If the core profile is enabled then we can't get the vertex attributes unless there is
        // an active VAO. Otherwise there can be an error with some OpenGL implementations (notably Apple's).
        // If the core profile is not enabled then pre-OpenGL 3.0 behavior is possible in which the
        // application may not be using VAOs and thus there may be active vertex attributes.
        GLint   VertexAttribEnabledSaved[kSavedVertexAttribCount];
        GLint   VertexAttribSizeSaved[kSavedVertexAttribCount];
        GLint   VertexAttribTypeSaved[kSavedVertexAttribCount];
        GLint   VertexAttribNormalizedSaved[kSavedVertexAttribCount];
        GLint   VertexAttribStrideSaved[kSavedVertexAttribCount];
        GLvoid* VertexAttribPointerSaved[kSavedVertexAttribCount];

        if(VertexArrayBindingSaved || !GLVersionInfo.IsCoreProfile)
        {
            for(GLuint i = 0; i < kSavedVertexAttribCount; i++)
            {
                // https://www.opengl.org/sdk/docs/man/docbook4/xhtml/glVertexAttribPointer.xml
                glGetVertexAttribiv(i, GL_VERTEX_ATTRIB_ARRAY_ENABLED, &VertexAttribEnabledSaved[i]);

                glGetVertexAttribiv(i,       GL_VERTEX_ATTRIB_ARRAY_SIZE,       &VertexAttribSizeSaved[i]);
                glGetVertexAttribiv(i,       GL_VERTEX_ATTRIB_ARRAY_TYPE,       &VertexAttribTypeSaved[i]);
                glGetVertexAttribiv(i,       GL_VERTEX_ATTRIB_ARRAY_NORMALIZED, &VertexAttribNormalizedSaved[i]);
                glGetVertexAttribiv(i,       GL_VERTEX_ATTRIB_ARRAY_STRIDE,     &VertexAttribStrideSaved[i]);
                glGetVertexAttribPointerv(i, GL_VERTEX_ATTRIB_ARRAY_POINTER,    &VertexAttribPointerSaved[i]); 

                OVR_ASSERT(glGetError() == 0);
            }
        }
        //
        // End of save state


        // Load the graphics if not loaded already.
        if (!pTexture)
        {
            LoadGraphics();
        }

        // Calculate ortho projection.
        GetOrthoProjection(RenderState, OrthoProjection);

        // Set the rendering to be to the eye texture.
        glBindFramebuffer(GL_FRAMEBUFFER, FrameBuffer);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, eyeTextureGL->OGL.TexId, 0);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, 0); // We aren't using depth, as we currently want this to overwrite everything.
        // GLenum DrawBuffers[1] = { GL_COLOR_ATTACHMENT0 };
        // glDrawBuffers(OVR_ARRAY_COUNT(DrawBuffers), DrawBuffers);
        GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
        OVR_ASSERT(status == GL_FRAMEBUFFER_COMPLETE); OVR_UNUSED(status);
        OVR_ASSERT(glGetError() == 0);

        // Set up the viewport
        const GLint   x = (GLint)eyeTextureGL->Texture.Header.RenderViewport.Pos.x;
        const GLint   y = (GLint)eyeTextureGL->Texture.Header.RenderViewport.Pos.y; // Note that GL uses bottom-up coordinates.
        const GLsizei w = (GLsizei)eyeTextureGL->Texture.Header.RenderViewport.Size.w;
        const GLsizei h = (GLsizei)eyeTextureGL->Texture.Header.RenderViewport.Size.h;
        glViewport(x, y, w, h);
        OVR_ASSERT(glGetError() == 0);

        // Set fixed-function render states
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        OVR_ASSERT(glGetError() == 0);
        #if defined(OVR_OS_MAC) || defined(OVR_OS_LINUX) // On Mac/Linux we are directly using OpenGL functions instead of function pointers.
            glDepthRange(0.0,  1.0);
        #else
            if(glDepthRange) // If we can use the double version (glDepthRangef isn't available with older OpenGL, glDepthRange isn't available with OpenGL ES)...
                glDepthRange(0.0,  1.0);
            else
                glDepthRangef(0.f,  1.f);
        #endif
        glDepthMask(GL_FALSE);
        glDisable(GL_DEPTH_TEST); // Disabling depth test should also have the effect of glDepthMask(GL_FALSE).
        glDisable(GL_CULL_FACE);
        glFrontFace(GL_CW);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glDisable(GL_DITHER);
        glDisable(GL_RASTERIZER_DISCARD);
        glDisable(GL_SCISSOR_TEST);
        if (GLVersionInfo.WholeVersion >= 302) // OpenGL 3.2 or later
        {
            glDisable(GL_SAMPLE_MASK);
        }
        glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_FALSE);
        OVR_ASSERT(glGetError() == 0);

        // Enable the buffer and shaders we use.
        ShaderFill fill(pShaderSet);
        if(pTexture)
        {
            fill.SetTexture(0, pTexture);
            OVR_ASSERT(glGetError() == 0);
        }

        // Set shader uniforms.
        const float scale  = HSWDISPLAY_SCALE * ((RenderState.OurHMDInfo.HmdType == HmdType_DK1) ? 0.70f : 1.f);
        pShaderSet->SetUniform2f("Scale", scale, scale / 2.f); // X and Y scale. Y is a fixed proportion to X in order to give a certain aspect ratio.
        pShaderSet->SetUniform2f("PositionOffset", OrthoProjection[eye].GetTranslation().x, 0.0f);
        OVR_ASSERT(glGetError() == 0);

        // Set vertex attributes
        // To do: We must add support for vertext array objects (VAOs) here. When using an OpenGL 3.2+ core profile, 
        // the application is required to use vertex array objects and glVertexAttribPointer will fail otherwise.

        if (GLVersionInfo.SupportsVAO)
        {
            OVR_ASSERT(VAO != 0);
            
            #ifdef OVR_OS_MAC
                if(GLVersionInfo.WholeVersion >= 302)
                    glBindVertexArray(VAO);
                else
            	    glBindVertexArrayAPPLE(VAO);
            #else
		    	glBindVertexArray(VAO);
            #endif
        }

        if(!VAOInitialized) // This executes for the case that VAO isn't supported.
        {
            glBindBuffer(GL_ARRAY_BUFFER, pVB->GLBuffer); // This must be called before glVertexAttribPointer is called below.
            OVR_ASSERT(glGetError() == 0);

            const GLuint shaderProgram = pShaderSet->Prog;
            GLint attributeLocationArray[kVertexAttribCount];

            attributeLocationArray[0] = glGetAttribLocation(shaderProgram, "Position");
            glVertexAttribPointer(attributeLocationArray[0], sizeof(Vector3f)/sizeof(float), GL_FLOAT,         false, sizeof(HASWVertex), reinterpret_cast<char*>(offsetof(HASWVertex, Pos)));

            attributeLocationArray[1] = glGetAttribLocation(shaderProgram, "Color");
            glVertexAttribPointer(attributeLocationArray[1], sizeof(Color)/sizeof(uint8_t),  GL_UNSIGNED_BYTE,  true, sizeof(HASWVertex), reinterpret_cast<char*>(offsetof(HASWVertex, C)));  // True because we want it to convert [0,255] to [0,1] for us.

            attributeLocationArray[2] = glGetAttribLocation(shaderProgram, "TexCoord");
            glVertexAttribPointer(attributeLocationArray[2], sizeof(float[2])/sizeof(float), GL_FLOAT,         false, sizeof(HASWVertex), reinterpret_cast<char*>(offsetof(HASWVertex, U)));
            OVR_ASSERT(glGetError() == 0);

            for (size_t i = 0; i < kVertexAttribCount; i++)
                glEnableVertexAttribArray((GLuint)i);
            OVR_ASSERT(glGetError() == 0);

            for (size_t i = kVertexAttribCount; i < kSavedVertexAttribCount; i++)
                glDisableVertexAttribArray((GLuint)i);
            OVR_ASSERT(glGetError() == 0);
        }

        fill.Set(Prim_TriangleStrip);
        OVR_ASSERT(glGetError() == 0);

        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        OVR_ASSERT(glGetError() == 0);

        if (GLVersionInfo.SupportsVAO)
        {
            VAOInitialized = true;

            #ifdef OVR_OS_MAC
                if(GLVersionInfo.WholeVersion >= 302)
                    glBindVertexArray(0);
                else
            	    glBindVertexArrayAPPLE(0);
            #else
		    	glBindVertexArray(0);
            #endif
        }

        // Restore state
        // We restore the state in the reverse order that we saved it.
        // To do: Make the code below avoid changes that are effectively no-ops.
        //
        if (GLVersionInfo.SupportsVAO)
        {
            #ifdef OVR_OS_MAC
                if(GLVersionInfo.WholeVersion >= 302)
                    glBindVertexArray(VertexArrayBindingSaved);
                else
                    glBindVertexArrayAPPLE(VertexArrayBindingSaved);
            #else
                glBindVertexArray(VertexArrayBindingSaved);
            #endif
        }

        if(VertexArrayBindingSaved || !GLVersionInfo.IsCoreProfile) // If the OpenGL version is older or in core profile compatibility mode, or if there's a VAO currently installed...
        {
            for (GLuint i = 0; i < kSavedVertexAttribCount; i++)
            {
                glVertexAttribPointer(i, VertexAttribSizeSaved[i], VertexAttribTypeSaved[i], (GLboolean)VertexAttribNormalizedSaved[i], VertexAttribStrideSaved[i], VertexAttribPointerSaved[i]);

                if(VertexAttribEnabledSaved[i])
                    glEnableVertexAttribArray(i);
                else
                    glDisableVertexAttribArray(i);

                OVR_ASSERT(glGetError() == 0);
            }
        }
    
        glBindTexture(GL_TEXTURE_2D, TextureBindingSaved);
        glActiveTexture(ActiveTextureSaved);
        glUseProgram(ProgramSaved);
        glBindBuffer(GL_ARRAY_BUFFER, ArrayBufferBindingSaved);
        glColorMask((GLboolean)ColorWriteMaskSaved[0], (GLboolean)ColorWriteMaskSaved[1], (GLboolean)ColorWriteMaskSaved[2], (GLboolean)ColorWriteMaskSaved[3]);

        if (GLVersionInfo.WholeVersion >= 302) // OpenGL 3.2 or later
        {
            if(SampleMaskSaved)
                glEnable(GL_SAMPLE_MASK);
            else
                glDisable(GL_SAMPLE_MASK);
        }

        if(ScissorTestSaved)
            glEnable(GL_SCISSOR_TEST);
        else
            glDisable(GL_SCISSOR_TEST);

        if(RasterizerDiscardSaved)
            glEnable(GL_RASTERIZER_DISCARD);
        else
            glDisable(GL_RASTERIZER_DISCARD);

        if(DitherSaved)
            glEnable(GL_DITHER);
        else
            glDisable(GL_DITHER);

        // With OpenGL 4.0+, we may need to be aware of glBlendFuncSeparatei.
        glBlendFuncSeparate(BlendSrcRGBSaved, BlendDstRGBSaved, BlendSrcAlphaSaved, BlendDstAlphaSaved);

        if(BlendSaved)
            glEnable(GL_BLEND);
        else
            glDisable(GL_BLEND);

        glFrontFace(FrontFaceSaved);

        if(CullFaceSaved)
            glEnable(GL_CULL_FACE);
        else
            glDisable(GL_CULL_FACE);

        if(DepthTestSaved)
            glEnable(GL_DEPTH_TEST);
        else
            glDisable(GL_DEPTH_TEST);

        glDepthMask(DepthWriteMaskSaved ? GL_TRUE : GL_FALSE);  
        #if defined(OVR_OS_MAC) || defined(OVR_OS_LINUX) // On Mac/Linux we are directly using OpenGL functions instead of function pointers.
            glDepthRange(DepthRangeSaved[0], DepthRangeSaved[1]);
        #else
            if(glDepthRange) // If we can use the double version (glDepthRangef may not be available)...
                glDepthRange(DepthRangeSaved[0], DepthRangeSaved[1]);
            else
                glDepthRangef(DepthRangefSaved[0], DepthRangefSaved[1]);
        #endif
        // For OpenGL 3.x+ core profile mode, glPolygonMode allows only GL_FRONT_AND_BACK and not separate GL_FRONT and GL_BACK.
        glPolygonMode(GL_FRONT_AND_BACK, PolygonModeSaved[0]);
        glViewport(ViewportSaved[0], ViewportSaved[1], ViewportSaved[2], ViewportSaved[3]);
        glBindTexture(GL_TEXTURE_2D, TextureBinding2DSaved);
        glBindFramebuffer(GL_FRAMEBUFFER, FrameBufferBindingSaved);
        
        if(!GLVersionInfo.IsCoreProfile)
            glRenderMode(RenderModeSaved);

        OVR_ASSERT(glGetError() == 0);
        //
        // End of restore state
    }
}

 
}}} // namespace OVR::CAPI::GL
