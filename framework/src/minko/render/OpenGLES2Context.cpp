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

#include "minko/render/OpenGLES2Context.hpp"

#include "minko/render/CompareMode.hpp"
#include "minko/render/WrapMode.hpp"
#include "minko/render/TextureFilter.hpp"
#include "minko/render/MipFilter.hpp"
#include "minko/render/TextureFormat.hpp"
#include "minko/render/TextureFormatInfo.hpp"
#include "minko/render/TriangleCulling.hpp"
#include "minko/render/StencilOperation.hpp"
#include "minko/log/Logger.hpp"

#include <iomanip>

#ifndef GL_GLEXT_PROTOTYPES
# define GL_GLEXT_PROTOTYPES
#endif

#if MINKO_PLATFORM == MINKO_PLATFORM_WINDOWS
# if defined(MINKO_PLUGIN_ANGLE)
#  include "GLES2/gl2.h"
#  include "GLES2/gl2ext.h"
# else
#  if !defined(MINKO_PLUGIN_OFFSCREEN) // temporary
#   include "GL/glew.h"
#   include "GL/wglew.h"
#  else
#   include <windows.h>
#   include <GL/gl.h>
#   include <GL/glu.h>
#   include <GL/glext.h>
#  endif
# endif
#elif MINKO_PLATFORM == MINKO_PLATFORM_OSX
# include <OpenGL/gl3.h>
# include <OpenGL/glext.h>
#elif MINKO_PLATFORM == MINKO_PLATFORM_LINUX
# include <GL/gl.h>
# include <GL/glext.h>
#elif MINKO_PLATFORM == MINKO_PLATFORM_IOS
# include <OpenGLES/ES2/gl.h>
# include <OpenGLES/ES2/glext.h>
#elif MINKO_PLATFORM == MINKO_PLATFORM_ANDROID
# include <GLES2/gl2.h>
# include <GLES2/gl2ext.h>
# include <dlfcn.h>
#elif MINKO_PLATFORM == MINKO_PLATFORM_HTML5
# include <GLES2/gl2.h>
# include <GLES2/gl2ext.h>
# include <EGL/egl.h>
#endif

#ifndef GL_ES_VERSION_2_0
# undef glGenFramebuffers
# define glGenFramebuffers glGenFramebuffersEXT

# undef glBindFramebuffer
# define glBindFramebuffer glBindFramebufferEXT

# undef glFramebufferTexture2D
# define glFramebufferTexture2D glFramebufferTexture2DEXT

# undef glGenRenderbuffers
# define glGenRenderbuffers glGenRenderbuffersEXT

# undef glBindRenderbuffer
# define glBindRenderbuffer glBindRenderbufferEXT

# undef glRenderbufferStorage
# define glRenderbufferStorage glRenderbufferStorageEXT

# undef glFramebufferRenderbuffer
# define glFramebufferRenderbuffer glFramebufferRenderbufferEXT

# undef glCheckFramebufferStatus
# define glCheckFramebufferStatus glCheckFramebufferStatusEXT

# undef glDeleteFramebuffers
# define glDeleteFramebuffers glDeleteFramebuffersEXT

# undef glDeleteRenderbuffers
# define glDeleteRenderbuffers glDeleteRenderbuffersEXT

# undef glGenerateMipmap
# define glGenerateMipmap glGenerateMipmapEXT

#endif

#if MINKO_PLATFORM == MINKO_PLATFORM_ANDROID
PFNGLBINDVERTEXARRAYOESPROC glBindVertexArray;
PFNGLDELETEVERTEXARRAYSOESPROC glDeleteVertexArrays;
PFNGLGENVERTEXARRAYSOESPROC glGenVertexArrays;
PFNGLISVERTEXARRAYOESPROC glIsVertexArray;
#endif

#if MINKO_PLATFORM == MINKO_PLATFORM_IOS || MINKO_PLATFORM == MINKO_PLATFORM_HTML5
# undef glGenVertexArrays
# define glGenVertexArrays glGenVertexArraysOES

# undef glBindVertexArray
# define glBindVertexArray glBindVertexArrayOES

# undef glDeleteVertexArrays
# define glDeleteVertexArrays glDeleteVertexArraysOES
#endif

#if MINKO_PLATFORM == MINKO_PLATFORM_OSX
# undef glGenVertexArrays
# define glGenVertexArrays glGenVertexArraysAPPLE

# undef glBindVertexArray
# define glBindVertexArray glBindVertexArrayAPPLE

# undef glDeleteVertexArrays
# define glDeleteVertexArrays glDeleteVertexArraysAPPLE
#endif

using namespace minko;
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

OpenGLES2Context::CompareFuncsMap OpenGLES2Context::_compareFuncs = OpenGLES2Context::initializeDepthFuncsMap();
OpenGLES2Context::CompareFuncsMap
OpenGLES2Context::initializeDepthFuncsMap()
{
	CompareFuncsMap m;

	m[CompareMode::ALWAYS]			= GL_ALWAYS;
	m[CompareMode::EQUAL]			= GL_EQUAL;
	m[CompareMode::GREATER]			= GL_GREATER;
	m[CompareMode::GREATER_EQUAL]	= GL_GEQUAL;
	m[CompareMode::LESS]			= GL_LESS;
	m[CompareMode::LESS_EQUAL]		= GL_LEQUAL;
	m[CompareMode::NEVER]			= GL_NEVER;
	m[CompareMode::NOT_EQUAL]		= GL_NOTEQUAL;

	return m;
}

OpenGLES2Context::StencilOperationMap OpenGLES2Context::_stencilOps = OpenGLES2Context::initializeStencilOperationsMap();
OpenGLES2Context::StencilOperationMap
OpenGLES2Context::initializeStencilOperationsMap()
{
	StencilOperationMap m;

	m[StencilOperation::KEEP]		= GL_KEEP;
	m[StencilOperation::ZERO]		= GL_ZERO;
	m[StencilOperation::REPLACE]	= GL_REPLACE;
	m[StencilOperation::INCR]		= GL_INCR;
	m[StencilOperation::INCR_WRAP]	= GL_INCR_WRAP;
	m[StencilOperation::DECR]		= GL_DECR;
	m[StencilOperation::DECR_WRAP]	= GL_DECR_WRAP;
	m[StencilOperation::INVERT]		= GL_INVERT;

	return m;
}

std::unordered_map<TextureFormat, uint, Hash<TextureFormat>> OpenGLES2Context::_availableTextureFormats;

