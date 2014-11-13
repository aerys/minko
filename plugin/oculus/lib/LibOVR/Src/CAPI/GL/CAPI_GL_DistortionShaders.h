/************************************************************************************
 
 Filename    :   CAPI_GL_Shaders.h
 Content     :   Distortion shader header for GL
 Created     :   November 11, 2013
 Authors     :   David Borel, Volga Aksoy
 
 Copyright   :   Copyright 2013 Oculus VR, Inc. All Rights reserved.
 
 Use of this software is subject to the terms of the Oculus Inc license
 agreement provided at the time of installation or download, or which
 otherwise accompanies this software in either electronic or hard copy form.
 
 ************************************************************************************/


#ifndef OVR_CAPI_GL_Shaders_h
#define OVR_CAPI_GL_Shaders_h


#include "CAPI_GL_Util.h"

namespace OVR { namespace CAPI { namespace GL {
    
    static const char glsl2Prefix[] =
    "#version 110\n"
    "#extension GL_ARB_shader_texture_lod : enable\n"
    "#define _FRAGCOLOR_DECLARATION\n"
    "#define _VS_IN attribute\n"
    "#define _VS_OUT varying\n"
    "#define _FS_IN varying\n"
    "#define _TEXTURELOD texture2DLod\n"
    "#define _TEXTURE texture2D\n"
    "#define _FRAGCOLOR gl_FragColor\n";
    
    static const char glsl3Prefix[] =
    "#version 150\n"
    "#define _FRAGCOLOR_DECLARATION out vec4 FragColor;\n"
    "#define _VS_IN in\n"
    "#define _VS_OUT out\n"
    "#define _FS_IN in\n"
    "#define _TEXTURELOD textureLod\n"
    "#define _TEXTURE texture\n"
    "#define _FRAGCOLOR FragColor\n";
    
    static const char SimpleQuad_vs[] =
    "uniform vec2 PositionOffset;\n"
    "uniform vec2 Scale;\n"
    
    "_VS_IN vec3 Position;\n"
    
	"void main()\n"
	"{\n"
	"	gl_Position = vec4(Position.xy * Scale + PositionOffset, 0.5, 1.0);\n"
	"}\n";
    
    const OVR::CAPI::GL::ShaderBase::Uniform SimpleQuad_vs_refl[] =
    {
        { "PositionOffset", OVR::CAPI::GL::ShaderBase::VARTYPE_FLOAT, 0, 8 },
        { "Scale",          OVR::CAPI::GL::ShaderBase::VARTYPE_FLOAT, 8, 8 },
    };
    
    static const char SimpleQuad_fs[] =
    "uniform vec4 Color;\n"
    
    "_FRAGCOLOR_DECLARATION\n"
    
	"void main()\n"
	"{\n"
	"    _FRAGCOLOR = Color;\n"
	"}\n";
    
    const OVR::CAPI::GL::ShaderBase::Uniform SimpleQuad_fs_refl[] =
    {
        { "Color", OVR::CAPI::GL::ShaderBase::VARTYPE_FLOAT, 0, 16 },
    };

    static const char SimpleQuadGamma_fs[] =
        "uniform vec4 Color;\n"

        "_FRAGCOLOR_DECLARATION\n"

        "void main()\n"
        "{\n"
        "    _FRAGCOLOR.rgb = pow(Color.rgb, vec3(2.2));\n"
        "    _FRAGCOLOR.a = Color.a;\n"
        "}\n";

    const OVR::CAPI::GL::ShaderBase::Uniform SimpleQuadGamma_fs_refl[] =
    {
        { "Color", OVR::CAPI::GL::ShaderBase::VARTYPE_FLOAT, 0, 16 },
    };

    // This must be prefixed with glsl2Prefix or glsl3Prefix before being compiled.
    static const char SimpleTexturedQuad_vs[] =
        "uniform vec2 PositionOffset;\n"
        "uniform vec2 Scale;\n"

