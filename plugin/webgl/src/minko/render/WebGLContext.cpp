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
WebGLContext::setShaderSource(const unsigned int shader,
							  const std::string& source)
{
	const std::string src = "#version 100\n" + source;
	const char* sourceString = src.c_str();
	
	glShaderSource(shader, 1, &sourceString, 0);
	
	checkForErrors();
}

std::vector<ProgramInputs::UniformInput>
WebGLContext::getUniformInputs(const uint program)
{
    std::vector<ProgramInputs::UniformInput> inputs;

    int total = -1;
    int maxUniformNameLength = 128;

    //glGetProgramiv(program, GL_ACTIVE_UNIFORM_MAX_LENGTH, &maxUniformNameLength);
    glGetProgramiv(program, GL_ACTIVE_UNIFORMS, &total);

    for (int i = 0; i < total; ++i)
    {
        int nameLength = -1;
        int size = -1;
        GLenum type = GL_ZERO;
        std::vector<char> name(maxUniformNameLength);

        glGetActiveUniform(program, i, maxUniformNameLength, &nameLength, &size, &type, &name[0]);
        checkForErrors();

        name[nameLength] = 0;

        ProgramInputs::Type	inputType = convertInputType(type);
        int					location = glGetUniformLocation(program, &name[0]);

        if (location >= 0 && inputType != ProgramInputs::Type::unknown)
            inputs.push_back({ std::string(&name[0], nameLength), inputType, location });
    }

    return inputs;
}

std::vector<ProgramInputs::AttributeInput>
WebGLContext::getAttributeInputs(const uint program)
{
    std::vector<ProgramInputs::AttributeInput> inputs;

    int total = -1;
    int maxAttributeNameLength = 128;

    //glGetProgramiv(program, GL_ACTIVE_ATTRIBUTE_MAX_LENGTH, &maxAttributeNameLength);
    glGetProgramiv(program, GL_ACTIVE_ATTRIBUTES, &total);

    for (int i = 0; i < total; ++i)
    {
        int nameLength = -1;
        int size = -1;
        GLenum type = GL_ZERO;
        std::vector<char> name(maxAttributeNameLength);

        glGetActiveAttrib(program, i, maxAttributeNameLength, &nameLength, &size, &type, &name[0]);
        checkForErrors();

        name[nameLength] = 0;

        int location = glGetAttribLocation(program, &name[0]);

        if (location >= 0)
            inputs.push_back({ std::string(&name[0], nameLength), location });
    }

    return inputs;
}
