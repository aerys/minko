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

#include "sparsehash/sparse_hash_map"

using namespace minko;
using namespace minko::render;
using namespace minko::data;

const uint ProgramSignature::_maxNumMacros = sizeof(MaskType) * 8;

ProgramSignature::ProgramSignature(const data::MacroBindingMap& macroBindings,
                                   const EffectVariables&       variables,
                                   const Store&			        targetData,
                                   const Store&			        rendererData,
                                   const Store&			        rootData) :
    _mask(0)
{
    _values.reserve(_maxNumMacros);
    _macros.reserve(_maxNumMacros);

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
            MacroBindingMap::MacroType type = macroBindings.types.at(macroName);

			// WARNING: we do not support more than 64 macro bindings
            if (macroId == _maxNumMacros)
				throw;

			_mask |= MaskType(1) << macroId; // update program signature

            _macros.push_back(macroName);
            _types.push_back(type);
            if (type != MacroBindingMap::MacroType::UNSET)
			{
				// update program signature
                auto value = getValueFromStore(
                    macroIsDefined ? store : macroBindings.defaultValues,
                    propertyName,
                    type
                );

                if (type == MacroBindingMap::MacroType::INT)
                    value = std::max(
                        macroBinding.minValue,
                        std::min(macroBinding.maxValue, Any::cast<int>(value))
                    );

                _values.push_back(value);
			}
		}
		++macroId;
	}

    for (auto provider : macroBindings.defaultValues.providers())
    {
        for (const auto& propertyNameAndValue : provider->values())
        {
            _mask |= MaskType(1) << macroId;
            _macros.push_back(propertyNameAndValue.first);
            _types.push_back(macroBindings.types.at(*propertyNameAndValue.first));
            _values.push_back(*propertyNameAndValue.second);

            ++macroId;
        }
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

        if (_types[i] != MacroBindingMap::MacroType::UNSET)
        {
            switch (_types[i])
            {
            case MacroBindingMap::MacroType::BOOL:
                if (Any::cast<bool>(_values[j]) != Any::cast<bool>(x._values[j]))
                    return false;
                break;
            case MacroBindingMap::MacroType::BOOL2:
                if (Any::cast<math::bvec2>(_values[j]) != Any::cast<math::bvec2>(x._values[j]))
                    return false;
                break;
            case MacroBindingMap::MacroType::BOOL3:
                if (Any::cast<math::bvec3>(_values[j]) != Any::cast<math::bvec3>(x._values[j]))
                    return false;
                break;
            case MacroBindingMap::MacroType::BOOL4:
                if (Any::cast<math::bvec4>(_values[j]) != Any::cast<math::bvec4>(x._values[j]))
                    return false;
                break;
            case MacroBindingMap::MacroType::INT:
                if (Any::cast<int>(_values[j]) != Any::cast<int>(x._values[j]))
                    return false;
                break;
            case MacroBindingMap::MacroType::INT2:
                if (Any::cast<math::ivec2>(_values[j]) != Any::cast<math::ivec2>(x._values[j]))
                    return false;
                break;
            case MacroBindingMap::MacroType::INT3:
                if (Any::cast<math::ivec3>(_values[j]) != Any::cast<math::ivec3>(x._values[j]))
                    return false;
                break;
            case MacroBindingMap::MacroType::INT4:
                if (Any::cast<math::ivec4>(_values[j]) != Any::cast<math::ivec4>(x._values[j]))
                    return false;
                break;
            case MacroBindingMap::MacroType::FLOAT:
                if (Any::cast<float>(_values[j]) != Any::cast<float>(x._values[j]))
                    return false;
                break;
            case MacroBindingMap::MacroType::FLOAT2:
                if (Any::cast<math::vec2>(_values[j]) != Any::cast<math::vec2>(x._values[j]))
                    return false;
                break;
            case MacroBindingMap::MacroType::FLOAT3:
                if (Any::cast<math::vec3>(_values[j]) != Any::cast<math::vec3>(x._values[j]))
                    return false;
                break;
            case MacroBindingMap::MacroType::FLOAT4:
                if (Any::cast<math::vec4>(_values[j]) != Any::cast<math::vec4>(x._values[j]))
                    return false;
                break;
            case MacroBindingMap::MacroType::UNSET:
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
    auto typeIndex  = 0;
    auto macroIndex = 0;
    auto valueIndex = 0;

    for (auto j = 0; j < _maxNumMacros; ++j)
    {
        if ((_mask & (MaskType(1) << j)) != 0)
        {
            switch (_types[typeIndex])
            {
                case MacroBindingMap::MacroType::UNSET:
                    program.define(*_macros[macroIndex]);
                    break;
                case MacroBindingMap::MacroType::BOOL:
                    program.define(*_macros[macroIndex], Any::unsafe_cast<bool>(_values[valueIndex++]));
                    break;
                case MacroBindingMap::MacroType::BOOL2:
                    program.define(*_macros[macroIndex], Any::unsafe_cast<math::bvec2>(_values[valueIndex++]));
                    break;
                case MacroBindingMap::MacroType::BOOL3:
                    program.define(*_macros[macroIndex], Any::unsafe_cast<math::bvec3>(_values[valueIndex++]));
                    break;
                case MacroBindingMap::MacroType::BOOL4:
                    program.define(*_macros[macroIndex], Any::unsafe_cast<math::bvec4>(_values[valueIndex++]));
                    break;
                case MacroBindingMap::MacroType::INT:
                    program.define(*_macros[macroIndex], Any::unsafe_cast<int>(_values[valueIndex++]));
                    break;
                case MacroBindingMap::MacroType::INT2:
                    program.define(*_macros[macroIndex], Any::unsafe_cast<math::ivec2>(_values[valueIndex++]));
                    break;
                case MacroBindingMap::MacroType::INT3:
                    program.define(*_macros[macroIndex], Any::unsafe_cast<math::ivec3>(_values[valueIndex++]));
                    break;
                case MacroBindingMap::MacroType::INT4:
                    program.define(*_macros[macroIndex], Any::unsafe_cast<math::ivec4>(_values[valueIndex++]));
                    break;
                case MacroBindingMap::MacroType::FLOAT:
                    program.define(*_macros[macroIndex], Any::unsafe_cast<float>(_values[valueIndex++]));
                    break;
                case MacroBindingMap::MacroType::FLOAT2:
                    program.define(*_macros[macroIndex], Any::unsafe_cast<math::vec2>(_values[valueIndex++]));
                    break;
                case MacroBindingMap::MacroType::FLOAT3:
                    program.define(*_macros[macroIndex], Any::unsafe_cast<math::vec3>(_values[valueIndex++]));
                    break;
                case MacroBindingMap::MacroType::FLOAT4:
                    program.define(*_macros[macroIndex], Any::unsafe_cast<math::vec4>(_values[valueIndex++]));
                    break;
            }

            ++typeIndex;
            ++macroIndex;
        }
    }
}

Any
ProgramSignature::getValueFromStore(const data::Store&                  store,
                                    const std::string&                  propertyName,
                                    const MacroBindingMap::MacroType&   type)
{
    switch (type)
    {
    case MacroBindingMap::MacroType::BOOL:
        return store.get<bool>(propertyName);
    case MacroBindingMap::MacroType::BOOL2:
        return store.get<math::bvec2>(propertyName);
    case MacroBindingMap::MacroType::BOOL3:
        return store.get<math::bvec3>(propertyName);
    case MacroBindingMap::MacroType::BOOL4:
        return store.get<math::bvec4>(propertyName);
    case MacroBindingMap::MacroType::INT:
        return store.get<int>(propertyName);
    case MacroBindingMap::MacroType::INT2:
        return store.get<math::ivec2>(propertyName);
    case MacroBindingMap::MacroType::INT3:
        return store.get<math::ivec3>(propertyName);
    case MacroBindingMap::MacroType::INT4:
        return store.get<math::ivec4>(propertyName);
    case MacroBindingMap::MacroType::FLOAT:
        return store.get<float>(propertyName);
    case MacroBindingMap::MacroType::FLOAT2:
        return store.get<math::vec2>(propertyName);
    case MacroBindingMap::MacroType::FLOAT3:
        return store.get<math::vec3>(propertyName);
    case MacroBindingMap::MacroType::FLOAT4:
        return store.get<math::vec4>(propertyName);
    case MacroBindingMap::MacroType::UNSET:
        throw;
    }

    throw;
}
