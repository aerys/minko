/************************************************************************************

Filename    :   CAPI_D3D1X_Util.cpp
Content     :   D3D9 utility functions for rendering
Created     :   March 7 , 2014
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

#include "CAPI_D3D9_DistortionRenderer.h"
#define OVR_D3D_VERSION 9
#include "../../OVR_CAPI_D3D.h"


namespace OVR { namespace CAPI { namespace D3D9 {


#define PRECOMPILE_FLAG 0
#if !PRECOMPILE_FLAG
//To make these, you need to run it with PRECOMPILE_FLAG, which also uses them, so good for debugging.
//Then cut and paste these from the output window.
//Then turn off the flag.
DWORD precompiledVertexShaderSrc[96] = {4294836736,3080190,1111577667,28,130,4294836736,2,28,33024,123,68,131074,655361,88,0,104,2,131073,88,0,1415936325,1970230127,1432707954,1717981014,7628147,196609,131073,1,0,1415936325,1970230127,1432707954,1633899350,1979737452,1597136755,1766654000,1936683619,544499311,539578920,1280527432,1634226976,544367972,1886220099,1919249513,841890080,892939833,825437746,2868916529,83886161,2685337601,1065353216,0,1056964608,0,33554463,2147483648,2416902144,33554463,2147614720,2416902145,33554463,2147483653,2416902146,33554463,2147549189,2416902147,33554463,2147614725,2416902148,33554433,2147680256,2699296768,67108868,3758292992,2162425856,2430861314,2699296770,67108868,3758292993,2162425856,2430861315,2699296770,67108868,3758292994,2162425856,2430861316,2699296770,67108868,3222208512,2416181248,2689597441,2686779393,33554433,3758161923,2415919105,65535,};
DWORD precompiledVertexShaderTimewarpSrc[310] = {4294836992,4587518,1111577667,28,222,4294836992,4,28,33024,215,108,1310722,5373956,124,0,140,262146,1179652,124,0,157,131074,655361,180,0,196,2,131073,180,0,1382381893,1952543855,1164865385,2868929646,196611,262148,1,0,1382381893,1952543855,1399746409,1953653108,1702446336,1867738964,1701016181,1716475477,1952805734,2880154368,196609,131073,1,0,1415936325,1970230127,1432707954,1633899350,1979737452,1597202291,1766654000,1936683619,544499311,539578920,1280527432,1634226976,544367972,1886220099,1919249513,841890080,892939833,825437746,2868916529,83886161,2685337601,1065353216,0,1056964608,0,33554463,2147483648,2416902144,33554463,2147549184,2416902145,33554463,2147614720,2416902146,33554463,2147483653,2416902147,33554463,2147549189,2416902148,33554463,2147614725,2416902149,33554463,2147483648,3759079424,33554463,2147483653,3758292993,33554463,2147549189,3758292994,33554463,2147614725,3758292995,33554463,2147680261,3758161924,33554433,2147549184,2695495684,50331650,2147549185,2164260864,2695495700,33554433,2147614720,2695495685,50331650,2147614721,2169831424,2695495701,33554433,2147745792,2695495686,50331650,2147745793,2175401984,2695495702,33554433,2148007936,2695495687,50331650,2148007937,2180972544,2695495703,67108868,2148466688,2415919105,2162425857,2162425856,67108868,2148466689,2416181251,2689597441,2684682241,50331657,2147549186,2162425856,2162425857,33554438,2147549186,2147483650,33554433,2147680259,2699296772,50331650,2147876866,2177892355,2697986068,67108868,2147549187,2415919105,2158624770,2689925124,67108868,2147549188,2415919105,2153054210,2684354564,33554433,2147680261,2699296773,50331650,2147876866,2177105925,2697199637,67108868,2147614723,2415919105,2153054210,2689925125,67108868,2147614724,2415919105,2158624770,2684354565,33554433,2147680261,2699296774,50331650,2147811333,2177171461,2697265174,67108868,2147745795,2415919105,2147483653,2689925126,67108868,2147745796,2415919105,2158624773,2684354566,33554433,2147680261,2699296775,50331650,2148073477,2166685701,2686779415,67108868,2148007939,2415919105,2147483653,2689925127,67108868,2148007940,2415919105,2164195333,2684354567,50331657,2147549189,2162425860,2162425857,50331657,2147614725,2162425859,2162425857,50331653,2147680257,2147483650,2162425861,33554433,2147680258,2699296768,67108868,3758292993,2162425858,2162425857,2699296770,67108868,2148466689,2416181252,2689597441,2684682241,50331657,2147549189,2162425860,2162425857,50331657,2147614725,2162425859,2162425857,50331657,2147549185,2162425856,2162425857,33554438,2147549185,2147483649,50331653,2147680257,2147483649,2162425861,67108868,3758292994,2162425858,2162425857,2699296770,67108868,2148466689,2416181253,2689597441,2684682241,50331657,2147549188,2162425860,2162425857,50331657,2147614724,2162425859,2162425857,50331657,2147549184,2162425856,2162425857,33554438,2147549184,2147483648,50331653,2147680256,2147483648,2162425860,67108868,3758292995,2162425858,2162425856,2699296770,67108868,3759079424,2416181248,2689597441,2686779393,33554433,3758161924,2415919106,65535,};
DWORD precompiledPixelShaderSrc[84] = {4294902528,2228222,1111577667,28,79,4294902528,1,28,33024,72,48,3,131073,56,0,1954047316,6648437,786436,65537,1,0,861893488,1291858015,1869767529,1952870259,693250080,1397508128,1750278220,1919247457,1836008224,1701603696,775495794,959330610,858665525,3223857,83886161,2685337600,1065353216,0,0,0,33554463,2147483653,2416115712,33554463,2147549189,2416115713,33554463,2147614725,2416115714,33554463,2147680261,2415984643,33554463,2415919104,2685339648,50331714,2148466688,2430861312,2699298816,67108868,2148073472,2147483648,2690908160,2686779392,50331714,2148466689,2430861313,2699298816,33554433,2147614720,2153054209,50331714,2148466689,2430861314,2699298816,33554433,2147745792,2158624769,50331653,2148468736,2162425856,2415919107,65535,};

#else
#include "d3dcompiler.h"
#pragma comment(lib, "C:\\Program Files (x86)\\Microsoft DirectX SDK (June 2010)\\Lib\\x86\\D3DCompiler.lib")
/***************************************************************************/
const char* VertexShaderSrc = 
	
	"float2 EyeToSourceUVScale  : register(c0);                                           \n"
	"float2 EyeToSourceUVOffset : register(c2);                                           \n"
	
	"void main(in float2 Position    : POSITION,    in float  TimeWarp    : POSITION1, \n"
	"          in float  Vignette    : POSITION2,   in float2 TexCoord0   : TEXCOORD0, \n"
	"          in float2 TexCoord1   : TEXCOORD1,   in float2 TexCoord2   : TEXCOORD2, \n"
	"          out float4 oPosition  : SV_Position, out float2 oTexCoord0 : TEXCOORD0, \n"
	"          out float2 oTexCoord1 : TEXCOORD1,   out float2 oTexCoord2 : TEXCOORD2, \n"
	"          out float oVignette   : TEXCOORD3)                                      \n"
	"{                                                                                 \n"
	"    oTexCoord0 = EyeToSourceUVScale * TexCoord0 + EyeToSourceUVOffset;                  \n"
	"    oTexCoord1 = EyeToSourceUVScale * TexCoord1 + EyeToSourceUVOffset;                  \n"
	"    oTexCoord2 = EyeToSourceUVScale * TexCoord2 + EyeToSourceUVOffset;                  \n"
	"    oVignette  = Vignette;                                                        \n"
	"    oPosition  = float4(Position.xy, 0.5, 1.0);                                   \n"
	"}";

