/*,
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

#include "minko/render/DrawCallPool.hpp"
#include "minko/render/DrawCall.hpp"
#include "minko/component/Renderer.hpp"
#include "minko/component/Surface.hpp"
#include "minko/render/Effect.hpp"
#include "minko/render/Program.hpp"
#include "minko/data/Container.hpp"
#include "minko/scene/Node.hpp"
#include "minko/render/Blending.hpp"
#include "minko/geometry/Geometry.hpp"
#include "minko/data/ArrayProvider.hpp"

using namespace minko;
using namespace minko::math;
using namespace minko::render;
using namespace minko::component;
using namespace minko::data;

const unsigned int DrawCallPool::NUM_FALLBACK_ATTEMPTS = 32;

std::unordered_map<std::string, std::pair<std::string, int>> DrawCallPool::_variablePropertyNameToPosition;


DrawCallPool::DrawCallPool(Renderer::Ptr renderer):
	_renderer(renderer),
	_mustZSort(true)
{
}

const std::list<DrawCall::Ptr>&
DrawCallPool::drawCalls()
{
	const bool doZSort = _mustZSort || !_toCollect.empty();
	//if (_toCollect.empty() && _toRemove.empty())
		//return _drawCalls;

	for (auto& surface : _toRemove)
	{
		deleteDrawCalls(surface);
		cleanSurface(surface);		
	}
	_toRemove.clear();

	for (auto& surface : _toCollect)
	{
		auto& newDrawCalls = generateDrawCall(surface, NUM_FALLBACK_ATTEMPTS);
		_drawCalls.insert(_drawCalls.end(), newDrawCalls.begin(), newDrawCalls.end());
	}
	_toCollect.clear();

	if (doZSort)
		_drawCalls.sort(&DrawCallPool::compareDrawCalls);
	_mustZSort = false;

	return _drawCalls;
}


bool
DrawCallPool::compareDrawCalls(DrawCall::Ptr a, 
							   DrawCall::Ptr b)
{
	const float	aPriority			= a->priority();
	const float	bPriority			= b->priority();
	const bool	arePrioritiesEqual	= fabsf(aPriority - bPriority) < 1e-3f;

	if (!arePrioritiesEqual)
		return aPriority > bPriority;

	if (a->zsorted() || b->zsorted())
	{
		static Vector3::Ptr aPosition = Vector3::create();
		static Vector3::Ptr bPosition = Vector3::create();

		a->getEyeSpacePosition(aPosition);
		b->getEyeSpacePosition(bPosition);

		return aPosition->z() > bPosition->z();
	}
	else
	{
		// ordered by target texture id, if any
		return a->target() && (!b->target() || (a->target()->id() > b->target()->id()));
	}
}

void
DrawCallPool::addSurface(Surface::Ptr surface)
{
	_surfaceToTechniqueChangedSlot[surface] = surface->techniqueChanged()->connect(std::bind(
		&DrawCallPool::techniqueChanged,
		shared_from_this(),
		std::placeholders::_1,
		std::placeholders::_2,
		std::placeholders::_3));

	_techniqueToMacroNames[surface].clear();

	_surfaceToVisibilityChangedSlot.insert(std::pair<SurfacePtr, VisibilityChangedSlot>(surface, surface->visibilityChanged()->connect(std::bind(
		&DrawCallPool::visibilityChanged,
		shared_from_this(),
		std::placeholders::_1,
		std::placeholders::_2))));

	_surfaceToVisibilityChangedSlot.insert(std::pair<SurfacePtr, VisibilityChangedSlot>(surface, surface->computedVisibilityChanged()->connect(std::bind(
		&DrawCallPool::visibilityChanged,
		shared_from_this(),
		std::placeholders::_1,
		std::placeholders::_2))));

	_surfaceToIndexChangedSlot.insert(std::pair<SurfacePtr, ArrayProviderIndexChangedSlot>(surface, surface->geometry()->data()->indexChanged()->connect(std::bind(
		&DrawCallPool::dataProviderIndexChanged,
		shared_from_this(),
		std::placeholders::_1,
		std::placeholders::_2,
		surface
		))));

	auto arrayProviderMaterial = std::dynamic_pointer_cast<data::ArrayProvider>(surface->material());

	if (arrayProviderMaterial)
		_surfaceToIndexChangedSlot.insert(std::pair<SurfacePtr, ArrayProviderIndexChangedSlot>(surface, arrayProviderMaterial->indexChanged()->connect(std::bind(
		&DrawCallPool::dataProviderIndexChanged,
		shared_from_this(),
		std::placeholders::_1,
		std::placeholders::_2,
		surface
		))));

	//if (std::find(_toCollect.begin(), _toCollect.end(), surface) == _toCollect.end())
	_toCollect.insert(surface);
}

void
DrawCallPool::dataProviderIndexChanged(std::shared_ptr<data::ArrayProvider> provider, uint index, SurfacePtr surface)
{
	std::cout << "indexChanged" << std::endl;

	//if (std::find(_toCollect.begin(), _toCollect.end(), surface) == _toCollect.end())
	//{
	//	std::cout << "   update surface drawcalls" << std::endl;
		deleteDrawCalls(surface);
		_toCollect.insert(surface);
	//}
}

void
DrawCallPool::removeSurface(Surface::Ptr surface)
{
	std::cout << "removeSurface" << std::endl;
	auto surfaceToCollectIt = std::find(_toCollect.begin(), _toCollect.end(), surface);

	if (surfaceToCollectIt == _toCollect.end())
		_toRemove.insert(surface);
	else
		_toCollect.erase(surfaceToCollectIt);
}

void
DrawCallPool::cleanSurface(Surface::Ptr	surface)
{
	_surfaceToTechniqueChangedSlot.erase(surface);
	_surfaceToDrawCalls[surface].clear();
	_macroAddedOrRemovedSlots[surface].clear();
	_macroChangedSlots[surface].clear();
	_numMacroListeners[surface].clear();
	_drawcallToZSortNeededSlots.erase(surface);
	_surfaceToIndexChangedSlot.erase(surface);
}

void
DrawCallPool::techniqueChanged(Surface::Ptr			surface, 
							   const std::string&	technique, 
							   bool					updateDrawCall)
{
	deleteDrawCalls(surface);
	if (updateDrawCall && std::find(_toCollect.begin(), _toCollect.end(), surface) == _toCollect.end())
		_toCollect.insert(surface);
}

void
DrawCallPool::visibilityChanged(Surface::Ptr	surface, 
								bool			value)
{
	bool visible = surface->visible() && surface->computedVisibility();

	if (visible && _invisibleSurfaces.find(surface) != _invisibleSurfaces.end()) // visible and already wasn't visible before
	{
		_toCollect.insert(surface);
		_invisibleSurfaces.erase(surface);
	}
	else if (!visible && _invisibleSurfaces.find(surface) == _invisibleSurfaces.end()) // not visible but was visible before
	{
		_toRemove.insert(surface);
		_invisibleSurfaces.insert(surface);
	}
	
}

DrawCallPool::DrawCallList&
DrawCallPool::generateDrawCall(Surface::Ptr	surface,
							   unsigned int	numAttempts)
{
	if (_surfaceToDrawCalls.find(surface) != _surfaceToDrawCalls.end())
		deleteDrawCalls(surface);

	bool							mustFallbackTechnique	= false;
	std::shared_ptr<render::Effect> drawCallEffect			= surface->effect();
	std::string						technique				= surface->technique();

	if (_renderer->effect())
	{
		drawCallEffect	= _renderer->effect();
		technique		= _renderer->effect()->techniques().begin()->first;
	}

	_surfaceToDrawCalls	[surface] = std::list<DrawCall::Ptr>();

	for (const auto& pass : drawCallEffect->technique(technique))
	{
		auto drawCall = initializeDrawCall(pass, surface);

		if (drawCall)
		{
			_surfaceToDrawCalls[surface].push_back(drawCall);

			_drawcallToZSortNeededSlots[surface][drawCall] = drawCall->zsortNeeded()->connect(std::bind(	
				&DrawCallPool::zsortNeededHandler,
				shared_from_this(),
				surface,
				drawCall
			));
			
			//_drawCallAdded->execute(shared_from_this(), surface, drawCall);
		}
		else
		{
			// one pass failed without any viable fallback, fallback the whole technique then.
			mustFallbackTechnique = true;
			break;
		}
	}

	if (mustFallbackTechnique)
	{
		_surfaceToDrawCalls[surface].clear();

		if (numAttempts > 0 && drawCallEffect->hasFallback(technique))
		{
			surface->setTechnique(drawCallEffect->fallback(technique), false);

			return generateDrawCall(surface, numAttempts - 1);
		}
	}
	else
		watchMacroAdditionOrDeletion(surface);

	return _surfaceToDrawCalls[surface];
}

DrawCall::Ptr
DrawCallPool::initializeDrawCall(Pass::Ptr		pass, 
								 Surface::Ptr	surface,
								 DrawCall::Ptr	drawcall)
{
#ifdef DEBUG
	if (pass == nullptr)
		throw std::invalid_argument("pass");
#endif // DEBUG

	const auto rendererData = _renderer->targets()[0]->data();
	const float	priority	= drawcall ? drawcall->priority() : pass->states()->priority();
	const auto	target		= surface->targets()[0];
	const auto	targetData	= target->data();
	const auto	rootData	= target->root()->data();

	std::list<ContainerProperty>	booleanMacros;
	std::list<ContainerProperty>	integerMacros;
	std::list<ContainerProperty>	incorrectIntegerMacros;
	std::unordered_map<std::string, std::string> drawCallVariables;

	if (drawcall)
		drawCallVariables = drawcall->variablesToValue();
	else
	{
		drawCallVariables["geometryId"] = std::to_string(surface->_geometryId);
		drawCallVariables["materialId"] = std::to_string(surface->_materialId);
	}

	std::unordered_map<std::string, MacroBinding> macroBindings = pass->macroBindings(
		std::bind(
			&DrawCallPool::replaceVariable,
			shared_from_this(),
			std::placeholders::_1,
			std::placeholders::_2),
		drawCallVariables);

	auto program = getWorkingProgram(
		surface,
		pass,
		macroBindings,
		targetData,
		rendererData,
		rootData,
		booleanMacros,
		integerMacros,
		incorrectIntegerMacros);

	if (!program)
		return nullptr;
	
	if (drawcall == nullptr)
	{
		drawcall = DrawCall::create(
			pass->attributeBindings(),
			pass->uniformBindings(),
			pass->stateBindings(),
			pass->states(),
			std::bind(
				&DrawCallPool::replaceVariable,
				shared_from_this(),
				std::placeholders::_1,
				std::placeholders::_2),
			drawCallVariables
		);

		_drawCallToPass[surface][drawcall]			= pass;
		_drawCallToRendererData[surface][drawcall]	= rendererData;

		Effect::Ptr effect = _renderer->effect() ? _renderer->effect() : surface->effect();

		for (auto& technique : effect->techniques())
		{
			auto& techniqueName = technique.first;

			for (auto& pass : technique.second)
				for (auto& macroBinding : pass->macroBindings(std::bind(
					&DrawCallPool::replaceVariable,
						shared_from_this(),
						std::placeholders::_1,
						std::placeholders::_2),
					drawCallVariables))
				_techniqueToMacroNames[surface][techniqueName].insert(std::get<0>(macroBinding.second));
		}

		for (const auto& binding : macroBindings)
		{
			data::ContainerProperty macro(binding.second, targetData, rendererData, rootData);

			_macroNameToDrawCalls[surface][macro.name()].push_back(drawcall);

			if (macro.container())
			{
				auto&		listeners		= _numMacroListeners;
				const int	numListeners	= listeners[surface].count(macro) == 0 ? 0 : listeners[surface][macro];

				if (numListeners == 0)
					macroChangedHandler(macro.container(), macro.name(), surface, MacroChange::ADDED);
			}
		}
	}

	drawcall->configure(program, targetData, rendererData, rootData);

	return drawcall;
}

std::shared_ptr<Program>
DrawCallPool::getWorkingProgram(Surface::Ptr					surface,
								Pass::Ptr						pass,
								const MacroBindingsMap&			macroBindings,
								Container::Ptr					targetData,
								Container::Ptr					rendererData,
								Container::Ptr					rootData,
								std::list<ContainerProperty>&	booleanMacros,
								std::list<ContainerProperty>&	integerMacros,
								std::list<ContainerProperty>&	incorrectIntegerMacros)
{
	Program::Ptr program = nullptr;

	do
	{
		program = pass->selectProgram(
			macroBindings,
			targetData, 
			rendererData, 
			rootData, 
			booleanMacros, 
			integerMacros, 
			incorrectIntegerMacros
		);

#ifdef DEBUG_FALLBACK
	assert(incorrectIntegerMacros.empty() != (program==nullptr));
#endif // DEBUG_FALLBACK

		forgiveMacros	(surface, booleanMacros, integerMacros,	TechniquePass(surface->technique(), pass));
		blameMacros		(surface, incorrectIntegerMacros,		TechniquePass(surface->technique(), pass));

		break;

		/*
		if (program)
			break;
		else
		{
#ifdef DEBUG_FALLBACK
			std::cout << "fallback:\tpass '" << pass->name() << "'\t-> pass '" << pass->fallback() << "'" << std::endl;
#endif // DEBUG_FALLBACK

			const std::vector<Pass::Ptr>& passes = _effect->technique(_technique);
			auto fallbackIt = std::find_if(passes.begin(), passes.end(), [&](const Pass::Ptr& p)
			{
				return p->name() == pass->fallback();
			});

			if (fallbackIt == passes.end())
				break;
			else
				pass = *fallbackIt;
		}
		*/
	}
	while(true);

	return program;
}

