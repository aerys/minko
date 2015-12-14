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

#include "minko/AbstractCanvas.hpp"
#include "minko/scene/Node.hpp"
#include "minko/scene/NodeSet.hpp"
#include "minko/component/Surface.hpp"
#include "minko/render/DrawCall.hpp"
#include "minko/render/Effect.hpp"
#include "minko/render/Pass.hpp"
#include "minko/render/AbstractTexture.hpp"
#include "minko/render/AbstractContext.hpp"
#include "minko/component/SceneManager.hpp"
#include "minko/component/PerspectiveCamera.hpp"
#include "minko/file/AssetLibrary.hpp"
#include "minko/render/DrawCallPool.hpp"
#include "minko/data/AbstractFilter.hpp"
#include "minko/geometry/Geometry.hpp"
#include "minko/material/Material.hpp"

using namespace minko;
using namespace minko::component;
using namespace minko::scene;
using namespace minko::render;

Renderer::Renderer(std::shared_ptr<render::AbstractTexture> renderTarget,
				   EffectPtr								effect,
				   const std::string&						effectTechnique,
				   float									priority) :
    AbstractComponent(scene::BuiltinLayout::DEFAULT),
    _backgroundColor(0),
    _viewportBox(0, 0, -1, -1),
	_scissorBox(0, 0, -1, -1),
	_enabled(true),
    _mustZSort(true),
	_renderingBegin(Signal<Ptr>::create()),
	_renderingEnd(Signal<Ptr>::create()),
	_beforePresent(Signal<Ptr>::create()),
	//_surfaceTechniqueChangedSlot(),
	_effect(effect),
	_effectTechnique(effectTechnique),
    _clearBeforeRender(true),
	_priority(priority),
	_renderTarget(renderTarget),
	_postProcessingGeom(nullptr),
	/*_targetDataFilters(),
	_rendererDataFilters(),
	_rootDataFilters(),
	_targetDataFilterChangedSlots(),
	_rendererDataFilterChangedSlots(),
	_rootDataFilterChangedSlots(),
	_lightMaskFilter(data::LightMaskFilter::create()),*/
	_filterChanged(Signal<Ptr, data::AbstractFilter::Ptr, data::Binding::Source, SurfacePtr>::create()),
	_numDrawCalls(0),
	_numTriangles(0)
{
}

// TODO #Clone
/*
Renderer::Renderer(const Renderer& renderer, const CloneOption& option) :
	_backgroundColor(renderer._backgroundColor),
	_viewportBox(),
	_scissorBox(),
	_enabled(renderer._enabled),
	_renderingBegin(Signal<Ptr>::create()),
	_renderingEnd(Signal<Ptr>::create()),
	_beforePresent(Signal<Ptr>::create()),
	_surfaceDrawCalls(),
	_surfaceTechniqueChangedSlot(),
	_effect(nullptr),
    _clearBeforeRender(true),
	_priority(renderer._priority),
	_targetDataFilters(),
	_rendererDataFilters(),
	_rootDataFilters(),
	_targetDataFilterChangedSlots(),
	_rendererDataFilterChangedSlots(),
	_rootDataFilterChangedSlots(),
	_lightMaskFilter(data::LightMaskFilter::create()),
	_filterChanged(Signal<Ptr, data::AbstractFilter::Ptr, data::BindingSource, SurfacePtr>::create())
{
	if (renderer._renderTarget)
	{
		renderer._renderTarget->upload();
		_renderTarget = renderer._renderTarget;
	}
}

AbstractComponent::Ptr
Renderer::clone(const CloneOption& option)
{
	auto renderer = std::shared_ptr<Renderer>(new Renderer(*this, option));

	return renderer;
}
*/

void
Renderer::reset()
{
    _toCollect.clear();
    _surfaceToDrawCallIterator.clear();
    _drawCallPool.clear();
}

void
Renderer::initializePostProcessingGeometry()
{
	auto context = _sceneManager->assets()->context();
	auto vb = render::VertexBuffer::create(context, {
		-1.f, 	1.f, 	0.f,	1.f,
		-1.f, 	-1.f, 	0.f, 	0.f,
		1.f, 	1.f, 	1.f, 	1.f,
		1.f, 	1.f, 	1.f, 	1.f,
		-1.f, 	-1.f, 	0.f, 	0.f,
		1.f, 	-1.f, 	1.f, 	0.f,
	});
	vb->addAttribute("position", 2);
	vb->addAttribute("uv", 2, 2);

	auto p = data::Provider::create();
	p->set("postProcessingPosition", vb->attribute("position"));
	p->set("postProcessingUV", vb->attribute("uv"));

	_postProcessingGeom = geometry::Geometry::create();
	_postProcessingGeom->addVertexBuffer(vb);
	// _postProcessingGeom->indices(render::IndexBuffer::create(context, { 0, 2, 1, 1, 2, 3 }));

	target()->data().addProvider(p);
}

