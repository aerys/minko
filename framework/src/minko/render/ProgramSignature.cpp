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

#include "minko/data/Container.hpp"
#include "minko/data/ContainerProperty.hpp"

using namespace minko;
using namespace minko::render;
using namespace minko::data;

/*static*/ const uint ProgramSignature::MAX_NUM_BINDINGS = 32;

void
ProgramSignature::build(const MacroBindingMap&			macroBindings,
						data::Container::Ptr			targetData,
						data::Container::Ptr			rendererData,
						data::Container::Ptr			rootData,
						std::string&					defines,
						std::list<ContainerProperty>&	booleanMacros,
						std::list<ContainerProperty>&	integerMacros,
						std::list<ContainerProperty>&	incorrectIntegerMacros)
{
	_mask = 0;
	_values.clear();
	_values.resize(MAX_NUM_BINDINGS, 0);

	defines.clear();

	booleanMacros.clear();
	integerMacros.clear();
	incorrectIntegerMacros.clear();

	unsigned int macroId = 0;

	for (auto& macroBinding : macroBindings)
    {
		ContainerProperty	macro					(macroBinding.second, targetData, rendererData, rootData);
		const bool			macroExists				= (macro.container() != nullptr); 
		const bool			isMacroInteger			= macroExists && macro.container()->propertyHasType<int>(macro.name(), true);

		const auto&			defaultMacro			= std::get<2>(macroBinding.second);
		const auto			defaultMacroExists		= defaultMacro.semantic == data::MacroBindingDefaultValueSemantic::PROPERTY_EXISTS;
		const bool			isDefaultMacroInteger	= defaultMacro.semantic == data::MacroBindingDefaultValueSemantic::VALUE;
		const bool			canUseDefaultMacro		= defaultMacroExists || isDefaultMacroInteger;

		if (macroExists || canUseDefaultMacro)
		{
			// WARNING: we do not support more than 32 macro bindings
			if (macroId == MAX_NUM_BINDINGS)
				throw;

			_mask |= 1 << macroId; // update program signature


			if (isMacroInteger || isDefaultMacroInteger)
			{
				const int	min		= std::get<3>(macroBinding.second);
				const int	max		= std::get<4>(macroBinding.second);

				int			value	= isMacroInteger 
					? macro.container()->get<int>(macro.name())
					: defaultMacro.value.value;

				// for beta 1 : clamp integer macros instead for using fallback technique
				value = std::max(min, std::min(max, value)); 

				// update program signature
				_values[macroId] = value; 

				if (value < min || value > max)
				{
					if (macroExists)
						incorrectIntegerMacros.push_back(macro);
				
					throw; // for beta 1, macros are clamped and cannot get out-of-bounds!
				}
				else
				{
					defines += "#define " + macroBinding.first + " " + std::to_string(value) + "\n";
				
					if (macroExists)
						integerMacros.push_back(macro);
				}
			}
			else if (macroExists || defaultMacroExists)
			{
				defines += "#define " + macroBinding.first + "\n";

				if (macroExists)
					booleanMacros.push_back(macro);
			}
		}
		++macroId;
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
