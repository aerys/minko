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
#include "minko/render/Program.hpp"
#include "minko/render/Shader.hpp"
#include "minko/render/DrawCall.hpp"
#include "minko/render/States.hpp"
#include "minko/render/ProgramSignature.hpp"

using namespace minko;
using namespace minko::render;
using namespace minko::data;

Pass::Pass(const std::string&		name,
		   Program::Ptr				program,
		   const BindingMap&		attributeBindings,
		   const BindingMap&		uniformBindings,
		   const BindingMap&		stateBindings,
		   const MacroBindingMap&	macroBindings,
           std::shared_ptr<States>  states,
		   const std::string&		fallback) :
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
    _macroFunctions()
{
}

std::pair<std::shared_ptr<Program>, const ProgramSignature*>
Pass::selectProgram(const std::unordered_map<std::string, std::string>& vars,
					const Container&			                        targetData,
					const Container&			                        rendererData,
					const Container&			                        rootData)
{
	Program::Ptr program;

	if (_macroBindings.size() == 0)
		program = _programTemplate;
	else
	{
		ProgramSignature* signature = new ProgramSignature(_macroBindings, vars, targetData, rendererData, rootData);
		const auto foundProgramIt = std::find_if(
            _signatureToProgram.begin(),
            _signatureToProgram.end(),
            [&](const std::pair<const ProgramSignature*, Program::Ptr>& signatureAndProgram)
            {
                return *std::get<0>(signatureAndProgram) == *signature;
            }
        );

        if (foundProgramIt != _signatureToProgram.end())
        {
			program = foundProgramIt->second;
            delete signature;
        }
		else
		{
			program = Program::create(_programTemplate, true);
            signature->updateProgram(*program);

			_signatureToProgram[signature] = program;
		}

        return std::pair<std::shared_ptr<Program>, const ProgramSignature*>(finalizeProgram(program), signature);
	}

    return std::pair<std::shared_ptr<Program>, const ProgramSignature*>(finalizeProgram(program), nullptr);
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
                for (auto& func : _macroFunctions)
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
