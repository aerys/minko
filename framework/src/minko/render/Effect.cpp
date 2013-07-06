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

#include "Effect.hpp"
#include "minko/data/Container.hpp"

using namespace minko::render;
using namespace minko::data;

Effect::Effect(std::vector<Effect::ProgramPtr>&					shaders,
			   std::unordered_map<std::string, std::string>&	attributeBindings,
			   std::unordered_map<std::string, std::string>&	uniformBindings,
			   std::unordered_map<std::string, std::string>&	stateBindings) :
	std::enable_shared_from_this<Effect>(),
	_shaders(shaders),
	_attributeBindings(attributeBindings),
	_uniformBindings(uniformBindings),
	_stateBindings(stateBindings),
	_data(Provider::create())
{
	auto i = 0;

	for (auto shader : shaders)
		_data->set<Effect::ProgramPtr>("effect/pass" + std::to_string(i++), shader);
}

void
Effect::propertyChangedHandler(std::shared_ptr<Container> bindings, const std::string& propertyName)
{
	std::cout << "Effect::propertyChangedHandler: " << propertyName << std::endl;
	// FIXME: fork?
}