        "_VS_IN vec3 Position;\n"
        "_VS_IN vec4 Color;\n"
        "_VS_IN vec2 TexCoord;\n"
  
        "_VS_OUT vec4 oColor;\n"
        "_VS_OUT vec2 oTexCoord;\n"

        "void main()\n"
        "{\n"
	    "	gl_Position = vec4(Position.xy * Scale + PositionOffset, 0.5, 1.0);\n"
        "   oColor = Color;\n"
        "   oTexCoord = TexCoord;\n"
        "}\n";

    // The following declaration is copied from the generated D3D SimpleTexturedQuad_vs_refl.h file, with D3D_NS renamed to GL.
    const OVR::CAPI::GL::ShaderBase::Uniform SimpleTexturedQuad_vs_refl[] =
    {
	    { "PositionOffset", OVR::CAPI::GL::ShaderBase::VARTYPE_FLOAT, 0, 8 },
	    { "Scale",          OVR::CAPI::GL::ShaderBase::VARTYPE_FLOAT, 8, 8 },
    };


    // This must be prefixed with glsl2Prefix or glsl3Prefix before being compiled.
    static const char SimpleTexturedQuad_ps[] =
        "uniform sampler2D Texture0;\n"
    
        "_FS_IN vec4 oColor;\n"
        "_FS_IN vec2 oTexCoord;\n"
    
        "_FRAGCOLOR_DECLARATION\n"

        "void main()\n"
        "{\n"
        "   _FRAGCOLOR = oColor * _TEXTURE(Texture0, oTexCoord);\n"
        "}\n";

    // The following is copied from the generated D3D SimpleTexturedQuad_ps_refl.h file, with D3D_NS renamed to GL.
    const OVR::CAPI::GL::ShaderBase::Uniform SimpleTexturedQuad_ps_refl[] =
    {
	    { "Color", 	OVR::CAPI::GL::ShaderBase::VARTYPE_FLOAT, 0, 16 },
    };

    
    static const char Distortion_vs[] =
    "uniform vec2 EyeToSourceUVScale;\n"
    "uniform vec2 EyeToSourceUVOffset;\n"
    
    "_VS_IN vec2 Position;\n"
    "_VS_IN vec4 Color;\n"
    "_VS_IN vec2 TexCoord0;\n"
    
    "_VS_OUT vec4 oColor;\n"
    "_VS_OUT vec2 oTexCoord0;\n"
    
    "void main()\n"
    "{\n"
    "   gl_Position.x = Position.x;\n"
    "   gl_Position.y = Position.y;\n"
    "   gl_Position.z = 0.5;\n"
    "   gl_Position.w = 1.0;\n"
    // Vertex inputs are in TanEyeAngle space for the R,G,B channels (i.e. after chromatic aberration and distortion).
    // Scale them into the correct [0-1],[0-1] UV lookup space (depending on eye)
    "   oTexCoord0 = TexCoord0 * EyeToSourceUVScale + EyeToSourceUVOffset;\n"
    "   oColor = Color;\n"              // Used for vignette fade.
    "}\n";
    
    const OVR::CAPI::GL::ShaderBase::Uniform Distortion_vs_refl[] =
    {
        { "EyeToSourceUVScale",  OVR::CAPI::GL::ShaderBase::VARTYPE_FLOAT, 0, 8 },
        { "EyeToSourceUVOffset", OVR::CAPI::GL::ShaderBase::VARTYPE_FLOAT, 8, 8 },
    };
    
    static const char Distortion_fs[] =
    "uniform sampler2D Texture0;\n"
    
    "_FS_IN vec4 oColor;\n"
    "_FS_IN vec2 oTexCoord0;\n"
    
    "_FRAGCOLOR_DECLARATION\n"
    
    "void main()\n"
    "{\n"
    "   _FRAGCOLOR = _TEXTURELOD(Texture0, oTexCoord0, 0.0);\n"
    "   _FRAGCOLOR.a = 1.0;\n"
    "}\n";
    
    
    static const char DistortionTimewarp_vs[] =
    "uniform vec2 EyeToSourceUVScale;\n"
    "uniform vec2 EyeToSourceUVOffset;\n"
    "uniform mat4 EyeRotationStart;\n"
    "uniform mat4 EyeRotationEnd;\n"
    