void
Renderer::targetAdded(std::shared_ptr<Node> target)
{
    // Comment due to reflection component
	//if (target->components<Renderer>().size() > 1)
	//	throw std::logic_error("There cannot be two Renderer on the same node.");

	if (_effect)
		target->data().addProvider(_effect->data(), Surface::EFFECT_COLLECTION_NAME);

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
	_renderingBeginSlot = nullptr;
	_surfaceChangedSlots.clear();

	_rootDescendantAddedSlot = nullptr;
	_rootDescendantRemovedSlot = nullptr;
	_componentAddedSlot = nullptr;
	_componentRemovedSlot = nullptr;

	_drawCallPool.clear();

	if (_effect)
		target->data().removeProvider(_effect->data(), Surface::EFFECT_COLLECTION_NAME);
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
	), std::numeric_limits<float>::max());

	_rootDescendantRemovedSlot = target->root()->removed().connect(std::bind(
		&Renderer::rootDescendantRemovedHandler,
		std::static_pointer_cast<Renderer>(shared_from_this()),
		std::placeholders::_1,
		std::placeholders::_2,
		std::placeholders::_3
	), std::numeric_limits<float>::max());

	_componentAddedSlot = target->root()->componentAdded().connect(std::bind(
		&Renderer::componentAddedHandler,
		std::static_pointer_cast<Renderer>(shared_from_this()),
		std::placeholders::_1,
		std::placeholders::_2,
		std::placeholders::_3
	), std::numeric_limits<float>::max());

	_componentRemovedSlot = target->root()->componentRemoved().connect(std::bind(
		&Renderer::componentRemovedHandler,
		std::static_pointer_cast<Renderer>(shared_from_this()),
		std::placeholders::_1,
		std::placeholders::_2,
		std::placeholders::_3
	), std::numeric_limits<float>::max());

	//_lightMaskFilter->root(target->root());
    reset();

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
Renderer::rootDescendantRemovedHandler(std::shared_ptr<Node> 	node,
									   std::shared_ptr<Node> 	target,
									   std::shared_ptr<Node> 	parent)
{
	auto surfaceNodes = NodeSet::create(target)
		->descendants(true)
        ->where([](scene::Node::Ptr node)
        {
            return node->hasComponent<Surface>();
        });

	for (auto surfaceNode : surfaceNodes->nodes())
		for (auto surface : surfaceNode->components<Surface>())
		{
			unwatchSurface(surface, surfaceNode);
			removeSurface(surface);
		}
}

void
Renderer::componentAddedHandler(std::shared_ptr<Node>				node,
								std::shared_ptr<Node>				target,
								std::shared_ptr<AbstractComponent>	ctrl)
{
	auto surfaceCtrl = std::dynamic_pointer_cast<Surface>(ctrl);
    auto sceneManager = std::dynamic_pointer_cast<SceneManager>(ctrl);
    auto perspectiveCamera = std::dynamic_pointer_cast<PerspectiveCamera>(ctrl);

	if (surfaceCtrl)
        _toCollect.insert(surfaceCtrl);
	else if (sceneManager)
		setSceneManager(sceneManager);
    else if (perspectiveCamera)
    {
        _worldToScreenMatrixPropertyChangedSlot = perspectiveCamera->target()->data().propertyChanged("worldToScreenMatrix").connect(
            [&](data::Store&, data::Provider::Ptr, const data::Provider::PropertyName&)
            {
                _mustZSort = true;
            }
        );
    }
}

void
Renderer::componentRemovedHandler(std::shared_ptr<Node>					node,
								  std::shared_ptr<Node>					target,
								  std::shared_ptr<AbstractComponent>	cmp)
{
	auto surface = std::dynamic_pointer_cast<Surface>(cmp);
	auto sceneManager = std::dynamic_pointer_cast<SceneManager>(cmp);
    auto perspectiveCamera = std::dynamic_pointer_cast<PerspectiveCamera>(cmp);

	if (surface)
	{
		unwatchSurface(surface, target);
		removeSurface(surface);
	}
    else if (sceneManager)
        setSceneManager(nullptr);
    else if (perspectiveCamera)
        _worldToScreenMatrixPropertyChangedSlot = nullptr;
}

