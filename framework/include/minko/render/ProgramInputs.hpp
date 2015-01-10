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

namespace minko
{
	namespace render
	{
		class ProgramInputs
		{
		public:
			enum class Type
			{
				unknown,
				int1,
				int2,
				int3,
				int4,
				bool1,
				bool2,
				bool3,
				bool4,
				float1,
				float2,
				float3,
				float4,
				float9,
				float16,
				sampler2d,
				samplerCube
			};

            struct AbstractInput
            {
                std::string name;
                int location;

                virtual
                ~AbstractInput()
                {}

            protected:
                AbstractInput(const std::string& name, int location) :
                    name(name),
                    location(location)
                {}
            };

            struct UniformInput : public AbstractInput
            {
                Type type;
				int size;

                UniformInput(const std::string& name, int location, int size, Type type) :
                    AbstractInput(name, location),
					size(size),
                    type(type)
                {}
            };

            struct AttributeInput : public AbstractInput
            {
                AttributeInput(const std::string& name, int location) :
                    AbstractInput(name, location)
                {}
            };

		public:
			static
			std::string
			typeToString(const Type type)
			{
				switch (type)
				{
					case Type::unknown:
						return "unknown";
					case Type::int1:
						return "int1";
					case Type::int2:
						return "int2";
					case Type::int3:
						return "int3";
					case Type::int4:
						return "int4";
					case Type::bool1:
						return "bool1";
					case Type::bool2:
						return "bool2";
					case Type::bool3:
						return "bool3";
					case Type::bool4:
						return "bool4";
					case Type::float1:
						return "float1";
					case Type::float2:
						return "float2";
					case Type::float3:
						return "float3";
					case Type::float4:
						return "float4";
					case Type::float9:
						return "float9";
					case Type::float16:
						return "float16";
					case Type::sampler2d:
						return "sampler2d";
					case Type::samplerCube:
						return "samplerCube";
					default:
						throw std::invalid_argument("type");
				}
			}

            inline
            const std::vector<UniformInput>&
            uniforms() const
            {
                return _uniforms;
            }

            inline
            const std::vector<AttributeInput>&
            attributes() const
            {
                return _attributes;
            }

            ProgramInputs()
            {}

            ProgramInputs(const ProgramInputs& inputs) :
                _uniforms(inputs._uniforms),
                _attributes(inputs._attributes)
            {}

            ProgramInputs(ProgramInputs&& p) :
                _uniforms(std::move(p._uniforms)),
                _attributes(std::move(p._attributes))
            {}

            ProgramInputs(const std::vector<UniformInput>&     uniforms,
                          const std::vector<AttributeInput>&   attributes) :
                _uniforms(uniforms),
                _attributes(attributes)
            {}

            ProgramInputs(std::vector<UniformInput>&&     uniforms,
                          std::vector<AttributeInput>&&   attributes) :
                _uniforms(uniforms),
                _attributes(attributes)
            {}

            ProgramInputs&
            operator=(ProgramInputs&& rhs)
            {
                if (this != &rhs)
                {
                    _uniforms = std::move(rhs._uniforms);
                    _attributes = std::move(rhs._attributes);
                }

                return *this;
            }

            ProgramInputs&
            operator=(const ProgramInputs& rhs)
            {
                if (this != &rhs)
                {
                    _uniforms = rhs._uniforms;
                    _attributes = rhs._attributes;
                }

                return *this;
            }

        private:
            std::vector<UniformInput>   _uniforms;
            std::vector<AttributeInput> _attributes;
		};
	}
}
