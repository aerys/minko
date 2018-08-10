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

/*
#include "minko/net/HTTPOptions.hpp"
#include "minko/net/HTTPRequest.hpp"
*/
#include "ClippingPlane.hpp"
#include "ClippingPlaneScript.hpp"
//#include "SymbolScript.hpp"
//#include "player/util/String.hpp"

using namespace minko;
using namespace minko::component;
using namespace minko::data;
using namespace minko::file;
using namespace minko::input;
//using namespace minko::net;
using namespace minko::scene;

using namespace player;
using namespace player::component;

ClippingPlaneScript::ClippingPlaneScript() :
    AbstractScript(),
    _visibleClippingPlane(-1),
    _editionIsActive(false),
    _planeTransforms({ math::mat4() }),
    _activePlane(0)
{
}

void
ClippingPlaneScript::start(NodePtr target)
{
    // add a default plane transform
    _planeTransforms.push_back(math::mat4());

    _root = target->root();
    _sceneManager = _root->component<SceneManager>();
    _canvas = _sceneManager->canvas();

#ifdef MINKO_PLUGIN_HTML_OVERLAY
    _overlay = HtmlOverlay::instance();
    
    _overlayMessageSlot = _overlay->onmessage()->connect(std::bind(
        &ClippingPlaneScript::htmlOverlayMessageHandler,
        std::static_pointer_cast<ClippingPlaneScript>(shared_from_this()),
        std::placeholders::_1,
        std::placeholders::_2
    ));
#endif

    auto cameraNode = Node::Ptr();

    auto cameraNodes = NodeSet::create(_root)
        ->descendants(true)
        ->where([](Node::Ptr descendant) -> bool
        {
            return descendant->hasComponent<Camera>();
        });

    if (!cameraNodes->nodes().empty())
        cameraNode = cameraNodes->nodes().front();

    if (cameraNode != _cameraNode)
    {
        _cameraNode = cameraNode;
    }

    // fixme
    // handle multiple planes

    if (_activeClippingPlaneNode == nullptr)
    {
        auto targetChildren = NodeSet::create(target)
            ->children(true)
            ->where([](Node::Ptr child) -> bool { return child->hasComponent<ClippingPlane>(); });

        if (!targetChildren->nodes().empty())
            _activeClippingPlaneNode = targetChildren->nodes().front();

        _activeClippingPlaneNode->component<ClippingPlane>()->enable(true);
    }
}

#ifdef MINKO_PLUGIN_HTML_OVERLAY
void
ClippingPlaneScript::htmlOverlayMessageHandler(AbstractDOMPtr dom, std::string message)
{
    auto messageParts = String::split(message, ";");

    if (messageParts[0] == "activeClippingPlane")
    {
        _activePlane = std::atoi(messageParts[1].c_str());

        showClippingPlane(_activePlane);
    }
    else if (messageParts[0] == "addClippingPlane")
    {
        _planeTransforms.push_back(math::mat4());
    }
    else if (messageParts[0] == "showClippingPlane")
    {
        showClippingPlane(_activePlane);
    }
    else if (messageParts[0] == "hideClippingPlane")
    {
        if (_visibleClippingPlane != -1)
            hideClippingPlane();
    }
    else if (messageParts[0] == "editClippingPlane")
    {
        if (_activePlane != -1)
        {
            if (_visibleClippingPlane == -1)
                showClippingPlane(_activePlane);

            if (!_editionIsActive)
                beginEdition();
        }
    }
}
#endif

void
ClippingPlaneScript::update(NodePtr target)
{
}

void
ClippingPlaneScript::stop(NodePtr target)
{
    _root = nullptr;
    _canvas = nullptr;
    _sceneManager = nullptr;

    _leftButtonClickSlot = nullptr;

#ifdef MINKO_PLUGIN_HTML_OVERLAY
    _overlay = nullptr;
    _overlayMessageSlot = nullptr;
#endif
}

