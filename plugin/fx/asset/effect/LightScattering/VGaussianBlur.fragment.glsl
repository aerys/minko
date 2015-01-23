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
Original "Faster Gaussian Blur" implementation by Nilson Souto
http://xissburg.com/faster-gaussian-blur-in-glsl/
*/

#ifdef FRAGMENT_SHADER

#ifdef GL_ES
precision mediump float;
#endif

varying vec2 vTexcoord;

uniform sampler2D uTexture;
uniform float uTextureSize;

void main(void)
{
	vec4 fragmentColor = texture2D( uTexture, vTexcoord) * 0.2270270270;
	
	fragmentColor += texture2D( uTexture, vTexcoord + vec2(0.0, 0.0) / uTextureSize) * 0.2270270270;
	fragmentColor += texture2D( uTexture, vTexcoord - vec2(0.0, 0.0) / uTextureSize) * 0.2270270270;
	
	fragmentColor += texture2D( uTexture, vTexcoord + vec2(0.0, 1.3846153846) / uTextureSize) * 0.3162162162;
	fragmentColor += texture2D( uTexture, vTexcoord - vec2(0.0, 1.3846153846) / uTextureSize) * 0.3162162162;
	
	fragmentColor += texture2D( uTexture, vTexcoord + vec2(0.0, 3.2307692308) / uTextureSize) * 0.0702702703;
	fragmentColor += texture2D( uTexture, vTexcoord - vec2(0.0, 3.2307692308) / uTextureSize) * 0.0702702703;
	
	gl_FragColor = fragmentColor;
}

#endif
