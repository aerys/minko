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
#include "minko/render/AbstractContext.hpp"
#include "minko/render/Shader.hpp"

namespace minko
{
	namespace render
	{
		class Program :
			public AbstractResource
		{
		public:
			typedef std::shared_ptr<Program>					Ptr;

		private:
			typedef std::shared_ptr<render::VertexBuffer>		VertexBufferPtr;
			typedef std::shared_ptr<render::IndexBuffer>		IndexBufferPtr;
			typedef std::shared_ptr<render::AbstractTexture>	AbstractTexturePtr;
			typedef std::shared_ptr<render::Texture>			TexturePtr;
			typedef std::shared_ptr<render::CubeTexture>		CubeTexturePtr;
			typedef std::shared_ptr<render::AbstractContext>	AbstractContextPtr;
			typedef std::shared_ptr<render::ProgramInputs>		ProgramInputsPtr;
			typedef std::shared_ptr<math::Vector2>				Vector2Ptr;
			typedef std::shared_ptr<math::Vector3>				Vector3Ptr;
			typedef std::shared_ptr<math::Vector4>				Vector4Ptr;

		private:
			std::shared_ptr<Shader>								_vertexShader;
			std::shared_ptr<Shader>								_fragmentShader;
			ProgramInputsPtr									_inputs;

			std::unordered_map<int, float>						_uniformFloat;
			std::unordered_map<int, Vector2Ptr>					_uniformFloat2;
			std::unordered_map<int, Vector3Ptr>					_uniformFloat3;
			std::unordered_map<int, Vector4Ptr>					_uniformFloat4;
			std::unordered_map<int, AbstractTexturePtr>			_textures;
			std::unordered_map<int, VertexBufferPtr>			_vertexBuffers;
			IndexBufferPtr										_indexBuffer;

		public:
			inline static
			Ptr
			create(AbstractContextPtr context)
			{
				return std::shared_ptr<Program>(new Program(context));
			}

			inline static
			Ptr
			create(Ptr program, bool deepCopy = false)
			{
				auto p = create(program->_context);

				p->_vertexShader	= deepCopy ? Shader::create(program->_vertexShader) : program->_vertexShader;
				p->_fragmentShader	= deepCopy ? Shader::create(program->_fragmentShader) : program->_fragmentShader;
				p->_inputs			= program->inputs();
				p->_textures		= program->_textures;
				p->_vertexBuffers	= program->_vertexBuffers;
				p->_indexBuffer		= program->_indexBuffer;

				return p;
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
			const std::unordered_map<int, float>&
			uniformFloat() const
			{
				return _uniformFloat;
			}

			inline
			const std::unordered_map<int, Vector2Ptr>&
			uniformFloat2() const
			{
				return _uniformFloat2;
			}

			inline
			const std::unordered_map<int, Vector3Ptr>&
			uniformFloat3() const
			{
				return _uniformFloat3;
			}

			inline
			const std::unordered_map<int, Vector4Ptr>&
			uniformFloat4() const
			{
				return _uniformFloat4;
			}

			inline
			const std::unordered_map<int, AbstractTexturePtr>&
			textures() const
			{
				return _textures;
			}

			inline
			const std::unordered_map<int, VertexBufferPtr>&
			vertexBuffers() const
			{
				return _vertexBuffers;
			}

			inline
			bool
			hasVertexBufferLocation(uint vertexLocation) const
			{
				return _vertexBuffers.find(vertexLocation) != _vertexBuffers.end();
			}

			inline
			IndexBufferPtr
			indexBuffer() const
			{
				return _indexBuffer;
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
			setUniform(const std::string&, float);

			void
			setUniform(const std::string&, float, float);

			void
			setUniform(const std::string&, float, float, float);

			void
			setUniform(const std::string&, float, float, float, float);

			void
			setUniform(const std::string&, AbstractTexturePtr);

			void
			setUniform(const std::string&, TexturePtr);
			
			void
			setUniform(const std::string&, CubeTexturePtr);

			void
			setVertexAttribute(const std::string& name, unsigned int attributeSize, const std::vector<float>& data);

			void
			setIndexBuffer(const std::vector<unsigned short>&);

		private:
			Program(AbstractContextPtr context);
		};
	}
}
