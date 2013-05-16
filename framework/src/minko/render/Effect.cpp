#include "Effect.hpp"
#include "minko/scene/data/DataBindings.hpp"

using namespace minko::render;

Effect::Effect(Effect::DataBindingsPtr bindings, std::vector<Effect::GLSLProgramPtr> shaders) :
	std::enable_shared_from_this<Effect>(),
	_bindings(bindings),
	_shaders(shaders)
{
}

void
Effect::propertyChangedHandler(Effect::DataBindingsPtr bindings, const std::string& propertyName)
{
	std::cout << "Effect::propertyChangedHandler: " << propertyName << std::endl;
}

Effect::ptr
Effect::bind(const std::string& bindingName, const std::string& propertyName)
{
	_bindings->propertyChanged(bindingName)->add(std::bind(
		&Effect::propertyChangedHandler,
		shared_from_this(),
		std::placeholders::_1,
		std::placeholders::_2
	));

	return shared_from_this();
}