OpenGLES2Context::OpenGLES2Context() :
	_errorsEnabled(false),
	_textures(),
	_textureSizes(),
	_textureHasMipmaps(),
	_viewportX(0),
	_viewportY(0),
	_viewportWidth(0),
	_viewportHeight(0),
	_currentTarget(0),
	_currentIndexBuffer(0),
	_currentVertexBuffer(8, 0),
	_currentVertexSize(8, -1),
	_currentVertexStride(8, -1),
	_currentVertexOffset(8, -1),
	_currentBoundTexture(0),
	_currentTexture(8, 0),
	_currentProgram(0),
	_currentTriangleCulling(TriangleCulling::BACK),
	_currentWrapMode(),
	_currentTextureFilter(),
	_currentMipFilter(),
	_currentBlendingMode(Blending::Mode::DEFAULT),
	_currentColorMask(true),
	_currentDepthMask(true),
	_currentDepthFunc(CompareMode::UNSET),
	_currentStencilFunc(CompareMode::UNSET),
	_currentStencilRef(0),
	_currentStencilMask(0x1),
	_currentStencilFailOp(StencilOperation::UNSET),
	_currentStencilZFailOp(StencilOperation::UNSET),
	_currentStencilZPassOp(StencilOperation::UNSET),
	_vertexAttributeEnabled(32u, false),
	_stencilBits(0)
{
#if (MINKO_PLATFORM == MINKO_PLATFORM_WINDOWS) && !defined(MINKO_PLUGIN_ANGLE) && !defined(MINKO_PLUGIN_OFFSCREEN)
	glewInit();
#endif

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);

	const char* glVendor = reinterpret_cast<const char*>(glGetString(GL_VENDOR));
	const char* glRenderer = reinterpret_cast<const char*>(glGetString(GL_RENDERER));
	const char* glVersion = reinterpret_cast<const char*>(glGetString(GL_VERSION));

	_driverInfo = std::string(glVendor ? glVendor : "(unknown vendor)")
		+ " " + std::string(glRenderer ? glRenderer : "(unknown renderer)")
		+ " " + std::string(glVersion ? glVersion : "(unknown version)");

#ifdef GL_ES_VERSION_2_0
	_oglMajorVersion = 2;
	_oglMinorVersion = 0;
#else
	glGetIntegerv(GL_MAJOR_VERSION, &_oglMajorVersion);
	glGetIntegerv(GL_MINOR_VERSION, &_oglMinorVersion);
#endif

	// init. viewport x, y, width and height
	std::vector<int> viewportSettings(4);
	glGetIntegerv(GL_VIEWPORT, &viewportSettings[0]);
	_viewportX = viewportSettings[0];
	_viewportY = viewportSettings[1];
	_viewportWidth = viewportSettings[2];
	_viewportHeight = viewportSettings[3];

	setColorMask(true);
	setDepthTest(true, CompareMode::LESS);

#ifdef GL_ES_VERSION_2_0
	glGetIntegerv(GL_STENCIL_BITS, &_stencilBits);
#else
    glGetFramebufferAttachmentParameteriv(GL_FRAMEBUFFER, GL_STENCIL, GL_FRAMEBUFFER_ATTACHMENT_STENCIL_SIZE, &_stencilBits);
#endif

	if (_stencilBits)
	{
		glEnable(GL_STENCIL_TEST);
		setStencilTest(CompareMode::ALWAYS, 0, 0x1, StencilOperation::KEEP, StencilOperation::KEEP, StencilOperation::KEEP);
	}

	initializeExtFunctions();
}

void
OpenGLES2Context::initializeExtFunctions()
{
#if MINKO_PLATFORM == MINKO_PLATFORM_ANDROID
    void *libhandle = dlopen("libGLESv2.so", RTLD_LAZY);

    glBindVertexArray = (PFNGLBINDVERTEXARRAYOESPROC)dlsym(libhandle, "glBindVertexArrayOES");
    glDeleteVertexArrays = (PFNGLDELETEVERTEXARRAYSOESPROC)dlsym(libhandle, "glDeleteVertexArraysOES");
    glGenVertexArrays = (PFNGLGENVERTEXARRAYSOESPROC)dlsym(libhandle, "glGenVertexArraysOES");
    glIsVertexArray = (PFNGLISVERTEXARRAYOESPROC)dlsym(libhandle, "glIsVertexArrayOES");
#endif
}

OpenGLES2Context::~OpenGLES2Context()
{
	for (auto& vertexBuffer : _vertexBuffers)
		glDeleteBuffers(1, &vertexBuffer);

	for (auto& indexBuffer : _indexBuffers)
		glDeleteBuffers(1, &indexBuffer);

	for (auto& texture : _textures)
		deleteTexture(texture);

	for (auto& program : _programs)
		glDeleteProgram(program);

	for (auto& vertexShader : _vertexShaders)
		glDeleteShader(vertexShader);

	for (auto& fragmentShader : _fragmentShaders)
		glDeleteShader(fragmentShader);
}

void
OpenGLES2Context::configureViewport(const uint x,
									const uint y,
									const uint width,
									const uint height)
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
OpenGLES2Context::clear(float 	red,
						float 	green,
						float 	blue,
						float 	alpha,
						float 	depth,
						uint 	stencil,
						uint 	mask)
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
#ifdef GL_ES_VERSION_2_0
	glClearDepthf(depth);
#else
	glClearDepth(depth);
#endif

	// http://www.opengl.org/sdk/docs/man/xhtml/glClearStencil.xml
	//
	// void glClearStencil(GLint s)
	// Specifies the index used when the stencil buffer is cleared. The initial value is 0.
	//
	// glClearStencil specify the clear value for the stencil buffer
	if (_stencilBits)
		glClearStencil(stencil);

	// http://www.opengl.org/sdk/docs/man/xhtml/glClear.xml
	//
	// void glClear(GLbitfield mask);
	// mask
	// Bitwise OR of masks that indicate the buffers to be cleared. The three masks are GL_COLOR_BUFFER_BIT,
	// GL_DEPTH_BUFFER_BIT, and GL_STENCIL_BUFFER_BIT.
	//
	// glClear clear buffers to preset values
	mask = (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT) & mask;
	if (mask & GL_DEPTH_BUFFER_BIT)
		glDepthMask(_currentDepthMask = true);
	glClear(mask);
}

void
OpenGLES2Context::present()
{
	// http://www.opengl.org/sdk/docs/man/xhtml/glFlush.xml
	//
	// force execution of GL commands in finite time
	//glFlush();

	setRenderToBackBuffer();
}

void
OpenGLES2Context::drawTriangles(const uint indexBuffer, const uint firstIndex, const int numTriangles)
{
	/*if (_currentIndexBuffer != indexBuffer)
	{
		_currentIndexBuffer = indexBuffer;*/

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
	//}

	// http://www.opengl.org/sdk/docs/man/xhtml/glDrawElements.xml
	//
	// void glDrawElements(GLenum mode, GLsizei count, GLenum type, const GLvoid* indices);
	// mode Specifies what kind of primitives to render.
	// count Specifies the number of elements to be rendered.
	// type Specifies the type of the values in indices.
	// indices Specifies a pointer to the location where the indices are stored.
	//
	// glDrawElements render primitives from array data
	glDrawElements(GL_TRIANGLES, numTriangles * 3, GL_UNSIGNED_SHORT, reinterpret_cast<GLvoid*>(firstIndex));

	checkForErrors();
}

