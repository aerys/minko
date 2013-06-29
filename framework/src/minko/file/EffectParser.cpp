#include "EffectParser.hpp"
#include "minko/render/GLSLProgram.hpp"
#include "minko/render/Effect.hpp"
#include "minko/file/Loader.hpp"
#include "minko/file/Options.hpp"
#include "json/json.h"

using namespace minko::file;
using namespace minko::render;

EffectParser::EffectParser() :
	_numDependencies(0),
	_numLoadedDependencies(0),
	_complete(Signal<EffectParser::ptr>::create())
{
}

void
EffectParser::parse(std::shared_ptr<Options>	options,
					const std::vector<char>&	data)
{
	Json::Value root;
	Json::Reader reader;

	_context = options->context();
	
	if (!reader.parse(&data[0], &data[data.size() - 1],	root, false))
		throw std::invalid_argument("data");
	
	for (auto pass : root.get("passes", 0))
		_programs.push_back(std::pair<std::string, std::string>(
			pass.get("vertexShader", 0).asString(),
			pass.get("fragmentShader", 0).asString()
		));

	auto bindings = root.get("bindings", 0);
	for (auto propertyName : bindings.getMemberNames())
		_bindings[bindings.get(propertyName, 0).asString()] = propertyName;

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

	_dependenciesCode += std::string(&loader->data()[0], loader->data().size()) + "\r\n";

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
	std::vector<std::shared_ptr<GLSLProgram>> programs;

	for (auto program : _programs)
		programs.push_back(GLSLProgram::create(
			_context, _dependenciesCode + program.first, _dependenciesCode + program.second
		));

	_effect = Effect::create(programs);

	for (auto binding : _bindings)
		_effect->bindInput(binding.first, binding.second);

	_complete->execute(shared_from_this());
}
