/*
Copyright (c) 2015 Aerys

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

#include "minko/Minko.hpp"
#include "minko/MinkoSDL.hpp"
#include "minko/MinkoTTF.hpp"

using namespace minko;
using namespace minko::component;

const std::string EFFECT_FILENAME = "effect/Basic.effect";
const std::string FONT_FILENAME = "font/MyFont.ttf";

int
main(int argc, char** argv)
{
    auto canvas = Canvas::create("Minko Example - TTF");
    auto sceneManager = SceneManager::create(canvas);

	sceneManager->assets()->loader()->options()
		->resizeSmoothly(true)
		->generateMipmaps(true);

    sceneManager->assets()->loader()
		->queue(EFFECT_FILENAME);

    auto cubeGeometry = geometry::CubeGeometry::create(sceneManager->assets()->context());
    sceneManager->assets()->geometry("cubeGeometry", cubeGeometry);

    auto root = scene::Node::create("root")
        ->addComponent(sceneManager);

    auto textMeshes = std::vector<scene::Node::Ptr>{
        scene::Node::create("textMesh1")->addComponent(Transform::create(math::translate(math::vec3(0.f, 1.f, 0.f)))),
        scene::Node::create("textMesh2")->addComponent(Transform::create(math::translate(math::vec3(0.f, -1.f, 0.f))))
    };

    auto textGeometries = std::vector<geometry::TextGeometry::Ptr>{
        geometry::TextGeometry::create(canvas->context())->setText(FONT_FILENAME, "Text in blue with a scale of 0.006", .006f, true),
        geometry::TextGeometry::create(canvas->context())->setText(FONT_FILENAME, "Larger text in white.", .01f, true)
    };

    auto defaultTextMaterial = material::Material::create()->set({
        { "diffuseColor", math::vec4(.15f, .55f, .88f, 1.f) },
        { "alphaMap", textGeometries.front()->atlasTexture()->sampler() },
        { "triangleCulling", render::TriangleCulling::NONE },
        { render::States::PROPERTY_BLENDING_SOURCE, render::Blending::Source::SRC_ALPHA },
        { render::States::PROPERTY_BLENDING_DESTINATION, render::Blending::Destination::ONE_MINUS_SRC_ALPHA }
    });

    auto textMaterials = std::vector<material::Material::Ptr>{
        defaultTextMaterial,
        material::Material::create(defaultTextMaterial)->set({{"diffuseColor", math::vec4(1.f)}})
    };

    auto camera = scene::Node::create("camera")
        ->addComponent(Renderer::create(0x7f7f7fff))
        ->addComponent(Transform::create(
            math::inverse(
                math::lookAt(
                    math::vec3(0.f, 0.f, 5.f), math::vec3(), math::vec3(0, 1, 0)
                )
            )
        ))
        ->addComponent(Camera::create(math::perspective(.785f, canvas->aspectRatio(), 0.1f, 1000.f)));

    root->addChild(camera);

    auto _ = sceneManager->assets()->loader()->complete()->connect([=](file::Loader::Ptr loader)
    {
        auto material = material::BasicMaterial::create();

        for (auto i = 0; i < textMeshes.size(); ++i)
        {
            auto geometry = textGeometries.at(i);
            auto material = textMaterials.at(i);

            textMeshes.at(i)->addComponent(Surface::create(geometry, material, sceneManager->assets()->effect(EFFECT_FILENAME)));
            root->addChild(textMeshes.at(i));
        }
    });

    auto resized = canvas->resized()->connect([&](AbstractCanvas::Ptr canvas, uint w, uint h)
    {
        camera->component<Camera>()->projectionMatrix(math::perspective(.785f, canvas->aspectRatio(), 0.1f, 1000.f));
    });

    auto enterFrame = canvas->enterFrame()->connect([&](AbstractCanvas::Ptr canvas, float time, float deltaTime, bool shouldRender)
    {
        sceneManager->nextFrame(time, deltaTime, shouldRender);
    });

    sceneManager->assets()->loader()->load();
    canvas->run();

    return 0;
}
