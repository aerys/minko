#include "Effect.hpp"
#include "minko/scene/data/DataBindings.hpp"

using namespace minko::render;
using namespace minko::scene::data;

Effect::Effect(std::vector<Effect::GLSLProgramPtr> shaders) :
	std::enable_shared_from_this<Effect>(),
	_shaders(shaders),
	_data(DataProvider::create())
{
	auto i = 0;

	for (auto shader : shaders)
		_data->setProperty("effect/pass" + std::to_string(i++), shader);
}

void
Effect::propertyChangedHandler(std::shared_ptr<DataBindings> bindings, const std::string& propertyName)
{
	std::cout << "Effect::propertyChangedHandler: " << propertyName << std::endl;
	// FIXME: fork?
}

Effect::ptr
Effect::bindInput(const std::string& bindingName, const std::string& programInputName)
{
	_inputNameToBindingName[programInputName] = bindingName;

	return shared_from_this();
}
