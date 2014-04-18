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

/*static*/ const unsigned int									DrawCallPool::NUM_FALLBACK_ATTEMPTS		= 32;
/*static*/ std::unordered_map<DrawCall::Ptr, Vector3::Ptr>		DrawCallPool::_cachedDrawcallPositions;

std::unordered_map<std::string, std::pair<std::string, int>>	DrawCallPool::_variablePropertyNameToPosition;


DrawCallPool::DrawCallPool(Renderer::Ptr renderer):
	_renderer(renderer),
	_toCollect(),
	_toRemove(),
	_invisibleSurfaces(),
	_surfaceToDrawCalls(),
	_drawcallToSurface(),
	_drawcallToMacroChangedSlot(),
	_drawcallToZSortNeededSlot(),
	_drawCalls(),
	_dirtyDrawCalls(),
	_mustZSort(true),
	_surfaceToTechniqueChangedSlot(),
	_surfaceToVisibilityChangedSlots(),
	_surfaceToIndexChangedSlots(),
	_surfaceBadMacroToTechniques(),
	_surfaceBadMacroToChangedSlot(),
	_rendererFilterChangedSlot(nullptr)
{
	if (_renderer == nullptr)
		throw std::invalid_argument("renderer");
}

void
DrawCallPool::initialize()
{
	_rendererFilterChangedSlot = _renderer->filterChanged()->connect([=](Renderer::Ptr r, AbstractFilterPtr f, data::BindingSource s){
		rendererFilterChangedHandler(r, f, s);
	});
}

const std::list<DrawCall::Ptr>&
DrawCallPool::drawCalls()
{
	const bool doZSort = _mustZSort || !_toCollect.empty();

	for (auto& surface : _toRemove)
		cleanSurface(surface);		
	_toRemove.clear();

	auto dirtyDrawCalls = _dirtyDrawCalls;
	_dirtyDrawCalls.clear();

	for (auto& d : dirtyDrawCalls)
		refreshDrawCall(d);


	for (auto& surface : _toCollect)
	{
		auto& newDrawCalls = generateDrawCall(surface, NUM_FALLBACK_ATTEMPTS);

		_drawCalls.insert(_drawCalls.end(), newDrawCalls.begin(), newDrawCalls.end());
	}
	_toCollect.clear();

	if (doZSort)
	{
		_cachedDrawcallPositions.clear();
		_drawCalls.sort(&DrawCallPool::compareDrawCalls);
	}
	_mustZSort = false;

	return _drawCalls;
}

/*static*/
bool
DrawCallPool::compareDrawCalls(DrawCall::Ptr a, 
							   DrawCall::Ptr b)
{
	const float	aPriority			= a->priority();
	const float	bPriority			= b->priority();
	const bool	arePrioritiesEqual	= fabsf(aPriority - bPriority) < 1e-3f;

	if (!arePrioritiesEqual)
		return aPriority > bPriority;

	if (a->zSorted() || b->zSorted())
	{
		static Vector3::Ptr aPosition = Vector3::create();
		static Vector3::Ptr bPosition = Vector3::create();

		getDrawcallEyePosition(a, aPosition);
		getDrawcallEyePosition(b, bPosition);

		return aPosition->z() > bPosition->z();
	}
	else
	{
		// ordered by target texture id, if any
		return a->target() && (!b->target() || (a->target()->id() > b->target()->id()));
	}
}

/*static*/
Vector3::Ptr
DrawCallPool::getDrawcallEyePosition(DrawCall::Ptr drawcall, 
									 Vector3::Ptr output)
{
	const auto foundPositionIt = _cachedDrawcallPositions.find(drawcall);

	if (foundPositionIt != _cachedDrawcallPositions.end())
	{
		if (output == nullptr)
			output = Vector3::create(foundPositionIt->second);
		else
			output->copyFrom(foundPositionIt->second);

		return output;
	}
	else
		return drawcall->getEyeSpacePosition(output);
}

