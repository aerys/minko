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
WebGLContext::uploadTextureData(const unsigned int 	texture,
								    unsigned int 		width,
								    unsigned int 		height,
								    unsigned int 		mipLevel,
								    void*				data)
{
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexImage2D(GL_TEXTURE_2D, mipLevel, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
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

const unsigned int
WebGLContext::createTexture(unsigned int 	width,
							    unsigned int 	height,
							    bool			mipMapping,
                                bool            optimizeForRenderToTexture)
{
	unsigned int texture;

	// make sure width is a power of 2
	if (!((width != 0) && !(width & (width - 1))))
		throw std::invalid_argument("width");

	// make sure height is a power of 2
	if (!((height != 0) && !(height & (height - 1))))
		throw std::invalid_argument("height");

	// http://www.opengl.org/sdk/docs/man/xhtml/glGenTextures.xml
	//
	// void glGenTextures(GLsizei n, GLuint* textures)
	// n Specifies the number of texture names to be generated.
	// textures Specifies an array in which the generated texture names are stored.
	//
	// glGenTextures generate texture names
	glGenTextures(1, &texture);

	// http://www.opengl.org/sdk/docs/man/xhtml/glBindTexture.xml
	//
	// void glBindTexture(GLenum target, GLuint texture);
	// target Specifies the target to which the texture is bound.
	// texture Specifies the name of a texture.
	//
	// glBindTexture bind a named texture to a texturing target
	glBindTexture(GL_TEXTURE_2D, texture);

	// http://www.opengl.org/sdk/docs/man/xhtml/glTexImage2D.xml
	//
	// void glTexImage2D(GLenum target, GLint level, GLint internalFormat, GLsizei width, GLsizei height, GLint border,
	// GLenum format, GLenum type, const GLvoid* data);
	// target Specifies the target texture.
	// level Specifies the level-of-detail number. Level 0 is the base image level. Level n is the nth mipmap reduction
	// image. If target is GL_TEXTURE_RECTANGLE or GL_PROXY_TEXTURE_RECTANGLE, level must be 0.
	// internalFormat Specifies the number of color components in the texture. Must be one of base internal formats given in Table 1,
	// one of the sized internal formats given in Table 2, or one of the compressed internal formats given in Table 3,
	// below.
	// width Specifies the width of the texture image.
	// height Specifies the height of the texture image.
	// border This value must be 0.
	// format Specifies the format of the pixel data.
	// type Specifies the data type of the pixel data
	// data Specifies a pointer to the image data in memory.
	//
	// glTexImage2D specify a two-dimensional texture image
	if (mipMapping)
		for (unsigned int size = width > height ? width : height;
			 size > 0;
			 size = size >> 1, width = width >> 1, height = height >> 1)
		{
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
		}
	else
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);

	_textures.push_back(texture);

    if (optimizeForRenderToTexture)
        createRTTBuffers(texture, width, height);

	return texture;
}

void
WebGLContext::setUniformMatrix4x4(unsigned int location, unsigned int size, bool transpose, const float* values)
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