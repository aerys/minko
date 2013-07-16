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

#include "Program.hpp"
#include "minko/render/AbstractContext.hpp"
#include "minko/render/OpenGLES2Context.hpp"

using namespace minko::render;

Program::Program(Program::AbstractContextPtr context) :
	AbstractResource(context)
{
}

void
Program::upload()
{
	_vertexShader = context()->createVertexShader();
	_context->setShaderSource(_vertexShader, _vertexShaderSource);
	_context->compileShader(_vertexShader);
	std::cout << "VS" << std::endl <<
	std::static_pointer_cast<OpenGLES2Context>(_context)->getShaderCompilationLogs(_vertexShader)
	<< std::endl << "/VS" << std::endl << std::endl;

	_fragmentShader = context()->createFragmentShader();
	_context->setShaderSource(_fragmentShader, _fragmentShaderSource);
	_context->compileShader(_fragmentShader);
	std::cout << "FS" << std::endl <<
	std::static_pointer_cast<OpenGLES2Context>(_context)->getShaderCompilationLogs(_fragmentShader)
	<< std::endl << "/FS" << std::endl << std::endl;
	
	_id = context()->createProgram();
	_context->attachShader(_id, _vertexShader);
	_context->attachShader(_id, _fragmentShader);
	_context->linkProgram(_id);

	_inputs = _context->getProgramInputs(_id);
}

void
Program::dispose()
{
	_context->deleteVertexShader(_vertexShader);
	_context->deleteFragmentShader(_fragmentShader);
	_context->deleteProgram(_id);

	_vertexShader = -1;
	_fragmentShader = -1;
	_id = -1;
}
