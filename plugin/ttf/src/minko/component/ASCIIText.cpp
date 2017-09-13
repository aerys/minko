#include "minko/component/ASCIIText.hpp"
#include "minko/component/SceneManager.hpp"
#include "minko/component/Surface.hpp"
#include "minko/file/AssetLibrary.hpp"
#include "minko/file/Loader.hpp"
#include "minko/file/Options.hpp"
#include "minko/geometry/TextGeometry.hpp"
#include "minko/material/Material.hpp"
#include "minko/render/AbstractContext.hpp"
#include "minko/render/AbstractTexture.hpp"
#include "minko/render/Priority.hpp"
#include "minko/render/TriangleCulling.hpp"
#include "minko/scene/Node.hpp"

using namespace minko;
using namespace minko::component;

math::vec2
ASCIIText::textSize() const
{
    if (!_textSurface)
        return math::vec2();

    return std::static_pointer_cast<geometry::TextGeometry>(_textSurface->geometry())->textSize();
}

ASCIIText::Ptr
ASCIIText::text(const std::string& text)
{
    if (_text == text)
        return std::static_pointer_cast<ASCIIText>(shared_from_this());

    _text = text;

    rebuildGeometry();

    return std::static_pointer_cast<ASCIIText>(shared_from_this());
}

ASCIIText::Ptr
ASCIIText::atlas(render::AbstractTexture::Ptr atlas)
{
    if (_atlas == atlas)
        return std::static_pointer_cast<ASCIIText>(shared_from_this());

    _atlas = atlas;

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

    if (!_atlas)
        return;

    if (_textSurface && target->hasComponent(_textSurface))
        target->removeComponent(_textSurface);

    auto context = _atlas->context();

    auto geometry = geometry::TextGeometry::create(context)
        ->setText(_atlas, _text, 1.f, true)
        ->computeNormals();
    geometry->upload();

    if (!_material->data()->hasProperty("alphaThreshold"))
        _material->data()->set("alphaThreshold", .3f);
    if (!_material->data()->hasProperty("triangleCulling"))
        _material->data()->set("triangleCulling", render::TriangleCulling::NONE);
    if (!_material->data()->hasProperty("priority"))
        _material->data()->set("priority", render::Priority::TRANSPARENT);
    if (!_material->data()->hasProperty("blendingMode"))
        _material->data()->set("blendingMode", render::Blending::Mode::ALPHA);
    if (!_material->data()->hasProperty("blendingSource"))
        _material->data()->set("blendingSource", render::Blending::Source::SRC_ALPHA);
    if (!_material->data()->hasProperty("blendingDestination"))
        _material->data()->set("blendingDestination", render::Blending::Destination::ONE_MINUS_SRC_ALPHA);

    _material->data()->set("alphaMap", _atlas->sampler());

    _textSurface = Surface::create(
        geometry,
        _material,
        _effect
    );

    target->addComponent(_textSurface);
}