/***************************************************************************/
const char* VertexShaderTimewarpSrc = 
	
	"float2 EyeToSourceUVScale    : register(c0);                                         \n"
	"float2 EyeToSourceUVOffset   : register(c2);                                         \n"
	"float4x4 EyeRotationStart : register(c4);                                         \n"
	"float4x4 EyeRotationEnd   : register(c20);                                        \n"
	
	"float2 TimewarpTexCoord(float2 TexCoord, float4x4 rotMat)                         \n"
	"{                                                                                 \n"
	"    float3 transformed = float3( mul ( rotMat, float4(TexCoord.xy, 1, 1) ).xyz);  \n"
	"    float2 flattened = (transformed.xy / transformed.z);                          \n"
	"    return(EyeToSourceUVScale * flattened + EyeToSourceUVOffset);                       \n"
	"}                                                                                 \n"
	"void main(in float2 Position    : POSITION,    in float  TimeWarp    : POSITION1, \n"
	"          in float  Vignette    : POSITION2,   in float2 TexCoord0   : TEXCOORD0, \n"
	"          in float2 TexCoord1   : TEXCOORD1,   in float2 TexCoord2   : TEXCOORD2, \n"
	"          out float4 oPosition  : SV_Position, out float2 oTexCoord0 : TEXCOORD0, \n"
	"          out float2 oTexCoord1 : TEXCOORD1,   out float2 oTexCoord2 : TEXCOORD2, \n"
	"          out float oVignette   : TEXCOORD3)                                      \n"
	"{                                                                                 \n"
	"    float4x4 lerpedEyeRot = lerp(EyeRotationStart, EyeRotationEnd, TimeWarp);     \n"
	"    oTexCoord0  = TimewarpTexCoord(TexCoord0,lerpedEyeRot);                       \n"
	"    oTexCoord1  = TimewarpTexCoord(TexCoord1,lerpedEyeRot);                       \n"
	"    oTexCoord2  = TimewarpTexCoord(TexCoord2,lerpedEyeRot);                       \n"
	"    oVignette  = Vignette;                                                        \n"
	"    oPosition  = float4(Position.xy, 0.5, 1.0);                                   \n"
	"}";