void
DrawCallPool::addSurface(Surface::Ptr surface)
{
	_surfaceToTechniqueChangedSlot[surface] = surface->techniqueChanged()->connect(std::bind(
		&DrawCallPool::techniqueChangedHandler,
		shared_from_this(),
		std::placeholders::_1,
		std::placeholders::_2,
		std::placeholders::_3));

	_surfaceToVisibilityChangedSlots.insert(std::pair<SurfacePtr, VisibilityChanged::Slot>(surface, surface->visibilityChanged()->connect(std::bind(
		&DrawCallPool::visibilityChangedHandler,
		shared_from_this(),
		std::placeholders::_1,
		std::placeholders::_2,
		std::placeholders::_3))));

	_surfaceToVisibilityChangedSlots.insert(std::pair<SurfacePtr, VisibilityChanged::Slot>(surface, surface->computedVisibilityChanged()->connect(std::bind(
		&DrawCallPool::visibilityChangedHandler,
		shared_from_this(),
		std::placeholders::_1,
		std::placeholders::_2,
		std::placeholders::_3))));

	// drawcall variable monitoring handlers

	auto geometrySlot		= surface->geometry()->data()->indexChanged()->connect(std::bind(
		&DrawCallPool::drawCallVariablesChangedHandler,
		shared_from_this(),
		std::placeholders::_1,
		std::placeholders::_2,
		surface
	));

	_surfaceToIndexChangedSlots.insert(std::pair<Surface::Ptr, ArrayIndexChanged::Slot>(surface, geometrySlot));

	auto arrayMaterial		= std::dynamic_pointer_cast<data::ArrayProvider>(surface->material());
	if (arrayMaterial)
	{
		auto materialSlot	= arrayMaterial->indexChanged()->connect(std::bind(
			&DrawCallPool::drawCallVariablesChangedHandler,
			shared_from_this(),
			std::placeholders::_1,
			std::placeholders::_2,
			surface
		));

		_surfaceToIndexChangedSlots.insert(std::pair<Surface::Ptr, ArrayIndexChanged::Slot>(surface, materialSlot));
	}

	//if (std::find(_toCollect.begin(), _toCollect.end(), surface) == _toCollect.end())
	if (surface->visible(_renderer))
		_toCollect.insert(surface);
	else
		_invisibleSurfaces.insert(surface);
}


void
DrawCallPool::removeSurface(Surface::Ptr surface)
{
	auto foundSurfaceIt = _toCollect.find(surface);

	if (foundSurfaceIt == _toCollect.end())
		_toRemove.insert(surface);
	else
		_toCollect.erase(foundSurfaceIt);
}

void
DrawCallPool::cleanSurface(Surface::Ptr	surface)
{
	deleteDrawCalls(surface);

	_surfaceToDrawCalls[surface].clear();
	_surfaceToTechniqueChangedSlot.erase(surface);
	//_surfaceToVisibilityChangedSlots.erase(surface);
	_surfaceToIndexChangedSlots.erase(surface);

	if (_surfaceBadMacroToTechniques.count(surface))
		_surfaceBadMacroToTechniques.erase(surface);
	if (_surfaceBadMacroToChangedSlot.count(surface))
		_surfaceBadMacroToChangedSlot.erase(surface);
}

void
DrawCallPool::techniqueChangedHandler(Surface::Ptr			surface, 
									  const std::string&	technique, 
									  bool					updateDrawCall)
{
	deleteDrawCalls(surface);
	if (updateDrawCall && std::find(_toCollect.begin(), _toCollect.end(), surface) == _toCollect.end())
		_toCollect.insert(surface);
}

