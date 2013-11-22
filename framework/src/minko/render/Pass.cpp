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

#include "Pass.hpp"

#include "minko/data/Container.hpp"
#include "minko/render/Program.hpp"
#include "minko/render/Shader.hpp"
#include "minko/render/DrawCall.hpp"
#include "minko/render/States.hpp"
#include "minko/render/ProgramSignature.hpp"

using namespace minko;
using namespace minko::render;

Pass::Pass(const std::string&				name,
		   std::shared_ptr<render::Program>	program,
		   const data::BindingMap&			attributeBindings,
		   const data::BindingMap&			uniformBindings,
		   const data::BindingMap&			stateBindings,
		   const data::MacroBindingMap&		macroBindings,
           std::shared_ptr<States>          states) :
	_name(name),
	_programTemplate(program),
	_attributeBindings(attributeBindings),
	_uniformBindings(uniformBindings),
	_stateBindings(stateBindings),
	_macroBindings(macroBindings),
    _states(states),
	_signatureToProgram()
{
}

std::shared_ptr<Program>
Pass::selectProgram(std::shared_ptr<data::Container> data,
					std::shared_ptr<data::Container> rendererData,
					std::shared_ptr<data::Container> rootData,
					std::list<std::string>&			 bindingDefines,
					std::list<std::string>&			 bindingValues)
{
	bindingDefines.clear();
	bindingValues.clear();

	Program::Ptr program;

	if (_macroBindings.size() == 0)
		program = _programTemplate;
	else
	{
		ProgramSignature signature;
		signature.build(_macroBindings, data, rendererData, rootData);

		const auto foundProgramIt = _signatureToProgram.find(signature);

		if (foundProgramIt != _signatureToProgram.end())
			program = foundProgramIt->second;
		else
		{
			const uint				signatureMask	= signature.mask();
			const std::vector<int>&	signatureValues	= signature.values();
			std::string				defines			= "";
			uint					i				= 0;

			// create shader header with #defines
			for (const auto& macroBinding : _macroBindings)
            {
				const auto& defaultValue = std::get<2>(macroBinding.second);
				const auto hasDefaultValue = defaultValue.semantic != data::MacroBindingDefaultValueSemantic::UNSET;

				if (hasDefaultValue || signatureMask & (1 << i))
				{
					const auto&	propertyName = std::get<0>(macroBinding.second);
					const auto& bindingSource = std::get<1>(macroBinding.second);
					const auto propetyExists = defaultValue.semantic == data::MacroBindingDefaultValueSemantic::PROPERTY_EXISTS;
					const auto& container = propertyName.empty() ? nullptr
						: bindingSource == data::BindingSource::TARGET && data->hasProperty(propertyName) ? data
						: bindingSource == data::BindingSource::RENDERER && rendererData->hasProperty(propertyName) ? rendererData
						: bindingSource == data::BindingSource::ROOT && rootData->hasProperty(propertyName) ? rootData
						: nullptr;

					if (defaultValue.semantic == data::MacroBindingDefaultValueSemantic::VALUE
						|| (container && container->propertyHasType<int>(propertyName)))
					{
						const auto defaultIntValue = defaultValue.value.value;

						if ((defaultIntValue > 0) || signatureValues[i] > 0)
						{
							auto value	= container ? signatureValues[i] : defaultIntValue;
							auto min	= std::get<3>(macroBinding.second);
							auto max	= std::get<4>(macroBinding.second);

							if ((min != -1 && value < min) || (max != -1 && value > max))
								return nullptr;

							defines += "#define " + macroBinding.first + " " + std::to_string(value) + "\n";
							bindingValues.push_back(propertyName);
						}
					}
					else if ((defaultValue.semantic == data::MacroBindingDefaultValueSemantic::PROPERTY_EXISTS
							  && defaultValue.value.propertyExists)
							 || (container && container->hasProperty(propertyName)))
					{
						defines += "#define " + macroBinding.first + "\n";
						bindingDefines.push_back(propertyName);
					}
				}
				++i;
				if (i == signatureValues.size())
					break;
            }

#ifdef MINKO_NO_GLSL_STRUCT
			defines += "#define MINKO_NO_GLSL_STRUCT\n";
#endif // MINKO_NO_GLSL_STRUCT

			// for program template by adding #defines
			auto vs = Shader::create(
				_programTemplate->context(),
				Shader::Type::VERTEX_SHADER,
				defines + _programTemplate->vertexShader()->source()
			);
			auto fs = Shader::create(
				_programTemplate->context(),
				Shader::Type::FRAGMENT_SHADER,
				defines + _programTemplate->fragmentShader()->source()
			);

			program = Program::create(_programTemplate->context(), vs, fs);

			// register the program to this signature
			_signatureToProgram[signature] = program;
		}
	}

	return finalizeProgram(program);
}

Program::Ptr
Pass::finalizeProgram(Program::Ptr program)
{
	if (program)
	{
		try
		{
			if (!program->vertexShader()->isReady())
				program->vertexShader()->upload();
			if (!program->fragmentShader()->isReady())
				program->fragmentShader()->upload();
			if (!program->isReady())
			{
				program->upload();

				for (auto& func : _uniformFunctions)
					func(program);
			}
		}
		catch (std::exception& e)
		{
			if (_fallback.length())
				return nullptr;

			throw e;
		}
	}

	return program;
}
