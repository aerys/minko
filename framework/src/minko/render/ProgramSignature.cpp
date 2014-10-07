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

#include "minko/render/ProgramSignature.hpp"

#include "minko/render/Pass.hpp"
#include "minko/data/Container.hpp"
#include "minko/render/DrawCall.hpp"
#include "minko/data/MacroBinding.hpp"

using namespace minko;
using namespace minko::render;
using namespace minko::data;

ProgramSignature::ProgramSignature(const data::MacroBindingMap&                         macroBindings,
                                   const std::unordered_map<std::string, std::string>&  variables,
                                   const Container&			                            targetData,
                                   const Container&			                            rendererData,
                                   const Container&			                            rootData) :
    _mask(0)
{
    const uint maxNumMacros = sizeof(MaskType) * 8;

    _values.reserve(maxNumMacros);
    _macros.reserve(maxNumMacros);

	unsigned int macroId = 0;
	for (const auto& macroNameAndBinding : macroBindings)
    {
        const auto&	macroName = macroNameAndBinding.first;
        const auto&	macroBinding = macroNameAndBinding.second;
        auto propertyName = Container::getActualPropertyName(variables, macroBinding.propertyName());
        auto& container = macroBinding.source() == Binding::Source::TARGET
            ? targetData
            : (macroBinding.source() == Binding::Source::RENDERER ? rendererData : rootData);
        bool macroIsDefined = container.hasProperty(propertyName);

        if (macroIsDefined || macroBinding.hasDefautValue())
		{
			// WARNING: we do not support more than 32 macro bindings
            if (macroId == maxNumMacros)
				throw;

			_mask |= 1 << macroId; // update program signature

            _macros.push_back(macroName);
            _types.push_back(macroBinding.type());
            if (macroBinding.type() != Binding::Type::UNSET)
			{
                auto value = macroIsDefined
                    ? getValueFromContainer(macroBinding, container, propertyName)
                    : getDefaultValue(macroBinding);

				// update program signature
				_values.push_back(value); 
			}
		}
		++macroId;
	}
}

ProgramSignature::ProgramSignature(const ProgramSignature& signature) :
    _mask(signature._mask),
    _values(signature._values)
{
}

bool 
ProgramSignature::operator==(const ProgramSignature& x) const
{
	if (_mask != x._mask)
		return false;

    auto j = 0;
	for (unsigned int i = 0; i < _types.size(); ++i)
        if (_types[i] != x._types[i] || (_types[i] != Binding::Type::UNSET && _values[j] != x._values[j++]))
			return false;

	return true;
}

void
ProgramSignature::updateProgram(Program& program) const
{
    auto valueIndex = 0;

    for (auto i = 0u; i < _types.size(); ++i)
    {
        if ((_mask & (1 << i)) != 0)
        {
            switch (_types[i])
            {
                case Binding::Type::UNSET:
                    program.define(_macros[i]);
                    break;
                case Binding::Type::BOOL:
                    program.define(_macros[i], Any::unsafe_cast<bool>(_values[valueIndex++]));
                    break;
                case Binding::Type::BOOL2:
                    program.define(_macros[i], Any::unsafe_cast<math::bvec2>(_values[valueIndex++]));
                    break;
                case Binding::Type::BOOL3:
                    program.define(_macros[i], Any::unsafe_cast<math::bvec3>(_values[valueIndex++]));
                    break;
                case Binding::Type::BOOL4:
                    program.define(_macros[i], Any::unsafe_cast<math::bvec4>(_values[valueIndex++]));
                    break;
                case Binding::Type::INT:
                    program.define(_macros[i], Any::unsafe_cast<int>(_values[valueIndex++]));
                    break;
                case Binding::Type::INT2:
                    program.define(_macros[i], Any::unsafe_cast<math::ivec2>(_values[valueIndex++]));
                    break;
                case Binding::Type::INT3:
                    program.define(_macros[i], Any::unsafe_cast<math::ivec3>(_values[valueIndex++]));
                    break;
                case Binding::Type::INT4:
                    program.define(_macros[i], Any::unsafe_cast<math::ivec4>(_values[valueIndex++]));
                    break;
                case Binding::Type::FLOAT:
                    program.define(_macros[i], Any::unsafe_cast<float>(_values[valueIndex++]));
                    break;
                case Binding::Type::FLOAT2:
                    program.define(_macros[i], Any::unsafe_cast<math::vec2>(_values[valueIndex++]));
                    break;
                case Binding::Type::FLOAT3:
                    program.define(_macros[i], Any::unsafe_cast<math::vec3>(_values[valueIndex++]));
                    break;
                case Binding::Type::FLOAT4:
                    program.define(_macros[i], Any::unsafe_cast<math::vec4>(_values[valueIndex++]));
                    break;
            }
        }
    }
}

