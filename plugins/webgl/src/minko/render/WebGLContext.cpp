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

#include <minko/render/CompareMode.hpp>

using namespace minko::render;

WebGLContext::BlendFactorsMap WebGLContext::_blendingFactors = WebGLContext::initializeBlendFactorsMap();

WebGLContext::BlendFactorsMap
WebGLContext::initializeBlendFactorsMap()
{
    BlendFactorsMap m;

    m[(uint)Blending::Source::ZERO]                       = GL_ZERO;
    m[(uint)Blending::Source::ONE]                        = GL_ONE;
    m[(uint)Blending::Source::SRC_COLOR]                  = GL_SRC_COLOR;
    m[(uint)Blending::Source::ONE_MINUS_SRC_COLOR]        = GL_ONE_MINUS_SRC_COLOR;
    m[(uint)Blending::Source::SRC_ALPHA]                  = GL_SRC_ALPHA;
    m[(uint)Blending::Source::ONE_MINUS_SRC_ALPHA]        = GL_ONE_MINUS_SRC_ALPHA;
    m[(uint)Blending::Source::DST_ALPHA]                  = GL_DST_ALPHA;
    m[(uint)Blending::Source::ONE_MINUS_DST_ALPHA]        = GL_ONE_MINUS_DST_ALPHA;

    m[(uint)Blending::Destination::ZERO]                  = GL_ZERO;
    m[(uint)Blending::Destination::ONE]                   = GL_ONE;
    m[(uint)Blending::Destination::DST_COLOR]             = GL_DST_COLOR;
    m[(uint)Blending::Destination::ONE_MINUS_DST_COLOR]   = GL_ONE_MINUS_DST_COLOR;
    m[(uint)Blending::Destination::ONE_MINUS_DST_ALPHA]   = GL_ONE_MINUS_DST_ALPHA;
    m[(uint)Blending::Destination::ONE_MINUS_SRC_ALPHA]   = GL_ONE_MINUS_SRC_ALPHA;
    m[(uint)Blending::Destination::DST_ALPHA]             = GL_DST_ALPHA;
    m[(uint)Blending::Destination::ONE_MINUS_DST_ALPHA]   = GL_ONE_MINUS_DST_ALPHA;

    return m;
}

WebGLContext::DepthFuncsMap WebGLContext::_depthFuncs = WebGLContext::initializeDepthFuncsMap();
WebGLContext::DepthFuncsMap
WebGLContext::initializeDepthFuncsMap()
{
	DepthFuncsMap m;

	m[CompareMode::ALWAYS]			= GL_ALWAYS;
	m[CompareMode::EQUAL]			= GL_EQUAL;
	m[CompareMode::GREATER]			= GL_GREATER;
	m[CompareMode::GREATER_EQUAL]	= GL_GREATER | GL_EQUAL;
	m[CompareMode::LESS]			= GL_LESS;
	m[CompareMode::LESS_EQUAL]		= GL_LESS | GL_EQUAL;
	m[CompareMode::NEVER]			= GL_NEVER;
	m[CompareMode::NOT_EQUAL]		= GL_NOTEQUAL;

	return m;
}

WebGLContext::WebGLContext()
{
	glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
}

WebGLContext::~WebGLContext()
{
	for (auto& vertexBuffer : _vertexBuffers)
		glDeleteBuffers(1, &vertexBuffer);

	for (auto& indexBuffer : _indexBuffers)
		glDeleteBuffers(1, &indexBuffer);

	for (auto& texture : _textures)
		glDeleteTextures(1, &texture);

	for (auto& program : _programs)
		glDeleteProgram(program);

	for (auto& vertexShader : _vertexShaders)
		glDeleteShader(vertexShader);

	for (auto& fragmentShader : _fragmentShaders)
		glDeleteShader(fragmentShader);
}

void
WebGLContext::configureViewport(const unsigned int x,
				  				   const unsigned int y,
				  				   const unsigned int width,
				  				   const unsigned int height)
{
	glViewport(x, y, width, height);
}