void
OpenGLES2Context::drawTriangles(const uint firstIndex, const int numTriangles)
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	_currentIndexBuffer = 0;
	glDrawArrays(GL_TRIANGLES, firstIndex, numTriangles * 3);

	checkForErrors();
}

const uint
OpenGLES2Context::createVertexBuffer(const uint size)
{
	uint vertexBuffer;

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
	glBufferData(GL_ARRAY_BUFFER, size * sizeof(GLfloat), 0, GL_STATIC_DRAW);

	_vertexBuffers.push_back(vertexBuffer);

	checkForErrors();

	return vertexBuffer;
}

void
OpenGLES2Context::uploadVertexBufferData(const uint vertexBuffer,
										 const uint offset,
										 const uint size,
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

	checkForErrors();
}

void
OpenGLES2Context::deleteVertexBuffer(const uint vertexBuffer)
{
	for (auto& currentVertexBuffer : _currentVertexBuffer)
		if (currentVertexBuffer == vertexBuffer)
			currentVertexBuffer = 0;

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

	checkForErrors();
}

void
OpenGLES2Context::setVertexBufferAt(const uint	position,
									const uint	vertexBuffer,
									const uint	size,
									const uint	stride,
									const uint	offset)
{
	bool vertexAttributeEnabled = vertexBuffer > 0;
    bool vertexBufferChanged = (_currentVertexBuffer[position] != vertexBuffer) || vertexAttributeEnabled;

	if (vertexBufferChanged)
	{
		glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
		checkForErrors();

		_currentVertexBuffer[position] = vertexBuffer;
	}

	if (vertexBufferChanged
		|| _currentVertexSize[position] != size
		|| _currentVertexStride[position] != stride
		|| _currentVertexOffset[position] != offset)
	{
		// http://www.khronos.org/opengles/sdk/docs/man/xhtml/glVertexAttribPointer.xml
		glVertexAttribPointer(
			position,
			size,
			GL_FLOAT,
			GL_FALSE,
			sizeof(GLfloat) * stride,
			(void*)(sizeof(GLfloat) * offset)
		);
		checkForErrors();

		_currentVertexSize[position] = size;
		_currentVertexStride[position] = stride;
		_currentVertexOffset[position] = offset;
	}

    if (vertexBufferChanged || _vertexAttributeEnabled[position] != vertexAttributeEnabled)
    {
        if (vertexAttributeEnabled)
        {
            glEnableVertexAttribArray(position);
            checkForErrors();

            _vertexAttributeEnabled[position] = true;
        }
        else
        {
            glDisableVertexAttribArray(position);
            checkForErrors();

            _vertexAttributeEnabled[position] = false;

            return;
        }
    }
}


int
OpenGLES2Context::createVertexAttributeArray()
{
#ifdef GL_ES_VERSION_2_0
    if (!supportsExtension("vertex_array_object"))
        return -1;
#endif

    uint vao;

    glGenVertexArrays(1, &vao);
    checkForErrors();

    return vao;
}

void
OpenGLES2Context::setVertexAttributeArray(const uint vertexArray)
{
    glBindVertexArray(vertexArray);
    checkForErrors();
}

void
OpenGLES2Context::deleteVertexAttributeArray(const uint vertexArray)
{
    glDeleteVertexArrays(1, &vertexArray);
    checkForErrors();
}

const uint
OpenGLES2Context::createIndexBuffer(const uint size)
{
	uint indexBuffer;

	glGenBuffers(1, &indexBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);

	_currentIndexBuffer = indexBuffer;

	glBufferData(GL_ELEMENT_ARRAY_BUFFER, size * sizeof(GLushort), 0, GL_STATIC_DRAW);

	_indexBuffers.push_back(indexBuffer);

	checkForErrors();

	return indexBuffer;
}

void
OpenGLES2Context::uploaderIndexBufferData(const uint 	indexBuffer,
										  const uint 	offset,
										  const uint 	size,
										  void*					data)
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);

	_currentIndexBuffer = indexBuffer;

	glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, offset * sizeof(GLushort), size * sizeof(GLushort), data);

	checkForErrors();
}

void
OpenGLES2Context::deleteIndexBuffer(const uint indexBuffer)
{
	if (_currentIndexBuffer == indexBuffer)
		_currentIndexBuffer = 0;

	_indexBuffers.erase(std::find(_indexBuffers.begin(), _indexBuffers.end(), indexBuffer));

	glDeleteBuffers(1, &indexBuffer);

	checkForErrors();
}

uint
OpenGLES2Context::createTexture(TextureType	type,
								uint 		width,
								uint 		height,
								bool		mipMapping,
								bool        optimizeForRenderToTexture)
{
    return createTexture(type, width, height, mipMapping, optimizeForRenderToTexture, true);
}

uint OpenGLES2Context::createRectangleTexture(TextureType   type,
                                              uint          width,
                                              uint          height)
{
    return createTexture(type, width, height, false, false, false);
}

