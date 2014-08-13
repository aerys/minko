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
    _states.reserve(maxNumMacros);

	unsigned int macroId = 0;
	for (const auto& macroNameAndBinding : macroBindings)
    {
        const auto&	macroName = macroNameAndBinding.first;
        const auto&	macroBinding = macroNameAndBinding.second;
        auto propertyName = Container::getActualPropertyName(variables, macroBinding.propertyName);
        auto container = macroBinding.source == BindingSource::TARGET
            ? targetData
            : (macroBinding.source == BindingSource::RENDERER ? rendererData : rootData);
        bool macroIsDefined = container.hasProperty(propertyName);
        bool macroIsInteger = (macroIsDefined && macroBinding.isInteger)
            || macroBinding.defaultState == MacroBinding::State::DEFINED_INTEGER_VALUE;

        if (macroIsDefined || macroBinding.defaultState != MacroBinding::State::UNDEFINED)
		{
			// WARNING: we do not support more than 32 macro bindings
            if (macroId == maxNumMacros)
				throw;

			_mask |= 1 << macroId; // update program signature

            _macros.push_back(macroName);
            if (macroIsInteger)
			{
				int	value = macroIsDefined ? container.get<int>(propertyName) : macroBinding.defaultValue.value;

				value = std::max(macroBinding.minValue, std::min(macroBinding.maxValue, value));

				// update program signature
				_values.push_back(value); 
                _states.push_back(MacroBinding::State::DEFINED_INTEGER_VALUE);
			}
            else
                _states.push_back(MacroBinding::State::DEFINED);
		}
		++macroId;
	}
}

ProgramSignature::ProgramSignature(const ProgramSignature& signature) :
    _mask(signature._mask),
    _values(signature._values),
    _states(signature._states)
{
}

bool 
ProgramSignature::operator==(const ProgramSignature& x) const
{
	if (_mask != x._mask)
		return false;

    auto j = 0;
	for (unsigned int i = 0; i < _states.size(); ++i)
        if (_states[i] != x._states[i] &&
            (_states[i] == MacroBinding::State::UNDEFINED
             || _states[i] == MacroBinding::State::DEFINED
             || (_states[i] == MacroBinding::State::DEFINED_INTEGER_VALUE && _values[j] != x._values[j++])))
			return false;

	return true;
}

void
ProgramSignature::updateProgram(Program& program) const
{
    auto valueIndex = 0;

    for (auto i = 0u; i < _states.size(); ++i)
    {
        switch (_states[i])
        {
            case MacroBinding::State::DEFINED:
                program.define(_macros[i]);
                break;
            case MacroBinding::State::DEFINED_INTEGER_VALUE:
                program.define(_macros[i], _values[valueIndex++]);
                break;
            default:
                //nothing
                break;
        }
    }
}
