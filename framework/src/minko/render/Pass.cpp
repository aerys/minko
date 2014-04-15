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

#include "minko/render/Pass.hpp"

#include "minko/data/Container.hpp"
#include "minko/data/ContainerProperty.hpp"
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
           std::shared_ptr<States>          states,
		   const std::string&				fallback) :
	_name(name),
	_programTemplate(program),
	_attributeBindings(attributeBindings),
	_uniformBindings(uniformBindings),
	_stateBindings(stateBindings),
	_macroBindings(macroBindings),
    _states(states),
	_fallback(fallback),
	_signatureToProgram(),
	_uniformFunctions(),
	_attributeFunctions(),
	_indexFunction(nullptr),
	_undefinedMacros(),
	_definedBoolMacros(),
	_definedIntMacros()
{
}

std::shared_ptr<Program>
Pass::selectProgram(FormatNameFunction					formatNameFunc,
					std::shared_ptr<data::Container>	targetData,
					std::shared_ptr<data::Container>	rendererData,
					std::shared_ptr<data::Container>	rootData,
					std::list<data::ContainerProperty>&	booleanMacros,
					std::list<data::ContainerProperty>&	integerMacros,
					std::list<data::ContainerProperty>&	incorrectIntegerMacros)
{
	booleanMacros.clear();
	integerMacros.clear();
	incorrectIntegerMacros.clear();

	Program::Ptr program;

	if (_macroBindings.size() == 0)
		program = _programTemplate;
	else
	{
		std::string			defines = "";
		ProgramSignature	signature;

		signature.build(
			shared_from_this(),
			formatNameFunc,
			targetData,
			rendererData,
			rootData,
			defines, 
			booleanMacros,
			integerMacros,
			incorrectIntegerMacros
		);

#ifdef DEBUG_FALLBACK
		if (!incorrectIntegerMacros.empty())
			for (auto& m : incorrectIntegerMacros)
				std::cout << "- incorrect macro\t'" << m.name() << "' from container[" << m.container().get() << "]" << std::endl;
		if (!defines.empty())
			std::cout << "MACRO DEFINES\n" << defines << std::endl;
#endif // DEBUG_FALLBACK

		if (!incorrectIntegerMacros.empty())
			return nullptr;

		const auto foundProgramIt = _signatureToProgram.find(signature);

		if (foundProgramIt != _signatureToProgram.end())
			program = foundProgramIt->second;
		else
		{
			// compile a new shader program from template with macros

#ifdef MINKO_NO_GLSL_STRUCT
			defines += "#define MINKO_NO_GLSL_STRUCT\n";
#endif // MINKO_NO_GLSL_STRUCT

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

			program							= Program::create(_programTemplate->context(), vs, fs);
			_signatureToProgram[signature]	= program;
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
				for (auto& func : _attributeFunctions)
					func(program);
				if (_indexFunction)
					_indexFunction->operator()(program);
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

void
Pass::getExplicitDefinitions(std::unordered_map<std::string, data::MacroBindingDefault>& macroNameToValue) const
{
	macroNameToValue.clear();

	for (auto& macroName : _definedBoolMacros)
		if (!isExplicitlyUndefined(macroName))
		{
			data::MacroBindingDefault macroValue;

			macroValue.semantic				= data::MacroBindingDefaultValueSemantic::PROPERTY_EXISTS;
			macroValue.value.propertyExists = true;

			macroNameToValue[macroName] = macroValue;
		}

	for (auto& macroNameAndValue : _definedIntMacros)
	{
		const auto& macroName	= macroNameAndValue.first;

		if (!isExplicitlyUndefined(macroName))
		{
			data::MacroBindingDefault	macroValue;

			macroValue.semantic		= data::MacroBindingDefaultValueSemantic::VALUE;
			macroValue.value.value	= macroNameAndValue.second;

			macroNameToValue[macroName] = macroValue; // integer definitions will overwrite boolean definitions
		}
	}
}