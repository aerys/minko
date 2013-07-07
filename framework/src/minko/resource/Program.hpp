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

#include "minko/resource/AbstractResource.hpp"
#include "minko/render/ProgramInputs.hpp"

namespace minko
{
	namespace resource
	{
		class Program :
			public AbstractResource
		{
		public:
			typedef std::shared_ptr<Program>	Ptr;

		private:
			typedef std::shared_ptr<render::AbstractContext>		AbstractContextPtr;
			typedef std::shared_ptr<render::ProgramInputs>	ProgramInputsPtr;

		private:
			unsigned int		_vertexShader;
			unsigned int		_fragmentShader;
			std::string 		_vertexShaderSource;
			std::string			_fragmentShaderSource;
			ProgramInputsPtr	_inputs;

		public:
			inline static
			Ptr
			create(AbstractContextPtr	context)
			{
				return std::shared_ptr<Program>(new Program(context));
			}

			inline static
			Ptr
			create(AbstractContextPtr	context,
				   const std::string& 	vertexShaderSource,
				   const std::string& 	fragmentShaderSource)
			{
				auto p = create(context);

				p->vertexShaderSource(vertexShaderSource);
				p->fragmentShaderSource(fragmentShaderSource);

				return p;
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
			const std::string&
			vertexShaderSource()
			{
				return _vertexShaderSource;
			}

			inline
			void
			vertexShaderSource(const std::string& source)
			{
				_vertexShaderSource	= source;
			}

			inline
			const std::string&
			fragmentShaderSource()
			{
				return _fragmentShaderSource;
			}

			inline
			void
			fragmentShaderSource(const std::string& source)
			{
				_fragmentShaderSource = source;
			}

			inline
			ProgramInputsPtr
			inputs()
			{
				return _inputs;
			}

			void
			upload();

			void
			dispose();

		private:
			Program(AbstractContextPtr context);
		};
	}
}
