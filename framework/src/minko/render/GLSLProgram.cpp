#include "GLSLProgram.hpp"
#include "minko/render/context/AbstractContext.hpp"

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
