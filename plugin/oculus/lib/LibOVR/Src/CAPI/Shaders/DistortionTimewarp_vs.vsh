float2   EyeToSourceUVScale;
float2   EyeToSourceUVOffset;
float4x4 EyeRotationStart;
float4x4 EyeRotationEnd;

float2 TimewarpTexCoordToWarpedPos(float2 inTexCoord, float4x4 rotMat)
{
    // Vertex inputs are in TanEyeAngle space for the R,G,B channels (i.e. after chromatic aberration and distortion).
    // These are now "real world" vectors in direction (x,y,1) relative to the eye of the HMD.    
    // Apply the 3x3 timewarp rotation to these vectors.
	float3 transformed = float3( mul ( rotMat, float4(inTexCoord,1,1) ).xyz);
    // Project them back onto the Z=1 plane of the rendered images.
	float2 flattened = transformed.xy / transformed.z;
    // Scale them into ([0,0.5],[0,1]) or ([0.5,0],[0,1]) UV lookup space (depending on eye)
	return flattened * EyeToSourceUVScale + EyeToSourceUVOffset;
	
}

void main(in float2 Position    : POSITION,
          in float4 Color       : COLOR0,
          in float2 TexCoord0   : TEXCOORD0,    
          out float4 oPosition  : SV_Position,
          out float1 oColor     : COLOR,
          out float2 oTexCoord0 : TEXCOORD0)
{

    oPosition.x = Position.x;
    oPosition.y = Position.y;
    oPosition.z = 0.5;
    oPosition.w = 1.0;

    float timewarpLerpFactor = Color.a;
    float4x4 lerpedEyeRot = lerp(EyeRotationStart, EyeRotationEnd, timewarpLerpFactor);	

    // Warped positions are a bit more involved, hence a separate function
	oTexCoord0 = TimewarpTexCoordToWarpedPos(TexCoord0, lerpedEyeRot);
    oColor = Color.r;              // Used for vignette fade.
}


