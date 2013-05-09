#pragma once

#include "minko/Common.hpp"
#include "minko/render/ShaderProgramInputs.hpp"

namespace minko
{
	namespace render
	{
		using namespace minko::render;
		using namespace minko::render::context;
		using namespace minko::scene::data;

		class GLSLProgram :
			public std::enable_shared_from_this<GLSLProgram>
		{
		public:
			typedef std::shared_ptr<GLSLProgram>	ptr;

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
			ptr
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

			static
			ptr
			fromFiles(GLSLProgram::AbstractContextPtr 	context,
		  			  const std::string& 				vertexShaderFile,
					  const std::string& 				fragmentShaderFile);

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

			static
			const std::string
			loadShaderFile(const std::string& filename);
		};
	}
}