    "_VS_IN vec2 Position;\n"
    "_VS_IN vec4 Color;\n"
    "_VS_IN vec2 TexCoord0;\n"
    
    "_VS_OUT vec4 oColor;\n"
    "_VS_OUT vec2 oTexCoord0;\n"
    
    "void main()\n"
    "{\n"
    "   gl_Position.x = Position.x;\n"
    "   gl_Position.y = Position.y;\n"
    "   gl_Position.z = 0.0;\n"
    "   gl_Position.w = 1.0;\n"
    
    // Vertex inputs are in TanEyeAngle space for the R,G,B channels (i.e. after chromatic aberration and distortion).
    // These are now "real world" vectors in direction (x,y,1) relative to the eye of the HMD.
    "   vec3 TanEyeAngle = vec3 ( TexCoord0.x, TexCoord0.y, 1.0 );\n"
    
    // Accurate time warp lerp vs. faster
#if 1
    // Apply the two 3x3 timewarp rotations to these vectors.
	"   vec3 TransformedStart = (EyeRotationStart * vec4(TanEyeAngle, 0)).xyz;\n"
	"   vec3 TransformedEnd   = (EyeRotationEnd * vec4(TanEyeAngle, 0)).xyz;\n"
    // And blend between them.
    "   vec3 Transformed = mix ( TransformedStart, TransformedEnd, Color.a );\n"
#else
    "   mat4 EyeRotation = mix ( EyeRotationStart, EyeRotationEnd, Color.a );\n"
    "   vec3 Transformed   = EyeRotation * TanEyeAngle;\n"
#endif
    
    // Project them back onto the Z=1 plane of the rendered images.
    "   float RecipZ = 1.0 / Transformed.z;\n"
    "   vec2 Flattened = vec2 ( Transformed.x * RecipZ, Transformed.y * RecipZ );\n"
    
    // These are now still in TanEyeAngle space.
    // Scale them into the correct [0-1],[0-1] UV lookup space (depending on eye)
    "   vec2 SrcCoord = Flattened * EyeToSourceUVScale + EyeToSourceUVOffset;\n"
    "   oTexCoord0 = SrcCoord;\n"
    "   oColor = vec4(Color.r, Color.r, Color.r, Color.r);\n"              // Used for vignette fade.
    "}\n";

    
    const OVR::CAPI::GL::ShaderBase::Uniform DistortionTimewarp_vs_refl[] =
    {
        { "EyeToSourceUVScale",  OVR::CAPI::GL::ShaderBase::VARTYPE_FLOAT, 0, 8 },
        { "EyeToSourceUVOffset", OVR::CAPI::GL::ShaderBase::VARTYPE_FLOAT, 8, 8 },
    };
    
    static const char DistortionChroma_vs[] =
    "uniform vec2 EyeToSourceUVScale;\n"
    "uniform vec2 EyeToSourceUVOffset;\n"
    
    "_VS_IN vec2 Position;\n"
    "_VS_IN vec4 Color;\n"
    "_VS_IN vec2 TexCoord0;\n"
    "_VS_IN vec2 TexCoord1;\n"
    "_VS_IN vec2 TexCoord2;\n"
    
    "_VS_OUT vec4 oColor;\n"
    "_VS_OUT vec2 oTexCoord0;\n"
    "_VS_OUT vec2 oTexCoord1;\n"
    "_VS_OUT vec2 oTexCoord2;\n"
    
    "void main()\n"
    "{\n"
    "   gl_Position.x = Position.x;\n"
    "   gl_Position.y = Position.y;\n"
    "   gl_Position.z = 0.5;\n"
    "   gl_Position.w = 1.0;\n"
    
