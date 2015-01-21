/*
Copyright (c) 2014 Aerys

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and
associated documentation files (the "Software"), to deal in the Software without restriction,
including without limitation the rights to use, copy, modify, merge, publish, distribute,
sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or
substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR 	IMPLIED, INCLUDING
BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include "minko/component/FrustumDisplay.hpp"

#include "minko/scene/Node.hpp"
#include "minko/scene/Layout.hpp"
#include "minko/component/SceneManager.hpp"
#include "minko/geometry/Geometry.hpp"
#include "minko/geometry/LineGeometry.hpp"
#include "minko/material/BasicMaterial.hpp"
#include "minko/render/VertexBuffer.hpp"
#include "minko/render/IndexBuffer.hpp"
#include "minko/render/Blending.hpp"
#include "minko/render/TriangleCulling.hpp"
#include "minko/render/Priority.hpp"
#include "minko/render/CompareMode.hpp"
#include "minko/file/AssetLibrary.hpp"
#include "minko/file/Loader.hpp"
#include "minko/file/Options.hpp"

using namespace minko;
using namespace minko::component;

FrustumDisplay::FrustumDisplay(const math::mat4& projection) :
    _projection(projection),
    _surface(nullptr),
    _addedSlot(nullptr),
    _material(material::BasicMaterial::create())
{
    _material
        ->diffuseColor(math::vec4(1.f, 1.f, 1.f, .1f))
        // .depthFunction(render::CompareMode::ALWAYS)
        ->priority(render::Priority::LAST)
        ->depthMask(false)
        ->zSorted(true)
        ->triangleCulling(render::TriangleCulling::NONE)
        ->blendingMode(render::Blending::Mode::ADDITIVE);
}

void
FrustumDisplay::targetAdded(std::shared_ptr<scene::Node> target)
{
    if (target->root()->hasComponent<SceneManager>())
        addedHandler(nullptr, target, nullptr);
    else
        _addedSlot = target->added().connect(std::bind(
            &FrustumDisplay::addedHandler,
            std::static_pointer_cast<FrustumDisplay>(shared_from_this()),
            std::placeholders::_1,
            std::placeholders::_2,
            std::placeholders::_3
        ));
}

void
FrustumDisplay::targetRemoved(std::shared_ptr<scene::Node> target)
{
    _addedSlot = nullptr;

    if (target->hasComponent(_surface))
        target->removeComponent(_surface);
    _surface = nullptr;

    if (target->hasComponent(_lines))
        target->removeComponent(_lines);
    _lines = nullptr;
}

void
FrustumDisplay::addedHandler(scene::Node::Ptr node, scene::Node::Ptr target, scene::Node::Ptr added)
{
    if (!_surface && target->root()->hasComponent<SceneManager>())
        initialize();
}

void
FrustumDisplay::initialize()
{
    auto vertices = getVertices();
    auto assets = target()->root()->component<SceneManager>()->assets();

    auto effect = assets->effect("effect/Basic.effect");
    if (!effect)
    {
        auto loader = file::Loader::create(assets->loader());

        loader->options()->loadAsynchronously(false);
        loader->queue("effect/Basic.effect");
        loader->load();

        effect = assets->effect("effect/Basic.effect");
    }

    auto geom = initializeFrustumGeometry(vertices, assets->context());

    _surface = Surface::create(geom, _material, effect, "transparent");
    _surface->layoutMask(scene::BuiltinLayout::DEBUG_ONLY);
    target()->addComponent(_surface);

    initializeLines(vertices);
}

void
FrustumDisplay::initializeLines(const std::vector<math::vec3>& vertices)
{
    auto assets = target()->root()->component<SceneManager>()->assets();

    auto effect = assets->effect("effect/Line.effect");
    if (!effect)
    {
        auto loader = file::Loader::create(assets->loader());

        loader->options()->loadAsynchronously(false);
        loader->queue("effect/Line.effect");
        loader->load();

        effect = assets->effect("effect/Line.effect");
    }

    geometry::LineGeometry::Ptr lines = geometry::LineGeometry::create(assets->context());

    lines->moveTo(vertices[0]);
    for (auto i = 1; i < 4; ++i)
        lines->lineTo(vertices[i]);
    lines->lineTo(vertices[0]);
    lines->moveTo(vertices[4]);
    for (auto i = 4; i < 8; ++i)
        lines->lineTo(vertices[i]);
    lines->lineTo(vertices[4]);
    for (auto i = 0; i < 4; ++i)
        lines->moveTo(vertices[i])->lineTo(vertices[i + 4]);
    lines->upload();

    _lines = Surface::create(lines, _material, effect);
    _lines->layoutMask(scene::BuiltinLayout::DEBUG_ONLY);
    target()->addComponent(_lines);
}

geometry::Geometry::Ptr
FrustumDisplay::initializeFrustumGeometry(const std::vector<math::vec3>&            vertices,
                                          std::shared_ptr<render::AbstractContext>  context)
{
    auto vb = render::VertexBuffer::create(context, math::value_ptr(vertices[0]), vertices.size() * 3);
    vb->addAttribute("position", 3);
    vb->upload();

    auto ib = render::IndexBuffer::create(context, {
        0, 3, 1, 1, 3, 2, // near
        4, 5, 7, 5, 6, 7, // far
        4, 0, 5, 5, 0, 1, // top
        7, 6, 3, 6, 2, 3, // bottom
        4, 3, 0, 4, 7, 3, // left
        5, 1, 6, 1, 2, 6  // right
    });
    ib->upload();

    auto geom = geometry::Geometry::create();
    geom->addVertexBuffer(vb);
    geom->indices(ib);

    return geom;
}

std::vector<math::vec3>
FrustumDisplay::getVertices()
{
    math::mat4 invProj = math::inverse(_projection);
    std::vector<math::vec4> vv = {
        invProj * math::vec4(-1.f, 1.f, -1.f, 1.f),
        invProj * math::vec4(1.f, 1.f, -1.f, 1.f),
        invProj * math::vec4(1.f, -1.f, -1.f, 1.f),
        invProj * math::vec4(-1.f, -1.f, -1.f, 1.f),
        invProj * math::vec4(-1.f, 1.f, 1.f, 1.f),
        invProj * math::vec4(1.f, 1.f, 1.f, 1.f),
        invProj * math::vec4(1.f, -1.f, 1.f, 1.f),
        invProj * math::vec4(-1.f, -1.f, 1.f, 1.f)
    };

    std::vector<math::vec3> vertices;
    for (const auto& v : vv)
        vertices.push_back((v / v.w).xyz());

    return vertices;
}
