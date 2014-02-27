/*
Copyright (c) 2013 Aerys

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and
associated documentation files (the "Software"), to deal in the Software without restriction,
including without limitation the rights to use, copy, modify, merge, publish, distribute,
sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or
substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING
BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

/*
Original "Pseudo Lens Flare" implementation by John Chapman
http://john-chapman-graphics.blogspot.co.uk/2013/02/pseudo-lens-flare.html
*/

#ifdef FRAGMENT_SHADER

#ifdef GL_ES
precision mediump float;
#endif

uniform sampler2D uInputTex;

uniform int uSamples;// = 8;
uniform float uDispersal;// = 0.25;
uniform float uHaloWidth;// = 1.0;
uniform float uDistortion;// = 1.0;

uniform vec2 uTextureSize;

varying vec2 vTexcoord;

vec3 textureDistorted(sampler2D tex,
					  vec2 texcoord,
					  vec2 direction,
					  vec3 distortion)
{
	return vec3(
		texture2D(tex, texcoord + direction * distortion.r).r,
		texture2D(tex, texcoord + direction * distortion.g).g,
		texture2D(tex, texcoord + direction * distortion.b).b
	);
}

void main()
{
	vec2 texcoord = -vTexcoord + vec2(1.0); // flip texcoordoords
	vec2 texelSize = 1.0 / uTextureSize;
	
	vec2 ghostVec = (vec2(0.5) - texcoord) * uDispersal;
	vec2 haloVec = normalize(ghostVec) * uHaloWidth;
	
	vec3 distortion = vec3(-texelSize.x * uDistortion, 0.0, texelSize.x * uDistortion);

	// sample ghosts:
	vec3 result = vec3(0.0);
	for (int i = 0; i < uSamples; ++i) {
		vec2 offset = fract(texcoord + ghostVec * float(i));
		
		float weight = length(vec2(0.5) - offset) / length(vec2(0.5));
		weight = pow(1.0 - weight, 10.0);
	
		result += textureDistorted(
			uInputTex,
			offset,
			normalize(ghostVec),
			distortion
		) * weight;
	}
	
	//result *= texture(uLensColor, length(vec2(0.5) - texcoord) / length(vec2(0.5)));

	//	sample halo:
	float weight = length(vec2(0.5) - fract(texcoord + haloVec)) / length(vec2(0.5));
	weight = pow(1.0 - weight, 10.0);
	result += textureDistorted(
		uInputTex,
		fract(texcoord + haloVec),
		normalize(ghostVec),
		distortion
	) * weight;
	
	gl_FragColor = vec4(result, 1);
}

#endif
