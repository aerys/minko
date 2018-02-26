#if defined(__APPLE__)
# include "TargetConditionals.h"
# include "apple/dom/AppleWebViewDOMEngine.hpp"
#elif defined(__ANDROID__)
# include "android/dom/AndroidWebViewDOMEngine.hpp"
#elif defined(CHROMIUM)
# include "chromium/dom/ChromiumDOMEngine.hpp"
#elif defined(EMSCRIPTEN)
# include "emscripten/dom/EmscriptenDOMEngine.hpp"
#endif

#include "minko/component/HtmlOverlay.hpp"
#include "minko/scene/Node.hpp"
#include "minko/data/Store.hpp"


using namespace minko;
using namespace minko::component;

#if defined(__APPLE__)
# include "TargetConditionals.h"
using namespace apple;
using namespace apple::dom;
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
	_cleared(false),
    _secure(true)
{
#if defined(__APPLE__)
    AppleWebViewDOMEngine::Ptr engine = AppleWebViewDOMEngine::create();
    _domEngine = engine;
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
HtmlOverlay::targetAdded(scene::Node::Ptr target)
{
    if (target->root() != nullptr && target->root()->hasComponent<SceneManager>())
        _sceneManager = target->root()->component<SceneManager>();
    else if (_sceneManager == nullptr)
        throw std::logic_error("root node should have a SceneManager");
    
    _canvas = _sceneManager->canvas();

#if defined(__APPLE__)
# include "TargetConditionals.h"
    AppleWebViewDOMEngine::Ptr engine = std::dynamic_pointer_cast<AppleWebViewDOMEngine>(_domEngine);
    engine->initialize(_canvas, _sceneManager);
#elif defined(__ANDROID__)
    AndroidWebViewDOMEngine::Ptr engine = std::dynamic_pointer_cast<AndroidWebViewDOMEngine>(_domEngine);
    engine->initialize(_canvas, _sceneManager);
#elif defined(CHROMIUM)
    ChromiumDOMEngine::Ptr engine = std::dynamic_pointer_cast<ChromiumDOMEngine>(_domEngine);
    engine->initialize(_canvas, _sceneManager, target, _secure);
#elif defined(EMSCRIPTEN)
    EmscriptenDOMEngine::Ptr engine = std::dynamic_pointer_cast<EmscriptenDOMEngine>(_domEngine);
    engine->initialize(_canvas, _sceneManager);
#endif
}

void
HtmlOverlay::targetRemoved(scene::Node::Ptr	target)
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

void
HtmlOverlay::enableRenderToTexture(std::shared_ptr<minko::render::AbstractTexture> texture)
{
    _domEngine->enableRenderToTexture(texture);
}

void
HtmlOverlay::disableRenderToTexture()
{
    _domEngine->disableRenderToTexture();
}
