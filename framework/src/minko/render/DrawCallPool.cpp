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
#include "minko/render/Priority.hpp"

using namespace minko;
using namespace minko::math;
using namespace minko::render;

const unsigned int DrawCallPool::NUM_FALLBACK_ATTEMPTS = 32;

DrawCallPool::DrawCallPool(RendererPtr renderer):
	_renderer(renderer)
{
}

const std::list<std::shared_ptr<DrawCall>>&
DrawCallPool::drawCalls()
{
	unsigned int _numToCollect				= _toCollect.size();
	unsigned int _numToRemove				= _toRemove.size();


	if (_numToCollect == 0 && _numToRemove == 0)
		return _drawCalls;
	
	for (uint removedSurfaceIndex = 0; removedSurfaceIndex < _numToRemove; ++removedSurfaceIndex)
	{
		deleteDrawCalls(_toRemove[removedSurfaceIndex]);
		cleanSurface(_toRemove[removedSurfaceIndex]);
	}

	for (uint surfaceIndex = 0; surfaceIndex < _numToCollect; ++surfaceIndex)
	{
		auto& newDrawCalls = generateDrawCall(_toCollect[surfaceIndex], NUM_FALLBACK_ATTEMPTS);
		_drawCalls.insert(_drawCalls.end(), newDrawCalls.begin(), newDrawCalls.end());
	}
	
	_drawCalls.sort(&DrawCallPool::compareDrawCalls);
	_toRemove.resize(0);
	_toCollect.erase(_toCollect.begin(), _toCollect.begin() + _numToCollect);

	return _drawCalls;
}


bool
DrawCallPool::compareDrawCalls(DrawCallPtr a, DrawCallPtr b)
{
	const float	aPriority			= a->priority();
	const float	bPriority			= b->priority();
	const bool	arePrioritiesEqual	= fabsf(aPriority - bPriority) < 1e-3f;

	if (!arePrioritiesEqual)
		return aPriority > bPriority;

	// aPriority == bPriority
	const bool	areTransparent		= priority::LAST < aPriority && !( aPriority > priority::TRANSPARENT);

	if (areTransparent && (a->zsorted() || b->zsorted()))
	{
		static Vector3::Ptr aPosition = Vector3::create();
		static Vector3::Ptr bPosition = Vector3::create();

		a->getEyeSpacePosition(aPosition);
		b->getEyeSpacePosition(bPosition);

		// z-sort
		return aPosition->z() > bPosition->z();
	}
	else
	{
		// ordered by target texture id, if any
		return a->target() && (!b->target() || (a->target()->id() > b->target()->id()));
	}
}

void
DrawCallPool::addSurface(SurfacePtr surface)
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

	Effect::Ptr effect = (_renderer->effect() ? _renderer->effect() : surface->effect());

	for (auto& technique : effect->techniques())
	{
		auto& techniqueName = technique.first;

		for (auto& pass : technique.second)
			for (auto& macroBinding : pass->macroBindings())
				_techniqueToMacroNames[surface][techniqueName].insert(std::get<0>(macroBinding.second));
	}

	_toCollect.push_back(surface);
}

void
DrawCallPool::removeSurface(SurfacePtr surface)
{
	_toRemove.push_back(surface);
}

void
DrawCallPool::cleanSurface(SurfacePtr surface)
{
	_surfaceToTechniqueChangedSlot.erase(surface);
	_surfaceToDrawCalls[surface].clear();
	_macroAddedOrRemovedSlots[surface].clear();
	_macroChangedSlots[surface].clear();
	_numMacroListeners[surface].clear();
}

void
DrawCallPool::techniqueChanged(SurfacePtr surface, const std::string& technique, bool updateDrawCall)
{
	//_toRemove.push_back(surface);
	deleteDrawCalls(surface);
	if (updateDrawCall)
		_toCollect.push_back(surface);
}

void
DrawCallPool::visibilityChanged(SurfacePtr surface, bool value)
{
	bool visible = surface->visible() && surface->computedVisibility();

	if (visible && _invisibleSurfaces.find(surface) != _invisibleSurfaces.end()) // visible and already wasn't visible before
	{
		_toCollect.push_back(surface);
		_invisibleSurfaces.erase(surface);
	}
	else if (!visible && _invisibleSurfaces.find(surface) == _invisibleSurfaces.end()) // not visible but was visible before
	{
		_toRemove.push_back(surface);
		_invisibleSurfaces.insert(surface);
	}
	
}

