/*
Copyright (c) 2014 Aerys

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
			typedef std::shared_ptr<render::AbstractContext>	AbsContextPtr;

		private:
			const std::string		_name;
			std::shared_ptr<Shader> _vertexShader;
			std::shared_ptr<Shader>	_fragmentShader;
			ProgramInputs   		_inputs;

			std::set<std::string>	_setUniforms;
            std::set<std::string>   _setTextures;
            std::set<std::string>   _setAttributes;
			std::set<std::string>	_definedMacros;

		public:
			inline static
			Ptr
			create(const std::string& name, AbsContextPtr context)
			{
				return std::shared_ptr<Program>(new Program(name, context));
			}

			inline static
			Ptr
			create(Ptr program, bool deepCopy = false)
			{
				auto p = create(program->_name, program->_context);

				p->_vertexShader	= deepCopy ? Shader::create(program->_vertexShader) : program->_vertexShader;
				p->_fragmentShader	= deepCopy ? Shader::create(program->_fragmentShader) : program->_fragmentShader;
				p->_inputs			= program->_inputs;
                p->_setTextures     = program->_setTextures;
                p->_setAttributes   = program->_setAttributes;

				return p;
			}

			inline static
			Ptr
			create(const std::string		name,
				   AbsContextPtr		    context,
				   std::shared_ptr<Shader>	vertexShader,
				   std::shared_ptr<Shader>	fragmentShader)
			{
				auto p = create(name, context);

				p->_vertexShader  = vertexShader;
				p->_fragmentShader = fragmentShader;

				return p;
			}

			inline
			const std::string&
			name()
			{
				return _name;
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
            const std::set<std::string>&
            setTextureNames() const
            {
                return _setTextures;
            }

            inline
            const std::set<std::string>&
            setAttributeNames() const
            {
                return _setAttributes;
            }

			inline
			const std::set<std::string>&
			setUniformNames() const
			{
				return _setUniforms;
			}

			inline
			const std::set<std::string>&
			definedMacroNames() const
			{
				return _definedMacros;
			}

			inline
			const ProgramInputs&
			inputs() const
			{
				return _inputs;
			}

			void
			upload();

			void
			dispose();

			template <typename T, int size>
			Program&
			setUniform(const std::string& name, uint count, T* v)
			{
                auto it = std::find_if(_inputs.uniforms().begin(), _inputs.uniforms().end(), [&](const ProgramInputs::UniformInput& u)
                {
                    return u.name == name;
                });

                if (it != _inputs.uniforms().end())
                {
                    auto oldProgram = _context->currentProgram();

                    _context->setProgram(_id);
                    setUniformOnContext<T, size>(it->location, count, v);
                    _context->setProgram(oldProgram);

					_setUniforms.insert(name);
                }

                return *this;
			}

            template <typename T>
            Program&
            setUniform(const std::string& name, T v)
            {
                return setUniform<T, 1>(name, 1, &v);
            }

            template <typename T, int P>
            Program&
            setUniform(const std::string& name, math::tvec2<T, P> value)
            {
                return setUniform<T, 2>(name, 1, math::value_ptr(value));
            }

            template <typename T, int P>
            Program&
            setUniform(const std::string& name, math::tvec3<T, P> value)
            {
                return setUniform<T, 3>(name, 1, math::value_ptr(value));
            }

            template <typename T, int P>
            Program&
            setUniform(const std::string& name, math::tvec4<T, P> value)
            {
                return setUniform<T, 4>(name, 1, math::value_ptr(value));
            }

            Program&
			setUniform(const std::string&, AbstractTexturePtr);

            Program&
			setUniform(const std::string&, TexturePtr);

            Program&
			setUniform(const std::string&, CubeTexturePtr);

            Program&
            define(const std::string& macroName)
            {
                _vertexShader->define(macroName);
                _fragmentShader->define(macroName);
				_definedMacros.insert(macroName);

                return *this;
            }

            template <typename T>
            Program&
            define(const std::string& macroName, T value)
            {
                _vertexShader->define(macroName, value);
                _fragmentShader->define(macroName, value);
				_definedMacros.insert(macroName);

                return *this;
            }

            inline
            Program&
            setAttribute(const std::string& name, const VertexAttribute& attribute)
            {
                return setAttribute(name, attribute, name);
            }

            Program&
            setAttribute(const std::string& name, const VertexAttribute& attribute, const std::string& attributeName);

            ~Program()
            {
                dispose();
            }

		private:
			Program(const std::string& name, AbsContextPtr context);

            template <typename T, size_t size>
            void
            setUniformOnContext(uint location, uint count, T* v)
            {}
		};

        template <>
        inline
        void
        Program::setUniformOnContext<float, 1>(uint location, uint count, float* v)
        {
            _context->setUniformFloat(location, count, v);
        }

        template <>
        inline
        void
        Program::setUniformOnContext<float, 2>(uint location, uint count, float* v)
        {
            _context->setUniformFloat2(location, count, v);
        }

        template <>
        inline
        void
        Program::setUniformOnContext<float, 3>(uint location, uint count, float* v)
        {
            _context->setUniformFloat3(location, count, v);
        }

        template <>
        inline
        void
        Program::setUniformOnContext<float, 4>(uint location, uint count, float* v)
        {
            _context->setUniformFloat4(location, count, v);
        }

        template <>
        inline
        void
        Program::setUniformOnContext<int, 1>(uint location, uint count, int* v)
        {
            _context->setUniformInt(location, count, v);
        }

        template <>
        inline
        void
        Program::setUniformOnContext<int, 2>(uint location, uint count, int* v)
        {
            _context->setUniformInt2(location, count, v);
        }

        template <>
        inline
        void
        Program::setUniformOnContext<int, 3>(uint location, uint count, int* v)
        {
            _context->setUniformInt3(location, count, v);
        }

        template <>
        inline
        void
        Program::setUniformOnContext<int, 4>(uint location, uint count, int* v)
        {
            _context->setUniformInt4(location, count, v);
        }
	}
}