void
WebGLContext::clear(float 			red,
					   float 			green,
					   float 			blue,
					   float 			alpha,
					   float 			depth,
					   unsigned int 	stencil,
					   unsigned int 	mask)
{
	// http://www.opengl.org/sdk/docs/man/xhtml/glClearColor.xml
	//
	// void glClearColor(GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha);
	// red, green, blue, alpha Specify the red, green, blue, and alpha values used when the color buffers are cleared.
	// The initial values are all 0.
	//
	// glClearColor specify clear values for the color buffers
	glClearColor(red, green, blue, alpha);


	// http://www.opengl.org/sdk/docs/man/xhtml/glClearDepth.xml
	//
	// void glClearDepth(GLdouble depth);
	// void glClearDepthf(GLfloat depth);
	// depth Specifies the depth value used when the depth buffer is cleared. The initial value is 1.
	//
	// glClearDepth specify the clear value for the depth buffer
	#ifdef EMSCRIPTEN
	glClearDepthf(depth);
#else // EMSCRIPTEN
	glClearDepth(depth);
#endif // EMSCRIPTEN

	// http://www.opengl.org/sdk/docs/man/xhtml/glClearStencil.xml
	//
	// void glClearStencil(GLint s)
	// Specifies the index used when the stencil buffer is cleared. The initial value is 0.
	//
	// glClearStencil specify the clear value for the stencil buffer
	glClearStencil(stencil);

	// http://www.opengl.org/sdk/docs/man/xhtml/glClear.xml
	//
	// void glClear(GLbitfield mask);
	// mask
	// Bitwise OR of masks that indicate the buffers to be cleared. The three masks are GL_COLOR_BUFFER_BIT,
	// GL_DEPTH_BUFFER_BIT, and GL_STENCIL_BUFFER_BIT.
	//
	// glClear clear buffers to preset values
	glClear((GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT |GL_STENCIL_BUFFER_BIT) & mask);
}

void
WebGLContext::present()
{
	// http://www.opengl.org/sdk/docs/man/xhtml/glFlush.xml
	//
	// force execution of GL commands in finite time
	//glFlush();
}

void
WebGLContext::drawTriangles(const unsigned int indexBuffer, const int numTriangles)
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);

	// http://www.opengl.org/sdk/docs/man/xhtml/glDrawElements.xml
	//
	// void glDrawElements(GLenum mode, GLsizei count, GLenum type, const GLvoid* indices);
	// mode Specifies what kind of primitives to render.
	// count Specifies the number of elements to be rendered.
	// type Specifies the type of the values in indices.
	// indices Specifies a pointer to the location where the indices are stored.
	//
	// glDrawElements render primitives from array data
	glDrawElements(GL_TRIANGLES, numTriangles, GL_UNSIGNED_SHORT, (void*)0);
}

const unsigned int
WebGLContext::createVertexBuffer(const unsigned int size)
{
	unsigned int vertexBuffer;

	// http://www.opengl.org/sdk/docs/man/xhtml/glGenBuffers.xml
	//
	// void glGenBuffers(GLsizei n, GLuint* buffers);
	// n Specifies the number of buffer object names to be vertexBufferd.
	// buffers Specifies an array in which the generated buffer object names are stored.
	//
	// glGenBuffers returns n buffer object names in buffers. There is no
	// guarantee that the names form a contiguous set of integers; however,
	// it is guaranteed that none of the returned names was in use immediately
	// before the call to glGenBuffers.
	glGenBuffers(1, &vertexBuffer);

	// http://www.opengl.org/sdk/docs/man/xhtml/glBindBuffer.xml
	//
	// void glBindBuffer(GLenum target, GLuint buffer);
	// target Specifies the target to which the buffer object is bound.
	// buffer Specifies the name of a buffer object.
	//
	// glBindBuffer binds a buffer object to the specified buffer binding point.
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);

	// http://www.opengl.org/sdk/docs/man/xhtml/glBufferData.xml
	//
	// void glBufferData(GLenum target, GLsizeiptr size, const GLvoid* data, GLenum usage)
	// target Specifies the target buffer object.
	// size Specifies the size in bytes of the buffer object's new data store.
	// data Specifies a pointer to data that will be copied into the data store for initialization, or NULL if no data is to be copied.
	// usage Specifies the expected usage pattern of the data store.
	//
	// glBufferData creates and initializes a buffer object's data store
	glBufferData(GL_ARRAY_BUFFER, size * sizeof(GL_FLOAT), 0, GL_DYNAMIC_DRAW);

	_vertexBuffers.push_back(vertexBuffer);

	return vertexBuffer;
}

