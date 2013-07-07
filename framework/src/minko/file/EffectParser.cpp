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

#include "EffectParser.hpp"
#include "minko/resource/Program.hpp"
#include "minko/render/Effect.hpp"
#include "minko/file/Loader.hpp"
#include "minko/file/Options.hpp"
#include "json/json.h"

using namespace minko::file;
using namespace minko::render;
using namespace minko::resource;

EffectParser::EffectParser() :
	_numDependencies(0),
	_numLoadedDependencies(0)
{
}

void
EffectParser::parse(const std::string&					filename,
					std::shared_ptr<Options>			options,
					const std::vector<unsigned char>&	data,
					std::shared_ptr<AssetsLibrary>		assetsLibrary)
{
	Json::Value root;
	Json::Reader reader;

	_assetsLibrary	= assetsLibrary;
	_context		= options->context();
	
	if (!reader.parse((const char*)&data[0], (const char*)&data[data.size() - 1],	root, false))
		throw std::invalid_argument("data");

	_effectName = root.get("name", filename).asString();
	
	for (auto pass : root.get("passes", 0))
		_programs.push_back(std::pair<std::string, std::string>(
			pass.get("vertexShader", 0).asString(),
			pass.get("fragmentShader", 0).asString()
		));

	auto attributeBindings = root.get("attributeBindings", 0);
	if (attributeBindings.isObject())
		for (auto propertyName : attributeBindings.getMemberNames())
			_attributeBindings[propertyName] = attributeBindings.get(propertyName, 0).asString();

	auto uniformBindings = root.get("uniformBindings", 0);
	if (uniformBindings.isObject())
		for (auto propertyName : uniformBindings.getMemberNames())
			_uniformBindings[propertyName] = uniformBindings.get(propertyName, 0).asString();

	auto stateBindings = root.get("stateBindings", 0);
	if (stateBindings.isObject())
		for (auto propertyName : stateBindings.getMemberNames())
			_stateBindings[propertyName] = stateBindings.get(propertyName, 0).asString();

	auto require = root.get("includes", 0);
	if (require.isArray())
	{
		_numDependencies = require.size();

		for (unsigned int requireId = 0; requireId < _numDependencies; requireId++)
		{
			auto loader = Loader::create();

			_loaderCompleteSlots[loader] = loader->complete()->connect(std::bind(
				&EffectParser::dependencyCompleteHandler, shared_from_this(), std::placeholders::_1
			));
			_loaderErrorSlots[loader] = loader->error()->connect(std::bind(
				&EffectParser::dependencyErrorHandler, shared_from_this(), std::placeholders::_1
			));

			loader->load(require[requireId].asString(), options);
		}
	}
	
	if (_numDependencies == 0)
		finalize();
}

void
EffectParser::dependencyCompleteHandler(std::shared_ptr<Loader> loader)
{
	++_numLoadedDependencies;

	_dependenciesCode += std::string((char*)&loader->data()[0], loader->data().size()) + "\r\n";

	if (_numDependencies == _numLoadedDependencies)
		finalize();
}

void
EffectParser::dependencyErrorHandler(std::shared_ptr<Loader> loader)
{
	std::cout << "error" << std::endl;
}

void
EffectParser::finalize()
{
	std::vector<std::shared_ptr<Program>> programs;

	for (auto& program : _programs)
    {
        auto p = Program::create(
			_context, _dependenciesCode + program.first, _dependenciesCode + program.second
		);

        p->upload();
		programs.push_back(p);
    }

	_effect = Effect::create(programs, _attributeBindings, _uniformBindings, _stateBindings);

	_assetsLibrary->effect(_effectName, _effect);
	_complete->execute(shared_from_this());
}
