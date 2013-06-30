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

#pragma once

#include "minko/Common.hpp"
#include "minko/render/ShaderProgramInputs.hpp"

namespace minko
{
	namespace render
	{
		using namespace minko::render;
		using namespace minko::render::context;
		using namespace minko::data;

		class GLSLProgram :
			public std::enable_shared_from_this<GLSLProgram>
		{
		public:
			typedef std::shared_ptr<GLSLProgram>	Ptr;

		private:
			typedef std::shared_ptr<AbstractContext>		AbstractContextPtr;
			typedef std::shared_ptr<ShaderProgramInputs>	ShaderProgramInputsPtr;

		private:
			AbstractContextPtr		_context;
			const unsigned int		_program;
			const unsigned int		_vertexShader;
			const unsigned int		_fragmentShader;
			const std::string 		_vertexShaderSource;
			const std::string		_fragmentShaderSource;
			ShaderProgramInputsPtr	_inputs;

		public:
			inline static
			Ptr
			create(AbstractContextPtr	context,
				   const std::string& 	vertexShaderSource,
				   const std::string& 	fragmentShaderSource)
			{
				return std::shared_ptr<GLSLProgram>(new GLSLProgram(
					context,
					vertexShaderSource,
					fragmentShaderSource
				));
			}

			inline
			const unsigned int
			program()
			{
				return _program;
			}

			inline
			const unsigned int
			vertexShader()
			{
				return _vertexShader;
			}

			inline
			const unsigned int
			fragmentShader()
			{
				return _fragmentShader;
			}

			inline
			ShaderProgramInputsPtr
			inputs()
			{
				return _inputs;
			}

		private:
			GLSLProgram(AbstractContextPtr context,
					    const std::string& vertexShaderSource,
					    const std::string& fragmentShaderSource);
		};
	}
}