void
DrawCallPool::deleteDrawCalls(Surface::Ptr surface)
{
	auto& drawCalls = _surfaceToDrawCalls[surface];

	while (!drawCalls.empty())
	{
		auto drawCall = drawCalls.front();

		//_drawCallRemoved->execute(shared_from_this(), surface, drawCall);
		drawCalls.pop_front();

		_drawCallToPass[surface].erase(drawCall);
		_drawCallToRendererData[surface].erase(drawCall);
		_drawcallToZSortNeededSlots[surface].erase(drawCall);

		for (auto& drawcallsIt : _macroNameToDrawCalls[surface])
		{
			auto& macroName			= drawcallsIt.first;
			auto& macroDrawcalls	= drawcallsIt.second;
			auto  drawcallIt		= std::find(macroDrawcalls.begin(), macroDrawcalls.end(), drawCall);
			
			if (drawcallIt != macroDrawcalls.end())
				macroDrawcalls.erase(drawcallIt);
		}

		_drawCalls.remove(drawCall);
	}

	// erase in a subsequent step the entries corresponding to macro names which do not monitor any drawcall anymore.
	for (auto drawcallsIt = _macroNameToDrawCalls[surface].begin();
		drawcallsIt != _macroNameToDrawCalls[surface].end();
		)
		if (drawcallsIt->second.empty())
			drawcallsIt = _macroNameToDrawCalls[surface].erase(drawcallsIt);
		else
			++drawcallsIt;
}

