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

#include "WebGLContext.hpp"

#ifdef EMSCRIPTEN
#include <GLES2/gl2.h>
#include <EGL/egl.h>
#else // EMSCRIPTEN
#include <GL/glew.h>
#endif // EMSCRIPTEN

using namespace minko::render;

WebGLContext::WebGLContext() :
	OpenGLES2Context()
{}


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

	    ProgramInputs::Type inputType = ProgramInputs::Type::unknown;

	    switch (type)
	    {
	    	case GL_FLOAT:
	    		inputType = ProgramInputs::Type::float1;
	    		break;
	    	case GL_INT:
	    		inputType = ProgramInputs::Type::int1;
	    		break;
	    	case GL_FLOAT_VEC2:
	    		inputType = ProgramInputs::Type::float2;
	    		break;
	    	case GL_INT_VEC2:
	    		inputType = ProgramInputs::Type::int2;
		    	break;
	    	case GL_FLOAT_VEC3:
	    		inputType = ProgramInputs::Type::float3;
	    		break;
	    	case GL_INT_VEC3:
	    		inputType = ProgramInputs::Type::int3;
	    		break;
	    	case GL_FLOAT_VEC4:
	    		inputType = ProgramInputs::Type::float4;
	    		break;
	    	case GL_INT_VEC4:
	    		inputType = ProgramInputs::Type::int4;
	    		break;
	    	case GL_FLOAT_MAT3:
	    		inputType = ProgramInputs::Type::float9;
		    	break;
	    	case GL_FLOAT_MAT4:
	    		inputType = ProgramInputs::Type::float16;
	    		break;
			case GL_SAMPLER_2D:
				inputType = ProgramInputs::Type::sampler2d;
				break;
			default:
				throw std::logic_error("unsupported type");
	    }

	    int location = glGetUniformLocation(program, &name[0]);

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
