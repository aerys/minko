/*
Copyright (c) 2014 Aerys

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

#include "minko/component/Renderer.hpp"

#include "minko/scene/Node.hpp"
#include "minko/scene/NodeSet.hpp"
#include "minko/component/Surface.hpp"
#include "minko/render/DrawCall.hpp"
#include "minko/render/Effect.hpp"
#include "minko/render/Pass.hpp"
#include "minko/render/AbstractTexture.hpp"
#include "minko/render/AbstractContext.hpp"
#include "minko/component/SceneManager.hpp"
#include "minko/file/AssetLibrary.hpp"
#include "minko/render/DrawCallPool.hpp"
#include "minko/data/AbstractFilter.hpp"
#include "minko/data/LightMaskFilter.hpp"
#include "minko/data/Collection.hpp"
#include "minko/geometry/Geometry.hpp"
#include "minko/material/Material.hpp"

using namespace minko;
using namespace minko::component;
using namespace minko::scene;
using namespace minko::render;

const unsigned int Renderer::NUM_FALLBACK_ATTEMPTS = 32;


Renderer::Renderer(std::shared_ptr<render::AbstractTexture> renderTarget,
				   EffectPtr								effect,
				   float									priority) :
    _backgroundColor(0),
    _viewportBox(),
	_scissorBox(),
	_enabled(true),
	_renderingBegin(Signal<Ptr>::create()),
	_renderingEnd(Signal<Ptr>::create()),
	_beforePresent(Signal<Ptr>::create()),
	//_surfaceTechniqueChangedSlot(),
	_effect(effect),
	_priority(priority),
	/*_targetDataFilters(),
	_rendererDataFilters(),
	_rootDataFilters(),
	_targetDataFilterChangedSlots(),
	_rendererDataFilterChangedSlots(),
	_rootDataFilterChangedSlots(),
	_lightMaskFilter(data::LightMaskFilter::create()),*/
	_filterChanged(Signal<Ptr, data::AbstractFilter::Ptr, data::BindingSource, SurfacePtr>::create())
{
	if (renderTarget)
	{
		renderTarget->upload();
		_renderTarget = renderTarget;
	}

	//addFilter(_lightMaskFilter, data::BindingSource::ROOT);
}

void
Renderer::targetAdded(std::shared_ptr<Node> target)
{
    // Comment due to reflection component
	//if (target->components<Renderer>().size() > 1)
	//	throw std::logic_error("There cannot be two Renderer on the same node.");

	_addedSlot = target->added().connect(std::bind(
		&Renderer::addedHandler,
		std::static_pointer_cast<Renderer>(shared_from_this()),
		std::placeholders::_1,
		std::placeholders::_2,
		std::placeholders::_3
	));

	_removedSlot = target->removed().connect(std::bind(
		&Renderer::removedHandler,
		std::static_pointer_cast<Renderer>(shared_from_this()),
		std::placeholders::_1,
		std::placeholders::_2,
		std::placeholders::_3
	));

	addedHandler(target->root(), target, target->parent());
}

void
Renderer::targetRemoved(std::shared_ptr<Node> target)
{
	_addedSlot = nullptr;
	_removedSlot = nullptr;

	removedHandler(target->root(), target, target->parent());

	/*_targetDataFilters.clear();
	_rendererDataFilters.clear();
	_rootDataFilters.clear();*/
}

