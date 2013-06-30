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

#include "GLSLProgram.hpp"
#include "minko/render/AbstractContext.hpp"

using namespace minko::render;

GLSLProgram::GLSLProgram(GLSLProgram::AbstractContextPtr	context,
						 const std::string& 				vertexShaderSource,
						 const std::string &				fragmentShaderSource) :
	std::enable_shared_from_this<GLSLProgram>(),
	_context(context),
	_program(context->createProgram()),
	_vertexShader(context->createVertexShader()),
	_fragmentShader(context->createFragmentShader()),
	_vertexShaderSource(vertexShaderSource),
	_fragmentShaderSource(fragmentShaderSource)
{
	_context->setShaderSource(_vertexShader, vertexShaderSource);
	_context->compileShader(_vertexShader);

	_context->setShaderSource(_fragmentShader, fragmentShaderSource);
	_context->compileShader(_fragmentShader);

	_context->attachShader(_program, _vertexShader);
	_context->attachShader(_program, _fragmentShader);
	_context->linkProgram(_program);

	_inputs = _context->getProgramInputs(_program);
}