/***************************************************************************/
const char* PixelShaderSrc =
	
	" sampler2D Texture : register(s0);		                                           \n"

	"float4 main(in float4 oPosition  : SV_Position, in float2 oTexCoord0 : TEXCOORD0, \n"
	"            in float2 oTexCoord1 : TEXCOORD1,   in float2 oTexCoord2 : TEXCOORD2, \n"
	"            in float  oVignette  : TEXCOORD3) \n"
	"          : SV_Target                                                             \n" 
	"{                                                                                 \n"
    "	 float R = tex2D(Texture,oTexCoord0).r;		                                   \n"
    "	 float G = tex2D(Texture,oTexCoord1).g;		                                   \n"
    "	 float B = tex2D(Texture,oTexCoord2).b;		                                   \n"
	"    return (oVignette*float4(R,G,B,1));                                           \n"
	"}";

/*************************************************************/
ID3DBlob* ShaderCompile(char * shaderName, const char * shaderSrcString, const char * profile)
{
    ID3DBlob* pShaderCode = NULL;
    ID3DBlob* pErrorMsg = NULL;

	if (FAILED(D3DCompile(shaderSrcString, strlen(shaderSrcString),NULL,NULL,NULL,
						  "main",profile,D3DCOMPILE_OPTIMIZATION_LEVEL3,0,
						  &pShaderCode,&pErrorMsg)))					
 		MessageBoxA(NULL,(char *) pErrorMsg->GetBufferPointer(),"", MB_OK); 
	if (pErrorMsg) pErrorMsg->Release();

	//Now write out blob
	char tempString[1000];
	int numDWORDs = ((int)pShaderCode->GetBufferSize())/4;
	DWORD * ptr = (DWORD *)pShaderCode->GetBufferPointer();
	sprintf_s(tempString,"DWORD %s[%d] = {",shaderName,numDWORDs);
	OutputDebugStringA(tempString);
	for (int i = 0;i < numDWORDs; i++)
	{
		sprintf_s(tempString,"%lu,",ptr[i]);
		OutputDebugStringA(tempString);
	}
	OutputDebugStringA("};\n");

	return(pShaderCode);
}
#endif

/***********************************************************/
void DistortionRenderer::CreateDistortionShaders(void)
{
#if PRECOMPILE_FLAG
	ID3DBlob * pShaderCode;
	pShaderCode = ShaderCompile("precompiledVertexShaderSrc",VertexShaderSrc,"vs_2_0");
	Device->CreateVertexShader( ( DWORD* )pShaderCode->GetBufferPointer(), &VertexShader );
    pShaderCode->Release();

	pShaderCode = ShaderCompile("precompiledVertexShaderTimewarpSrc",VertexShaderTimewarpSrc,"vs_3_0");
	Device->CreateVertexShader( ( DWORD* )pShaderCode->GetBufferPointer(), &VertexShaderTimewarp );
    pShaderCode->Release();

	pShaderCode = ShaderCompile("precompiledPixelShaderSrc",PixelShaderSrc,"ps_3_0");
	Device->CreatePixelShader( ( DWORD* )pShaderCode->GetBufferPointer(), &PixelShader );
    pShaderCode->Release();
#else
	Device->CreateVertexShader( precompiledVertexShaderSrc, &VertexShader );
	Device->CreateVertexShader( precompiledVertexShaderTimewarpSrc, &VertexShaderTimewarp );
	Device->CreatePixelShader(  precompiledPixelShaderSrc, &PixelShader );
#endif
}