uint
OpenGLES2Context::createTexture(TextureType     type,
                                uint            width,
                                uint            height,
                                bool            mipMapping,
                                bool            optimizeForRenderToTexture,
                                bool            assertPowerOfTwoSized)
{
	uint texture;

    if (assertPowerOfTwoSized)
    {
	    // make sure width is a power of 2
	    if (!((width != 0) && !(width & (width - 1))))
		    throw std::invalid_argument("width");

	    // make sure height is a power of 2
	    if (!((height != 0) && !(height & (height - 1))))
		    throw std::invalid_argument("height");
    }
    else
    {
        if (mipMapping)
            throw std::logic_error("assertPowerOfTwoSized must be true when mipMapping is true");
    }

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
	const auto glTarget = type == TextureType::Texture2D
		? GL_TEXTURE_2D
		: GL_TEXTURE_CUBE_MAP;

	glBindTexture(glTarget, texture);

	_currentBoundTexture = texture;

	// default sampler states
	glTexParameteri(glTarget, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(glTarget, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(glTarget, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(glTarget, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	_textures.push_back(texture);
	_textureSizes[texture]			= std::make_pair(width, height);
	_textureHasMipmaps[texture]		= mipMapping;
	_textureTypes[texture]			= type;

	_currentWrapMode[texture]		= WrapMode::CLAMP;
	_currentTextureFilter[texture]	= TextureFilter::NEAREST;
	_currentMipFilter[texture]		= MipFilter::NONE;

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
	{
		uint level = 0;
		uint h = height;
		uint w = width;

		for (uint size = width > height ? width : height;
			 size > 0;
			 size = size >> 1, w = w >> 1, h = h >> 1)
		{
			if (type == TextureType::Texture2D)
				glTexImage2D(GL_TEXTURE_2D, level, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
			else
			{
				glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, level, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
				glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, level, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
				glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, level, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
				glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, level, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
				glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, level, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
				glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, level, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
			}

			++level;
		}
	}
	else
	{
		if (type == TextureType::Texture2D)
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
		else
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
			glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
			glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
			glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
		}
	}

	if (optimizeForRenderToTexture)
		createRTTBuffers(type, texture, width, height);

	checkForErrors();

	return texture;
}

uint
OpenGLES2Context::createCompressedTexture(TextureType     type,
                                          TextureFormat   format,
                                          unsigned int    width,
                                          unsigned int    height,
                                          bool            mipMapping)
{
    uint texture;

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
    const auto glTarget = type == TextureType::Texture2D
        ? GL_TEXTURE_2D
        : GL_TEXTURE_CUBE_MAP;

    glBindTexture(glTarget, texture);

    _currentBoundTexture = texture;

    // default sampler states
    glTexParameteri(glTarget, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(glTarget, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(glTarget, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(glTarget, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    _textures.push_back(texture);
    _textureSizes[texture]            = std::make_pair(width, height);
    _textureHasMipmaps[texture]        = mipMapping;
    _textureTypes[texture]            = type;

    _currentWrapMode[texture]        = WrapMode::CLAMP;
    _currentTextureFilter[texture]    = TextureFilter::NEAREST;
    _currentMipFilter[texture]        = MipFilter::NONE;

    const auto oglFormat = availableTextureFormats().at(format);

    if (mipMapping)
    {
        uint level = 0;
        uint h = height;
        uint w = width;

        for (uint size = width > height ? width : height;
             size > 0;
             size = size >> 1, w = w >> 1, h = h >> 1)
        {
        	const auto dataSize = TextureFormatInfo::textureSize(format, w, h);
        	const auto data = std::vector<unsigned char>(dataSize, 0);

         	if (type == TextureType::Texture2D)
         		glCompressedTexImage2D(GL_TEXTURE_2D, level, oglFormat, w, h, 0, dataSize, data.data());
            else
            {
         		glCompressedTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, level, oglFormat, w, h, 0, dataSize, data.data());
         		glCompressedTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, level, oglFormat, w, h, 0, dataSize, data.data());
         		glCompressedTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, level, oglFormat, w, h, 0, dataSize, data.data());
         		glCompressedTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, level, oglFormat, w, h, 0, dataSize, data.data());
         		glCompressedTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, level, oglFormat, w, h, 0, dataSize, data.data());
         		glCompressedTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, level, oglFormat, w, h, 0, dataSize, data.data());
            }

            ++level;
        }
    }
    else
    {
        const auto dataSize = TextureFormatInfo::textureSize(format, width, height);
    	const auto data = std::vector<unsigned char>(dataSize, 0);

        if (type == TextureType::Texture2D)
     		glCompressedTexImage2D(GL_TEXTURE_2D, 0, oglFormat, width, height, 0, dataSize, data.data());
        else
        {
     		glCompressedTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, oglFormat, width, height, 0, dataSize, data.data());
     		glCompressedTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, oglFormat, width, height, 0, dataSize, data.data());
     		glCompressedTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, oglFormat, width, height, 0, dataSize, data.data());
     		glCompressedTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, oglFormat, width, height, 0, dataSize, data.data());
     		glCompressedTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, oglFormat, width, height, 0, dataSize, data.data());
     		glCompressedTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, oglFormat, width, height, 0, dataSize, data.data());
        }
    }

    checkForErrors();

    return texture;
}

TextureType
OpenGLES2Context::getTextureType(uint textureId) const
{
	const auto foundTypeIt = _textureTypes.find(textureId);

	assert(foundTypeIt != _textureTypes.end());

	return foundTypeIt->second;
}