void
DrawCallPool::visibilityChangedHandler(Surface::Ptr		surface,
									   Renderer::Ptr	renderer,
									   bool				value)
{
	if (renderer != _renderer && renderer != nullptr)
		return;

	bool visible = surface->visible(_renderer) && surface->computedVisibility(_renderer);

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

void
DrawCallPool::drawCallVariablesChangedHandler(data::ArrayProvider::Ptr	provider, 
											  uint						index, 
											  Surface::Ptr				surface)
{
	if (_toRemove.find(surface) != _toRemove.end())
		_toCollect.insert(surface);
}

std::list<DrawCall::Ptr>&
DrawCallPool::generateDrawCall(Surface::Ptr	surface,
							   unsigned int	numAttempts)
{
	if (_surfaceToDrawCalls.find(surface) != _surfaceToDrawCalls.end())
		deleteDrawCalls(surface);

	bool							mustFallbackTechnique	= false;
	std::shared_ptr<render::Effect> effect					= surface->effect();
	std::string						technique				= surface->technique();

	if (_renderer->effect())
	{
		effect		= _renderer->effect();
		technique	= effect->techniques().begin()->first;
	}

	_surfaceToDrawCalls	[surface] = std::list<DrawCall::Ptr>();

	for (const auto& pass : effect->technique(technique))
	{
		auto drawCall = initializeDrawCall(surface, pass);

		if (drawCall)
		{
			_surfaceToDrawCalls[surface].push_back(drawCall);

			_drawcallToSurface[drawCall] = surface;

			_drawcallToMacroChangedSlot[drawCall] = drawCall->macroChanged()->connect([=](DrawCall::Ptr d, Container::Ptr c, const std::string& n){
				drawcallMacroChangedHandler(d, c, n);
			});

			_drawcallToZSortNeededSlot[drawCall] = drawCall->zsortNeeded()->connect([=](DrawCall::Ptr d){
				drawcallZSortNeededHandler(d);
			});			
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

		if (numAttempts > 0 && effect->hasFallback(technique))
		{
			surface->setEffectAndTechnique(effect, effect->fallback(technique), false);

			return generateDrawCall(surface, numAttempts - 1);
		}
	}

	return _surfaceToDrawCalls[surface];
}

DrawCall::Ptr
DrawCallPool::initializeDrawCall(Surface::Ptr	surface,
								 Pass::Ptr		pass,
								 DrawCall::Ptr	drawCall)
{
	if (pass == nullptr)
		return nullptr;

	const auto	target			= surface->targets()[0];

	auto	fullTargetData		= target->data();
	auto	fullRendererData	= _renderer->targets()[0]->data();
	auto	fullRootData		= target->root()->data();

	ContainerPtr targetData		= nullptr;
	ContainerPtr rendererData	= nullptr;
	ContainerPtr rootData		= nullptr;

	if (drawCall)
	{
		targetData		= drawCall->targetData();
		rendererData	= drawCall->rendererData();
		rootData		= drawCall->rootData();

		//drawCall->unbind();
	}

	_renderer->setFilterSurface(surface);

	const auto&	targetFilters	= _renderer->filters(data::BindingSource::TARGET);
	const auto&	rendererFilters	= _renderer->filters(data::BindingSource::RENDERER);
	const auto&	rootFilters		= _renderer->filters(data::BindingSource::ROOT);

	targetData		= fullTargetData->filter(targetFilters, targetData);
	rendererData	= fullRendererData->filter(rendererFilters, rendererData);
	rootData		= fullRootData->filter(rootFilters, rootData);

	// get drawcall's property name formatting function (dependent on filters!)
	auto geometryId	= targetData->getProviderIndex(surface->geometry()->data());
	auto materialId	= targetData->getProviderIndex(surface->material());

	std::unordered_map<std::string, std::string> drawCallVariables;

	drawCallVariables["geometryId"] = std::to_string(geometryId);
	drawCallVariables["materialId"] = std::to_string(materialId);

	FormatNameFunction formatNameFunc = std::bind(
		&DrawCallPool::formatPropertyName, 
		shared_from_this(), 
		std::placeholders::_1, 
		drawCallVariables
	);
	
	auto program = getWorkingProgram(
		surface,
		pass,
		formatNameFunc, 
		fullTargetData,
		fullRendererData,
		fullRootData,
		targetData,
		rendererData,
		rootData
	);

	if (!program)
		return nullptr;
	
	if (drawCall == nullptr)
		drawCall = DrawCall::create(pass);

	drawCall->configure(
		program, 
		formatNameFunc,
		fullTargetData,
		fullRendererData,
		fullRootData,
		targetData, 
		rendererData, 
		rootData
	);

	return drawCall;
}


std::shared_ptr<Program>
DrawCallPool::getWorkingProgram(Surface::Ptr			surface,
								Pass::Ptr				pass,
								FormatNameFunction		formatNameFunc,
								ContainerPtr			fullTargetData,
								ContainerPtr			fullRendererData,
								ContainerPtr			fullRootData,
								ContainerPtr			targetData,
								ContainerPtr			rendererData,
								ContainerPtr			rootData)
{
	// get program signature from filtered data containers
	std::list<std::string>	booleanMacros;
	std::list<std::string>	integerMacros;
	std::list<std::string>	incorrectIntegerMacros;	

	Program::Ptr program = pass->selectProgram(
		formatNameFunc, 
		targetData,
		rendererData,
		rootData,
		booleanMacros, 
		integerMacros, 
		incorrectIntegerMacros
	);

	// forgive good macros
	for (auto& macroName : integerMacros)
	{
		if (_surfaceBadMacroToTechniques[surface].count(macroName) > 0)
		{
			auto&	macroTechniques = _surfaceBadMacroToTechniques[surface][macroName];
			auto	foundTechIt		= std::find(macroTechniques.begin(), macroTechniques.end(), surface->technique());

			if (foundTechIt != macroTechniques.end())
			{
				macroTechniques.erase(foundTechIt);

				if (macroTechniques.empty() && _surfaceBadMacroToChangedSlot[surface].count(macroName) > 0)
					_surfaceBadMacroToChangedSlot[surface].erase(macroName);
			}
		}
	}

	// blame macros that made the current technique fail
	for (auto& macroName : incorrectIntegerMacros)
	{
		const auto& binding			= pass->macroBindings().find(macroName)->second;

		const auto& propertyName	= std::get<0>(binding);
		auto		source			= std::get<1>(binding);

		auto		container		= source == data::BindingSource::TARGET 
			? targetData
			: (source == data::BindingSource::RENDERER ? rendererData : rootData);

		auto		fullContainer	= source == data::BindingSource::TARGET 
			? fullTargetData
			: (source == data::BindingSource::RENDERER ? fullRendererData : fullRootData);

		const auto&	formattedName	= formatNameFunc(propertyName);

		const bool	inContainer		= container->hasProperty(formattedName);
		const bool	inFullContainer	= fullContainer->hasProperty(formattedName);

		if (inContainer)
		{
			if (container->isLengthProperty(formattedName) 
				&& fullContainer->hasProperty(formattedName))
			{
				auto slot = fullContainer->propertyReferenceChanged(formattedName)->connect([=](Container::Ptr c, const std::string& n){
					surfaceBadMacroChangedHandler(surface, macroName);
				});

				_surfaceBadMacroToTechniques[surface][macroName].insert(surface->technique());
				_surfaceBadMacroToChangedSlot[surface][macroName] = slot;
			}
			else
			{
				auto slot = container->propertyReferenceChanged(formattedName)->connect([=](Container::Ptr c, const std::string& n){
					surfaceBadMacroChangedHandler(surface, macroName);
				});

				_surfaceBadMacroToTechniques[surface][macroName].insert(surface->technique());
				_surfaceBadMacroToChangedSlot[surface][macroName] = slot;
			}
		}
		else if (fullContainer->hasProperty(propertyName))
		{
			auto slot = fullContainer->propertyReferenceChanged(propertyName)->connect([=](Container::Ptr c, const std::string& n){
				surfaceBadMacroChangedHandler(surface, macroName);
			});

			_surfaceBadMacroToTechniques[surface][macroName].insert(surface->technique());
			_surfaceBadMacroToChangedSlot[surface][macroName] = slot;
		}
	}

	return program;
}

void
DrawCallPool::deleteDrawCalls(Surface::Ptr surface)
{
	auto& drawCalls = _surfaceToDrawCalls[surface];

	while (!drawCalls.empty())
	{
		auto drawCall = drawCalls.front();
		drawCalls.pop_front();

		_drawcallToSurface.erase(drawCall);
		_drawcallToMacroChangedSlot.erase(drawCall);
		_drawcallToZSortNeededSlot.erase(drawCall);

		_drawCalls.remove(drawCall);
		_dirtyDrawCalls.erase(drawCall);
	}
}

void
DrawCallPool::drawcallMacroChangedHandler(DrawCall::Ptr			drawCall, 
										  Container::Ptr		container,
										  const std::string&	propertyName)
{
	if (drawCall)
		_dirtyDrawCalls.insert(drawCall);
}

void
DrawCallPool::drawcallZSortNeededHandler(DrawCall::Ptr)
{
	_mustZSort = true;
}

void
DrawCallPool::surfaceBadMacroChangedHandler(Surface::Ptr		surface, 
											const std::string&	macroName)
{
	if (surface == nullptr)
		return;

	auto effect = _renderer->effect() ? _renderer->effect() : surface->effect();

	if (_surfaceBadMacroToTechniques[surface].count(macroName) > 0)
	{
		const auto& techniques	= _surfaceBadMacroToTechniques[surface][macroName];

		if (!techniques.empty())
		{
			auto		techniqueIt	= _surfaceBadMacroToTechniques[surface][macroName].begin();
			const auto& technique	= *techniqueIt; // FIXME

			surface->setEffectAndTechnique(effect, technique);
		}
	}
}

void
DrawCallPool::rendererFilterChangedHandler(Renderer::Ptr, 
										   DrawCallPool::AbstractFilterPtr, 
										   data::BindingSource)
{
	std::cout << "TODO\tDrawCallPool::rendererFilterChangedHandler" << std::endl;
}

void
DrawCallPool::refreshDrawCall(DrawCall::Ptr drawCall)
{
	if (_drawcallToSurface.count(drawCall) == 0)
		return;

	auto surface	= _drawcallToSurface[drawCall];
	auto effect		= _renderer->effect() ? _renderer->effect() : surface->effect();
	auto technique	= surface->technique(); 

	if (initializeDrawCall(surface, drawCall->pass(), drawCall) == nullptr)
	{
		// failed to update drawcall, try to fallback
		if (effect->hasFallback(technique))
			surface->setEffectAndTechnique(effect, effect->fallback(technique), true);
	}
}

std::string
DrawCallPool::formatPropertyName(const std::string&								rawPropertyName, 
							     std::unordered_map<std::string, std::string>&	variableToValue)
{
	std::string propertyName = rawPropertyName;

	if (_variablePropertyNameToPosition.find(rawPropertyName) != _variablePropertyNameToPosition.end())
	{
		auto variablePosition = _variablePropertyNameToPosition[rawPropertyName];

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
			size_t position = rawPropertyName.rfind(key);

			if (position != std::string::npos)
			{
				propertyName.replace(position, key.size(), variableIt->second);
				_variablePropertyNameToPosition[rawPropertyName] = std::pair<std::string, int>(variableIt->first, position);
				changed = true;
			}
			variableIt++;
		}

		if (!changed)
			_variablePropertyNameToPosition[rawPropertyName] = std::pair<std::string, int>("", -1);
	}

	return propertyName;
}