void
ClippingPlaneScript::leftButtonClickHandler()
{
    if (_editionIsActive)
        endEdition();
}

void
ClippingPlaneScript::setClippingPlaneEffect()
{
    _activeClippingPlaneNode->component<ClippingPlane>()->enable(true);
/*
    auto symbolScript = target()->component<SymbolScript>();

    symbolScript->changeRenderMode("effect/Section/ClippingPlane.effect");
*/

    // TODO
}

void
ClippingPlaneScript::unsetClippingPlaneEffect()
{
/*
    auto symbolScript = target()->component<SymbolScript>();
    symbolScript->changeRenderMode("phong");

*/
    _activeClippingPlaneNode->component<ClippingPlane>()->enable(false);

    // TODO
}

void
ClippingPlaneScript::showClippingPlane(int id)
{
    if (_activeClippingPlaneNode == nullptr)
        return;

    _activeClippingPlaneNode->component<ClippingPlane>()->originNode()->component<Transform>()->matrix(
        _planeTransforms[_activePlane]
    );

    if (_visibleClippingPlane == -1)
        setClippingPlaneEffect();

    _visibleClippingPlane = id;
}

void
ClippingPlaneScript::hideClippingPlane()
{
    if (_editionIsActive)
    {
        _activeClippingPlaneNode->component<ClippingPlane>()->originNode()->component<Transform>()->matrix(
            _planeTransforms[_activePlane]
        );

        endEdition();
    }

    unsetClippingPlaneEffect();

    _visibleClippingPlane = -1;
}

void
ClippingPlaneScript::cameraModelToWorldChanged(const math::mat4& matrix)
{
    auto clippingPlane = _activeClippingPlaneNode->component<ClippingPlane>();
    auto planeNode = clippingPlane->originNode();

    const auto cameraPosition = math::vec3(matrix[3]);

    const auto cameraBasePosition = math::vec3(_editionCameraBaseTransform[3]);
    const auto planeBasePosition = math::vec3(_planeTransforms[_activePlane][3]);

    const auto cameraBaseDistance = math::distance(planeBasePosition, cameraBasePosition);
    const auto cameraDistance = math::distance(planeBasePosition, cameraPosition);

    const auto planePosition = planeBasePosition + (cameraDistance - cameraBaseDistance) * math::normalize(cameraPosition);

    planeNode->component<Transform>()->matrix(
        math::inverse(math::lookAt(planePosition, cameraPosition, math::vec3(0.f, 1.f, 0.f)))
    );
}

void
ClippingPlaneScript::beginEdition()
{
    if (_activeClippingPlaneNode == nullptr)
        return;

    _editionIsActive = true;

    auto clippingPlane = _activeClippingPlaneNode->component<ClippingPlane>();
    auto planeNode = clippingPlane->originNode();

    _planeTransforms[_activePlane] = planeNode->component<Transform>()->modelToWorldMatrix(true);
    _editionCameraBaseTransform = _cameraNode->component<Transform>()->modelToWorldMatrix(true);

    _cameraModelToWorldChangedSlot = _cameraNode->data().propertyChanged("modelToWorldMatrix").connect(
        [this](Store&                           store,
                Provider::Ptr                    provider,
                const Provider::PropertyName&    propertyName)
        {
            cameraModelToWorldChanged(provider->get<math::mat4>(propertyName));
        }
    );

    cameraModelToWorldChanged(_cameraNode->data().get<math::mat4>("modelToWorldMatrix"));

    _leftButtonClickSlot = _canvas->mouse()->leftButtonClick()->connect([this](Mouse::Ptr mouse) -> void
    {
        leftButtonClickHandler();
    }, 1.f);
}

void
ClippingPlaneScript::endEdition()
{
    _planeTransforms[_activePlane] = _activeClippingPlaneNode->component<ClippingPlane>()->originNode()->component<Transform>()->matrix();

    _editionIsActive = false;

    _cameraModelToWorldChangedSlot = nullptr;
}