Any
ProgramSignature::getValueFromContainer(const MacroBinding&     binding,
                                        const data::Container&  container,
                                        const std::string&      propertyName)
{
    switch (binding.type())
    {
    case Binding::Type::BOOL:
        return container.get<bool>(propertyName);
        break;
    case Binding::Type::BOOL2:
        return container.get<math::bvec2>(propertyName);
        break;
    case Binding::Type::BOOL3:
        return container.get<math::bvec3>(propertyName);
        break;
    case Binding::Type::BOOL4:
        return container.get<math::bvec4>(propertyName);
        break;
    case Binding::Type::INT:
        return std::max(
            binding.minValue(),
            std::min(binding.maxValue(), container.get<int>(propertyName))
        );
        break;
    case Binding::Type::INT2:
        return container.get<math::ivec2>(propertyName);
        break;
    case Binding::Type::INT3:
        return container.get<math::ivec3>(propertyName);
        break;
    case Binding::Type::INT4:
        return container.get<math::ivec4>(propertyName);
        break;
    case Binding::Type::FLOAT:
        return container.get<float>(propertyName);
        break;
    case Binding::Type::FLOAT2:
        return container.get<math::vec2>(propertyName);
        break;
    case Binding::Type::FLOAT3:
        return container.get<math::vec3>(propertyName);
        break;
    case Binding::Type::FLOAT4:
        return container.get<math::vec4>(propertyName);
        break;
    }

    throw;
}

Any
ProgramSignature::getDefaultValue(const MacroBinding& binding)
{
    switch (binding.type())
    {
        case Binding::Type::BOOL:
            return binding.defaultValue().values<bool>()[0];
            break;
        case Binding::Type::BOOL2:
            return math::make_vec2((bool*)&binding.defaultValue().values<bool>());
            break;
        case Binding::Type::BOOL3:
            return math::make_vec3((bool*)&binding.defaultValue().values<bool>());
            break;
        case Binding::Type::BOOL4:
            return math::make_vec4((bool*)&binding.defaultValue().values<bool>());
            break;
        case Binding::Type::INT:
            return binding.defaultValue().values<int>()[0];
            break;
        case Binding::Type::INT2:
            return math::make_vec2((int*)&binding.defaultValue().values<int>());
            break;
        case Binding::Type::INT3:
            return math::make_vec3((int*)&binding.defaultValue().values<int>());
            break;
        case Binding::Type::INT4:
            return math::make_vec4((int*)&binding.defaultValue().values<int>());
            break;
        case Binding::Type::FLOAT:
            return binding.defaultValue().values<float>()[0];
            break;
        case Binding::Type::FLOAT2:
            return math::make_vec2((float*)&binding.defaultValue().values<float>());
            break;
        case Binding::Type::FLOAT3:
            return math::make_vec3((float*)&binding.defaultValue().values<float>());
            break;
        case Binding::Type::FLOAT4:
            return math::make_vec4((float*)&binding.defaultValue().values<float>());
            break;
    }

    throw;
}
