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
#include "minko/render/ProgramInputs.hpp"
#include "minko/render/Texture.hpp"
#include "minko/render/AbstractContext.hpp"

namespace minko
{
	namespace render
	{
		class Program :
			public AbstractResource
		{
		public:
			typedef std::shared_ptr<Program>	Ptr;

		private:
			typedef std::shared_ptr<render::AbstractContext>	AbstractContextPtr;
			typedef std::shared_ptr<render::ProgramInputs>		ProgramInputsPtr;
			typedef std::shared_ptr<Texture>					TexturePtr;

		private:
			std::shared_ptr<Shader>				_vertexShader;
			std::shared_ptr<Shader>				_fragmentShader;
			ProgramInputsPtr					_inputs;

			std::unordered_map<int, TexturePtr> _textures;

		public:
			inline static
			Ptr
			create(AbstractContextPtr context)
			{
				return std::shared_ptr<Program>(new Program(context));
			}

			inline static
			Ptr
			create(AbstractContextPtr		context,
				   std::shared_ptr<Shader>	vertexShader,
				   std::shared_ptr<Shader>	fragmentShader)
			{
				auto p = create(context);

				p->_vertexShader  = vertexShader;
				p->_fragmentShader = fragmentShader;

				return p;
			}

			inline
			std::shared_ptr<Shader>
			vertexShader() const
			{
				return _vertexShader;
			}

			inline
			std::shared_ptr<Shader>
			fragmentShader() const
			{
				return _fragmentShader;
			}

			inline
			ProgramInputsPtr
			inputs() const
			{
				return _inputs;
			}

			inline
			const std::unordered_map<int, TexturePtr>
			textures() const
			{
				return _textures;
			}

			void
			upload();

			void
			dispose();

			template <typename... T>
			void
			setUniform(const std::string& name, const T&... values)
			{
				if (!_inputs->hasName(name))
					return;

				auto oldProgram = _context->currentProgram();

				_context->setProgram(_id);
				_context->setUniform(_inputs->location(name), values...);
				_context->setProgram(oldProgram);
			}

			void
			setUniform(const std::string& name, std::shared_ptr<render::Texture> texture)
			{
				if (!_inputs->hasName(name))
					return;

				_textures[_inputs->location(name)] = texture;
			}

		private:
			Program(AbstractContextPtr context);
		};
	}
}