void
Renderer::addSurface(Surface::Ptr surface)
{
	if (_surfaceToDrawCallIterator.count(surface) != 0)
		throw std::invalid_argument("surface");

	if (!checkSurfaceLayout(surface))
		return;

    render::EffectVariables variables = _variables;

	variables.push_back({ "surfaceUuid", surface->uuid() });
	variables.push_back({ "geometryUuid", surface->geometry()->uuid() });
	variables.push_back({ "materialUuid", surface->material()->uuid() });
	variables.push_back({ "effectUuid", _effect ? _effect->uuid() : surface->effect()->uuid() });

    _surfaceToDrawCallIterator[surface] = _drawCallPool.addDrawCalls(
        _effect ? _effect : surface->effect(),
        _effect ? _effectTechnique : surface->technique(),
        variables,
        surface->target()->root()->data(),
        target()->data(),
        surface->target()->data()
    );

    auto callback = std::bind(
        &Renderer::surfaceGeometryOrMaterialChangedHandler,
        std::static_pointer_cast<Renderer>(shared_from_this()),
        surface
    );

    _surfaceChangedSlots[surface].push_back(surface->geometryChanged().connect(callback));
    _surfaceChangedSlots[surface].push_back(surface->materialChanged().connect(callback));
    _surfaceChangedSlots[surface].push_back(surface->effectChanged().connect(std::bind(
        &Renderer::surfaceEffectChangedHandler,
        std::static_pointer_cast<Renderer>(shared_from_this()),
        surface
    )));
}

void
Renderer::removeSurface(Surface::Ptr surface)
{
    if (_toCollect.erase(surface) == 0)
    {
		if (_surfaceToDrawCallIterator.count(surface) != 0)
		{
	        _drawCallPool.removeDrawCalls(_surfaceToDrawCallIterator[surface]);
	        _surfaceToDrawCallIterator.erase(surface);
	        _surfaceChangedSlots.erase(surface);
		}
    }
}

void
Renderer::surfaceGeometryOrMaterialChangedHandler(Surface::Ptr surface)
{
    // The surface's material, geometry or effect is different
    // we completely remove the surface and re-add it again because
    // it's way simpler than just updating what has changed.

    render::EffectVariables variables = _variables;
    variables.push_back({ "surfaceUuid", surface->uuid() });
	variables.push_back({ "geometryUuid", surface->geometry()->uuid() });
	variables.push_back({ "materialUuid", surface->material()->uuid() });
	variables.push_back({ "effectUuid", _effect ? _effect->uuid() : surface->effect()->uuid() });

    _drawCallPool.invalidateDrawCalls(_surfaceToDrawCallIterator[surface], variables);
    //removeSurface(surface);
    //_toCollect.insert(surface);
}

void
Renderer::surfaceEffectChangedHandler(SurfacePtr surface)
{
    removeSurface(surface);
    _toCollect.insert(surface);
}

void
Renderer::render(render::AbstractContext::Ptr	context,
				 render::AbstractTexture::Ptr	renderTarget)
{
    if (!_enabled)
		return;

    const bool forceSort = !_toCollect.empty();

    // some surfaces have been added during the frame and collected
    // in _toCollect: we now have to take them into account to build
    // the corresponding draw calls before rendering
    for (auto& surface : _toCollect)
	{
		watchSurface(surface);
        addSurface(surface);
	}

    _toCollect.clear();

	_renderingBegin->execute(std::static_pointer_cast<Renderer>(shared_from_this()));

	auto rt = _renderTarget ? _renderTarget : renderTarget;

	if (_scissorBox.z >= 0 && _scissorBox.w >= 0)
		context->setScissorTest(true, _scissorBox);
	else
		context->setScissorTest(false, _scissorBox);

	if (rt)
		context->setRenderToTexture(rt->id(), true);
	else
		context->setRenderToBackBuffer();

	if (_viewportBox.z >= 0 && _viewportBox.w >= 0)
		context->configureViewport(_viewportBox.x, _viewportBox.y, _viewportBox.z, _viewportBox.w);

	if (_clearBeforeRender)
	{
		context->clear(
			((_backgroundColor >> 24) & 0xff) / 255.f,
			((_backgroundColor >> 16) & 0xff) / 255.f,
			((_backgroundColor >> 8) & 0xff) / 255.f,
			(_backgroundColor & 0xff) / 255.f
		);
	}

    _drawCallPool.update(forceSort, _mustZSort);

    _mustZSort = false;

    const auto& drawCalls = _drawCallPool.drawCalls();

	_numDrawCalls = 0;
	_numTriangles = 0;
    for (const auto& priorityToDrawCalls : drawCalls)
        for (const auto& drawCalls : priorityToDrawCalls.second)
            for (auto drawCall : drawCalls)
			{
				if (drawCall->enabled())
				{
	                drawCall->render(context, rt, _viewportBox, _backgroundColor);
					++_numDrawCalls;
					_numTriangles += drawCall->numTriangles();
				}
			}

    _beforePresent->execute(std::static_pointer_cast<Renderer>(shared_from_this()));

    context->present();

    _renderingEnd->execute(std::static_pointer_cast<Renderer>(shared_from_this()));
}

