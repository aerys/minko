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

#include <minko/Minko.hpp>
#include <minko/MinkoSDL.hpp>

#include "minko/geometry/StarGeometry.hpp"

using namespace minko;
using namespace minko::math;
using namespace minko::scene;
using namespace minko::material;
using namespace minko::geometry;
using namespace minko::component;
using namespace minko::render;

vec4
generateColor()
{
    return vec4(rand() / float(RAND_MAX), rand() / float(RAND_MAX), rand() / float(RAND_MAX), 0.5);
}

math::uint
generateHexColor()
{
    vec4 color = generateColor();

    const unsigned char r = static_cast<unsigned char>(floorf(255.0f * color.x));
    const unsigned char g = static_cast<unsigned char>(floorf(255.0f * color.y));
    const unsigned char b = static_cast<unsigned char>(floorf(255.0f * color.z));
    const unsigned char a = static_cast<unsigned char>(floorf(255.0f * color.w));

    return ((r << 24) | (g << 16) | (b << 8) | a);
}

void
generateStars(unsigned int numStars,
              file::AssetLibrary::Ptr assets,
              std::vector<Node::Ptr>& starNodes)
{
    if (assets == nullptr)
        throw std::invalid_argument("assets");

    starNodes.clear();
    starNodes.resize(numStars, nullptr);

    for (unsigned int i = 0; i < numStars; ++i)
    {
        starNodes[i] = Node::create("star_" + std::to_string(i))
            ->addComponent(Transform::create())
            ->addComponent(Surface::create(
                assets->geometry("smallStar"),
                BasicMaterial::create()
                    ->diffuseColor(generateColor())
                    ->colorMask(true)
                    ->depthMask(false)
                    ->depthFunction(CompareMode::ALWAYS)
                    ->stencilFunction(CompareMode::EQUAL)
                    ->stencilReference(1)
                    ->stencilMask(0xff)
                    ->stencilFailOperation(StencilOperation::KEEP),
                assets->effect("effect/Basic.effect")
            ));

        const float minX    = -1.0f;
        const float rangeX    = 1.0f - minX;
        const float minY    = -1.0f;
        const float rangeY    = 1.0f - minY;
        
        starNodes[i]->component<Transform>()->matrix(
            math::translate(math::vec3(
                minX + (rand() / float(RAND_MAX)) * rangeX,
                minY + (rand() / float(RAND_MAX)) * rangeY,
                0.0f
            )) *
            math::rotate(
                2.0f * float(M_PI) * (rand() / float(RAND_MAX)),
                math::vec3(0.f, 0.f, 1.f)
            ) *
            starNodes[i]->component<Transform>()->matrix() *
            math::scale(math::vec3(0.25f, 0.25f, 0.25f))
        );
    }
}

int
main(int argc, char** argv)
{
    auto canvas = Canvas::create("Minko Example - Stencil", 1280, 720, Canvas::RESIZABLE | Canvas::STENCIL);
    auto sceneManager = SceneManager::create(canvas);
    auto assets = sceneManager->assets();

    // setup assets
    sceneManager->assets()
        ->geometry("bigStar",      StarGeometry::create(sceneManager->assets()->context(), 5, 0.5f, 0.325f))
        ->geometry("smallStar",    StarGeometry::create(sceneManager->assets()->context(), 5, 0.5f, 0.25f))
        ->geometry("quad",         QuadGeometry::create(sceneManager->assets()->context()));

    sceneManager->assets()->loader()
        ->queue("effect/Basic.effect");

    unsigned int               numSmallStars    = 30;
    std::vector<Node::Ptr>     smallStars;

    auto root = Node::create("root")
        ->addComponent(sceneManager);

    auto camera = Node::create("camera")
        ->addComponent(Renderer::create(generateHexColor()))
        ->addComponent(Camera::create(math::perspective(.785f, canvas->aspectRatio(), 0.1f, 1000.f)))
        ->addComponent(Transform::create());

    camera->component<Transform>()->matrix(
        math::inverse(math::lookAt(vec3(0.f, 0.f, 3.f), vec3(0.f), vec3(0.f, 1.f, 0.f)))
    );

    auto bigStarNode = Node::create("bigStarNode")
        ->addComponent(Transform::create());

    auto quadNode = Node::create("quadNode")
        ->addComponent(Transform::create());

    root->addChild(camera);

    auto _ = sceneManager->assets()->loader()->complete()->connect([&](file::Loader::Ptr loader)
    {
        bigStarNode
            ->addComponent(Surface::create(
                assets->geometry("bigStar"),
                BasicMaterial::create()
                    ->diffuseColor(vec4(1.0f, 1.0f, 1.0f, 1.0f))
                    ->colorMask(false)
                    ->depthMask(false)
                    ->depthFunction(CompareMode::ALWAYS)
                    ->stencilFunction(CompareMode::NEVER)
                    ->stencilReference(1)
                    ->stencilMask(0xff)
                    ->stencilFailOperation(StencilOperation::REPLACE)
                    ->triangleCulling(TriangleCulling::BACK),
                assets->effect("effect/Basic.effect")
            ));

        bigStarNode->component<Transform>()->matrix(
            math::scale(math::vec3(2.5f, 2.5f, 2.5f)) *
            bigStarNode->component<Transform>()->matrix()
        );

        quadNode
            ->addComponent(Surface::create(
                assets->geometry("quad"),
                BasicMaterial::create()
                    ->diffuseColor(generateColor())
                    ->colorMask(true)
                    ->depthMask(false)
                    ->depthFunction(CompareMode::ALWAYS)
                    ->stencilFunction(CompareMode::EQUAL)
                    ->stencilReference(1)
                    ->stencilMask(0xff)
                    ->stencilFailOperation(StencilOperation::KEEP)
                    ->triangleCulling(TriangleCulling::BACK),
                assets->effect("effect/Basic.effect")
            ));


        quadNode->component<Transform>()->matrix(
            math::scale(math::vec3(4.0f, 4.0f, 4.0f)) *
            quadNode->component<Transform>()->matrix()
        );

        generateStars(numSmallStars, sceneManager->assets(), smallStars);

        // stencil writing pass
        root->addChild(bigStarNode);
        // stencil fetching pass
        root->addChild(quadNode);

        for (auto& star : smallStars)
            root->addChild(star);
    });

    auto enterFrame = canvas->enterFrame()->connect([&](AbstractCanvas::Ptr canvas, float time, float deltaTime)
    {
        bigStarNode->component<Transform>()->matrix(
            math::rotate(.001f, math::vec3(0.f, 0.f, 1.f)) *
            bigStarNode->component<Transform>()->matrix()
        );

        for (auto& star : smallStars)
            star->component<Transform>()->matrix(
                star->component<Transform>()->matrix() *
                math::rotate(-0.025f, math::vec3(0.f, 0.f, 1.f))
            );

        sceneManager->nextFrame(time, deltaTime);
    });

    sceneManager->assets()->loader()->load();
    canvas->run();
}