void
WebGLContext::uploadVertexBufferData(const unsigned int 	vertexBuffer,
									    const unsigned int 	offset,
									    const unsigned int 	size,
									    void* 				data)
{
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);

	// http://www.opengl.org/sdk/docs/man/xhtml/glBufferSubData.xml
	//
	// void glBufferSubData(GLenum target, GLintptr offset, GLsizeiptr size, const GLvoid* data);
	// target Specifies the target buffer object
	// offset Specifies the offset into the buffer object's data store where data replacement will begin, measured in bytes.
	// size Specifies the size in bytes of the data store region being replaced.
	// data Specifies a pointer to the new data that will be copied into the data store.
	//
	// glBufferSubData updates a subset of a buffer object's data store
	glBufferSubData(GL_ARRAY_BUFFER, offset * sizeof(GL_FLOAT), size * sizeof(GL_FLOAT), data);
}

void
WebGLContext::deleteVertexBuffer(const unsigned int vertexBuffer)
{
	_vertexBuffers.erase(std::find(_vertexBuffers.begin(), _vertexBuffers.end(), vertexBuffer));

	// http://www.opengl.org/sdk/docs/man/xhtml/glDeleteBuffers.xml
	//
	// void glDeleteBuffers(GLsizei n, const GLuint* buffers)
	// n Specifies the number of buffer objects to be deleted.
	// buffers Specifies an array of buffer objects to be deleted.
	//
	// glDeleteBuffers deletes n buffer objects named by the elements of the array buffers. After a buffer object is
	// deleted, it has no contents, and its name is free for reuse (for example by glGenBuffers). If a buffer object
	// that is currently bound is deleted, the binding reverts to 0 (the absence of any buffer object).
	glDeleteBuffers(1, &vertexBuffer);
}

void
WebGLContext::setVertexBufferAt(const unsigned int	position,
								    const unsigned int	vertexBuffer,
								    const unsigned int	size,
								    const unsigned int	stride,
								    const unsigned int	offset)
{
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);

	// http://www.khronos.org/opengles/sdk/docs/man/xhtml/glVertexAttribPointer.xml
	glVertexAttribPointer(
		position,
		size,
		GL_FLOAT,
		GL_FALSE,
		sizeof(GL_FLOAT) * stride,
		(void*)(sizeof(GL_FLOAT) * offset)
	);

	glEnableVertexAttribArray(position);
}

const unsigned int
WebGLContext::createIndexBuffer(const unsigned int size)
{
	unsigned int indexBuffer;

	glGenBuffers(1, &indexBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, size * sizeof(GL_UNSIGNED_SHORT), 0, GL_DYNAMIC_DRAW);

	_indexBuffers.push_back(indexBuffer);

	return indexBuffer;
}

void
WebGLContext::uploaderIndexBufferData(const unsigned int 	indexBuffer,
										  const unsigned int 	offset,
										  const unsigned int 	size,
										  void*					data)
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
	glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, offset * sizeof(GL_UNSIGNED_SHORT), size * sizeof(GL_UNSIGNED_SHORT), data);
}

void
WebGLContext::deleteIndexBuffer(const unsigned int indexBuffer)
{
	_indexBuffers.erase(std::find(_indexBuffers.begin(), _indexBuffers.end(), indexBuffer));

	glDeleteBuffers(1, &indexBuffer);
}

const unsigned int
WebGLContext::createTexture(unsigned int 	width,
							    unsigned int 	height,
							    bool			mipMapping)
{
	unsigned int texture;

	// make sure width is a power of 2
	if (!((width != 0) && !(width & (width - 1))))
		throw ;

	// make sure height is a power of 2
	if (!((height != 0) && !(height & (height - 1))))
		throw ;

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

	return texture;
}

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
WebGLContext::deleteTexture(const unsigned int texture)
{
	_textures.erase(std::find(_textures.begin(), _textures.end(), texture));

	glDeleteTextures(1, &texture);
}

void
WebGLContext::setTextureAt(const unsigned int	position,
							   const int			texture,
							   const int			location)
{
	glActiveTexture(GL_TEXTURE0 + position);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	if (location >= 0)
		glUniform1i(location, position);
}

const unsigned int
WebGLContext::createProgram()
{
	return glCreateProgram();
}

void
WebGLContext::attachShader(const unsigned int program, const unsigned int shader)
{
	glAttachShader(program, shader);
}

void
WebGLContext::linkProgram(const unsigned int program)
{
	glLinkProgram(program);
}