void
Renderer::addedHandler(std::shared_ptr<Node> node,
						std::shared_ptr<Node> target,
						std::shared_ptr<Node> parent)
{
	findSceneManager();

	_rootDescendantAddedSlot = target->root()->added().connect(std::bind(
		&Renderer::rootDescendantAddedHandler,
		std::static_pointer_cast<Renderer>(shared_from_this()),
		std::placeholders::_1,
		std::placeholders::_2,
		std::placeholders::_3
	));

	_rootDescendantRemovedSlot = target->root()->removed().connect(std::bind(
		&Renderer::rootDescendantRemovedHandler,
		std::static_pointer_cast<Renderer>(shared_from_this()),
		std::placeholders::_1,
		std::placeholders::_2,
		std::placeholders::_3
	));

	_componentAddedSlot = target->root()->componentAdded().connect(std::bind(
		&Renderer::componentAddedHandler,
		std::static_pointer_cast<Renderer>(shared_from_this()),
		std::placeholders::_1,
		std::placeholders::_2,
		std::placeholders::_3
	));

	_componentRemovedSlot = target->root()->componentRemoved().connect(std::bind(
		&Renderer::componentRemovedHandler,
		std::static_pointer_cast<Renderer>(shared_from_this()),
		std::placeholders::_1,
		std::placeholders::_2,
		std::placeholders::_3
	), 10.f);

	//_lightMaskFilter->root(target->root());

	rootDescendantAddedHandler(nullptr, target->root(), nullptr);
}

void
Renderer::removedHandler(std::shared_ptr<Node> node,
						  std::shared_ptr<Node> target,
						  std::shared_ptr<Node> parent)
{
	findSceneManager();

	_rootDescendantAddedSlot	= nullptr;
	_rootDescendantRemovedSlot	= nullptr;
	_componentAddedSlot			= nullptr;
	_componentRemovedSlot		= nullptr;

	rootDescendantRemovedHandler(nullptr, target->root(), nullptr);
}

void
Renderer::rootDescendantAddedHandler(std::shared_ptr<Node> node,
									  std::shared_ptr<Node> target,
									  std::shared_ptr<Node> parent)
{
    auto surfaceNodes = NodeSet::create(target)
		->descendants(true)
        ->where([](scene::Node::Ptr node)
        {
            return node->hasComponent<Surface>();
        });

	for (auto surfaceNode : surfaceNodes->nodes())
		for (auto surface : surfaceNode->components<Surface>())
            _toCollect.insert(surface);
}

void
Renderer::rootDescendantRemovedHandler(std::shared_ptr<Node> node,
									    std::shared_ptr<Node> target,
									    std::shared_ptr<Node> parent)
{
	auto surfaceNodes = NodeSet::create(target)
		->descendants(true)
        ->where([](scene::Node::Ptr node)
        {
            return node->hasComponent<Surface>();
        });

	for (auto surfaceNode : surfaceNodes->nodes())
		for (auto surface : surfaceNode->components<Surface>())
			removeSurface(surface);
}

void
Renderer::componentAddedHandler(std::shared_ptr<Node>				node,
								 std::shared_ptr<Node>				target,
								 std::shared_ptr<AbstractComponent>	ctrl)
{
	auto surfaceCtrl = std::dynamic_pointer_cast<Surface>(ctrl);
	auto sceneManager = std::dynamic_pointer_cast<SceneManager>(ctrl);
	
	if (surfaceCtrl)
        _toCollect.insert(surfaceCtrl);
	else if (sceneManager)
		setSceneManager(sceneManager);
}

void
Renderer::componentRemovedHandler(std::shared_ptr<Node>					node,
								  std::shared_ptr<Node>					target,
								  std::shared_ptr<AbstractComponent>	ctrl)
{
	auto surfaceCtrl = std::dynamic_pointer_cast<Surface>(ctrl);
	auto sceneManager = std::dynamic_pointer_cast<SceneManager>(ctrl);

	if (surfaceCtrl)
		removeSurface(surfaceCtrl);
	else if (sceneManager)
		setSceneManager(nullptr);
}

