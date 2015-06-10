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

#include "minko/data/Binding.hpp"
#include "minko/data/MacroBinding.hpp"
#include "minko/data/Store.hpp"

namespace minko
{
    namespace data
    {
        template <typename T = data::Binding>
        struct BindingMapBase
        {
            std::unordered_map<std::string, T> bindings;
            data::Store defaultValues;

            BindingMapBase()
            {
            }

            BindingMapBase(std::unordered_map<std::string, T>   bindings,
                           const data::Store&                   defaultValues) :
                bindings(bindings),
                defaultValues(defaultValues)
            {
            }

            BindingMapBase(const BindingMapBase& m) :
                bindings(m.bindings),
                defaultValues(m.defaultValues, true)
            {
            }

            virtual
            ~BindingMapBase() = default;
        };

        typedef BindingMapBase<data::Binding>   BindingMap;

        struct MacroBindingMap : public BindingMapBase<data::MacroBinding>
        {
            enum class MacroType
            {
                UNSET,
				INT,
				INT2,
				INT3,
				INT4,
				BOOL,
				BOOL2,
				BOOL3,
				BOOL4,
				FLOAT,
				FLOAT2,
				FLOAT3,
				FLOAT4,
				FLOAT9,
				FLOAT16
            };

            std::unordered_map<std::string, MacroType> types;

            MacroBindingMap() :
                BindingMapBase(),
                types()
            {
            }

            MacroBindingMap(std::unordered_map<std::string, MacroBinding>   bindings,
                            const data::Store&                              defaultValues,
                            std::unordered_map<std::string, MacroType>      types) :
                BindingMapBase(bindings, defaultValues),
                types(types)
            {
            }

            MacroBindingMap(const MacroBindingMap& m) :
                BindingMapBase(m.bindings, m.defaultValues),
                types(m.types)
            {
            }

            static
            MacroType
            stringToMacroType(const std::string& s)
            {
                if (s == "int")
                    return MacroType::INT;
                if (s == "int2")
                    return MacroType::INT2;
                if (s == "int3")
                    return MacroType::INT3;
                if (s == "int4")
                    return MacroType::INT4;

                if (s == "float")
                    return MacroType::FLOAT;
                if (s == "float2")
                    return MacroType::FLOAT2;
                if (s == "float3")
                    return MacroType::FLOAT3;
                if (s == "float4")
                    return MacroType::FLOAT4;

                if (s == "bool")
                    return MacroType::BOOL;
                if (s == "bool2")
                    return MacroType::BOOL2;
                if (s == "bool3")
                    return MacroType::BOOL3;
                if (s == "bool4")
                    return MacroType::BOOL4;

                return MacroType::UNSET;
            }
        };
    }
}
