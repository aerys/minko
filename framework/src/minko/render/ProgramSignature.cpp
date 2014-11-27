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
#include "minko/data/Store.hpp"
#include "minko/render/DrawCall.hpp"
#include "minko/data/BindingMap.hpp"
#include "minko/data/MacroBinding.hpp"

using namespace minko;
using namespace minko::render;
using namespace minko::data;

ProgramSignature::ProgramSignature(const data::MacroBindingMap&                         macroBindings,
                                   const std::unordered_map<std::string, std::string>&  variables,
                                   const Store&			                            targetData,
                                   const Store&			                            rendererData,
                                   const Store&			                            rootData) :
    _mask(0)
{
    const uint maxNumMacros = sizeof(MaskType) * 8;

    _values.reserve(maxNumMacros);
    _macros.reserve(maxNumMacros);

	unsigned int macroId = 0;
	for (const auto& macroNameAndBinding : macroBindings.bindings)
    {
        const auto&	macroName = macroNameAndBinding.first;
        const auto&	macroBinding = macroNameAndBinding.second;
        auto propertyName = Store::getActualPropertyName(variables, macroBinding.propertyName);
        auto& store = macroBinding.source == Binding::Source::TARGET
            ? targetData
            : (macroBinding.source == Binding::Source::RENDERER ? rendererData : rootData);
        bool macroIsDefined = store.hasProperty(propertyName);
        bool hasDefaultValue = macroBindings.defaultValues.hasProperty(propertyName);

        if (macroIsDefined || hasDefaultValue)
		{
			// WARNING: we do not support more than 32 macro bindings
            if (macroId == maxNumMacros)
				throw;

			_mask |= 1 << macroId; // update program signature

            _macros.push_back(macroName);
            _types.push_back(macroBinding.type);
            if (macroBinding.type != MacroBinding::Type::UNSET)
			{
				// update program signature
                _values.push_back(getValueFromStore(
                    macroBinding,
                    macroIsDefined ? store : macroBindings.defaultValues,
                    propertyName
                ));
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
    {
        if (_types[i] != x._types[i])
            return false;
        
        if (_types[i] != MacroBinding::Type::UNSET)
        {
            switch (_types[i])
            {
            case MacroBinding::Type::BOOL:
                if (Any::cast<bool>(_values[j]) != Any::cast<bool>(x._values[j]))
                    return false;
                break;
            case MacroBinding::Type::BOOL2:
                if (Any::cast<math::bvec2>(_values[j]) != Any::cast<math::bvec2>(x._values[j]))
                    return false;
                break;
            case MacroBinding::Type::BOOL3:
                if (Any::cast<math::bvec3>(_values[j]) != Any::cast<math::bvec3>(x._values[j]))
                    return false;
                break;
            case MacroBinding::Type::BOOL4:
                if (Any::cast<math::bvec4>(_values[j]) != Any::cast<math::bvec4>(x._values[j]))
                    return false;
                break;
            case MacroBinding::Type::INT:
                if (Any::cast<int>(_values[j]) != Any::cast<int>(x._values[j]))
                    return false;
                break;
            case MacroBinding::Type::INT2:
                if (Any::cast<math::ivec2>(_values[j]) != Any::cast<math::ivec2>(x._values[j]))
                    return false;
                break;
            case MacroBinding::Type::INT3:
                if (Any::cast<math::ivec3>(_values[j]) != Any::cast<math::ivec3>(x._values[j]))
                    return false;
                break;
            case MacroBinding::Type::INT4:
                if (Any::cast<math::ivec4>(_values[j]) != Any::cast<math::ivec4>(x._values[j]))
                    return false;
                break;
            case MacroBinding::Type::FLOAT:
                if (Any::cast<float>(_values[j]) != Any::cast<float>(x._values[j]))
                    return false;
                break;
            case MacroBinding::Type::FLOAT2:
                if (Any::cast<math::vec2>(_values[j]) != Any::cast<math::vec2>(x._values[j]))
                    return false;
                break;
            case MacroBinding::Type::FLOAT3:
                if (Any::cast<math::vec3>(_values[j]) != Any::cast<math::vec3>(x._values[j]))
                    return false;
                break;
            case MacroBinding::Type::FLOAT4:
                if (Any::cast<math::vec4>(_values[j]) != Any::cast<math::vec4>(x._values[j]))
                    return false;
                break;
            case MacroBinding::Type::UNSET:
                throw;
            }

            ++j;
        }
    }

	return true;
}

void
ProgramSignature::updateProgram(Program& program) const
{
    auto valueIndex = 0;
    auto i = 0;

    for (auto j = 0; j < 32; ++j)
    {
        if ((_mask & (1 << j)) != 0)
        {
            switch (_types[i])
            {
                case MacroBinding::Type::UNSET:
                    program.define(_macros[i]);
                    break;
                case MacroBinding::Type::BOOL:
                    program.define(_macros[i], Any::unsafe_cast<bool>(_values[i]));
                    break;
                case MacroBinding::Type::BOOL2:
                    program.define(_macros[i], Any::unsafe_cast<math::bvec2>(_values[i]));
                    break;
                case MacroBinding::Type::BOOL3:
                    program.define(_macros[i], Any::unsafe_cast<math::bvec3>(_values[i]));
                    break;
                case MacroBinding::Type::BOOL4:
                    program.define(_macros[i], Any::unsafe_cast<math::bvec4>(_values[i]));
                    break;
                case MacroBinding::Type::INT:
                    program.define(_macros[i], Any::unsafe_cast<int>(_values[i]));
                    break;
                case MacroBinding::Type::INT2:
                    program.define(_macros[i], Any::unsafe_cast<math::ivec2>(_values[i]));
                    break;
                case MacroBinding::Type::INT3:
                    program.define(_macros[i], Any::unsafe_cast<math::ivec3>(_values[i]));
                    break;
                case MacroBinding::Type::INT4:
                    program.define(_macros[i], Any::unsafe_cast<math::ivec4>(_values[i]));
                    break;
                case MacroBinding::Type::FLOAT:
                    program.define(_macros[i], Any::unsafe_cast<float>(_values[i]));
                    break;
                case MacroBinding::Type::FLOAT2:
                    program.define(_macros[i], Any::unsafe_cast<math::vec2>(_values[i]));
                    break;
                case MacroBinding::Type::FLOAT3:
                    program.define(_macros[i], Any::unsafe_cast<math::vec3>(_values[i]));
                    break;
                case MacroBinding::Type::FLOAT4:
                    program.define(_macros[i], Any::unsafe_cast<math::vec4>(_values[i]));
                    break;
            }

            ++i;
        }
    }
}

Any
ProgramSignature::getValueFromStore(const MacroBinding&     binding,
                                        const data::Store&  store,
                                        const std::string&      propertyName)
{
    switch (binding.type)
    {
    case MacroBinding::Type::BOOL:
        return store.get<bool>(propertyName);
        break;
    case MacroBinding::Type::BOOL2:
        return store.get<math::bvec2>(propertyName);
        break;
    case MacroBinding::Type::BOOL3:
        return store.get<math::bvec3>(propertyName);
        break;
    case MacroBinding::Type::BOOL4:
        return store.get<math::bvec4>(propertyName);
        break;
    case MacroBinding::Type::INT:
        return std::max(
            binding.minValue,
            std::min(binding.maxValue, store.get<int>(propertyName))
        );
        break;
    case MacroBinding::Type::INT2:
        return store.get<math::ivec2>(propertyName);
        break;
    case MacroBinding::Type::INT3:
        return store.get<math::ivec3>(propertyName);
        break;
    case MacroBinding::Type::INT4:
        return store.get<math::ivec4>(propertyName);
        break;
    case MacroBinding::Type::FLOAT:
        return store.get<float>(propertyName);
        break;
    case MacroBinding::Type::FLOAT2:
        return store.get<math::vec2>(propertyName);
        break;
    case MacroBinding::Type::FLOAT3:
        return store.get<math::vec3>(propertyName);
        break;
    case MacroBinding::Type::FLOAT4:
        return store.get<math::vec4>(propertyName);
        break;
    case MacroBinding::Type::UNSET:
        throw;
    }

    throw;
}
