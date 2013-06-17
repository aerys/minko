#include "EffectParser.hpp"
#include "minko/render/GLSLProgram.hpp"
#include "minko/render/Effect.hpp"
#include "json/json.h"

using namespace minko::file;
using namespace minko::render;

void
EffectParser::parse(std::shared_ptr<AbstractContext> context, const std::string& data)
{
	Json::Value root;
	Json::Reader reader;

	if (!reader.parse(data,	root, false))
		throw std::invalid_argument("data");

	std::vector<std::shared_ptr<GLSLProgram>> programs;

	for (auto pass : root.get("passes", 0))
	{
		auto vertexShader = pass.get("vertexShader", 0).asString();
		auto fragmentShader = pass.get("fragmentShader", 0).asString();

		programs.push_back(GLSLProgram::create(context, vertexShader, fragmentShader));
	}

	_effect = Effect::create(programs);

	auto bindings = root.get("bindings", 0);
	for (auto propertyName : bindings.getMemberNames())
	{
		std::cout << bindings.get(propertyName, 0).asString() << " => " << propertyName << std::endl;
		_effect->bindInput(bindings.get(propertyName, 0).asString(), propertyName);
	}
}
