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

#include "minko/render/Pass.hpp"

#include "minko/data/Store.hpp"
#include "minko/render/Program.hpp"
#include "minko/render/Shader.hpp"
#include "minko/render/DrawCall.hpp"
#include "minko/render/States.hpp"
#include "minko/render/ProgramSignature.hpp"

using namespace minko;
using namespace minko::render;
using namespace minko::data;

Pass::Pass(const std::string&		name,
		   bool						isForward,
		   Program::Ptr				program,
		   const BindingMap&		attributeBindings,
		   const BindingMap&		uniformBindings,
		   const BindingMap&		stateBindings,
		   const MacroBindingMap&	macroBindings) :
	_name(name),
	_isForward(isForward),
	_programTemplate(program),
	_attributeBindings(attributeBindings),
	_uniformBindings(uniformBindings),
	_stateBindings(stateBindings),
	_macroBindings(macroBindings),
    _states(_stateBindings.defaultValues.providers().front()),
	_signatureToProgram(),
	_uniformFunctions(),
	_attributeFunctions(),
    _macroFunctions()
{
}

std::pair<std::shared_ptr<Program>, const ProgramSignature*>
Pass::selectProgram(const EffectVariables&  vars,
					const Store&			targetData,
					const Store&			rendererData,
					const Store&			rootData)
{
	Program::Ptr program = nullptr;
    ProgramSignature* signature = nullptr;

	if (_macroBindings.bindings.size() == 0)
		program = _programTemplate;
	else
	{
		signature = new ProgramSignature(_macroBindings, vars, targetData, rendererData, rootData);
		const auto foundProgramIt = std::find_if(
            _signatureToProgram.begin(),
            _signatureToProgram.end(),
            [&](const std::pair<const ProgramSignature*, Program::Ptr>& signatureAndProgram)
            {
                return *signatureAndProgram.first == *signature;
            }
        );

        if (foundProgramIt != _signatureToProgram.end())
        {
			program = foundProgramIt->second;
            delete signature;
            signature = foundProgramIt->first;
        }
		else
		{
			program = Program::create(_programTemplate, true);
            signature->updateProgram(*program);

			_signatureToProgram[signature] = program;
		}

	}

    return std::pair<std::shared_ptr<Program>, const ProgramSignature*>(finalizeProgram(program), signature);
}

Program::Ptr
Pass::finalizeProgram(Program::Ptr program)
{
	if (!program->vertexShader()->isReady())
		program->vertexShader()->upload();
	if (!program->fragmentShader()->isReady())
		program->fragmentShader()->upload();
	if (!program->isReady())
	{
		program->upload();

		for (auto& nameAndFunc : _uniformFunctions)
			nameAndFunc.second(program);
		for (auto& nameAndFunc : _attributeFunctions)
			nameAndFunc.second(program);
        for (auto& nameAndFunc : _macroFunctions)
			nameAndFunc.second(program);
	}

	return program;
}