void
DrawCallPool::macroChangedHandler(Container::Ptr		container, 
								  const std::string&	propertyName, 
								  Surface::Ptr			surface, 
								  MacroChange			change)
{
	const ContainerProperty	macro(propertyName, container);
	Effect::Ptr				effect = (_renderer->effect() ? _renderer->effect() : surface->effect());

	if (change == MacroChange::REF_CHANGED && !_surfaceToDrawCalls[surface].empty())
	{
		const DrawCallList					drawCalls = _macroNameToDrawCalls[surface][macro.name()];
		std::unordered_set<Container::Ptr>	failedDrawcallRendererData;

		for (auto& drawCall : drawCalls)
		{
			auto rendererData	= _drawCallToRendererData[surface][drawCall];
			auto pass			= _drawCallToPass[surface][drawCall];
	
			if (!initializeDrawCall(pass, surface, drawCall))
				failedDrawcallRendererData.insert(rendererData);
		}

		if (!failedDrawcallRendererData.empty())
		{
			if (effect->hasFallback(surface->technique()))
				surface->setTechnique(effect->fallback(surface->technique()), true);
		}
	}
	else if (_techniqueToMacroNames[surface].count(surface->technique()) != 0 
		&&   _techniqueToMacroNames[surface][surface->technique()].find(macro.name()) != _techniqueToMacroNames[surface][surface->technique()].end())
	{
		int numListeners = _numMacroListeners[surface].count(macro) == 0 ? 0 : _numMacroListeners[surface][macro];

		if (change == MacroChange::ADDED)
		{
			if (numListeners == 0)
				_macroChangedSlots[surface][macro] = macro.container()->propertyReferenceChanged(macro.name())->connect(std::bind(
					&DrawCallPool::macroChangedHandler,
					shared_from_this(),
					macro.container(),
					macro.name(),
					surface,
					MacroChange::REF_CHANGED
				));

			_numMacroListeners[surface][macro] = numListeners + 1;
		}
		else if (change == MacroChange::REMOVED)
		{
			if (surface->numTargets() > 0)
				macroChangedHandler(macro.container(), macro.name(), surface, MacroChange::REF_CHANGED);

			_numMacroListeners[surface][macro] = numListeners - 1;

			if (_numMacroListeners[surface][macro] == 0)
				_macroChangedSlots[surface].erase(macro);
		}
	}
}


