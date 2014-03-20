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
	_renderingBegin(Signal<Ptr>::create()),
	_renderingEnd(Signal<Ptr>::create()),
	_beforePresent(Signal<Ptr>::create()),
	_surfaceDrawCalls(),
	_surfaceTechniqueChangedSlot(),
	_effect(effect),
	_priority(priority)
{
	if (renderTarget)
	{
		renderTarget->upload();
		_renderTarget = renderTarget;
	}
}

void
Renderer::initialize()
{
	_drawCallPool = DrawCallPool::create(shared_from_this());

	_targetAddedSlot = targetAdded()->connect(std::bind(
		&Renderer::targetAddedHandler,
		shared_from_this(),
		std::placeholders::_1,
		std::placeholders::_2
	));	

	_targetRemovedSlot = targetRemoved()->connect(std::bind(
		&Renderer::targetRemovedHandler,
		shared_from_this(),
		std::placeholders::_1,
		std::placeholders::_2
	));
}

void
Renderer::targetAddedHandler(std::shared_ptr<AbstractComponent> ctrl,
							 std::shared_ptr<Node> 			target)
{
	if (target->components<Renderer>().size() > 1)
		throw std::logic_error("There cannot be two Renderer on the same node.");

	_addedSlot = target->added()->connect(std::bind(
		&Renderer::addedHandler,
		shared_from_this(),
		std::placeholders::_1,
		std::placeholders::_2,
		std::placeholders::_3
	));

	_removedSlot = target->removed()->connect(std::bind(
		&Renderer::removedHandler,
		shared_from_this(),
		std::placeholders::_1,
		std::placeholders::_2,
		std::placeholders::_3
	));

	addedHandler(target->root(), target, target->parent());
}

void
Renderer::targetRemovedHandler(std::shared_ptr<AbstractComponent> 	ctrl,
							    std::shared_ptr<Node> 				target)
{
	_addedSlot = nullptr;
	_removedSlot = nullptr;

	removedHandler(target->root(), target, target->parent());
}

void
Renderer::addedHandler(std::shared_ptr<Node> node,
						std::shared_ptr<Node> target,
						std::shared_ptr<Node> parent)
{
	findSceneManager();

	_rootDescendantAddedSlot = target->root()->added()->connect(std::bind(
		&Renderer::rootDescendantAddedHandler,
		shared_from_this(),
		std::placeholders::_1,
		std::placeholders::_2,
		std::placeholders::_3
	));

	_rootDescendantRemovedSlot = target->root()->removed()->connect(std::bind(
		&Renderer::rootDescendantRemovedHandler,
		shared_from_this(),
		std::placeholders::_1,
		std::placeholders::_2,
		std::placeholders::_3
	));

	_componentAddedSlot = target->root()->componentAdded()->connect(std::bind(
		&Renderer::componentAddedHandler,
		shared_from_this(),
		std::placeholders::_1,
		std::placeholders::_2,
		std::placeholders::_3
	));

	_componentRemovedSlot = target->root()->componentRemoved()->connect(std::bind(
		&Renderer::componentRemovedHandler,
		shared_from_this(),
		std::placeholders::_1,
		std::placeholders::_2,
		std::placeholders::_3
	), 10.f);

	rootDescendantAddedHandler(nullptr, target->root(), nullptr);
}

void
Renderer::removedHandler(std::shared_ptr<Node> node,
						  std::shared_ptr<Node> target,
						  std::shared_ptr<Node> parent)
{
	findSceneManager();

	_rootDescendantAddedSlot = nullptr;
	_rootDescendantRemovedSlot = nullptr;
	_componentAddedSlot = nullptr;
	_componentRemovedSlot = nullptr;

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
			addSurface(surface);
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
		addSurface(surfaceCtrl);
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
	_drawCallPool->addSurface(surface);
}

void
Renderer::removeSurface(Surface::Ptr surface)
{
	_drawCallPool->removeSurface(surface);
}

void
Renderer::render(render::AbstractContext::Ptr	context, 
				 render::AbstractTexture::Ptr	renderTarget)
{
	_drawCalls = _drawCallPool->drawCalls();
	
	_renderingBegin->execute(shared_from_this());

	if (!renderTarget)
		renderTarget = _renderTarget;
        
        bool bCustomViewport= false;
        if ( _viewportBox.width>=0 && _viewportBox.height>=0 )
        {
            bCustomViewport= true;
            context->configureViewport( _viewportBox.x,  _viewportBox.y,  _viewportBox.width,_viewportBox.height );
            context->setScissorTest( true, _viewportBox );
        }

	context->clear(
		((_backgroundColor >> 24) & 0xff) / 255.f,
		((_backgroundColor >> 16) & 0xff) / 255.f,
		((_backgroundColor >> 8) & 0xff) / 255.f,
		(_backgroundColor & 0xff) / 255.f
	);


	for (auto& drawCall : _drawCalls)
		drawCall->render(context, renderTarget);

        if ( bCustomViewport )
        {
            context->setScissorTest( false, _viewportBox  );
        }

	_beforePresent->execute(shared_from_this());

	context->present();

	_renderingEnd->execute(shared_from_this());
}

void
Renderer::findSceneManager()
{
	NodeSet::Ptr roots = NodeSet::create(targets())
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
			_renderingBeginSlot = _sceneManager->renderingBegin()->connect(std::bind(
				&Renderer::sceneManagerRenderingBeginHandler,
				shared_from_this(),
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