/***************************************************/
void DistortionRenderer::CreateVertexDeclaration(void)
{
	static const D3DVERTEXELEMENT9 VertexElements[7] =	{
        { 0,  0, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
        { 0,  8, D3DDECLTYPE_FLOAT1, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 1 },
        { 0, 12, D3DDECLTYPE_FLOAT1, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 2 },
        { 0, 16, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },
        { 0, 24, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 1 },
        { 0, 32, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 2 },
		D3DDECL_END()	};
    Device->CreateVertexDeclaration( VertexElements, &VertexDecl );
}


/******************************************************/
void DistortionRenderer::CreateDistortionModels(void)
{
	//Make the distortion models
	for (int eye=0;eye<2;eye++)
	{		
		FOR_EACH_EYE * e = &eachEye[eye];
		ovrDistortionMesh meshData;
		ovrHmd_CreateDistortionMesh(HMD,
            RState.EyeRenderDesc[eye].Eye,
            RState.EyeRenderDesc[eye].Fov,
                                    RState.DistortionCaps,
                                    &meshData);

		e->numVerts = meshData.VertexCount;
		e->numIndices = meshData.IndexCount;

		Device->CreateVertexBuffer( (e->numVerts)*sizeof(ovrDistortionVertex),0, 0,
                                    D3DPOOL_MANAGED, &e->dxVerts, NULL );
		ovrDistortionVertex * dxv; 	e->dxVerts->Lock( 0, 0, (void**)&dxv, 0 );
		for (int v=0;v<e->numVerts;v++) dxv[v] = meshData.pVertexData[v];
		e->dxVerts->Unlock();

		Device->CreateIndexBuffer( (e->numIndices)*sizeof(u_short),0, D3DFMT_INDEX16,
                                   D3DPOOL_MANAGED, &e->dxIndices, NULL );
		unsigned short* dxi; e->dxIndices->Lock( 0, 0, (void**)&dxi, 0 );
		for (int i=0;i<e->numIndices;i++) dxi[i] = meshData.pIndexData[i];
		e->dxIndices->Unlock();

		ovrHmd_DestroyDistortionMesh( &meshData );
	}
}

/**********************************************************/
void DistortionRenderer::RenderBothDistortionMeshes(void)
{
	Device->BeginScene();

	D3DCOLOR clearColor = D3DCOLOR_RGBA(
		(int)(RState.ClearColor[0] * 255.0f),
		(int)(RState.ClearColor[1] * 255.0f),
		(int)(RState.ClearColor[2] * 255.0f),
		(int)(RState.ClearColor[3] * 255.0f));

	Device->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_STENCIL | D3DCLEAR_ZBUFFER, clearColor, 0, 0);

	for (int eye=0; eye<2; eye++)
	{
		FOR_EACH_EYE * e = &eachEye[eye];
		D3DVIEWPORT9 vp;
        vp.X=0; vp.Y=0;
        vp.Width=ScreenSize.w;	vp.Height=ScreenSize.h;
        vp.MinZ=0; vp.MaxZ = 1;

		Device->SetViewport(&vp);
		Device->SetStreamSource( 0, e->dxVerts,0, sizeof(ovrDistortionVertex) );
		Device->SetVertexDeclaration( VertexDecl ); 
		Device->SetIndices( e->dxIndices );
		Device->SetPixelShader( PixelShader );
		Device->SetTexture( 0, e->texture);

		//Choose which vertex shader, with associated additional inputs
		if (RState.DistortionCaps & ovrDistortionCap_TimeWarp)
		{          
			Device->SetVertexShader( VertexShaderTimewarp );  

            ovrMatrix4f timeWarpMatrices[2];            
            ovrHmd_GetEyeTimewarpMatrices(HMD, (ovrEyeType)eye,
                                          RState.EyeRenderPoses[eye], timeWarpMatrices);

			//Need to transpose the matrices
			timeWarpMatrices[0] = Matrix4f(timeWarpMatrices[0]).Transposed();
			timeWarpMatrices[1] = Matrix4f(timeWarpMatrices[1]).Transposed();

            // Feed identity like matrices in until we get proper timewarp calculation going on
			Device->SetVertexShaderConstantF(4, (float *) &timeWarpMatrices[0],4);
			Device->SetVertexShaderConstantF(20,(float *) &timeWarpMatrices[1],4);
        }
		else
		{
			Device->SetVertexShader( VertexShader );  
		}

		//Set up vertex shader constants
		Device->SetVertexShaderConstantF( 0, ( FLOAT* )&(e->UVScaleOffset[0]), 1 );
		Device->SetVertexShaderConstantF( 2, ( FLOAT* )&(e->UVScaleOffset[1]), 1 );

		Device->DrawIndexedPrimitive( D3DPT_TRIANGLELIST,0,0,e->numVerts,0,e->numIndices/3);
	}

	Device->EndScene();
}

}}}
