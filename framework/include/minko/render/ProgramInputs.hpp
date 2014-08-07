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
            typedef std::shared_ptr<ProgramInputs>    Ptr;

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
                samplerCube,
                attribute,
                constant
            };

        public:
            static
            Ptr
            create(std::shared_ptr<AbstractContext> context,
                   const unsigned int               program,
                   const std::vector<std::string>&  names,
                   const std::vector<Type>&         types,
                   const std::vector<unsigned int>& locations)
            {
                return std::shared_ptr<ProgramInputs>(new ProgramInputs(
                    context,
                    program,
                    names,
                    types,
                    locations
                ));
            }

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

        public:

            inline
            bool
            hasName(const std::string& name) const
            {
                return _nameToType.find(name) != _nameToType.end();
            }

            inline
            const std::vector<std::string>&
            names() const
            {
                return _names;
            }

            inline
            const std::vector<Type>&
            types() const
            {
                return _types;
            }

            inline
            const std::vector<unsigned int>&
            locations() const
            {
                return _locations;
            }

            inline
            const int
            location(const std::string& name) const
            {
                auto foundLocationIt = _nameToLocation.find(name);

                return foundLocationIt != _nameToLocation.end() ? (int)foundLocationIt->second : -1;
            }

            inline
            const Type
            type(const std::string& name) const
            {
                auto foundTypeIt = _nameToType.find(name);

                return foundTypeIt != _nameToType.end() ? foundTypeIt->second : Type::unknown;
            }

        private:
            std::shared_ptr<AbstractContext>                _context;
            const unsigned int                              _program;
            std::vector<std::string>                        _names;
            std::vector<Type>                               _types;
            std::vector<unsigned int>                       _locations;
            std::unordered_map<std::string, Type>           _nameToType;
            std::unordered_map<std::string, unsigned int>   _nameToLocation;

        private:
            ProgramInputs(std::shared_ptr<AbstractContext>  context,
                          const unsigned int                program,
                          const std::vector<std::string>&   names,
                          const std::vector<Type>&          types,
                          const std::vector<unsigned int>&  locations) :
                _context(context),
                _program(program),
                _names(names),
                _types(types),
                _locations(locations),
                _nameToType(),
                _nameToLocation()
            {
#ifdef DEBUG
                if (_types.size() != _names.size() || _locations.size() != _names.size())
                    throw;
#endif // DEBUG

                for (unsigned int i = 0; i < _names.size(); ++i)
                {
                    const std::string& name = _names[i];
                    _nameToType[name]       = _types[i];
                    _nameToLocation[name]   = _locations[i];
                }
            }
        };
    }
}
