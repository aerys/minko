#include "GLSLProgram.hpp"
#include "minko/render/context/AbstractContext.hpp"

#include <fstream>

using namespace minko::render;

GLSLProgram::GLSLProgram(GLSLProgram::AbstractContextPtr context,
					   const std::string& 			  vertexShaderSource,
					   const std::string &			  fragmentShaderSource) :
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

GLSLProgram::ptr
GLSLProgram::fromFiles(GLSLProgram::AbstractContextPtr 	context,
		  			   const std::string& 				vertexShaderFile,
		  			   const std::string& 				fragmentShaderFile)
{
	return create(context, loadShaderFile(vertexShaderFile), loadShaderFile(fragmentShaderFile));
}

const std::string
GLSLProgram::loadShaderFile(const std::string& filename)
{
	std::fstream file(filename, std::ios::in | std::ios::ate);

	if (file.is_open())
	{
		unsigned int size = (unsigned int)file.tellg();
		std::vector<char> content(size + 1);

		content[size] = 0;
		file.seekg(0, std::ios::beg);
		file.read(&content[0], size);
		file.close();

		return std::string(&content[0]);
	}
	else
		throw std::invalid_argument("filename");
}