void
DrawCallPool::watchMacroAdditionOrDeletion(Surface::Ptr surface)
{
	_macroAddedOrRemovedSlots[surface].clear();

	if (surface->targets().empty())
		return;

	auto	rendererData	= _renderer->targets()[0]->data();
	auto&	target			= surface->targets().front();
	auto	targetData		= target->data();
	auto	rootData		= target->root()->data();

	_macroAddedOrRemovedSlots[surface].push_back(
		targetData->propertyAdded()->connect(std::bind(
			&DrawCallPool::macroChangedHandler,
			shared_from_this(),
			std::placeholders::_1,
			std::placeholders::_2,
			surface,
			MacroChange::ADDED
		))
	);

	_macroAddedOrRemovedSlots[surface].push_back(
		targetData->propertyRemoved()->connect(std::bind(
			&DrawCallPool::macroChangedHandler,
			shared_from_this(),
			std::placeholders::_1,
			std::placeholders::_2,
			surface,
			MacroChange::REMOVED
		))
	);

	_macroAddedOrRemovedSlots[surface].push_back(
		rendererData->propertyAdded()->connect(std::bind(
			&DrawCallPool::macroChangedHandler,
			shared_from_this(),
			std::placeholders::_1,
			std::placeholders::_2,
			surface,
			MacroChange::ADDED
		))
	);

	_macroAddedOrRemovedSlots[surface].push_back(
		rendererData->propertyRemoved()->connect(std::bind(
			&DrawCallPool::macroChangedHandler,
			shared_from_this(),
			std::placeholders::_1,
			std::placeholders::_2,
			surface,
			MacroChange::REMOVED
		))
	);

	_macroAddedOrRemovedSlots[surface].push_back(
		rootData->propertyAdded()->connect(std::bind(
			&DrawCallPool::macroChangedHandler,
			shared_from_this(),
			std::placeholders::_1,
			std::placeholders::_2,
			surface,
			MacroChange::ADDED
		))
	);

	_macroAddedOrRemovedSlots[surface].push_back(
		rootData->propertyRemoved()->connect(std::bind(
			&DrawCallPool::macroChangedHandler,
			shared_from_this(),
			std::placeholders::_1,
			std::placeholders::_2,
			surface,
			MacroChange::REMOVED
		))
	);
}

