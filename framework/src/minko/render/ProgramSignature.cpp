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

#include "ProgramSignature.hpp"

#include "minko/data/Container.hpp"

using namespace minko;
using namespace minko::render;
using namespace minko::data;

/*static*/ const uint ProgramSignature::MAX_NUM_BINDINGS = 32;

void
ProgramSignature::build(const MacroBindingMap&	macroBindings,
						data::Container::Ptr	targetData,
						data::Container::Ptr	rendererData,
						data::Container::Ptr	rootData)
{
	_mask = 0;
	_values.clear();
	_values.resize(MAX_NUM_BINDINGS, 0);

	unsigned int i = 0;

	for (auto& macroBinding : macroBindings)
    {
        auto& propertyName = std::get<0>(macroBinding.second);
		const auto& bindingSource = std::get<1>(macroBinding.second);
		const auto& container = propertyName.empty() ? nullptr
			: bindingSource == data::BindingSource::TARGET && targetData->hasProperty(propertyName) ? targetData
			: bindingSource == data::BindingSource::RENDERER && rendererData->hasProperty(propertyName) ? rendererData
			: bindingSource == data::BindingSource::ROOT && rootData->hasProperty(propertyName) ? rootData
			: nullptr;

        if (container && container->hasProperty(propertyName))
		{
			// WARNING: we do not support more than 32 macro bindings
			if (i == MAX_NUM_BINDINGS)
				throw;

			_mask |= 1 << i;
		}

		if (container && container->hasProperty(propertyName) && container->propertyHasType<int>(propertyName))
			_values[i] = container->get<int>(propertyName);

        ++i;
    }
}

bool 
ProgramSignature::operator==(const ProgramSignature& x) const
{
	if (_mask != x._mask)
		return false;

	for (unsigned int i = 0; i < MAX_NUM_BINDINGS; ++i)
	{
		if ((_mask >> i) != 0 
			&& _values[i] != x._values[i])
			return false;
	}
	return true;
}