void
WebGLContext::deleteProgram(const unsigned int program)
{
	_programs.erase(std::find(_programs.begin(), _programs.end(), program));

	glDeleteProgram(program);
}

void
WebGLContext::compileShader(const unsigned int shader)
{
	glCompileShader(shader);
}

void
WebGLContext::setProgram(const unsigned int program)
{
	glUseProgram(program);
}

void
WebGLContext::setShaderSource(const unsigned int shader,
							     const std::string& source)
{
	const char* sourceString = source.c_str();

	glShaderSource(shader, 1, &sourceString, 0);
}

const unsigned int
WebGLContext::createVertexShader()
{
	unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);

	_vertexShaders.push_back(vertexShader);

	return vertexShader;
}

void
WebGLContext::deleteVertexShader(const unsigned int vertexShader)
{
	_vertexShaders.erase(std::find(_vertexShaders.begin(), _vertexShaders.end(), vertexShader));

	glDeleteShader(vertexShader);
}

const unsigned int
WebGLContext::createFragmentShader()
{
	unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

	_fragmentShaders.push_back(fragmentShader);

	return fragmentShader;
}

void
WebGLContext::deleteFragmentShader(const unsigned int fragmentShader)
{
	_fragmentShaders.erase(std::find(_fragmentShaders.begin(), _fragmentShaders.end(), fragmentShader));

	glDeleteShader(fragmentShader);
}

std::shared_ptr<ProgramInputs>
WebGLContext::getProgramInputs(const unsigned int program)
{
	std::vector<std::string> names;
	std::vector<ProgramInputs::Type> types;
	std::vector<unsigned int> locations;

	fillUniformInputs(program, names, types, locations);
	fillAttributeInputs(program, names, types, locations);

	return ProgramInputs::create(shared_from_this(), program, names, types, locations);
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

	    if (location >= 0 && type != ProgramInputs::Type::unknown)
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

std::string
WebGLContext::getShaderCompilationLogs(const unsigned int shader)
{
	int compileStatus = -1;

	glGetShaderiv(shader, GL_COMPILE_STATUS, &compileStatus);

	if (compileStatus != GL_TRUE)
	{
		int logsLength = -1;
		char buffer[1024];
		int bufferLength = -1;

		glGetShaderSource(shader, 1024, &bufferLength, buffer);
		std::cout << buffer << bufferLength << std::endl;

		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logsLength);

		if (logsLength > 0)
		{
			std::vector<char> logs(logsLength);

			glGetShaderInfoLog(shader, logsLength, &logsLength, &logs[0]);

			return std::string(&logs[0]);
		}
	}

	return std::string();
}

std::string
WebGLContext::getProgramInfoLogs(const unsigned int program)
{
	int programInfoMaxLength = -1;
	int programInfoLength = -1;

	glGetProgramiv(program, GL_INFO_LOG_LENGTH, &programInfoMaxLength);

	std::vector<char> programInfo(programInfoMaxLength);

	glGetProgramInfoLog(program, programInfoMaxLength, &programInfoLength, &programInfo[0]);

	return std::string(&programInfo[0]);
}

void
WebGLContext::setUniform(unsigned int location, float value)
{
	glUniform1f(location, value);
}

void
WebGLContext::setUniform(unsigned int location, float value1, float value2)
{
	glUniform2f(location, value1, value2);
}

void
WebGLContext::setUniform(unsigned int location, float value1, float value2, float value3)
{
	glUniform3f(location, value1, value2, value3);
}

void
WebGLContext::setUniform(unsigned int location, float value1, float value2, float value3, float value4)
{
	glUniform4f(location, value1, value2, value3, value4);
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

void
WebGLContext::setBlendMode(Blending::Source source, Blending::Destination destination)
{
    glBlendFunc(
        _blendingFactors[(uint)source & 0x00ff],
        _blendingFactors[(uint)destination & 0xff00]
    );
}

void
WebGLContext::setBlendMode(Blending::Mode blendMode)
{
    glBlendFunc(
        _blendingFactors[(uint)blendMode & 0x00ff],
        _blendingFactors[(uint)blendMode & 0xff00]
    );
}

void
WebGLContext::setDepthTest(bool depthMask, CompareMode depthFunc)
{
	//throw std::logic_error("WebGLContext::setDepthTest is not reimplemented yet");
}

void
WebGLContext::readPixels(unsigned char* pixels)
{
	//throw std::logic_error("WebGLContext::readPixels is not reimplemented yet");
}