void
Renderer::addSurface(Surface::Ptr surface)
{
    std::unordered_map<std::string, std::string> variables;

    auto c = surface->target()->data();
    auto surfaceId = 0;

    // FIXME: find some way to avoid linear search for all the ids
    while (surface->target()->component<Surface>(surfaceId) != surface)
        ++surfaceId;

    variables["surfaceId"] = std::to_string(surfaceId);
    variables["geometryId"] = std::to_string(
        getProviderId(c, Surface::GEOMETRY_COLLECTION_NAME, surface->geometry()->data())
    );
    variables["materialId"] = std::to_string(
        getProviderId(c, Surface::MATERIAL_COLLECTION_NAME, surface->material()->data())
    );
    variables["effectId"] = std::to_string(
        getProviderId(c, Surface::EFFECT_COLLECTION_NAME, surface->effect()->data())
    );

    _surfaceToDrawCallIterator[surface] = _drawCallPool.addDrawCalls(
        surface->effect(),
        variables,
        surface->technique(),
        surface->target()->root()->data(),
        target()->data(),
        surface->target()->data()
    );

    _surfaceChangedSlot[surface] = surface->changed().connect(std::bind(
        &Renderer::surfaceChangedHandler,
        std::static_pointer_cast<Renderer>(shared_from_this()),
        std::placeholders::_1
    ));
}


void
Renderer::removeSurface(Surface::Ptr surface)
{
    if (_toCollect.erase(surface) == 0)
    {
        _drawCallPool.removeDrawCalls(_surfaceToDrawCallIterator[surface]);
        _surfaceToDrawCallIterator.erase(surface);
        _surfaceChangedSlot.erase(surface);
    }
}

void
Renderer::surfaceChangedHandler(Surface::Ptr surface)
{
    // The surface's material, geometry or effect is different
    // we completely remove the surface and re-add it again because
    // it's way simpler than just updating what has changed.
    //removeSurface(surface);

    // We don't actually add the surface right away: we collect it and it
    // will be added before the next frame rendering (if any) starts.
    //_toCollect.insert(surface);
}

uint
Renderer::getProviderId(const data::Container&  container,
                        const std::string&      collectionName,
                        data::Provider::Ptr     provider) const
{
    const auto& collections = container.collections();
    auto collectionIt = std::find_if(collections.begin(), collections.end(), [&](data::Collection::Ptr c)
    {
        return c->name() == collectionName;
    });

    assert(collectionIt != collections.end());

    const auto& collectionItems = (*collectionIt)->items();
    auto providerIt = std::find(collectionItems.begin(), collectionItems.end(), provider);

    assert(providerIt != collectionItems.end());

    return providerIt - collectionItems.begin();
}

void
Renderer::render(render::AbstractContext::Ptr	context, 
				 render::AbstractTexture::Ptr	renderTarget)
{
    if (!_enabled)
		return;

    // some surfaces have been added during the frame and collected
    // in _toCollect: we now have to take them into account to build
    // the corresponding draw calls before rendering
    for (auto& surface : _toCollect)
        addSurface(surface);
    _toCollect.clear();
    
	_renderingBegin->execute(std::static_pointer_cast<Renderer>(shared_from_this()));

	if (_renderTarget)
		renderTarget = _renderTarget;
        
    bool bCustomViewport = false;

	if (_scissorBox.width >= 0 && _scissorBox.height >= 0)
		context->setScissorTest(true, _scissorBox);
	else
		context->setScissorTest(false, _scissorBox);
	 
    if (_viewportBox.width >= 0 && _viewportBox.height >= 0)
    {
        bCustomViewport = true;
        context->configureViewport(_viewportBox.x, _viewportBox.y, _viewportBox.width, _viewportBox.height);
	}
	else
		context->configureViewport(0, 0, context->viewportWidth(), context->viewportHeight());
	
	if (renderTarget)
		context->setRenderToTexture(renderTarget->id(), true);
	else
    	context->setRenderToBackBuffer();
    context->clear(
	    ((_backgroundColor >> 24) & 0xff) / 255.f,
	    ((_backgroundColor >> 16) & 0xff) / 255.f,
	    ((_backgroundColor >> 8) & 0xff) / 255.f,
	    (_backgroundColor & 0xff) / 255.f
    );

    _drawCallPool.update();
    for (const auto& drawCall : _drawCallPool.drawCalls())
        // FIXME: render the draw call only if it's the right layout
	    //if ((drawCall->layouts() & layoutMask()) != 0)
		    drawCall->render(context, renderTarget);

    if (bCustomViewport)
        context->setScissorTest(false, _viewportBox);

    _beforePresent->execute(std::static_pointer_cast<Renderer>(shared_from_this()));

    context->present();

    _renderingEnd->execute(std::static_pointer_cast<Renderer>(shared_from_this()));
}

