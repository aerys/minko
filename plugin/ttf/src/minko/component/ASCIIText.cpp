#include "minko/component/ASCIIText.hpp"
#include "minko/component/SceneManager.hpp"
#include "minko/component/Surface.hpp"
#include "minko/file/AssetLibrary.hpp"
#include "minko/file/Loader.hpp"
#include "minko/file/Options.hpp"
#include "minko/geometry/TextGeometry.hpp"
#include "minko/render/AbstractContext.hpp"
#include "minko/render/AbstractTexture.hpp"
#include "minko/scene/Node.hpp"

using namespace minko;
using namespace minko::component;

ASCIIText::Ptr
ASCIIText::text(const std::string& text)
{
    if (_text == text)
        return std::static_pointer_cast<ASCIIText>(shared_from_this());

    _text = text;

    rebuildGeometry();

    return std::static_pointer_cast<ASCIIText>(shared_from_this());
}

void
ASCIIText::targetAdded(scene::Node::Ptr target)
{
    AbstractComponent::targetAdded(target);

    rebuildGeometry();
}

void
ASCIIText::targetRemoved(scene::Node::Ptr target)
{
    AbstractComponent::targetRemoved(target);

    if (_textSurface && target->hasComponent(_textSurface))
        target->removeComponent(_textSurface);
}

void
ASCIIText::rebuildGeometry()
{
    auto target = this->target();

    if (!target)
        return;

    if (_textSurface && target->hasComponent(_textSurface))
        target->removeComponent(_textSurface);

    auto context = _atlas->context();

    auto geometry = geometry::TextGeometry::create(context)
        ->setText(_atlas, _text, 1.f, true);

    _textSurface = Surface::create(
        geometry,
        _material,
        _effect
    );

    target->addComponent(_textSurface);
}
