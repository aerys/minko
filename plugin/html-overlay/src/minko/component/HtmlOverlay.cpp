#if defined(__APPLE__)
# include "TargetConditionals.h"
# include "macwebview/dom/MacWebViewDOMEngine.hpp"
//# if TARGET_IPHONE_SIMULATOR || TARGET_OS_IPHONE // iOS
//#  include "ioswebview/dom/IOSWebViewDOMEngine.hpp"
//# elif TARGET_OS_MAC // OSX
//#  include "osxwebview/dom/OSXWebViewDOMEngine.hpp"
//# endif
#elif defined(__ANDROID__)
# include "android/dom/AndroidWebViewDOMEngine.hpp"
#elif defined(CHROMIUM)
# include "chromium/dom/ChromiumDOMEngine.hpp"
#elif defined(EMSCRIPTEN)
# include "emscripten/dom/EmscriptenDOMEngine.hpp"
#endif

#include "minko/component/HtmlOverlay.hpp"
#include "minko/scene/Node.hpp"

using namespace minko;
using namespace minko::component;

#if defined(__APPLE__)
# include "TargetConditionals.h"
using namespace macwebview;
using namespace macwebview::dom;
//# if TARGET_IPHONE_SIMULATOR || TARGET_OS_IPHONE // iOS
//using namespace ioswebview;
//using namespace ioswebview::dom;
//# elif TARGET_OS_MAC // OSX
//using namespace osxwebview;
//using namespace osxwebview::dom;
//# endif
#elif defined(__ANDROID__)
using namespace android;
using namespace android::dom;
#elif defined(CHROMIUM)
using namespace chromium;
using namespace chromium::dom;
#elif defined(EMSCRIPTEN)
using namespace emscripten;
using namespace emscripten::dom;
#endif

HtmlOverlay::Ptr HtmlOverlay::_instance = nullptr;

HtmlOverlay::HtmlOverlay(int argc, char** argv) :
	AbstractComponent(),
	_cleared(false)
{
#if defined(__APPLE__)
# include "TargetConditionals.h"
    MacWebViewDOMEngine::Ptr engine = MacWebViewDOMEngine::create();
    _domEngine = engine;
//# if TARGET_IPHONE_SIMULATOR || TARGET_OS_IPHONE // iOS
//    IOSWebViewDOMEngine::Ptr engine = IOSWebViewDOMEngine::create();
//    _domEngine = engine;
//# elif TARGET_OS_MAC // OSX
//    OSXWebViewDOMEngine::Ptr engine = OSXWebViewDOMEngine::create();
//    _domEngine = engine;
//# endif
#elif defined(__ANDROID__)
    AndroidWebViewDOMEngine::Ptr engine = AndroidWebViewDOMEngine::create();
    _domEngine = engine;
#elif defined(CHROMIUM)
    ChromiumDOMEngine::Ptr engine = ChromiumDOMEngine::create(argc, argv);
    _domEngine = engine;
#elif defined(EMSCRIPTEN)
    EmscriptenDOMEngine::Ptr engine = EmscriptenDOMEngine::create();
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
HtmlOverlay::targetAddedHandler(AbstractComponent::Ptr ctrl, scene::Node::Ptr target)
{
#if defined(__APPLE__)
# include "TargetConditionals.h"
    MacWebViewDOMEngine::Ptr engine = std::dynamic_pointer_cast<MacWebViewDOMEngine>(_domEngine);
    engine->initialize(_canvas, _sceneManager);
//# if TARGET_IPHONE_SIMULATOR || TARGET_OS_IPHONE // iOS
//    IOSWebViewDOMEngine::Ptr engine = std::dynamic_pointer_cast<IOSWebViewDOMEngine>(_domEngine);
//    engine->initialize(_canvas, _sceneManager);
//# elif TARGET_OS_MAC // OSX
//    OSXWebViewDOMEngine::Ptr engine = std::dynamic_pointer_cast<OSXWebViewDOMEngine>(_domEngine);
//    engine->initialize(_canvas, _sceneManager);
//# endif
#elif defined(__ANDROID__)
    AndroidWebViewDOMEngine::Ptr engine = std::dynamic_pointer_cast<AndroidWebViewDOMEngine>(_domEngine);
    engine->initialize(_canvas, _sceneManager);
#elif defined(CHROMIUM)
    ChromiumDOMEngine::Ptr engine = std::dynamic_pointer_cast<ChromiumDOMEngine>(_domEngine);
    engine->initialize(_canvas, _sceneManager, target);
#elif defined(EMSCRIPTEN)
    EmscriptenDOMEngine::Ptr engine = std::dynamic_pointer_cast<EmscriptenDOMEngine>(_domEngine);
    engine->initialize(_canvas, _sceneManager);
#endif
}

void
HtmlOverlay::targetRemovedHandler(AbstractComponent::Ptr ctrl, scene::Node::Ptr	target)
{
#if defined(CHROMIUM)
	ChromiumDOMEngine::Ptr engine = std::dynamic_pointer_cast<ChromiumDOMEngine>(_domEngine);
	engine->remove();
#endif
}

bool
HtmlOverlay::visible()
{
	return _domEngine->visible();
}

void
HtmlOverlay::visible(bool value)
{
	_domEngine->visible(value);
}
