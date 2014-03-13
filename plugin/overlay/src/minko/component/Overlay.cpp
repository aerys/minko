#ifndef EMSCRIPTEN
#define CHROMIUM 1
#endif


#if defined(CHROMIUM)
#include "chromium/dom/ChromiumDOMEngine.hpp"
#endif
#if defined(EMSCRIPTEN)
#include "emscripten/dom/EmscriptenDOMEngine.hpp"
#endif
#include "minko/component/Overlay.hpp"
#include "minko/scene/Node.hpp"


using namespace minko;
using namespace minko::component;

#if defined(CHROMIUM)
using namespace chromium;
using namespace chromium::dom;
#endif

#if defined(EMSCRIPTEN)
using namespace emscripten;
using namespace emscripten::dom;
#endif

Overlay::Overlay() :
	AbstractComponent(),
	_cleared(false)
{
}

Overlay::~Overlay()
{
	clear();
}

void
Overlay::initialize(AbstractCanvas::Ptr canvas)
{
	_canvas = canvas;

	_targetAddedSlot = targetAdded()->connect(std::bind(
		&Overlay::targetAddedHandler,
		shared_from_this(),
		std::placeholders::_1,
		std::placeholders::_2
		));

	_targetRemovedSlot = targetRemoved()->connect(std::bind(
		&Overlay::targetRemovedHandler,
		shared_from_this(),
		std::placeholders::_1,
		std::placeholders::_2
		));


#if defined(CHROMIUM)
	ChromiumDOMEngine::Ptr engine = ChromiumDOMEngine::create();
	_domEngine = engine;
#endif

#if defined(EMSCRIPTEN)
	EmscriptenDOMEngine::Ptr engine = EmscriptenDOMEngine::create();
	_domEngine = engine;
#endif
}

void
Overlay::targetAddedHandler(AbstractComponent::Ptr	ctrl, scene::Node::Ptr		target)
{
#if defined(CHROMIUM)
	if (target->hasComponent<SceneManager>())
	{
		SceneManager::Ptr sceneManager = target->component<SceneManager>();
		
		ChromiumDOMEngine::Ptr engine = std::dynamic_pointer_cast<ChromiumDOMEngine>(_domEngine);
		engine->initialize(_canvas, sceneManager);
	}
#endif
	
#if defined(EMSCRIPTEN)
	if (target->hasComponent<SceneManager>())
	{
		SceneManager::Ptr sceneManager = target->component<SceneManager>();
		
		EmscriptenDOMEngine::Ptr engine = std::dynamic_pointer_cast<EmscriptenDOMEngine>(_domEngine);
		engine->initialize(_canvas, sceneManager);
	}
#endif
}

void
Overlay::targetRemovedHandler(AbstractComponent::Ptr	ctrl, scene::Node::Ptr		target)
{
	clear();
}

