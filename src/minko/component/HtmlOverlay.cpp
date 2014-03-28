#ifndef EMSCRIPTEN
#define CHROMIUM 1
#endif


#if defined(CHROMIUM)
#include "chromium/dom/ChromiumDOMEngine.hpp"
#endif
#if defined(EMSCRIPTEN)
#include "emscripten/dom/EmscriptenDOMEngine.hpp"
#endif
#include "minko/component/HtmlOverlay.hpp"
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

HtmlOverlay::HtmlOverlay(int argc, char** argv) :
	AbstractComponent(),
	_cleared(false)
{
#if defined(CHROMIUM)
	ChromiumDOMEngine::Ptr engine = ChromiumDOMEngine::create(argc, argv);
	_domEngine = engine;
#endif

#if defined(EMSCRIPTEN)
	EmscriptenDOMEngine::Ptr engine = EmscriptenDOMEngine::create();
	_domEngine = engine;
#endif
}

HtmlOverlay::~HtmlOverlay()
{
	clear();
}

void
HtmlOverlay::initialize(AbstractCanvas::Ptr canvas)
{
	_canvas = canvas;

	_targetAddedSlot = targetAdded()->connect(std::bind(
		&HtmlOverlay::targetAddedHandler,
		shared_from_this(),
		std::placeholders::_1,
		std::placeholders::_2
		));

	_targetRemovedSlot = targetRemoved()->connect(std::bind(
		&HtmlOverlay::targetRemovedHandler,
		shared_from_this(),
		std::placeholders::_1,
		std::placeholders::_2
		));
}

void
HtmlOverlay::targetAddedHandler(AbstractComponent::Ptr	ctrl, scene::Node::Ptr		target)
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
HtmlOverlay::targetRemovedHandler(AbstractComponent::Ptr	ctrl, scene::Node::Ptr		target)
{
	clear();
}