void
DrawCallPool::blameMacros(Surface::Ptr							surface,
						  const std::list<ContainerProperty>&	incorrectIntegerMacros,
						  const TechniquePass&					pass)
{
	for (auto& macro : incorrectIntegerMacros)
	{
		auto&	failedPasses = _incorrectMacroToPasses[surface][macro];
		auto	failedPassIt = std::find(failedPasses.begin(), failedPasses.end(), pass);
	
		if (failedPassIt == failedPasses.end())
		{
			failedPasses.push_back(pass);
	
#ifdef DEBUG_FALLBACK
			for (auto& techniqueName : _incorrectMacroToPasses[macro])
				std::cout << "'" << macro.name() << "' made [technique '" << pass.first << "' | pass '" << pass.second.get() << "'] fail" << std::endl;
#endif // DEBUG_FALLBACK
		}
	
		if (_incorrectMacroChangedSlot[surface].count(macro) == 0)
		{
#if defined(EMSCRIPTEN)
			// See issue #1848 in Emscripten: https://github.com/kripken/emscripten/issues/1848
			auto that = shared_from_this();
			_incorrectMacroChangedSlot[surface][macro] = macro.container()->propertyReferenceChanged(macro.name())->connect([&, that, surface, macro](data::Container::Ptr container, const std::string& propertyName) {
				that->incorrectMacroChangedHandler(surface, macro);
			});
#else
			_incorrectMacroChangedSlot[surface][macro] = macro.container()->propertyReferenceChanged(macro.name())->connect(std::bind(
				&DrawCallPool::incorrectMacroChangedHandler,
				shared_from_this(),
				surface,
				macro
			));
#endif
		}
	}
}

