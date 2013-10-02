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
#include "minko/render/Program.hpp"
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
	_macroPropertyNames(),
	_drawCalls(),
	_drawCallToPass(),
	_macroPropertyNameToDrawCalls(),
	_macroAddedOrRemovedSlots(),
	_targetMacroChangedSlots(),
	_numTargetMacroListeners(),
	_rootMacroChangedSlots(),
	_numRootMacroListeners()
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


	_macroPropertyNames.clear();
	for (const auto& pass : _effect->passes())
		for (const auto& binding : pass->macroBindings())
			_macroPropertyNames.insert(binding.second);
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
	auto data		= target->data();
	auto rootData	= target->root()->data();

	auto addedOrRemovedHandler = std::bind(
		&Surface::addedOrRemovedHandler,
		shared_from_this(),
		std::placeholders::_1,
		std::placeholders::_2,
		std::placeholders::_3
	);

	// watch for the addition/deletion of properties from the data containers
	_macroAddedOrRemovedSlots.push_back(
		data->propertyAdded()->connect(std::bind(
			&Surface::macroChangedHandler,
			shared_from_this(),
			std::placeholders::_1,
			std::placeholders::_2,
			MacroChange::ADDED,
			true
		))
	);

	_macroAddedOrRemovedSlots.push_back(
		rootData->propertyAdded()->connect(std::bind(
			&Surface::macroChangedHandler,
			shared_from_this(),
			std::placeholders::_1,
			std::placeholders::_2,
			MacroChange::ADDED,
			true
			))
	);

	_macroAddedOrRemovedSlots.push_back(
		data->propertyRemoved()->connect(std::bind(
			&Surface::macroChangedHandler,
			shared_from_this(),
			std::placeholders::_1,
			std::placeholders::_2,
			MacroChange::REMOVED,
			true
		))
	);

	_macroAddedOrRemovedSlots.push_back(
		rootData->propertyRemoved()->connect(std::bind(
			&Surface::macroChangedHandler,
			shared_from_this(),
			std::placeholders::_1,
			std::placeholders::_2,
			MacroChange::REMOVED,
			true
		))
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

	for (auto pass : _effect->passes())
		_drawCalls.push_back(initializeDrawCall(pass));
}

DrawCall::Ptr
Surface::initializeDrawCall(Pass::Ptr		pass, 
							DrawCall::Ptr	drawcall)
{
#ifdef DEBUG
	if (pass == nullptr)
		throw std::invalid_argument("pass");
#endif // DEBUG

	const auto target		= targets()[0];
	const auto targetData	= target->data();
	const auto rootData		= target->root()->data();

	std::list<std::string>	bindingDefines;
	std::list<std::string>	bindingValues;

	auto program = pass->selectProgram(targetData, rootData, bindingDefines, bindingValues);

	if (drawcall == nullptr)
	{
		drawcall = DrawCall::create(
			pass->attributeBindings(),
			pass->uniformBindings(),
			pass->stateBindings(),
			pass->states()
		);

		_drawCallToPass[drawcall] = pass;

		for (const auto& binding : pass->macroBindings())
		{
			const std::string&	propertyName	= binding.second;
			Container::Ptr		data			= getDataContainer(propertyName);

			_macroPropertyNameToDrawCalls[propertyName].push_back(drawcall);

			if (data == targetData || data == rootData)
			{
				auto&		listeners		= (data == targetData ? _numTargetMacroListeners : _numRootMacroListeners);
				const int	numListeners	= listeners.count(propertyName) == 0 ? 0 : listeners[propertyName];
				if (numListeners == 0)
					macroChangedHandler(data, propertyName, MacroChange::ADDED, true);
			}
		}
	}

	drawcall->configure(program, targetData, rootData);

	return drawcall;
}

