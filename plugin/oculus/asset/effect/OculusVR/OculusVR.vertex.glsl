#ifdef GL_ES
precision mediump float;
#endif

attribute vec2 aPosition;
attribute float aTimewarpLerpFactor;
attribute float aVignette;
attribute vec2 aTexCoord0;
attribute vec2 aTexCoord1;
attribute vec2 aTexCoord2;

uniform vec2 uEyeToSourceUVScale;
uniform vec2 uEyeToSourceUVOffset;

uniform mat4 uEyeRotationStart;
uniform mat4 uEyeRotationEnd;

varying vec2 vTexCoord0;
varying vec2 vTexCoord1;
varying vec2 vTexCoord2;
varying float vVignette;

/*                                     
float2 TimewarpTexCoord(float2 TexCoord, float4x4 rotMat)                              
{                                                                                      
	// Vertex inputs are in TanEyeAngle space for the R,G,B channels (i.e. after chromatic 
	// aberration and distortion). These are now real world vectors in direction (x,y,1) 
	// relative to the eye of the HMD.	Apply the 3x3 timewarp rotation to these vectors.
    float3 transformed = float3( mul ( rotMat, float4(TexCoord.xy, 1, 1) ).xyz);       

	// Project them back onto the Z=1 plane of the rendered images.
    float2 flattened = (transformed.xy / transformed.z);                               

	// Scale them into ([0,0.5],[0,1]) or ([0.5,0],[0,1]) UV lookup space (depending on eye)
    return(EyeToSourceUVScale * flattened + EyeToSourceUVOffset);                      
}                                                                                      
*/

vec2 timewarpTexCoord(vec2 texCoord, mat4 rotMat)
{
	vec3 transformed = vec3(rotMat * vec4(texCoord.xy, 1., 1.));
	vec2 flattened = (transformed.xy / transformed.z);

	return uEyeToSourceUVScale * flattened + uEyeToSourceUVOffset;
}

/*
void main(in float2  Position   : POSITION,  in float timewarpLerpFactor : POSITION1,  
          in float   Vignette   : POSITION2, in float2 TexCoord0         : TEXCOORD0,  
          in float2  TexCoord1  : TEXCOORD1, in float2 TexCoord2         : TEXCOORD2,  
          out float4 oPosition  : SV_Position,                                         
          out float2 oTexCoord0 : TEXCOORD0, out float2 oTexCoord1 : TEXCOORD1,        
          out float2 oTexCoord2 : TEXCOORD2, out float  oVignette  : TEXCOORD3)        
{                                                                                      
    float4x4 lerpedEyeRot = lerp(EyeRotationStart, EyeRotationEnd, timewarpLerpFactor);
    oTexCoord0  = TimewarpTexCoord(TexCoord0,lerpedEyeRot);                            
    oTexCoord1  = TimewarpTexCoord(TexCoord1,lerpedEyeRot);                            
    oTexCoord2  = TimewarpTexCoord(TexCoord2,lerpedEyeRot);                            
    oPosition = float4(Position.xy, 0.5, 1.0);    oVignette = Vignette;                
};
*/

mat4 lerp(mat4 a, mat4 b, float f)
{
	return mat4(
		mix(a[0], b[0], f),
		mix(a[1], b[1], f),
		mix(a[2], b[2], f),
		mix(a[3], b[3], f)
	);
}

void main(void)
{
	mat4 lerpedEyeRot = lerp(uEyeRotationStart, uEyeRotationEnd, aTimewarpLerpFactor);

	vTexCoord0 = timewarpTexCoord(aTexCoord0, lerpedEyeRot);
	vTexCoord1 = timewarpTexCoord(aTexCoord1, lerpedEyeRot);
	vTexCoord2 = timewarpTexCoord(aTexCoord2, lerpedEyeRot);
	vVignette = aVignette;

	gl_Position = vec4(aPosition.xy, .5, 1.);
}
 