    // Vertex inputs are in TanEyeAngle space for the R,G,B channels (i.e. after chromatic aberration and distortion).
    // Scale them into the correct [0-1],[0-1] UV lookup space (depending on eye)
    "   oTexCoord0 = TexCoord0 * EyeToSourceUVScale + EyeToSourceUVOffset;\n"
    "   oTexCoord1 = TexCoord1 * EyeToSourceUVScale + EyeToSourceUVOffset;\n"
    "   oTexCoord2 = TexCoord2 * EyeToSourceUVScale + EyeToSourceUVOffset;\n"
    
    "   oColor = Color;\n" // Used for vignette fade.
    "}\n";
    
    const OVR::CAPI::GL::ShaderBase::Uniform DistortionChroma_vs_refl[] =
    {
        { "EyeToSourceUVScale",  OVR::CAPI::GL::ShaderBase::VARTYPE_FLOAT, 0, 8 },
        { "EyeToSourceUVOffset", OVR::CAPI::GL::ShaderBase::VARTYPE_FLOAT, 8, 8 },
    };
    
    static const char DistortionChroma_fs[] =
    "uniform sampler2D Texture0;\n"
    
    "_FS_IN vec4 oColor;\n"
    "_FS_IN vec2 oTexCoord0;\n"
    "_FS_IN vec2 oTexCoord1;\n"
    "_FS_IN vec2 oTexCoord2;\n"
    
    "_FRAGCOLOR_DECLARATION\n"
    
    "void main()\n"
    "{\n"
    "   float ResultR = _TEXTURELOD(Texture0, oTexCoord0, 0.0).r;\n"
    "   float ResultG = _TEXTURELOD(Texture0, oTexCoord1, 0.0).g;\n"
    "   float ResultB = _TEXTURELOD(Texture0, oTexCoord2, 0.0).b;\n"
    
    "   _FRAGCOLOR = vec4(ResultR * oColor.r, ResultG * oColor.g, ResultB * oColor.b, 1.0);\n"
    "}\n";

    
    static const char DistortionTimewarpChroma_vs[] =
    "uniform vec2 EyeToSourceUVScale;\n"
    "uniform vec2 EyeToSourceUVOffset;\n"
    "uniform mat4 EyeRotationStart;\n"
    "uniform mat4 EyeRotationEnd;\n"
    
    "_VS_IN vec2 Position;\n"
    "_VS_IN vec4 Color;\n"
    "_VS_IN vec2 TexCoord0;\n"
    "_VS_IN vec2 TexCoord1;\n"
    "_VS_IN vec2 TexCoord2;\n"
    
    "_VS_OUT vec4 oColor;\n"
    "_VS_OUT vec2 oTexCoord0;\n"
    "_VS_OUT vec2 oTexCoord1;\n"
    "_VS_OUT vec2 oTexCoord2;\n"
    
    "void main()\n"
    "{\n"
    "   gl_Position.x = Position.x;\n"
    "   gl_Position.y = Position.y;\n"
    "   gl_Position.z = 0.0;\n"
    "   gl_Position.w = 1.0;\n"
    
    // Vertex inputs are in TanEyeAngle space for the R,G,B channels (i.e. after chromatic aberration and distortion).
    // These are now "real world" vectors in direction (x,y,1) relative to the eye of the HMD.
    "   vec3 TanEyeAngleR = vec3 ( TexCoord0.x, TexCoord0.y, 1.0 );\n"
    "   vec3 TanEyeAngleG = vec3 ( TexCoord1.x, TexCoord1.y, 1.0 );\n"
    "   vec3 TanEyeAngleB = vec3 ( TexCoord2.x, TexCoord2.y, 1.0 );\n"
    