void
DrawCallPool::incorrectMacroChangedHandler(Surface::Ptr				surface,
										   const ContainerProperty&	macro)
{
	if (_incorrectMacroToPasses[surface].count(macro) > 0)
	{
		surface->setTechnique(_incorrectMacroToPasses[surface][macro].front().first, true); // FIXME
	}
}

void
DrawCallPool::forgiveMacros(Surface::Ptr						surface,
							const std::list<ContainerProperty>&	booleanMacros,
							const std::list<ContainerProperty>&	integerMacros,
							const TechniquePass&				pass)
{
	for (auto& macro : integerMacros)
		if (_incorrectMacroToPasses[surface].count(macro) > 0)
		{
			auto&	failedPasses	= _incorrectMacroToPasses[surface][macro];
			auto	failedPassIt	= std::find(failedPasses.begin(), failedPasses.end(), pass);

			if (failedPassIt != failedPasses.end())
			{
				failedPasses.erase(failedPassIt);

				if (failedPasses.empty() 
					&& _incorrectMacroChangedSlot[surface].count(macro) > 0)
					_incorrectMacroChangedSlot[surface].erase(macro);
			}
		}
}

void
DrawCallPool::zsortNeededHandler(Surface::Ptr	surface, 
								 DrawCall::Ptr	drawcall)
{
	_mustZSort = true;
}


void
DrawCallPool::replaceVariable(std::string& propertyName, std::unordered_map<std::string, std::string>& variableToValue)
{
	if (_variablePropertyNameToPosition.find(propertyName) != _variablePropertyNameToPosition.end())
	{
		auto variablePosition = _variablePropertyNameToPosition[propertyName];

		if (variablePosition.second != -1)
		{
			auto key = "${" + variablePosition.first + "}";
			propertyName.replace(variablePosition.second, key.size(), variableToValue[variablePosition.first]);
		}
	}
	else
	{
		auto variableIt = variableToValue.begin();
		bool changed = false;

		while (variableIt != variableToValue.end())
		{
			auto key = "${" + variableIt->first + "}";
			size_t position = propertyName.rfind(key);

			if (position != std::string::npos)
			{
				propertyName.replace(position, key.size(), variableIt->second);
				_variablePropertyNameToPosition[propertyName] = std::pair<std::string, int>(variableIt->first, position);
				changed = true;
			}
			variableIt++;
		}

		if (!changed)
			_variablePropertyNameToPosition[propertyName] = std::pair<std::string, int>("", -1);
	}
}