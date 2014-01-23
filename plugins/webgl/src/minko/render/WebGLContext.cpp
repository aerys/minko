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

#include "minko/render/WebGLContext.hpp"

#ifdef EMSCRIPTEN
# include <GLES2/gl2.h>
# include <EGL/egl.h>
#endif // EMSCRIPTEN

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

void
WebGLContext::fillUniformInputs(const unsigned int						program,
								   std::vector<std::string>&				names,
								   std::vector<ProgramInputs::Type>&	types,
								   std::vector<unsigned int>&				locations)
{
	int total = -1;
	int maxUniformNameLength = 128;

	glUseProgram(program);
	glGetProgramiv(program, GL_ACTIVE_UNIFORMS, &total);

	for (int i = 0; i < total; ++i)
	{
    	int nameLength = -1;
    	int size = -1;
    	GLenum type = GL_ZERO;
    	std::vector<char> name(maxUniformNameLength);

    	glGetActiveUniform(program, i, maxUniformNameLength, &nameLength, &size, &type, &name[0]);

	    name[nameLength] = 0;

	    ProgramInputs::Type	inputType	= convertInputType(type);
	    int					location	= glGetUniformLocation(program, &name[0]);

	    if (location >= 0 && inputType != ProgramInputs::Type::unknown)
	    {
		    names.push_back(std::string(&name[0], nameLength));
		    types.push_back(inputType);
		    locations.push_back(location);
		}
	}
}

void
WebGLContext::fillAttributeInputs(const unsigned int							program,
									 std::vector<std::string>&					names,
								     std::vector<ProgramInputs::Type>&	types,
								     std::vector<unsigned int>&					locations)
{
	int total = -1;
	int maxAttributeNameLength = 128;

	glUseProgram(program);
	glGetProgramiv(program, GL_ACTIVE_ATTRIBUTES, &total);

	for (int i = 0; i < total; ++i)
	{
    	int nameLength = -1;
    	int size = -1;
    	GLenum type = GL_ZERO;
    	std::vector<char> name(maxAttributeNameLength);

		glGetActiveAttrib(program, i, maxAttributeNameLength, &nameLength, &size, &type, &name[0]);

	    name[nameLength] = 0;

	    ProgramInputs::Type inputType = ProgramInputs::Type::attribute;

		int location = glGetAttribLocation(program, &name[0]);

	    if (location >= 0)
	    {
		    names.push_back(std::string(&name[0]));
		    types.push_back(inputType);
		    locations.push_back(location);
		}
	}
}

void
WebGLContext::setUniform(const unsigned int& location, const unsigned int& size, bool transpose, const float* values)
{
	if (transpose)
	{
		static float	transposedValues[16];

		transposedValues[0] = values[0];
		transposedValues[1] = values[4];
		transposedValues[2] = values[8];
		transposedValues[3] = values[12];
		transposedValues[4] = values[1];
		transposedValues[5] = values[5];
		transposedValues[6] = values[9];
		transposedValues[7] = values[13];
		transposedValues[8] = values[2];
		transposedValues[9] = values[6];
		transposedValues[10] = values[10];
		transposedValues[11] = values[14];
		transposedValues[12] = values[3];
		transposedValues[13] = values[7];
		transposedValues[14] = values[11];
		transposedValues[15] = values[15];

		glUniformMatrix4fv(location, size, false, transposedValues);
	}
	else
		glUniformMatrix4fv(location, size, false, values);
}