void
OpenGLES2Context::uploadTexture2dData(uint 		texture,
									  uint 		width,
									  uint 		height,
									  uint 		mipLevel,
									  void*		data)
{
	assert(getTextureType(texture) == TextureType::Texture2D);

	glBindTexture(GL_TEXTURE_2D, texture);
	glTexImage2D(GL_TEXTURE_2D, mipLevel, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

	_currentBoundTexture = texture;

	checkForErrors();
}

void
OpenGLES2Context::uploadCubeTextureData(uint				texture,
										CubeTexture::Face	face,
										unsigned int 		width,
										unsigned int 		height,
										unsigned int 		mipLevel,
										void*				data)
{
	assert(getTextureType(texture) == TextureType::CubeTexture);

	glBindTexture(GL_TEXTURE_CUBE_MAP, texture);

	GLenum cubeFace;
	switch (face)
	{
	case minko::render::CubeTexture::Face::POSITIVE_X:
		cubeFace = GL_TEXTURE_CUBE_MAP_POSITIVE_X;
		break;
	case minko::render::CubeTexture::Face::NEGATIVE_X:
		cubeFace = GL_TEXTURE_CUBE_MAP_NEGATIVE_X;
		break;
	case minko::render::CubeTexture::Face::POSITIVE_Y:
		cubeFace = GL_TEXTURE_CUBE_MAP_POSITIVE_Y;
		break;
	case minko::render::CubeTexture::Face::NEGATIVE_Y:
		cubeFace = GL_TEXTURE_CUBE_MAP_NEGATIVE_Y;
		break;
	case minko::render::CubeTexture::Face::POSITIVE_Z:
		cubeFace = GL_TEXTURE_CUBE_MAP_POSITIVE_Z;
		break;
	case minko::render::CubeTexture::Face::NEGATIVE_Z:
		cubeFace = GL_TEXTURE_CUBE_MAP_NEGATIVE_Z;
		break;
	default:
		throw;
		break;
	}

	glTexImage2D(cubeFace, mipLevel, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

	_currentBoundTexture = texture;

	checkForErrors();
}

void
OpenGLES2Context::uploadCompressedTexture2dData(uint          texture,
                                                TextureFormat format,
                                                unsigned int  width,
                                                unsigned int  height,
                                                unsigned int  size,
                                                unsigned int  mipLevel,
                                                void*         data)
{
    assert(getTextureType(texture) == TextureType::Texture2D);

    const auto& formats = availableTextureFormats();

    glBindTexture(GL_TEXTURE_2D, texture);
    glCompressedTexSubImage2D(GL_TEXTURE_2D, mipLevel, 0, 0, width, height, formats.at(format), size, data);

    _currentBoundTexture = texture;

    checkForErrors();
}

void
OpenGLES2Context::uploadCompressedCubeTextureData(uint                texture,
                                                  CubeTexture::Face   face,
                                                  TextureFormat       format,
                                                  unsigned int        width,
                                                  unsigned int        height,
                                                  unsigned int        mipLevel,
                                                  void*               data)
{
    // FIXME
	throw;
}

void
OpenGLES2Context::activateMipMapping(uint texture)
{
    _textureHasMipmaps[texture] = true;
}

void
OpenGLES2Context::deleteTexture(uint texture)
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

	_textureSizes.erase(texture);
	_textureHasMipmaps.erase(texture);
	_textureTypes.erase(texture);

	_currentWrapMode.erase(texture);
	_currentTextureFilter.erase(texture);
	_currentMipFilter.erase(texture);

	for (unsigned int pos = 0; pos < _currentTexture.size(); ++pos)
		_currentTexture[pos] = (_currentTexture[pos] == texture ? 0 : _currentTexture[pos]);
	_currentBoundTexture = (_currentBoundTexture == texture ? 0 : _currentBoundTexture);

	checkForErrors();
}

void
OpenGLES2Context::setTextureAt(uint	position,
							   int	texture,
							   int	location)
{
	const bool textureIsValid = texture > 0;

	if (!textureIsValid)
		return;

	if (position >= _currentTexture.size())
		return;

	const auto glTarget	= getTextureType(texture) == TextureType::Texture2D
		? GL_TEXTURE_2D
		: GL_TEXTURE_CUBE_MAP;

	if (_currentTexture[position] != texture || _currentBoundTexture != texture)
	{
		glActiveTexture(GL_TEXTURE0 + position);
		glBindTexture(glTarget, texture);

		_currentTexture[position]	= texture;
		_currentBoundTexture		= texture;
	}

	if (textureIsValid && location >= 0)
		glUniform1i(location, position);

	checkForErrors();
}

void
OpenGLES2Context::setSamplerStateAt(uint			position,
									WrapMode		wrapping,
									TextureFilter	filtering,
									MipFilter		mipFiltering)
{
	const auto	texture		= _currentTexture[position];
	const auto	glTarget	= getTextureType(texture) == TextureType::Texture2D
		? GL_TEXTURE_2D
		: GL_TEXTURE_CUBE_MAP;

	auto active	= false;

	// disable mip mapping if mip maps are not available
	if (!_textureHasMipmaps[texture])
		mipFiltering = MipFilter::NONE;

	if (_currentWrapMode[texture] != wrapping)
	{
		_currentWrapMode[texture] = wrapping;

		glActiveTexture(GL_TEXTURE0 + position);
		active = true;

		switch (wrapping)
		{
		case WrapMode::CLAMP :
			glTexParameteri(glTarget, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(glTarget, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			break;
		case WrapMode::REPEAT :
			glTexParameteri(glTarget, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(glTarget, GL_TEXTURE_WRAP_T, GL_REPEAT);
			break;
		}
	}

	if (_currentTextureFilter[texture] != filtering || _currentMipFilter[texture] != mipFiltering)
	{
		_currentTextureFilter[texture] = filtering;
		_currentMipFilter[texture] = mipFiltering;

		if (!active)
			glActiveTexture(GL_TEXTURE0 + position);

		switch (filtering)
		{
		case TextureFilter::NEAREST :
			switch (mipFiltering)
			{
			case MipFilter::NONE :
				glTexParameteri(glTarget, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
				break;
			case MipFilter::NEAREST :
				glTexParameteri(glTarget, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
				break;
			case MipFilter::LINEAR :
				glTexParameteri(glTarget, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
				break;
			}

			glTexParameteri(glTarget, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			break;
		case TextureFilter::LINEAR :
			switch (mipFiltering)
			{
			case MipFilter::NONE :
				glTexParameteri(glTarget, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				break;
			case MipFilter::NEAREST :
				glTexParameteri(glTarget, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
				break;
			case MipFilter::LINEAR :
				glTexParameteri(glTarget, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
				break;
			}

			glTexParameteri(glTarget, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			break;
		}
	}

	checkForErrors();
}



const uint
OpenGLES2Context::createProgram()
{
	auto handle = glCreateProgram();

	checkForErrors();
	_programs.push_back(handle);

	return handle;
}

void
OpenGLES2Context::attachShader(const uint program, const uint shader)
{
	glAttachShader(program, shader);

	checkForErrors();
}

void
OpenGLES2Context::linkProgram(const uint program)
{
	glLinkProgram(program);

#ifdef DEBUG
	auto errors = getProgramInfoLogs(program);

	if (!errors.empty())
	{
		std::cout << errors << std::endl;
	}
#endif

	checkForErrors();
}

void
OpenGLES2Context::deleteProgram(const uint program)
{
	_programs.erase(std::find(_programs.begin(), _programs.end(), program));

	glDeleteProgram(program);

	checkForErrors();
}

void
OpenGLES2Context::compileShader(const uint shader)
{
	glCompileShader(shader);

#ifdef DEBUG
	auto errors = getShaderCompilationLogs(shader);

	if (!errors.empty())
	{
        std::string    source;
        getShaderSource(shader, source);

        LOG_DEBUG("Shader source (glShaderSource_" << shader << "):\n" << source);
        LOG_DEBUG("Shader errors (glShaderSource_" << shader << "):\n" << errors);

        throw std::runtime_error("Shader compilation failed. Enable debug logs to display errors.");
	}
#endif

	checkForErrors();
}

void
OpenGLES2Context::setProgram(const uint program)
{
	if (_currentProgram == program)
		return;

	_currentProgram = program;

	glUseProgram(program);

	checkForErrors();
}

static
std::string
glslVersionString()
{
    auto fullVersion = std::string(reinterpret_cast<const char*>(glGetString(GL_SHADING_LANGUAGE_VERSION)));

    if (fullVersion.empty())
        return "";

    auto glslVersion = std::string();

    const auto minorVersionSeparatorPosition = fullVersion.find_first_of(".");

    if (minorVersionSeparatorPosition == std::string::npos)
        return std::string(0, fullVersion.find_first_of(" "));

    glslVersion += std::string(fullVersion.begin(), fullVersion.begin() + minorVersionSeparatorPosition);

    const auto minorVersionPosition = minorVersionSeparatorPosition + 1;
    const auto buildVersionSeparatorPosition = fullVersion.find_first_of(". ", minorVersionPosition);

    if (buildVersionSeparatorPosition == std::string::npos)
        return glslVersion + "00";

    glslVersion += std::string(
        fullVersion.begin() + minorVersionPosition,
        fullVersion.begin() + buildVersionSeparatorPosition
    );

    return glslVersion;
}

void
OpenGLES2Context::setShaderSource(const uint shader,
								  const std::string& source)
{
    // TODO fixme
    // temporary allowing version > 120
    // implement new *Context to properly handle it

    const char* sourceString = source.c_str();

	glShaderSource(shader, 1, &sourceString, 0);

	checkForErrors();
}

void
OpenGLES2Context::getShaderSource(uint shader, std::string& source)
{
	source.clear();

	GLint	bufferSize	= 0;
	GLsizei	length		= 0;

	glGetShaderiv(shader, GL_SHADER_SOURCE_LENGTH, &bufferSize);
	if (bufferSize == 0)
		return;

	source.resize(bufferSize);
	glGetShaderSource(shader, bufferSize, &length, &source[0]);
	checkForErrors();
}


const uint
OpenGLES2Context::createVertexShader()
{
	uint vertexShader = glCreateShader(GL_VERTEX_SHADER);

	_vertexShaders.push_back(vertexShader);

	checkForErrors();

	return vertexShader;
}

void
OpenGLES2Context::deleteVertexShader(const uint vertexShader)
{
	_vertexShaders.erase(std::find(_vertexShaders.begin(), _vertexShaders.end(), vertexShader));

	glDeleteShader(vertexShader);

	checkForErrors();
}

const uint
OpenGLES2Context::createFragmentShader()
{
	uint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

	_fragmentShaders.push_back(fragmentShader);

	checkForErrors();

	return fragmentShader;
}

void
OpenGLES2Context::deleteFragmentShader(const uint fragmentShader)
{
	_fragmentShaders.erase(std::find(_fragmentShaders.begin(), _fragmentShaders.end(), fragmentShader));

	glDeleteShader(fragmentShader);

	checkForErrors();
}

ProgramInputs
OpenGLES2Context::getProgramInputs(const uint program)
{
	setProgram(program);

    return ProgramInputs(getUniformInputs(program), getAttributeInputs(program));
}

/*static*/
ProgramInputs::Type
OpenGLES2Context::convertInputType(unsigned int type)
{
	switch (type)
	{
		case GL_FLOAT:
			return ProgramInputs::Type::float1;
		case GL_FLOAT_VEC2:
			return ProgramInputs::Type::float2;
		case GL_FLOAT_VEC3:
			return ProgramInputs::Type::float3;
		case GL_FLOAT_VEC4:
			return ProgramInputs::Type::float4;
		case GL_INT:
			return ProgramInputs::Type::int1;
		case GL_INT_VEC2:
			return ProgramInputs::Type::int2;
		case GL_INT_VEC3:
			return ProgramInputs::Type::int3;
		case GL_INT_VEC4:
			return ProgramInputs::Type::int4;
		case GL_BOOL:
			return ProgramInputs::Type::bool1;
		case GL_BOOL_VEC2:
			return ProgramInputs::Type::bool2;
		case GL_BOOL_VEC3:
			return ProgramInputs::Type::bool3;
		case GL_BOOL_VEC4:
			return ProgramInputs::Type::bool4;
		case GL_FLOAT_MAT3:
			return ProgramInputs::Type::float9;
		case GL_FLOAT_MAT4:
			return ProgramInputs::Type::float16;
		case GL_SAMPLER_2D:
			return ProgramInputs::Type::sampler2d;
		case GL_SAMPLER_CUBE:
			return ProgramInputs::Type::samplerCube;
		default:
			throw std::logic_error("unsupported type");
			return ProgramInputs::Type::unknown;
	}
}


std::vector<ProgramInputs::UniformInput>
OpenGLES2Context::getUniformInputs(const uint program)
{
    std::vector<ProgramInputs::UniformInput> inputs;

	int total = -1;
	int maxUniformNameLength = -1;

	glGetProgramiv(program, GL_ACTIVE_UNIFORM_MAX_LENGTH, &maxUniformNameLength);
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
		int	location = glGetUniformLocation(program, &name[0]);

        if (location >= 0 && inputType != ProgramInputs::Type::unknown)
            inputs.emplace_back(std::string(&name[0], nameLength), location, size, inputType);
	}

    return inputs;
}

std::vector<ProgramInputs::AttributeInput>
OpenGLES2Context::getAttributeInputs(const uint program)
{
    std::vector<ProgramInputs::AttributeInput> inputs;

	int total = -1;
	int maxAttributeNameLength = -1;

	glGetProgramiv(program, GL_ACTIVE_ATTRIBUTE_MAX_LENGTH, &maxAttributeNameLength);
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
            inputs.emplace_back(std::string(&name[0], nameLength), location);
	}

    return inputs;
}

std::string
OpenGLES2Context::getShaderCompilationLogs(const uint shader)
{
	int compileStatus = -1;

	glGetShaderiv(shader, GL_COMPILE_STATUS, &compileStatus);

	if (compileStatus != GL_TRUE)
	{
		int logsLength = -1;
		char buffer[1024];
		int bufferLength = -1;

		glGetShaderSource(shader, 1024, &bufferLength, buffer);

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
OpenGLES2Context::getProgramInfoLogs(const uint program)
{
	int programInfoMaxLength = -1;
	int programInfoLength = -1;

	glGetProgramiv(program, GL_INFO_LOG_LENGTH, &programInfoMaxLength);

	if (programInfoMaxLength <= 0)
		return std::string();

	std::vector<char> programInfo(programInfoMaxLength);

	glGetProgramInfoLog(program, programInfoMaxLength, &programInfoLength, &programInfo[0]);

	return std::string(&programInfo[0]);
}

void
OpenGLES2Context::setBlendingMode(Blending::Source source, Blending::Destination destination)
{
	if ((static_cast<uint>(source) | static_cast<uint>(destination)) != static_cast<uint>(_currentBlendingMode))
	{
        _currentBlendingMode = (Blending::Mode)((uint)source | (uint)destination);

		glBlendFunc(
			_blendingFactors[static_cast<uint>(source) & 0x00ff],
			_blendingFactors[static_cast<uint>(destination) & 0xff00]
		);

		checkForErrors();
	}
}

void
OpenGLES2Context::setBlendingMode(Blending::Mode blendingMode)
{
    if (blendingMode != _currentBlendingMode)
	{
        _currentBlendingMode = blendingMode;

		glBlendFunc(
            _blendingFactors[static_cast<uint>(blendingMode) & 0x00ff],
            _blendingFactors[static_cast<uint>(blendingMode) & 0xff00]
		);

		checkForErrors();
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
		glDepthFunc(_compareFuncs[depthFunc]);

		checkForErrors();
	}
}

void
OpenGLES2Context::setColorMask(bool colorMask)
{
	if (_currentColorMask != colorMask)
	{
		_currentColorMask = colorMask;

		glColorMask(colorMask, colorMask, colorMask, colorMask);

		checkForErrors();
	}

}

void
OpenGLES2Context::setStencilTest(CompareMode stencilFunc,
								 int stencilRef,
								 uint stencilMask,
								 StencilOperation stencilFailOp,
								 StencilOperation stencilZFailOp,
								 StencilOperation stencilZPassOp)
{
	if (stencilFunc != _currentStencilFunc
		|| stencilRef != _currentStencilRef
		|| stencilMask != _currentStencilMask)
	{
		_currentStencilFunc	= stencilFunc;
		_currentStencilRef	= stencilRef;
		_currentStencilMask	= stencilMask;

		glStencilFunc(_compareFuncs[stencilFunc], stencilRef, stencilMask);

		checkForErrors();
	}


	if (stencilFailOp != _currentStencilFailOp
		|| stencilZFailOp != _currentStencilZFailOp
		|| stencilZPassOp != _currentStencilZPassOp)
	{
		_currentStencilFailOp	= stencilFailOp;
		_currentStencilZFailOp	= stencilZFailOp;
		_currentStencilZPassOp	= stencilZPassOp;

		glStencilOp(_stencilOps[stencilFailOp], _stencilOps[stencilZFailOp], _stencilOps[stencilZPassOp]);

		checkForErrors();
	}
}


void
OpenGLES2Context::readPixels(unsigned int x, unsigned int y, unsigned int width, unsigned int height, unsigned char* pixels)
{
	glReadPixels(x, y, width, height, GL_RGBA, GL_UNSIGNED_BYTE, pixels);

	checkForErrors();
}

void
OpenGLES2Context::setScissorTest(bool scissorTest, const math::ivec4& scissorBox)
{
	if (scissorTest == _scissorTest && scissorBox == _scissorBox)
		return;

	if (scissorTest)
	{
		glEnable(GL_SCISSOR_TEST);

		int		x = 0;
		int		y = 0;
		uint	width = 0;
		uint	height = 0;

		if (scissorBox.z < 0 || scissorBox.w < 0)
		{
			x		= _viewportX;
			y		= _viewportY;
			width	= _viewportWidth;
			height	= _viewportHeight;
		}
		else
		{
			x		= scissorBox.x;
			y		= scissorBox.y;
			width	= scissorBox.z;
			height	= scissorBox.w;
		}

		glScissor(x, y, width, height);
	}
	else
		glDisable(GL_SCISSOR_TEST);

	_scissorTest = scissorTest;
	_scissorBox = scissorBox;

	checkForErrors();
}

void
OpenGLES2Context::readPixels(unsigned char* pixels)
{
	glReadPixels(_viewportX, _viewportY, _viewportWidth, _viewportHeight, GL_RGBA, GL_UNSIGNED_BYTE, pixels);

	checkForErrors();
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

	checkForErrors();
}

void
OpenGLES2Context::setRenderToBackBuffer()
{
	if (_currentTarget == 0)
		return;

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
    configureViewport(_oldViewportX, _oldViewportY, _oldViewportWidth, _oldViewportHeight);

	_currentTarget = 0;

	checkForErrors();
}

void
OpenGLES2Context::setRenderToTexture(uint texture, bool enableDepthAndStencil)
{
	if (texture == _currentTarget)
		return;

	if (_frameBuffers.count(texture) == 0)
		throw std::logic_error("this texture cannot be used for RTT");

    if (!_currentTarget)
    {
        _oldViewportX = _viewportX;
        _oldViewportY = _viewportY;
        _oldViewportWidth = _viewportWidth;
        _oldViewportHeight = _viewportHeight;
    }
	_currentTarget = texture;

	glBindFramebuffer(GL_FRAMEBUFFER, _frameBuffers[texture]);
	checkForErrors();

	if (enableDepthAndStencil)
	{
		glBindRenderbuffer(GL_RENDERBUFFER, _renderBuffers[texture]);
		checkForErrors();
	}

	auto textureSize = _textureSizes[texture];

	configureViewport(0, 0, textureSize.first, textureSize.second);
	checkForErrors();
}

void
OpenGLES2Context::createRTTBuffers(TextureType	type,
								   uint			texture,
								   unsigned int	width,
								   unsigned int	height)
{
	uint frameBuffer = -1;

	// create a framebuffer object
	glGenFramebuffers(1, &frameBuffer);
	// bind the framebuffer object
	glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
	// attach a texture to the FBO
	if (type == TextureType::Texture2D)
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);
	else
	{
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + 0, GL_TEXTURE_CUBE_MAP_POSITIVE_X, texture, 0);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + 1, GL_TEXTURE_CUBE_MAP_NEGATIVE_X, texture, 0);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + 2, GL_TEXTURE_CUBE_MAP_POSITIVE_Y, texture, 0);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + 3, GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, texture, 0);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + 4, GL_TEXTURE_CUBE_MAP_POSITIVE_Z, texture, 0);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + 5, GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, texture, 0);
	}


	uint renderBuffer = -1;

	// gen renderbuffer
	glGenRenderbuffers(1, &renderBuffer);
	// bind renderbuffer
	glBindRenderbuffer(GL_RENDERBUFFER, renderBuffer);
	// init as a depth buffer
#ifdef GL_ES_VERSION_2_0
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, width, height);
#else
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, width, height);
#endif
	// FIXME: create & attach stencil buffer

	// attach to the FBO for depth
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, renderBuffer);

	auto status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (status != GL_FRAMEBUFFER_COMPLETE)
		throw;

	_frameBuffers[texture] = frameBuffer;
	_renderBuffers[texture] = renderBuffer;

	// unbind
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	checkForErrors();
}

uint
OpenGLES2Context::getError()
{
	auto error = glGetError();

	switch (error)
	{
	default:
		break;
	case GL_INVALID_ENUM:
		std::cerr << "GL_INVALID_ENUM" << std::endl;
		break;
	case GL_INVALID_FRAMEBUFFER_OPERATION:
		std::cerr << "GL_INVALID_FRAMEBUFFER_OPERATION" << std::endl;
		break;
	case GL_INVALID_VALUE:
		std::cerr << "GL_INVALID_VALUE" << std::endl;
		break;
	case GL_INVALID_OPERATION:
		std::cerr << "GL_INVALID_OPERATION" << std::endl;
		break;
	case GL_OUT_OF_MEMORY:
		std::cerr << "GL_OUT_OF_MEMORY" << std::endl;
		break;
	}

	return error;
}

void
OpenGLES2Context::generateMipmaps(uint texture)
{
	glBindTexture(GL_TEXTURE_2D, texture);

	// glGenerateMipmap exists in OpenGL ES 2.0+ or OpenGL 3.0+
	// https://www.khronos.org/opengles/sdk/docs/man/xhtml/glGenerateMipmap.xml
	// https://www.opengl.org/sdk/docs/man/html/glGenerateMipmap.xhtml
#ifndef GL_ES_VERSION_2_0
	if (_oglMajorVersion < 3)
	{
		if (supportsExtension("GL_SGIS_generate_mipmap"))
			glGenerateMipmapEXT(GL_TEXTURE_2D);
# ifdef DEBUG
		else
			throw std::runtime_error("Missing OpenGL extension: 'GL_SGIS_generate_mipmap'.");
# endif
	}
	else
#endif
		glGenerateMipmap(GL_TEXTURE_2D);

	checkForErrors();

	_currentBoundTexture = texture;
}

void
OpenGLES2Context::setUniformFloat(uint location, uint count, const float* v)
{
    glUniform1fv(location, count, v);
}

void
OpenGLES2Context::setUniformFloat2(uint location, uint count, const float* v)
{
    glUniform2fv(location, count, v);
}

void
OpenGLES2Context::setUniformFloat3(uint location, uint count, const float* v)
{
    glUniform3fv(location, count, v);
}

void
OpenGLES2Context::setUniformFloat4(uint location, uint count, const float* v)
{
    glUniform4fv(location, count, v);
}

void
OpenGLES2Context::setUniformMatrix4x4(uint location, uint count, const float* v)
{
    glUniformMatrix4fv(location, count, false, v);
}

void
OpenGLES2Context::setUniformInt(uint location, uint count, const int* v)
{
    glUniform1iv(location, count, v);
}

void
OpenGLES2Context::setUniformInt2(uint location, uint count, const int* v)
{
    glUniform2iv(location, count, v);
}

void
OpenGLES2Context::setUniformInt3(uint location, uint count, const int* v)
{
    glUniform3iv(location, count, v);
}

void
OpenGLES2Context::setUniformInt4(uint location, uint count, const int* v)
{
    glUniform4iv(location, count, v);
}

bool
OpenGLES2Context::supportsExtension(const std::string& extensionNameString)
{
    const auto availableExtensionRawStrings = reinterpret_cast<const char*>(glGetString(GL_EXTENSIONS));

    if (availableExtensionRawStrings == nullptr)
        return false;

    const auto availableExtensionStrings = std::string(availableExtensionRawStrings);

    return availableExtensionStrings.find(extensionNameString) != std::string::npos;
}

const std::unordered_map<TextureFormat, uint, Hash<TextureFormat>>&
OpenGLES2Context::availableTextureFormats()
{
    if (!_availableTextureFormats.empty())
        return _availableTextureFormats;

    auto& formats = _availableTextureFormats;

    formats.insert(std::make_pair(TextureFormat::RGB, GL_RGB));
    formats.insert(std::make_pair(TextureFormat::RGBA, GL_RGBA));

    auto formatCount = GLint();
    auto rawFormats = std::vector<GLenum>();

    glGetIntegerv(GL_NUM_COMPRESSED_TEXTURE_FORMATS, &formatCount);

    rawFormats.resize(formatCount);

    glGetIntegerv(GL_COMPRESSED_TEXTURE_FORMATS, reinterpret_cast<GLint*>(rawFormats.data()));

    for (auto rawFormat : rawFormats)
    {
        switch (rawFormat)
        {
#ifdef GL_EXT_texture_compression_dxt1
        case GL_COMPRESSED_RGB_S3TC_DXT1_EXT:
            formats.insert(std::make_pair(TextureFormat::RGB_DXT1, GL_COMPRESSED_RGB_S3TC_DXT1_EXT));
            break;
        case GL_COMPRESSED_RGBA_S3TC_DXT1_EXT:
            formats.insert(std::make_pair(TextureFormat::RGBA_DXT1, GL_COMPRESSED_RGBA_S3TC_DXT1_EXT));
            break;
#endif

#ifdef GL_EXT_texture_compression_s3tc
        case GL_COMPRESSED_RGBA_S3TC_DXT3_EXT:
            formats.insert(std::make_pair(TextureFormat::RGBA_DXT3, GL_COMPRESSED_RGBA_S3TC_DXT3_EXT));
            break;
        case GL_COMPRESSED_RGBA_S3TC_DXT5_EXT:
            formats.insert(std::make_pair(TextureFormat::RGBA_DXT5, GL_COMPRESSED_RGBA_S3TC_DXT5_EXT));
            break;
#endif

#ifdef GL_IMG_texture_compression_pvrtc
        case GL_COMPRESSED_RGB_PVRTC_2BPPV1_IMG:
            formats.insert(std::make_pair(TextureFormat::RGB_PVRTC1_2BPP, GL_COMPRESSED_RGB_PVRTC_2BPPV1_IMG));
            break;
        case GL_COMPRESSED_RGB_PVRTC_4BPPV1_IMG:
            formats.insert(std::make_pair(TextureFormat::RGB_PVRTC1_4BPP, GL_COMPRESSED_RGB_PVRTC_4BPPV1_IMG));
            break;
        case GL_COMPRESSED_RGBA_PVRTC_2BPPV1_IMG:
            formats.insert(std::make_pair(TextureFormat::RGBA_PVRTC1_2BPP, GL_COMPRESSED_RGBA_PVRTC_2BPPV1_IMG));
            break;
        case GL_COMPRESSED_RGBA_PVRTC_4BPPV1_IMG:
            formats.insert(std::make_pair(TextureFormat::RGBA_PVRTC1_4BPP, GL_COMPRESSED_RGBA_PVRTC_4BPPV1_IMG));
            break;
#endif

#ifdef GL_IMG_texture_compression_pvrtc2
        case GL_COMPRESSED_RGBA_PVRTC_2BPPV2_IMG:
            formats.insert(std::make_pair(TextureFormat::RGBA_PVRTC2_2BPP, COMPRESSED_RGBA_PVRTC_2BPPV2_IMG));
            break;
        case GL_COMPRESSED_RGBA_PVRTC_4BPPV2_IMG:
            formats.insert(std::make_pair(TextureFormat::RGBA_PVRTC2_4BPP, COMPRESSED_RGBA_PVRTC_4BPPV2_IMG));
            break;
#endif

#ifdef GL_OES_compressed_ETC1_RGB8_texture
        case GL_ETC1_RGB8_OES:
            formats.insert(std::make_pair(TextureFormat::RGB_ETC1, GL_ETC1_RGB8_OES));
            formats.insert(std::make_pair(TextureFormat::RGBA_ETC1, GL_ETC1_RGB8_OES));
            break;
#endif

#ifdef GL_AMD_compressed_ATC_texture
        case GL_ATC_RGB_AMD:
            formats.insert(std::make_pair(TextureFormat::RGB_ATITC, GL_ATC_RGB_AMD));
            break;
        case GL_ATC_RGBA_EXPLICIT_ALPHA_AMD:
            formats.insert(std::make_pair(TextureFormat::RGBA_ATITC, GL_ATC_RGBA_EXPLICIT_ALPHA_AMD));
            break;
#endif

        default:
            break;
        }
    }

    return formats;
}