void
Renderer::findSceneManager()
{
	NodeSet::Ptr roots = NodeSet::create(target())
		->roots()
		->where([](NodePtr node)
		{
			return node->hasComponent<SceneManager>();
		});

	if (roots->nodes().size() > 1)
		throw std::logic_error("Renderer cannot be in two separate scenes.");
	else if (roots->nodes().size() == 1)
		setSceneManager(roots->nodes()[0]->component<SceneManager>());		
	else
		setSceneManager(nullptr);
}

void
Renderer::setSceneManager(std::shared_ptr<SceneManager> sceneManager)
{
	if (sceneManager != _sceneManager)
	{
		if (sceneManager)
		{
			_sceneManager = sceneManager;
			_renderingBeginSlot = _sceneManager->renderingEnd()->connect(std::bind(
				&Renderer::sceneManagerRenderingBeginHandler,
				std::static_pointer_cast<Renderer>(shared_from_this()),
				std::placeholders::_1,
				std::placeholders::_2,
				std::placeholders::_3
			), _priority);
		}
		else
		{
			_sceneManager = nullptr;
			_renderingBeginSlot = nullptr;
		}
	}
}

void
Renderer::sceneManagerRenderingBeginHandler(std::shared_ptr<SceneManager>	sceneManager,
										    uint							frameId,
										    AbstractTexture::Ptr			renderTarget)
{
	render(sceneManager->assets()->context(), renderTarget);
}

Renderer::Ptr
Renderer::addFilter(data::AbstractFilter::Ptr	filter, 
					data::BindingSource			source)
{
    // FIXME
    /*
	if (filter)
	{
		auto& filters				= this->filtersRef(source);
		auto& filterChangedSlots	= this->filterChangedSlotsRef(source);

		if (filterChangedSlots.count(filter) == 0)
		{
			filters.insert(filter);
			filterChangedSlots[filter] = filter->changed()->connect([=](AbsFilterPtr, SurfacePtr surface){
				filterChangedHandler(filter, source, surface);
			});
		}
	}
    */

	return std::static_pointer_cast<Renderer>(shared_from_this());
}

Renderer::Ptr
Renderer::removeFilter(data::AbstractFilter::Ptr	filter, 
					   data::BindingSource			source)
{
    // FIXME
	/*if (filter)
	{
		auto& filters				= this->filtersRef(source);
		auto& filterChangedSlots	= this->filterChangedSlotsRef(source);

		auto foundFilterIt = filters.find(filter);
		if (foundFilterIt != filters.end())
		{
			filters.erase(foundFilterIt);
			filterChangedSlots.erase(filter);
		}
	}*/

	return std::static_pointer_cast<Renderer>(shared_from_this());
}

//Renderer::Ptr
//Renderer::setFilterSurface(Surface::Ptr surface)
//{
//	for (auto& f : _targetDataFilters)
//		f->currentSurface(surface);
//	for (auto& f : _rendererDataFilters)
//		f->currentSurface(surface);
//	for (auto& f : _rootDataFilters)
//		f->currentSurface(surface);
//
//	return std::static_pointer_cast<Renderer>(shared_from_this());
//}

void
Renderer::filterChangedHandler(data::AbstractFilter::Ptr	filter, 
							   data::BindingSource			source,
							   SurfacePtr					surface)
{
	_filterChanged->execute(
		std::static_pointer_cast<Renderer>(shared_from_this()),
		filter,
		source,
		surface
	);
}