void
Surface::macroChangedHandler(Container::Ptr		data,
					 		 const std::string&	propertyName,
							 MacroChange		change,
							 bool				countListeners)
{
	if (change != MacroChange::REF_CHANGED 
		&& _macroPropertyNames.find(propertyName) == _macroPropertyNames.end())
		// the property does not belong to the set of acceptable macro names.
		return;

	if (!_drawCalls.empty())
	{
#ifdef DEBUG_SHADER_FORK
		if (_macroPropertyNameToDrawCalls.count(propertyName) == 0)
			throw std::logic_error("Failed to retrieve the drawcalls associated with macro.");
#endif // DEBUG_SHADER_FORK

		const auto&	drawCalls = _macroPropertyNameToDrawCalls[propertyName];
	
		for (auto& drawCall : drawCalls)
		{
#ifdef DEBUG_SHADER_FORK
			if (_drawCallToPass.count(drawCall) == 0)
				throw std::logic_error("Failed to update drawcall: Drawcall not associated with any effect pass.");
#endif // DEBUG_SHADER_FORK

			auto pass = _drawCallToPass[drawCall];
			initializeDrawCall(pass, drawCall);
		}
	}

	if (change == MacroChange::REF_CHANGED)
		return;

#ifdef DEBUG_SHADER_FORK
	if (data != targets()[0]->data() && data != targets()[0]->root()->data())
		throw;
#endif // DEBUG_SHADER_FORK

	auto&	slots			= (data == targets()[0]->data()	? _targetMacroChangedSlots : _rootMacroChangedSlots);
	auto&	listeners		= (data == targets()[0]->data() ? _numTargetMacroListeners : _numRootMacroListeners);
	int		numListeners	= listeners.count(propertyName) == 0 ? 0 : listeners[propertyName];

	if (change == MacroChange::ADDED)
	{
		if (numListeners == 0)
			slots[propertyName] = data->propertyReferenceChanged(propertyName)->connect(std::bind(
				&Surface::macroChangedHandler,
				shared_from_this(),
				data,
				propertyName,
				MacroChange::REF_CHANGED,
				true
			));

		listeners[propertyName] = numListeners + 1;
	}
	else if (change == MacroChange::REMOVED)
	{
		listeners[propertyName] = numListeners - 1;

		if (listeners[propertyName] == 0)
			slots.erase(propertyName);
	}

#ifdef DEBUG_SHADER_FORK
	for (auto listenerIt : _numTargetMacroListeners)
		if (listenerIt.second > 0)
			std::cout << listenerIt.second << " change listeners for target macro\t'" << listenerIt.first << "'" << std::endl;
	for (auto listenerIt : _numRootMacroListeners)
		if (listenerIt.second > 0)
			std::cout << listenerIt.second << " change listeners for root macro\t'" << listenerIt.first << "'" << std::endl;
	std::cout << std::endl;
#endif // DEBUG_SHADER_FORK
}

void
Surface::deleteDrawCalls()
{
	_drawCalls.clear();
	_drawCallToPass.clear();
	_macroPropertyNameToDrawCalls.clear();
	_targetMacroChangedSlots.clear();
	_rootMacroChangedSlots.clear();
}

void
Surface::targetRemovedHandler(AbstractComponent::Ptr	ctrl,
							  scene::Node::Ptr			target)
{
	auto data = target->data();

	_addedSlot		= nullptr;
	_removedSlot	= nullptr;
	_macroAddedOrRemovedSlots.clear();

	data->removeProvider(_material);
	data->removeProvider(_geometry->data());
    data->removeProvider(_effect->data());

	deleteDrawCalls();
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

Container::Ptr
Surface::getDataContainer(const std::string& propertyName) const
{
	if (targets().empty())
		return nullptr;
	
	auto& data	= targets().front()->data();
	if (data->hasProperty(propertyName))
		return data;

	auto& rootData = targets().front()->root()->data();
	if (rootData->hasProperty(propertyName))
		return rootData;

	return nullptr;
}