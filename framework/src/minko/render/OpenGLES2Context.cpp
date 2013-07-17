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

#include "OpenGLES2Context.hpp"

#include "minko/render/CompareMode.hpp"
#include "minko/render/WrapMode.hpp"
#include "minko/render/TextureFilter.hpp"
#include "minko/render/MipFilter.hpp"
#include "minko/render/TriangleCulling.hpp"

#define GL_GLEXT_PROTOTYPES
#ifdef __APPLE__
# include <OpenGL/gl.h>
#elif _WIN32
# include "GL/glew.h"
#else
# include <GL/gl.h>
#endif

using namespace minko::render;

OpenGLES2Context::BlendFactorsMap OpenGLES2Context::_blendingFactors = OpenGLES2Context::initializeBlendFactorsMap();
OpenGLES2Context::BlendFactorsMap
OpenGLES2Context::initializeBlendFactorsMap()
{
    BlendFactorsMap m;

    m[static_cast<uint>(Blending::Source::ZERO)]                       = GL_ZERO;
    m[static_cast<uint>(Blending::Source::ONE)]                        = GL_ONE;
    m[static_cast<uint>(Blending::Source::SRC_COLOR)]                  = GL_SRC_COLOR;
    m[static_cast<uint>(Blending::Source::ONE_MINUS_SRC_COLOR)]        = GL_ONE_MINUS_SRC_COLOR;
    m[static_cast<uint>(Blending::Source::SRC_ALPHA)]                  = GL_SRC_ALPHA;
    m[static_cast<uint>(Blending::Source::ONE_MINUS_SRC_ALPHA)]        = GL_ONE_MINUS_SRC_ALPHA;
    m[static_cast<uint>(Blending::Source::DST_ALPHA)]                  = GL_DST_ALPHA;
    m[static_cast<uint>(Blending::Source::ONE_MINUS_DST_ALPHA)]        = GL_ONE_MINUS_DST_ALPHA;

    m[static_cast<uint>(Blending::Destination::ZERO)]                  = GL_ZERO;
    m[static_cast<uint>(Blending::Destination::ONE)]                   = GL_ONE;
    m[static_cast<uint>(Blending::Destination::DST_COLOR)]             = GL_DST_COLOR;
    m[static_cast<uint>(Blending::Destination::ONE_MINUS_DST_COLOR)]   = GL_ONE_MINUS_DST_COLOR;
    m[static_cast<uint>(Blending::Destination::ONE_MINUS_DST_ALPHA)]   = GL_ONE_MINUS_DST_ALPHA;
    m[static_cast<uint>(Blending::Destination::ONE_MINUS_SRC_ALPHA)]   = GL_ONE_MINUS_SRC_ALPHA;
    m[static_cast<uint>(Blending::Destination::DST_ALPHA)]             = GL_DST_ALPHA;
    m[static_cast<uint>(Blending::Destination::ONE_MINUS_DST_ALPHA)]   = GL_ONE_MINUS_DST_ALPHA;

    return m;
}

OpenGLES2Context::DepthFuncsMap OpenGLES2Context::_depthFuncs = OpenGLES2Context::initializeDepthFuncsMap();
OpenGLES2Context::DepthFuncsMap
OpenGLES2Context::initializeDepthFuncsMap()
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

OpenGLES2Context::OpenGLES2Context() :
	_viewportX(0),
	_viewportY(0),
	_viewportWidth(0),
	_viewportHeight(0),
	_currentIndexBuffer(-1),
	_currentVertexBuffer(8, -1),
	_currentVertexSize(8, -1),
	_currentVertexStride(8, -1),
	_currentVertexOffset(8, -1),
	_currentTexture(8, -1),
	_currentProgram(-1),
    _currentTriangleCulling(TriangleCulling::BACK)
{
#ifdef _WIN32
    glewInit();
#endif

	glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);

	// init. viewport x, y, width and height
	std::vector<int> viewportSettings(4);
	glGetIntegerv(GL_VIEWPORT, &viewportSettings[0]);
	_viewportX = viewportSettings[0];
	_viewportY = viewportSettings[1];
	_viewportWidth = viewportSettings[2];
	_viewportHeight = viewportSettings[3];

	setDepthTest(true, CompareMode::LESS);
}

OpenGLES2Context::~OpenGLES2Context()
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
OpenGLES2Context::configureViewport(const unsigned int x,
				  				    const unsigned int y,
				  				    const unsigned int width,
				  				    const unsigned int height)
{
	if (x != _viewportX || y != _viewportY || width != _viewportWidth || height != _viewportHeight)
	{
		_viewportX = x;
		_viewportY = y;
		_viewportWidth = width;
		_viewportHeight = height;

		glViewport(x, y, width, height);
	}
}