DrawCallPool::DrawCallList&
DrawCallPool::generateDrawCall(SurfacePtr	surface,
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

	_surfaceToDrawCalls[surface] = std::list<DrawCall::Ptr>();

	for (const auto& pass : drawCallEffect->technique(technique))
	{
		auto drawCall = initializeDrawCall(pass, surface);

		if (drawCall)
		{
			_surfaceToDrawCalls[surface].push_back(drawCall);
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

std::shared_ptr<DrawCall>
DrawCallPool::initializeDrawCall(std::shared_ptr<render::Pass>			pass, 
									std::shared_ptr<component::Surface>	surface,
									std::shared_ptr<DrawCall>			drawcall)
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

	std::list<data::ContainerProperty>	booleanMacros;
	std::list<data::ContainerProperty>	integerMacros;
	std::list<data::ContainerProperty>	incorrectIntegerMacros;

	auto program = getWorkingProgram(
		surface,
		pass, 
		targetData, 
		rendererData, 
		rootData, 
		booleanMacros, 
		integerMacros,
		incorrectIntegerMacros
	);

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

		_drawCallToPass[surface][drawcall]			= pass;
		_drawCallToRendererData[surface][drawcall]	= rendererData;

		for (const auto& binding : pass->macroBindings())
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
DrawCallPool::getWorkingProgram(std::shared_ptr<component::Surface>	surface,
								   std::shared_ptr<Pass>				pass,
								   std::shared_ptr<data::Container>		targetData,
								   std::shared_ptr<data::Container>		rendererData,
								   std::shared_ptr<data::Container>		rootData,
								   std::list<data::ContainerProperty>&	booleanMacros,
								   std::list<data::ContainerProperty>&	integerMacros,
								   std::list<data::ContainerProperty>&	incorrectIntegerMacros)
{
	Program::Ptr program = nullptr;

	do
	{
		program = pass->selectProgram(
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
DrawCallPool::deleteDrawCalls(SurfacePtr surface)
{
	auto& drawCalls = _surfaceToDrawCalls[surface];

	while (!drawCalls.empty())
	{
		auto drawCall = drawCalls.front();

		//_drawCallRemoved->execute(shared_from_this(), surface, drawCall);
		drawCalls.pop_front();

		_drawCallToPass[surface].erase(drawCall);
		_drawCallToRendererData[surface].erase(drawCall);

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
DrawCallPool::macroChangedHandler(ContainerPtr		container, 
									 const std::string& propertyName, 
									 SurfacePtr			surface, 
									 MacroChange		change)
{
	const data::ContainerProperty	macro(propertyName, container);
	Effect::Ptr						effect = (_renderer->effect() ? _renderer->effect() : surface->effect());


	if (change == MacroChange::REF_CHANGED && !_surfaceToDrawCalls[surface].empty())
	{
		const DrawCallList							drawCalls		= _macroNameToDrawCalls[surface][macro.name()];
		std::unordered_set<data::Container::Ptr>	failedDrawcallRendererData;

		for (std::shared_ptr<DrawCall> drawCall : drawCalls)
		{
			auto	rendererData	= _drawCallToRendererData[surface][drawCall];
			auto	pass			= _drawCallToPass[surface][drawCall];
	
			if (!initializeDrawCall(pass, surface, drawCall))
				failedDrawcallRendererData.insert(rendererData);
		}

		if (!failedDrawcallRendererData.empty())
		{
			// at least, one pass failed for good. must fallback the whole technique.
			//for (auto& rendererData : failedDrawcallRendererData)
			//	if (_drawCallToRendererData[surface].count(rendererData) > 0)
			//		deleteDrawCalls(surface);

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
			macroChangedHandler(macro.container(), macro.name(), surface, MacroChange::REF_CHANGED);

			_numMacroListeners[surface][macro] = numListeners - 1;

			if (_numMacroListeners[surface][macro] == 0)
				_macroChangedSlots[surface].erase(macro);
		}
	}
}


void
DrawCallPool::watchMacroAdditionOrDeletion(std::shared_ptr<component::Surface> surface)
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
DrawCallPool::blameMacros(SurfacePtr									surface,
							 const std::list<data::ContainerProperty>&	incorrectIntegerMacros,
							 const TechniquePass&						pass)
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
DrawCallPool::incorrectMacroChangedHandler(SurfacePtr						surface,
										   const data::ContainerProperty&	macro)
{
	if (_incorrectMacroToPasses[surface].count(macro) > 0)
	{
		surface->setTechnique(_incorrectMacroToPasses[surface][macro].front().first, true); // FIXME
	}
}

void
DrawCallPool::forgiveMacros(SurfacePtr											surface,
							   const std::list<data::ContainerProperty>&			booleanMacros,
							   const std::list<data::ContainerProperty>&			integerMacros,
							   const TechniquePass&									pass)
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
