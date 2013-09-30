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

#include "Surface.hpp"

#include "minko/scene/Node.hpp"
#include "minko/geometry/Geometry.hpp"
#include "minko/render/Effect.hpp"
#include "minko/render/DrawCall.hpp"
#include "minko/render/Pass.hpp"
#include "minko/data/Container.hpp"

using namespace minko;
using namespace minko::data;
using namespace minko::component;
using namespace minko::geometry;
using namespace minko::render;

Surface::Surface(Geometry::Ptr 			geometry,
				 data::Provider::Ptr 	material,
				 Effect::Ptr			effect) :
	AbstractComponent(),
	_geometry(geometry),
	_material(material),
	_effect(effect),
	_drawCalls(),
	_drawCallToPass(),
	_macroPropertyNameToDrawCalls()
{
}

void
Surface::initialize()
{
	_targetAddedSlot = targetAdded()->connect(std::bind(
		&Surface::targetAddedHandler,
		shared_from_this(),
		std::placeholders::_1,
		std::placeholders::_2
	));

	_targetRemovedSlot = targetRemoved()->connect(std::bind(
		&Surface::targetRemovedHandler,
		shared_from_this(),
		std::placeholders::_1,
		std::placeholders::_2
	));
}

/*
void
Surface::geometry(std::shared_ptr<geometry::Geometry> newGeometry)
{
	for (unsigned int i = 0; i < numTargets(); ++i)
	{
		std::shared_ptr<scene::Node> target = getTarget(i);

		target->data()->removeProvider(_geometry->data());
		target->data()->addProvider(newGeometry->data());

		_drawCalls.clear();

		for (auto pass : _effect->passes())
			_drawCalls.push_back(pass->createDrawCall(target->data(), target->root()->data()));
	}

	_geometry = newGeometry;
}
*/

void
Surface::geometry(std::shared_ptr<geometry::Geometry> newGeometry)
{
	for (unsigned int i = 0; i < targets().size(); ++i)
	{
		std::shared_ptr<scene::Node> target = targets()[i];

		target->data()->removeProvider(_geometry->data());
		target->data()->addProvider(newGeometry->data());

		createDrawCalls();
	}

	_geometry = newGeometry;
}

void
Surface::targetAddedHandler(AbstractComponent::Ptr	ctrl,
							scene::Node::Ptr		target)

{
	auto data = target->data();
	auto addedOrRemovedHandler = std::bind(
		&Surface::addedOrRemovedHandler,
		shared_from_this(),
		std::placeholders::_1,
		std::placeholders::_2,
		std::placeholders::_3
	);

	_addedSlot = target->added()->connect(addedOrRemovedHandler);
	_removedSlot = target->removed()->connect(addedOrRemovedHandler);

	data->addProvider(_material);
	data->addProvider(_geometry->data());
    data->addProvider(_effect->data());

    createDrawCalls();
}

void
Surface::createDrawCalls()
{
	deleteDrawCalls();

	auto target = targets()[0];
	std::list<std::string> bindingDefines;
	std::list<std::string> bindingValues;

	for (auto pass : _effect->passes())
		_drawCalls.push_back(initializeDrawCall(pass));
}

DrawCall::Ptr
Surface::initializeDrawCall(Pass::Ptr		pass, 
							DrawCall::Ptr	drawcall)
{
	auto target = targets()[0];

	std::list<std::string>	bindingDefines;
	std::list<std::string>	bindingValues;

	auto program = pass->selectProgram(target->data(), target->root()->data(), bindingDefines, bindingValues);

	if (drawcall == nullptr)
	{
		drawcall = DrawCall::create(
			pass->attributeBindings(),
			pass->uniformBindings(),
			pass->stateBindings(),
			pass->states()
		);

		_drawCallToPass[drawcall] = pass;

		for (auto& propertyName : bindingDefines)
			_macroPropertyNameToDrawCalls[propertyName].push_back(drawcall);

		for (auto& propertyName	: bindingValues)
			_macroPropertyNameToDrawCalls[propertyName].push_back(drawcall);
	}

	drawcall->configure(program, target->data(), target->root()->data());

	return drawcall;
}

void
Surface::macroPropertyChangedHandler(Container::Ptr,
									 const std::string& propertyName,
									 Pass::Ptr pass)
{
	const auto& drawcalls	= _macroPropertyNameToDrawCalls[propertyName];
	for (auto drawcall : drawcalls)
		initializeDrawCall(_drawCallToPass[drawcall], drawcall);
}

void
Surface::deleteDrawCalls()
{
	_drawCalls.clear();
	_drawCallToPass.clear();
	_macroPropertyNameToDrawCalls.clear();
}

void
Surface::targetRemovedHandler(AbstractComponent::Ptr	ctrl,
							  scene::Node::Ptr			target)
{
	auto data = target->data();

	_addedSlot = nullptr;
	_removedSlot = nullptr;

	data->removeProvider(_material);
	data->removeProvider(_geometry->data());
    data->removeProvider(_effect->data());

	_drawCalls.clear();
}

void
Surface::addedOrRemovedHandler(NodePtr node, NodePtr target, NodePtr ancestor)
{
	auto nodeData = targets()[0]->data();
	auto rootData = node->root()->data();
	auto i = 0;

	createDrawCalls();
}

void
Surface::propertyAddedHandler(std::shared_ptr<data::Container>  data,
                              const std::string&                propertyName)
{

}

void
Surface::propertyRemovedHandler(std::shared_ptr<data::Container>  data,
                                const std::string&                propertyName)
{
    
}