void
OpenGLES2Context::clear(float 			red,
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
	glClearDepth(depth);

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
OpenGLES2Context::present()
{
	// http://www.opengl.org/sdk/docs/man/xhtml/glFlush.xml
	//
	// force execution of GL commands in finite time
	//glFlush();

#ifdef DEBUG
    if (glGetError() != 0)
        throw;
#endif

    setRenderToBackBuffer();
}

void
OpenGLES2Context::drawTriangles(const unsigned int indexBuffer, const int numTriangles)
{
	if (_currentIndexBuffer != indexBuffer)
	{
		_currentIndexBuffer = indexBuffer;

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
	}

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
OpenGLES2Context::createVertexBuffer(const unsigned int size)
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
	glBufferData(GL_ARRAY_BUFFER, size * sizeof(GLfloat), 0, GL_DYNAMIC_DRAW);

	_vertexBuffers.push_back(vertexBuffer);

	return vertexBuffer;
}

void
OpenGLES2Context::uploadVertexBufferData(const unsigned int 	vertexBuffer,
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
	glBufferSubData(GL_ARRAY_BUFFER, offset * sizeof(GLfloat), size * sizeof(GLfloat), data);
}

void
OpenGLES2Context::deleteVertexBuffer(const unsigned int vertexBuffer)
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
OpenGLES2Context::setVertexBufferAt(const unsigned int	position,
								    const unsigned int	vertexBuffer,
								    const unsigned int	size,
								    const unsigned int	stride,
								    const unsigned int	offset)
{
	auto currentVertexBuffer = _currentVertexBuffer[position];

	if (currentVertexBuffer == vertexBuffer
		&& _currentVertexSize[position] == size
		&& _currentVertexStride[position] == stride
		&& _currentVertexOffset[position] == position)
		return ;

	_currentVertexBuffer[position] = vertexBuffer;
	_currentVertexSize[position] = size;
	_currentVertexStride[position] = stride;
	_currentVertexOffset[position] = offset;

	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);

	// http://www.khronos.org/opengles/sdk/docs/man/xhtml/glVertexAttribPointer.xml
	glVertexAttribPointer(
		position,
		size,
		GL_FLOAT,
		GL_FALSE,
		sizeof(GLfloat) * stride,
		(void*)(sizeof(GLfloat) * offset)
	);

	if (currentVertexBuffer < 0)
		glEnableVertexAttribArray(position);
}

const unsigned int
OpenGLES2Context::createIndexBuffer(const unsigned int size)
{
	unsigned int indexBuffer;

	glGenBuffers(1, &indexBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, size * sizeof(GLushort), 0, GL_DYNAMIC_DRAW);

	_indexBuffers.push_back(indexBuffer);

	return indexBuffer;
}

void
OpenGLES2Context::uploaderIndexBufferData(const unsigned int 	indexBuffer,
										  const unsigned int 	offset,
										  const unsigned int 	size,
										  void*					data)
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
	glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, offset * sizeof(GLushort), size * sizeof(GLushort), data);
}

void
OpenGLES2Context::deleteIndexBuffer(const unsigned int indexBuffer)
{
	_indexBuffers.erase(std::find(_indexBuffers.begin(), _indexBuffers.end(), indexBuffer));

	glDeleteBuffers(1, &indexBuffer);
}

const unsigned int
OpenGLES2Context::createTexture(unsigned int 	width,
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
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
		}
	else
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);

	_textures.push_back(texture);

    if (optimizeForRenderToTexture)
        createRTTBuffers(texture, width, height);

	return texture;
}

