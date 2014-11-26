#ifdef GL_ES
precision mediump float;
#endif

uniform sampler2D uTexture;

varying vec2 vTexCoord0;
varying vec2 vTexCoord1;
varying vec2 vTexCoord2;
varying float vVignette;

/*
Texture2D Texture   : register(t0);
SamplerState Linear : register(s0);
float4 main(in float4 oPosition  : SV_Position,  in float2 oTexCoord0 : TEXCOORD0,
            in float2 oTexCoord1 : TEXCOORD1,    in float2 oTexCoord2 : TEXCOORD2,
            in float  oVignette  : TEXCOORD3)    : SV_Target
{
	// 3 samples for fixing chromatic aberrations
    float R = Texture.Sample(Linear, oTexCoord0.xy).r;
    float G = Texture.Sample(Linear, oTexCoord1.xy).g;
    float B = Texture.Sample(Linear, oTexCoord2.xy).b;
    return (oVignette*float4(R,G,B,1));
};
*/

void 
main(void)
{
	float r = texture2D(uTexture, vec2(vTexCoord0.x, 1. - vTexCoord0.y)).r;
    float g = texture2D(uTexture, vec2(vTexCoord1.x, 1. - vTexCoord1.y)).g;
    float b = texture2D(uTexture, vec2(vTexCoord2.x, 1. - vTexCoord2.y)).b;

	gl_FragColor = vVignette * vec4(r, g, b, 1.);
}