    // Accurate time warp lerp vs. faster
#if 1
    // Apply the two 3x3 timewarp rotations to these vectors.
	"   vec3 TransformedRStart = (EyeRotationStart * vec4(TanEyeAngleR, 0)).xyz;\n"
	"   vec3 TransformedGStart = (EyeRotationStart * vec4(TanEyeAngleG, 0)).xyz;\n"
	"   vec3 TransformedBStart = (EyeRotationStart * vec4(TanEyeAngleB, 0)).xyz;\n"
	"   vec3 TransformedREnd   = (EyeRotationEnd * vec4(TanEyeAngleR, 0)).xyz;\n"
	"   vec3 TransformedGEnd   = (EyeRotationEnd * vec4(TanEyeAngleG, 0)).xyz;\n"
	"   vec3 TransformedBEnd   = (EyeRotationEnd * vec4(TanEyeAngleB, 0)).xyz;\n"
    
    // And blend between them.
    "   vec3 TransformedR = mix ( TransformedRStart, TransformedREnd, Color.a );\n"
    "   vec3 TransformedG = mix ( TransformedGStart, TransformedGEnd, Color.a );\n"
    "   vec3 TransformedB = mix ( TransformedBStart, TransformedBEnd, Color.a );\n"
#else
    "   mat3 EyeRotation;\n"
    "   EyeRotation[0] = mix ( EyeRotationStart[0], EyeRotationEnd[0], Color.a ).xyz;\n"
    "   EyeRotation[1] = mix ( EyeRotationStart[1], EyeRotationEnd[1], Color.a ).xyz;\n"
    "   EyeRotation[2] = mix ( EyeRotationStart[2], EyeRotationEnd[2], Color.a ).xyz;\n"
    "   vec3 TransformedR   = EyeRotation * TanEyeAngleR;\n"
    "   vec3 TransformedG   = EyeRotation * TanEyeAngleG;\n"
    "   vec3 TransformedB   = EyeRotation * TanEyeAngleB;\n"
#endif
    
    // Project them back onto the Z=1 plane of the rendered images.
    "   float RecipZR = 1.0 / TransformedR.z;\n"
    "   float RecipZG = 1.0 / TransformedG.z;\n"
    "   float RecipZB = 1.0 / TransformedB.z;\n"
    "   vec2 FlattenedR = vec2 ( TransformedR.x * RecipZR, TransformedR.y * RecipZR );\n"
    "   vec2 FlattenedG = vec2 ( TransformedG.x * RecipZG, TransformedG.y * RecipZG );\n"
    "   vec2 FlattenedB = vec2 ( TransformedB.x * RecipZB, TransformedB.y * RecipZB );\n"
    
    // These are now still in TanEyeAngle space.
    // Scale them into the correct [0-1],[0-1] UV lookup space (depending on eye)
    "   vec2 SrcCoordR = FlattenedR * EyeToSourceUVScale + EyeToSourceUVOffset;\n"
    "   vec2 SrcCoordG = FlattenedG * EyeToSourceUVScale + EyeToSourceUVOffset;\n"
    "   vec2 SrcCoordB = FlattenedB * EyeToSourceUVScale + EyeToSourceUVOffset;\n"
    
    "   oTexCoord0 = SrcCoordR;\n"
    "   oTexCoord1 = SrcCoordG;\n"
    "   oTexCoord2 = SrcCoordB;\n"
    
    "   oColor = vec4(Color.r, Color.r, Color.r, Color.r);\n"              // Used for vignette fade.
    "}\n";
    

    const OVR::CAPI::GL::ShaderBase::Uniform DistortionTimewarpChroma_vs_refl[] =
    {
        { "EyeToSourceUVScale",  OVR::CAPI::GL::ShaderBase::VARTYPE_FLOAT, 0, 8 },
        { "EyeToSourceUVOffset", OVR::CAPI::GL::ShaderBase::VARTYPE_FLOAT, 8, 8 },
        { "EyeRotationStart",    OVR::CAPI::GL::ShaderBase::VARTYPE_FLOAT, 16, 64 },
        { "EyeRotationEnd",      OVR::CAPI::GL::ShaderBase::VARTYPE_FLOAT, 80, 64 },
    };
    
}}} // OVR::CAPI::GL

#endif // OVR_CAPI_GL_Shaders_h