void
OpenGLES2Context::uploadTextureData(const unsigned int 	texture,
								    unsigned int 		width,
								    unsigned int 		height,
								    unsigned int 		mipLevel,
								    void*				data)
{
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexImage2D(GL_TEXTURE_2D, mipLevel, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
}

void
OpenGLES2Context::deleteTexture(const unsigned int texture)
{
	_textures.erase(std::find(_textures.begin(), _textures.end(), texture));

	glDeleteTextures(1, &texture);

    if (_frameBuffers.count(texture))
    {
        glDeleteFramebuffers(1, &_frameBuffers[texture]);
        _frameBuffers.erase(texture);

        glDeleteRenderbuffers(1, &_renderBuffers[texture]);
        _renderBuffers.erase(texture);
    }
}

void
OpenGLES2Context::setTextureAt(const unsigned int	position,
							   const int			texture,
							   const int			location)
{
	auto textureIsValid = texture > 0;

	if (_currentTexture[position] != texture)
	{
		_currentTexture[position] = texture;

		glActiveTexture(GL_TEXTURE0 + position);
		glBindTexture(GL_TEXTURE_2D, texture);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	}

	if (textureIsValid && location >= 0)
		glUniform1i(location, position);
}

void
OpenGLES2Context::setSamplerStateAt(const unsigned int position, WrapMode wrapping, TextureFilter filtering, MipFilter mipFiltering)
{
    glActiveTexture(GL_TEXTURE0 + position);

    switch (wrapping)
    {
    case WrapMode::CLAMP :
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        break;
    case WrapMode::REPEAT :
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        break;
    }

    switch (filtering)
    {
    case TextureFilter::NEAREST :
        switch (mipFiltering)
        {
        case MipFilter::NONE :
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            break;
        case MipFilter::NEAREST :
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
            break;
        case MipFilter::LINEAR :
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
            break;
        }

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        break;
    case TextureFilter::LINEAR :
        switch (mipFiltering)
        {
        case MipFilter::NONE :
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            break;
        case MipFilter::NEAREST :
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
            break;
        case MipFilter::LINEAR :
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            break;
        }

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        break;
    }
}

const unsigned int
OpenGLES2Context::createProgram()
{
	return glCreateProgram();
}

void
OpenGLES2Context::attachShader(const unsigned int program, const unsigned int shader)
{
	glAttachShader(program, shader);
}

void
OpenGLES2Context::linkProgram(const unsigned int program)
{
	glLinkProgram(program);
}

void
OpenGLES2Context::deleteProgram(const unsigned int program)
{
	_programs.erase(std::find(_programs.begin(), _programs.end(), program));

	glDeleteProgram(program);
}

void
OpenGLES2Context::compileShader(const unsigned int shader)
{
	glCompileShader(shader);
}

void
OpenGLES2Context::setProgram(const unsigned int program)
{
	if (_currentProgram == program)
		return;

	_currentProgram = program;

	glUseProgram(program);
}

void
OpenGLES2Context::setShaderSource(const unsigned int shader,
							      const std::string& source)
{
	const char* sourceString = source.c_str();

	glShaderSource(shader, 1, &sourceString, 0);
}

const unsigned int
OpenGLES2Context::createVertexShader()
{
	unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);

	_vertexShaders.push_back(vertexShader);

	return vertexShader;
}

void
OpenGLES2Context::deleteVertexShader(const unsigned int vertexShader)
{
	_vertexShaders.erase(std::find(_vertexShaders.begin(), _vertexShaders.end(), vertexShader));

	glDeleteShader(vertexShader);
}

const unsigned int
OpenGLES2Context::createFragmentShader()
{
	unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

	_fragmentShaders.push_back(fragmentShader);

	return fragmentShader;
}

void
OpenGLES2Context::deleteFragmentShader(const unsigned int fragmentShader)
{
	_fragmentShaders.erase(std::find(_fragmentShaders.begin(), _fragmentShaders.end(), fragmentShader));

	glDeleteShader(fragmentShader);
}

std::shared_ptr<ProgramInputs>
OpenGLES2Context::getProgramInputs(const unsigned int program)
{
	std::vector<std::string> names;
	std::vector<ProgramInputs::Type> types;
	std::vector<unsigned int> locations;

	fillUniformInputs(program, names, types, locations);
	fillAttributeInputs(program, names, types, locations);

	return ProgramInputs::create(shared_from_this(), program, names, types, locations);
}

void
OpenGLES2Context::fillUniformInputs(const unsigned int					program,
								    std::vector<std::string>&			names,
								    std::vector<ProgramInputs::Type>&	types,
								    std::vector<unsigned int>&			locations)
{
	int total = -1;
	int maxUniformNameLength = -1;

	glUseProgram(program);
	glGetProgramiv(program, GL_ACTIVE_UNIFORM_MAX_LENGTH, &maxUniformNameLength);
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
OpenGLES2Context::fillAttributeInputs(const unsigned int				program,
									 std::vector<std::string>&			names,
								     std::vector<ProgramInputs::Type>&	types,
								     std::vector<unsigned int>&			locations)
{
	int total = -1;
	int maxAttributeNameLength = -1;

	glUseProgram(program);
	glGetProgramiv(program, GL_ACTIVE_ATTRIBUTE_MAX_LENGTH, &maxAttributeNameLength);
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
		    names.push_back(std::string(&name[0], nameLength));
		    types.push_back(inputType);
		    locations.push_back(location);
		}
	}
}

