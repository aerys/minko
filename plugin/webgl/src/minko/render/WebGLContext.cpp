/*
Copyright (c) 2014 Aerys

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

#include "minko/render/WebGLContext.hpp"

#include <GLES2/gl2.h>
#include <EGL/egl.h>

using namespace minko::render;

WebGLContext::WebGLContext() :
    OpenGLES2Context()
{
}

void
WebGLContext::setUniform(const unsigned int&    location,
                         const unsigned int&    size,
                         bool                   transpose,
                         const float*           values)
{
    if (transpose)
    {
        float* transposed = new float[size << 4];

        for (uint i = 0; i < size; ++i)
        {
            const float* matrix  = values + (i << 4);
            float*       tmatrix = transposed + (i << 4);

            tmatrix[0]  = matrix[0];
            tmatrix[1]  = matrix[4];
            tmatrix[2]  = matrix[8];
            tmatrix[3]  = matrix[12];
            tmatrix[4]  = matrix[1];
            tmatrix[5]  = matrix[5];
            tmatrix[6]  = matrix[9];
            tmatrix[7]  = matrix[13];
            tmatrix[8]  = matrix[2];
            tmatrix[9]  = matrix[6];
            tmatrix[10] = matrix[10];
            tmatrix[11] = matrix[14];
            tmatrix[12] = matrix[3];
            tmatrix[13] = matrix[7];
            tmatrix[14] = matrix[11];
            tmatrix[15] = matrix[15];
        }

        glUniformMatrix4fv(location, size, false, transposed);

        delete[] transposed;
    }
    else
        glUniformMatrix4fv(location, size, false, values);
}
