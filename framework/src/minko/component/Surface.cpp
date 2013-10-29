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
				 Effect::Ptr			effect,
				 const std::string&		technique) :
	AbstractComponent(),
	_geometry(geometry),
	_material(material),
	_effect(effect),
	_technique(technique),
	_macroPropertyNames(),
	_drawCalls(),
	_drawCallToPass(),
	_macroPropertyNameToDrawCalls(),
	_macroAddedOrRemovedSlots(),
	_macroChangedSlots(),
	_numMacroListeners(),
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

	auto techniques = _effect->techniques();

	if (techniques.count(_technique) == 0)
		throw std::logic_error("The technique '" + _technique + "' does not exist.");

	_macroPropertyNames.clear();
	for (const auto& pass : techniques[_technique])
		for (const auto& binding : pass->macroBindings())
			_macroPropertyNames.insert(std::get<0>(binding.second));
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
	targetData->addProvider(_effect->data());
}

void
Surface::removedHandler(NodePtr node, NodePtr target, NodePtr ancestor)
{
	deleteAllDrawCalls();
}

void
Surface::watchMacroAdditionOrDeletion(std::shared_ptr<data::Container> rendererData)
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
		rendererData->propertyAdded()->connect(std::bind(
			&Surface::macroChangedHandler,
			shared_from_this(),
			std::placeholders::_1,
			std::placeholders::_2,
			MacroChange::ADDED
		))
	);

	_macroAddedOrRemovedSlots.push_back(
		rendererData->propertyRemoved()->connect(std::bind(
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
Surface::deleteAllDrawCalls()
{
	auto drawCallsMap = _drawCalls;

	for (auto& drawCalls : drawCallsMap)
		deleteDrawCalls(drawCalls.first);

	_macroPropertyNameToDrawCalls.clear();
	_macroChangedSlots.clear();
}

void
Surface::deleteDrawCalls(std::shared_ptr<data::Container> rendererData)
{
	auto& drawCalls = _drawCalls[rendererData];

	while (!drawCalls.empty())
	{
		auto drawCall = drawCalls.front();

		_drawCallRemoved->execute(shared_from_this(), drawCall);
		drawCalls.pop_front();

		_drawCallToPass.erase(drawCall);
		_drawCallToRendererData.erase(drawCall);

		auto macroPropertyNameToDrawCalls = _macroPropertyNameToDrawCalls;
		for (auto propertyNameAndDrawCall : macroPropertyNameToDrawCalls)
		{
			auto& dcs = propertyNameAndDrawCall.second;
			auto it = std::find(dcs.begin(), dcs.end(), drawCall);
			
			dcs.erase(it);

			if (dcs.size() == 0)
			{
				auto& propertyName = propertyNameAndDrawCall.first;

				_macroPropertyNameToDrawCalls.erase(propertyName);
				_macroChangedSlots.erase(propertyName);
			}
		}
	}

	_drawCalls.erase(rendererData);
}

Surface::DrawCallList
Surface::createDrawCalls(std::shared_ptr<data::Container> rendererData)
{
	if (_drawCalls.count(rendererData) != 0)
		deleteDrawCalls(rendererData);

	const auto& passes = _effect->technique(_technique);
	auto& drawCalls = _drawCalls[rendererData];

	for (const auto& pass : passes)
	{
		auto drawCall  = initializeDrawCall(pass, rendererData);

		if (!drawCall)
		{
			switchToFallbackTechnique();

			return createDrawCalls(rendererData);
		}

		drawCalls.push_back(drawCall);
		_drawCallAdded->execute(shared_from_this(), drawCall);
	}

	watchMacroAdditionOrDeletion(rendererData);

	return drawCalls;
}

DrawCall::Ptr
Surface::initializeDrawCall(Pass::Ptr		pass, 
							Container::Ptr	rendererData,
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

	auto program = getWorkingProgram(pass, targetData, rendererData, rootData, bindingDefines, bindingValues);

	if (!program)
		return nullptr;

	if (drawcall == nullptr)
	{
		drawcall = DrawCall::create(
			pass->attributeBindings(),
			pass->uniformBindings(),
			pass->stateBindings(),
			pass->states()
		);

		_drawCallToPass[drawcall] = pass;
		_drawCallToRendererData[drawcall] = rendererData;

		for (const auto& binding : pass->macroBindings())
		{
			const std::string&	propertyName	= std::get<0>(binding.second);
			Container::Ptr		data			= getDataContainer(propertyName, rendererData);

			_macroPropertyNameToDrawCalls[propertyName].push_back(drawcall);

			if (data == targetData || data == rootData)
			{
				auto&		listeners		= _numMacroListeners;
				const int	numListeners	= listeners.count(propertyName) == 0 ? 0 : listeners[propertyName];

				if (numListeners == 0)
					macroChangedHandler(data, propertyName, MacroChange::ADDED);
			}
		}
	}

	drawcall->configure(program, targetData, rendererData, rootData);

	return drawcall;
}

std::shared_ptr<Program>
Surface::getWorkingProgram(std::shared_ptr<Pass>	pass,
						   data::Container::Ptr		targetData,
						   data::Container::Ptr		rendererData,
						   data::Container::Ptr		rootData,
						   std::list<std::string>&	bindingDefines,
						   std::list<std::string>&	bindingValues)
{
	auto program = pass->selectProgram(targetData, rendererData, rootData, bindingDefines, bindingValues);

	while (!program)
	{
		auto passes = _effect->technique(_technique);
		auto fallbackIt = std::find_if(
			passes.begin(),
			passes.end(),
			[&](const Pass::Ptr& p)
			{
				return p->name() == pass->fallback();
			}
		);

		if (fallbackIt == passes.end())
			throw;

		pass = *fallbackIt;
		program = pass->selectProgram(targetData, rootData, rendererData, bindingDefines, bindingValues);
	}

	return program;
}

void
Surface::macroChangedHandler(Container::Ptr		data,
							 const std::string&	propertyName,
							 MacroChange		change)
{
	if (change == MacroChange::REF_CHANGED && !_drawCalls.empty())
	{
		const auto&	drawCalls = _macroPropertyNameToDrawCalls[propertyName];

		for (auto& drawCall : drawCalls)
		{
			auto pass = _drawCallToPass[drawCall];
			auto rendererData = _drawCallToRendererData[drawCall];

			if (!initializeDrawCall(pass, rendererData, drawCall))
			{
				switchToFallbackTechnique();

				deleteDrawCalls(rendererData);
				createDrawCalls(rendererData);
			}
		}
	}
	else if (_macroPropertyNames.find(propertyName) != _macroPropertyNames.end())
	{
		int		numListeners = _numMacroListeners.count(propertyName) == 0 ? 0 : _numMacroListeners[propertyName];

		if (change == MacroChange::ADDED)
		{
			if (numListeners == 0)
				_macroChangedSlots[propertyName] = data->propertyReferenceChanged(propertyName)->connect(std::bind(
					&Surface::macroChangedHandler,
					shared_from_this(),
					data,
					propertyName,
					MacroChange::REF_CHANGED
				));

			_numMacroListeners[propertyName] = numListeners + 1;
		}
		else if (change == MacroChange::REMOVED)
		{
			macroChangedHandler(data, propertyName, MacroChange::REF_CHANGED);

			_numMacroListeners[propertyName] = numListeners - 1;

			if (_numMacroListeners[propertyName] == 0)
				_macroChangedSlots.erase(propertyName);
		}
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
	data->removeProvider(_geometry->data());

	deleteAllDrawCalls();
}

Container::Ptr
Surface::getDataContainer(const std::string& propertyName, std::shared_ptr<data::Container> rendererData) const
{
	if (targets().empty())
		return nullptr;
	
	auto data = targets().front()->data();
	if (data->hasProperty(propertyName))
		return data;

	if (rendererData->hasProperty(propertyName))
		return rendererData;

	auto rootData = targets().front()->root()->data();
	if (rootData->hasProperty(propertyName))
		return rootData;

	return nullptr;
}

void
Surface::switchToFallbackTechnique()
{
#ifdef DEBUG_SHADER_FORK
	std::cout << "fallback for technique '" << _technique << "': ";
#endif
	if (_effect->hasFallback(_technique))
		_technique = _effect->fallback(_technique);

#ifdef DEBUG_SHADER_FORK
	std::cout << "'" << _technique << "'" << std::endl;
#endif	
}
