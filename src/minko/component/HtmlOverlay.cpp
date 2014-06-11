#if defined(__APPLE__)
# include "TargetConditionals.h"
#endif

#if defined(CHROMIUM)
# include "chromium/dom/ChromiumDOMEngine.hpp"
#endif
#if defined(EMSCRIPTEN)
# include "emscripten/dom/EmscriptenDOMEngine.hpp"
#endif
#if defined(TARGET_IPHONE_SIMULATOR) or defined(TARGET_OS_IPHONE) // iOS
# include "ioswebview/dom/IOSWebViewDOMEngine.hpp"
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

#if defined(TARGET_IPHONE_SIMULATOR) or defined(TARGET_OS_IPHONE) // iOS
using namespace ioswebview;
using namespace ioswebview::dom;
#endif

HtmlOverlay::Ptr HtmlOverlay::_instance = nullptr;

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
    
#if defined(TARGET_IPHONE_SIMULATOR) or defined(TARGET_OS_IPHONE) // iOS
    IOSWebViewDOMEngine::Ptr engine = IOSWebViewDOMEngine::create();
	_domEngine = engine;
#endif
}

HtmlOverlay::~HtmlOverlay()
{
	clear();
}

void
HtmlOverlay::initialize(AbstractCanvas::Ptr canvas, SceneManager::Ptr sceneManager)
{
	_canvas = canvas;
	_sceneManager = sceneManager;

	_targetAddedSlot = targetAdded()->connect(std::bind(
		&HtmlOverlay::targetAddedHandler,
		std::static_pointer_cast<HtmlOverlay>(shared_from_this()),
		std::placeholders::_1,
		std::placeholders::_2
		));

	_targetRemovedSlot = targetRemoved()->connect(std::bind(
		&HtmlOverlay::targetRemovedHandler,
		std::static_pointer_cast<HtmlOverlay>(shared_from_this()),
		std::placeholders::_1,
		std::placeholders::_2
		));
}

void
HtmlOverlay::targetAddedHandler(AbstractComponent::Ptr	ctrl, scene::Node::Ptr		target)
{
#if defined(CHROMIUM)
	ChromiumDOMEngine::Ptr engine = std::dynamic_pointer_cast<ChromiumDOMEngine>(_domEngine);
	engine->initialize(_canvas, _sceneManager, target);
#endif
	
#if defined(EMSCRIPTEN)
	EmscriptenDOMEngine::Ptr engine = std::dynamic_pointer_cast<EmscriptenDOMEngine>(_domEngine);
	engine->initialize(_canvas, _sceneManager);
#endif
    
#if defined(TARGET_IPHONE_SIMULATOR) or defined(TARGET_OS_IPHONE) // iOS
    IOSWebViewDOMEngine::Ptr engine = std::dynamic_pointer_cast<IOSWebViewDOMEngine>(_domEngine);
	engine->initialize(_canvas, _sceneManager);
#endif
}

void
HtmlOverlay::targetRemovedHandler(AbstractComponent::Ptr	ctrl, scene::Node::Ptr		target)
{
#if defined(CHROMIUM)
	ChromiumDOMEngine::Ptr engine = std::dynamic_pointer_cast<ChromiumDOMEngine>(_domEngine);
	engine->remove();
#endif
}

bool
HtmlOverlay::visible()
{
	_domEngine->visible();
}

void
HtmlOverlay::visible(bool value)
{
	_domEngine->visible(value);
}

