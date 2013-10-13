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
	_numRootMacroListeners(),
	_drawCallAdded(DrawCallChangedSignal::create()),
	_drawCallRemoved(DrawCallChangedSignal::create())
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

	_techniqueChangedSlot = _effect->techniqueChanged()->connect(std::bind(
		&Surface::techniqueChangedHandler,
		shared_from_this(),
		std::placeholders::_1,
		std::placeholders::_2,
		std::placeholders::_3
	));

	_macroPropertyNames.clear();
	for (const auto& pass : _effect->passes())
		for (const auto& binding : pass->macroBindings())
			_macroPropertyNames.insert(binding.second);
}

void
Surface::geometry(std::shared_ptr<geometry::Geometry> newGeometry)
{
	for (unsigned int i = 0; i < targets().size(); ++i)
	{
		std::shared_ptr<scene::Node> target = targets()[i];

		target->data()->removeProvider(_geometry->data());
		target->data()->addProvider(newGeometry->data());

		//createDrawCalls();
	}

	_geometry = newGeometry;
}

void
Surface::targetAddedHandler(AbstractComponent::Ptr	ctrl,
							scene::Node::Ptr		target)

{
	auto targetData	= target->data();

	_removedSlot = target->removed()->connect(std::bind(
		&Surface::removedHandler,
		shared_from_this(),
		std::placeholders::_1,
		std::placeholders::_2,
		std::placeholders::_3
	));

	targetData->addProvider(_material);
	targetData->addProvider(_geometry->data());
}

void
Surface::removedHandler(NodePtr node, NodePtr target, NodePtr ancestor)
{
	deleteDrawCalls();
	_macroAddedOrRemovedSlots.clear();
}

void
Surface::watchMacroAdditionOrDeletion()
{
	_macroAddedOrRemovedSlots.clear();

	if (targets().empty())
		return;

	auto&	target		= targets().front();
	auto	targetData	= target->data();
	auto	rootData	= target->root()->data();

#ifdef DEBUG_SHADER_FORK
	std::cout << "\n----- ADD/REM MACRO HANDLERS-- targetData[" << targetData.get() << "] -- rootData[" << rootData.get() << "] -----" << std::endl; 
#endif // DEBUG_SHADER_FORK

	_macroAddedOrRemovedSlots.push_back(
		targetData->propertyAdded()->connect(std::bind(
			&Surface::macroChangedHandler,
			shared_from_this(),
			std::placeholders::_1,
			std::placeholders::_2,
			MacroChange::ADDED
		))
	);

	_macroAddedOrRemovedSlots.push_back(
		targetData->propertyRemoved()->connect(std::bind(
			&Surface::macroChangedHandler,
			shared_from_this(),
			std::placeholders::_1,
			std::placeholders::_2,
			MacroChange::REMOVED
		))
	);

	_macroAddedOrRemovedSlots.push_back(
		rootData->propertyAdded()->connect(std::bind(
			&Surface::macroChangedHandler,
			shared_from_this(),
			std::placeholders::_1,
			std::placeholders::_2,
			MacroChange::ADDED
		))
	);

	_macroAddedOrRemovedSlots.push_back(
		rootData->propertyRemoved()->connect(std::bind(
			&Surface::macroChangedHandler,
			shared_from_this(),
			std::placeholders::_1,
			std::placeholders::_2,
			MacroChange::REMOVED
		))
	);
}

void
Surface::deleteDrawCalls()
{
	while (!_drawCalls.empty())
	{
		auto drawCall = _drawCalls.front();

		_drawCallRemoved->execute(shared_from_this(), drawCall);
		_drawCalls.pop_front();
	}
	_drawCallToPass.clear();

	_macroPropertyNameToDrawCalls.clear();
	_targetMacroChangedSlots.clear();
	_rootMacroChangedSlots.clear();
}

Surface::DrawCallList
Surface::createDrawCalls(std::shared_ptr<data::Container> rendererData)
{
	deleteDrawCalls();

	for (auto pass : _effect->passes())
	{
		auto drawCall  = initializeDrawCall(pass);

		_drawCalls.push_back(drawCall);
		_drawCallAdded->execute(shared_from_this(), drawCall);
	}

	watchMacroAdditionOrDeletion();

	return _drawCalls;
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
					macroChangedHandler(data, propertyName, MacroChange::ADDED);
			}
		}
	}

	drawcall->configure(program, targetData, rootData);

	return drawcall;
}

void
Surface::macroChangedHandler(Container::Ptr		data,
					 		 const std::string&	propertyName,
							 MacroChange		change)
{
#ifdef DEBUG_SHADER_FORK
	std::cout << "Surface::macroChangedHandler('" << propertyName << "')" << std::endl;
#endif
	if (change == MacroChange::REF_CHANGED && !_drawCalls.empty())
	{
#ifdef DEBUG_SHADER_FORK
		std::cout << "# Macro '" << propertyName << "' CHANGED -> reinit drawcalls" << std::endl;

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
	else if (_macroPropertyNames.find(propertyName) != _macroPropertyNames.end())
	{
#ifdef DEBUG_SHADER_FORK
		std::cout << "# Macro '" << propertyName << "' " << (change == MacroChange::ADDED ? "ADDED" : "REMOVED") << " -> update changed listeners" << std::endl;

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
					MacroChange::REF_CHANGED
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
}

void
Surface::targetRemovedHandler(AbstractComponent::Ptr	ctrl,
							  scene::Node::Ptr			target)
{
	auto data = target->data();

	_removedSlot	= nullptr;
	_macroAddedOrRemovedSlots.clear();

	data->removeProvider(_material);
	data->removeProvider(_geometry->data());

	deleteDrawCalls();
}

Container::Ptr
Surface::getDataContainer(const std::string& propertyName) const
{
	if (targets().empty())
		return nullptr;
	
	auto data	= targets().front()->data();
	if (data->hasProperty(propertyName))
		return data;

	auto rootData = targets().front()->root()->data();
	if (rootData->hasProperty(propertyName))
		return rootData;

	return nullptr;
}

void
Surface::techniqueChangedHandler(Effect::Ptr effect, const std::string& oldTechnique, const std::string& newTechnique)
{
	std::cout << "technique changed" << std::endl;
}
