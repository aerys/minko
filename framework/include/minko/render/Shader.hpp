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

#include "minko/render/AbstractResource.hpp"

namespace minko
{
	namespace render
	{
		class Shader :
			public AbstractResource
		{
		public:
			typedef std::shared_ptr<Shader> Ptr;

			enum class Type
			{
				VERTEX_SHADER,
				FRAGMENT_SHADER
			};

		private:
			Type		_type;
			std::string _source;

		public:
			inline static
			Ptr
			create(std::shared_ptr<AbstractContext> context, Type type)
			{
				return std::shared_ptr<Shader>(new Shader(context, type));
			}

			inline static
			Ptr
			create(std::shared_ptr<AbstractContext> context, Type type, const std::string& source)
			{
				auto s = create(context, type);

				s->_source = source;

				return s;
			}

			inline static
			Ptr
			create(Ptr shader)
			{
				return create(shader->_context, shader->_type, shader->_source);
			}

			inline
			Type
			type() const
			{
				return _type;
			}

			inline
			const std::string&
			source()
			{
				return _source;
			}

			inline
			void
			source(const std::string& source)
			{
				_source = source;
			}

            inline
            void
            define(const std::string& macroName)
            {
                _source = "#define " + macroName + "\n" + _source;
            }

            template <typename T>
            inline
            void
            define(const std::string& macroName, T value)
            {
                _source = "#define " + macroName + " " + std::to_string(value) + "\n" + _source;
            }

			void
			dispose();

			void
			upload();

		private:
			Shader(std::shared_ptr<AbstractContext> context,
				   Type								type) :
				AbstractResource(context),
				_type(type)
			{
			}
		};
	}
}