std::string
OpenGLES2Context::getShaderCompilationLogs(const unsigned int shader)
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
OpenGLES2Context::getProgramInfoLogs(const unsigned int program)
{
	int programInfoMaxLength = -1;
	int programInfoLength = -1;

	glGetProgramiv(program, GL_INFO_LOG_LENGTH, &programInfoMaxLength);

	std::vector<char> programInfo(programInfoMaxLength);

	glGetProgramInfoLog(program, programInfoMaxLength, &programInfoLength, &programInfo[0]);

	return std::string(&programInfo[0]);
}

void
OpenGLES2Context::setUniform(unsigned int location, float value)
{
	glUniform1f(location, value);
}

void
OpenGLES2Context::setUniform(unsigned int location, float value1, float value2)
{
	glUniform2f(location, value1, value2);
}

void
OpenGLES2Context::setUniform(unsigned int location, float value1, float value2, float value3)
{
	glUniform3f(location, value1, value2, value3);
}

void
OpenGLES2Context::setUniform(unsigned int location, float value1, float value2, float value3, float value4)
{
	glUniform4f(location, value1, value2, value3, value4);
}

void
OpenGLES2Context::setUniformMatrix4x4(unsigned int location, unsigned int size, bool transpose, const float* values)
{
	glUniformMatrix4fv(location, size, transpose, values);
}

void
OpenGLES2Context::setBlendMode(Blending::Source source, Blending::Destination destination)
{
	if ((static_cast<uint>(source) | static_cast<uint>(destination)) != static_cast<uint>(_currentBlendMode))
	{
		_currentBlendMode = (Blending::Mode)((uint)source | (uint)destination);

		glBlendFunc(
			_blendingFactors[static_cast<uint>(source) & 0x00ff],
			_blendingFactors[static_cast<uint>(destination) & 0xff00]
		);
	}
}

void
OpenGLES2Context::setBlendMode(Blending::Mode blendMode)
{
	if (blendMode != _currentBlendMode)
	{
		_currentBlendMode = blendMode;

		glBlendFunc(
			_blendingFactors[static_cast<uint>(blendMode) & 0x00ff],
			_blendingFactors[static_cast<uint>(blendMode) & 0xff00]
		);
	}
}

void
OpenGLES2Context::setDepthTest(bool depthMask, CompareMode depthFunc)
{
	if (depthMask != _currentDepthMask || depthFunc != _currentDepthFunc)
	{
		_currentDepthMask = depthMask;
		_currentDepthFunc = depthFunc;

		glDepthMask(depthMask);
		glDepthFunc(_depthFuncs[depthFunc]);
	}
}

void
OpenGLES2Context::readPixels(unsigned char* pixels)
{
	glReadPixels(_viewportX, _viewportY, _viewportWidth, _viewportHeight, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
}

void
OpenGLES2Context::setTriangleCulling(TriangleCulling triangleCulling)
{
    if (triangleCulling == _currentTriangleCulling)
        return;

    if (_currentTriangleCulling == TriangleCulling::NONE)
        glEnable(GL_CULL_FACE);
    _currentTriangleCulling = triangleCulling;

    switch (triangleCulling)
    {
    case TriangleCulling::NONE:
        glDisable(GL_CULL_FACE);
        break;
    case TriangleCulling::BACK :
        glCullFace(GL_BACK);
        break;
    case TriangleCulling::FRONT :
        glCullFace(GL_FRONT);
        break;
    case TriangleCulling::BOTH :
        glCullFace(GL_FRONT_AND_BACK);
        break;
    }
}

void
OpenGLES2Context::setRenderToBackBuffer()
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
}

void
OpenGLES2Context::setRenderToTexture(unsigned int texture, bool enableDepthAndStencil)
{
    if (_frameBuffers.count(texture) == 0)
        throw std::logic_error("this texture cannot be used for RTT");

    glBindFramebuffer(GL_FRAMEBUFFER, _frameBuffers[texture]);
    if (enableDepthAndStencil)
        glBindRenderbuffer(GL_RENDERBUFFER, _renderBuffers[texture]);
}

void
OpenGLES2Context::createRTTBuffers(unsigned int texture, unsigned int width, unsigned int height)
{
    unsigned int frameBuffer = -1;

    // create a framebuffer object 
    glGenFramebuffers(1, &frameBuffer);
    // bind the framebuffer object 
    glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
    // attach a texture to the FBO
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);

    unsigned int renderBuffer = -1;

    // gen renderbuffer
    glGenRenderbuffers(1, &renderBuffer);
    // bind renderbuffer
    glBindRenderbuffer(GL_RENDERBUFFER, renderBuffer);
    // init as a depth buffer
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, width, height);
    // attach to the FBO for depth
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, renderBuffer);

    // unbind
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);

    _frameBuffers[texture] = frameBuffer;
    _renderBuffers[texture] = renderBuffer;
}