void
Renderer::clear(std::shared_ptr<AbstractCanvas> canvas)
{
	auto backgroundColor = math::vec4(
		((_backgroundColor >> 24) & 0xff) / 255.f,
		((_backgroundColor >> 16) & 0xff) / 255.f,
		((_backgroundColor >> 8) & 0xff) / 255.f,
		(_backgroundColor & 0xff) / 255.f
	);

	clear(canvas, backgroundColor);
}

void
Renderer::clear(std::shared_ptr<AbstractCanvas> canvas, const math::vec4& clearColor)
{
	canvas->context()->clear(clearColor.r, clearColor.g, clearColor.b, clearColor.a);
	canvas->swapBuffers();
	canvas->context()->clear(clearColor.r, clearColor.g, clearColor.b, clearColor.a);
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

			initializePostProcessingGeometry();
		}
		else
		{
			_sceneManager = nullptr;
			_renderingBeginSlot = nullptr;

			if (_postProcessingGeom)
			{
				target()->data().removeProvider(_postProcessingGeom->data(), Surface::GEOMETRY_COLLECTION_NAME);
				_postProcessingGeom = nullptr;
			}
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
					data::Binding::Source			source)
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
					   data::Binding::Source			source)
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
							   data::Binding::Source			source,
							   SurfacePtr					surface)
{
	_filterChanged->execute(
		std::static_pointer_cast<Renderer>(shared_from_this()),
		filter,
		source,
		surface
	);
}

void
Renderer::surfaceLayoutMaskChangedHandler(Surface::Ptr surface)
{
    // FIXME
    // Use a _toEnable std::unordered_map<Surface::Ptr, bool>
    // to enable or disable a surface once in a frame and perform it after
    // _toCollect is processed.

	if (checkSurfaceLayout(surface))
	{
        enableDrawCalls(surface, true);

		if (_surfaceToDrawCallIterator.count(surface) == 0)
            _toCollect.insert(surface);
	}
	else
	{
        if ((surface->target()->layout() & scene::BuiltinLayout::HIDDEN) != 0 ||
            (surface->target()->layout() & scene::BuiltinLayout::INSIDE_FRUSTUM) == 0)
        {
            enableDrawCalls(surface, false);
        }
        else
        {
		    if (_surfaceToDrawCallIterator.count(surface) != 0)
	            removeSurface(surface);
        }
	}
}

void
Renderer::watchSurface(SurfacePtr surface)
{
	auto node = surface->target();

	if (_nodeLayoutChangedSlot.count(node) == 0)
	{
		_nodeLayoutChangedSlot[node] = node->layoutChanged().connect(
            [this](Node::Ptr n, Node::Ptr t)
            {
                for (auto surface : t->components<Surface>())
                    surfaceLayoutMaskChangedHandler(surface);
            }
		);
	}

	if (_surfaceLayoutMaskChangedSlot.count(surface) == 0)
	{
		_surfaceLayoutMaskChangedSlot[surface] = surface->layoutMaskChanged().connect(
            [this](AbsCmpPtr s)
            {
                surfaceLayoutMaskChangedHandler(std::static_pointer_cast<Surface>(s));
            }
		);
	}
}

void
Renderer::unwatchSurface(SurfacePtr surface, NodePtr node)
{
	_surfaceLayoutMaskChangedSlot.erase(surface);

	if (!node->hasComponent<Surface>())
		_nodeLayoutChangedSlot.erase(node);
}

bool
Renderer::checkSurfaceLayout(SurfacePtr surface)
{
	return (surface->target()->layout() & surface->layoutMask() & layoutMask()) != 0;
}

void
Renderer::layoutMask(const scene::Layout value)
{
	AbstractComponent::layoutMask(value);

	// completely reset the draw call pool
	if (target() != nullptr)
	{
        _drawCallPool.clear();

        rootDescendantRemovedHandler(nullptr, target()->root(), nullptr);
	}
}

void
Renderer::enableDrawCalls(SurfacePtr surface, bool enabled)
{
    auto drawCallIt = _surfaceToDrawCallIterator.find(surface);

    if (drawCallIt == _surfaceToDrawCallIterator.end())
        return;

    const auto drawCallId = drawCallIt->second;

    for (const auto& priorityToDrawCalls : _drawCallPool.drawCalls())
        for (const auto& drawCalls : priorityToDrawCalls.second)
            for (auto drawCall : drawCalls)
            {
                if (drawCall->batchIDs().size() > 1u)
                    throw std::runtime_error("");

                if (drawCall->batchIDs().front() == drawCallId)
                    drawCall->enabled(enabled);
            }
}
