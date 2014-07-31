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

using namespace minko;
using namespace minko::render;
using namespace minko::data;

//#define DEACTIVATE_FALLBACK // since beta 1

ProgramSignature::ProgramSignature(const data::MacroBindingMap&             macroBindings,
                                   const data::TranslatedPropertyNameMap&   translatedPropertyNames,
                                   Container::Ptr			                targetData,
                                   Container::Ptr			                rendererData,
                                   Container::Ptr			                rootData,
                                   std::string&			                    defines) :
    _mask(0),
    _values(),
    _states()
{
	unsigned int macroId	= 0;
	for (const auto& macroNameAndBinding : macroBindings)
    {
        const auto&	macroName = macroNameAndBinding.first;
        const auto&	macroBinding = macroNameAndBinding.second;
        auto propertyName = macroBinding.propertyName;
        auto container = macroBinding.source == BindingSource::TARGET
            ? targetData
            : (macroBinding.source == BindingSource::RENDERER ? rendererData : rootData);
        bool macroIsDefined = container->hasProperty(propertyName);
        bool macroIsInteger = macroIsDefined && container->propertyHasType<int>(propertyName, true);

        if (translatedPropertyNames.count(propertyName) != 0)
            propertyName = translatedPropertyNames.at(propertyName);

        if (macroIsDefined || macroBinding.defaultState != MacroBindingState::UNDEFINED)
		{
			// WARNING: we do not support more than 32 macro bindings
			if (macroId == 32)
				throw;

			_mask |= 1 << macroId; // update program signature

            if (macroIsInteger || macroBinding.defaultState == MacroBindingState::DEFINED_INTEGER_VALUE)
			{
				int	value = macroIsDefined 
					? container->get<int>(propertyName)
					: macroBinding.defaultValue.value;

				value = std::max(macroBinding.minValue, std::min(macroBinding.maxValue, value)); 

				// update program signature
				_values[macroId] = value; 

				defines += "#define " + macroName + " " + std::to_string(value) + "\n";
			}
			else
				defines += "#define " + macroName + "\n";
		}
		++macroId;
	}

    while (macroId < 32)
    {
        _values[macroId] = 0;
        _states[macroId] = MacroBindingState::UNDEFINED;
    }
}

bool 
ProgramSignature::operator==(const ProgramSignature& x) const
{
	if (_mask != x._mask)
		return false;

	for (unsigned int i = 0; i < 32; ++i)
		if (_values[i] != x._values[i] && _states[i] != x._states[i])
			return false;

	return true;
}
