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

#ifdef GL_ES
precision mediump float;
#endif
 
uniform sampler2D s_texture;
 
varying vec2 v_texCoord;
varying vec2 v_blurTexCoords[14];
 
void main()
{
    gl_FragColor = vec4(0.0);
    gl_FragColor += texture2D(s_texture, v_blurTexCoords[ 0])*0.0044299121055113265;
    gl_FragColor += texture2D(s_texture, v_blurTexCoords[ 1])*0.00895781211794;
    gl_FragColor += texture2D(s_texture, v_blurTexCoords[ 2])*0.0215963866053;
    gl_FragColor += texture2D(s_texture, v_blurTexCoords[ 3])*0.0443683338718;
    gl_FragColor += texture2D(s_texture, v_blurTexCoords[ 4])*0.0776744219933;
    gl_FragColor += texture2D(s_texture, v_blurTexCoords[ 5])*0.115876621105;
    gl_FragColor += texture2D(s_texture, v_blurTexCoords[ 6])*0.147308056121;
    gl_FragColor += texture2D(s_texture, v_texCoord         )*0.159576912161;
    gl_FragColor += texture2D(s_texture, v_blurTexCoords[ 7])*0.147308056121;
    gl_FragColor += texture2D(s_texture, v_blurTexCoords[ 8])*0.115876621105;
    gl_FragColor += texture2D(s_texture, v_blurTexCoords[ 9])*0.0776744219933;
    gl_FragColor += texture2D(s_texture, v_blurTexCoords[10])*0.0443683338718;
    gl_FragColor += texture2D(s_texture, v_blurTexCoords[11])*0.0215963866053;
    gl_FragColor += texture2D(s_texture, v_blurTexCoords[12])*0.00895781211794;
    gl_FragColor += texture2D(s_texture, v_blurTexCoords[13])*0.0044299121055113265;
}
