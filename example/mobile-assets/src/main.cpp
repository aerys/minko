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

#include "minko/Minko.hpp"
#include "minko/MinkoSDL.hpp"
#include "minko/MinkoASSIMP.hpp"
#include "minko/MinkoJPEG.hpp"
#include "minko/MinkoPNG.hpp"

using namespace minko;
using namespace minko::component;
using namespace minko::math;

const uint WINDOW_WIDTH = 800;
const uint WINDOW_HEIGHT = 600;

const std::string MODEL_FILENAME = "model/pirate.dae";
const std::string MINKO_LOGO = "texture/minko-logo.png";

int
main(int argc, char** argv)
{
    uint currentIndex = 0;
    uint previousIndex = 0;

    auto canvas = Canvas::create("Minko Example - Mobile assets", WINDOW_WIDTH, WINDOW_HEIGHT);
    auto sceneManager = SceneManager::create(canvas->context());

    // setup assets
	sceneManager->assets()->loader()->options()
        ->registerParser<file::ASSIMPParser>("dae")
        ->registerParser<file::PNGParser>("png")
        ->registerParser<file::JPEGParser>("jpg");

	sceneManager->assets()->loader()->queue("effect/Basic.effect");
    

    auto modelPaths = std::vector<std::string> 
    { 
        "model/pirate.dae", 
        "model/ninja.dae",
       // "model/infirmiere.dae",
    };

    // add the models to the asset list
    for (auto modelPath : modelPaths)
    {
		sceneManager->assets()->loader()->queue(modelPath);
    }

    // add Minko logo to the list of file to load
	sceneManager->assets()->loader()->queue(MINKO_LOGO);

	sceneManager->assets()->loader()->options()->generateMipmaps(true);
	sceneManager->assets()->loader()->options()->effect(sceneManager->assets()->effect("effect/Basic.effect"));

	auto complete = sceneManager->assets()->loader()->complete()->connect([&](file::AssetLibrary::Ptr assets)
    {
        auto root = scene::Node::create("root")->addComponent(sceneManager);

        auto camera = scene::Node::create("camera")
            ->addComponent(Renderer::create(0x7f7f7fff))
            ->addComponent(Transform::create(
            Matrix4x4::create()->lookAt(Vector3::create(0.f, 0.f, 0.f), Vector3::create(0.f, 0.f, 5.f))
            ))
            ->addComponent(PerspectiveCamera::create(
            (float) WINDOW_WIDTH / (float) WINDOW_HEIGHT, (float) PI * 0.25f, .1f, 1000.f)
            );
        root->addChild(camera);

        // Create a quad to display the Minko logo
        auto bannerMaterial = material::BasicMaterial::create();
        bannerMaterial->blendingMode(render::Blending::Mode::ALPHA);
        bannerMaterial->diffuseMap(assets->texture(MINKO_LOGO));

        auto banner = scene::Node::create("banner")
            ->addComponent(Transform::create(Matrix4x4::create()
                ->appendScale(4, 1, 0)
                ->appendTranslation(0.f, 1.f, 1.f)))
            ->addComponent(Surface::create(
                    geometry::QuadGeometry::create(assets->context()),
                    bannerMaterial,
                    assets->effect("effect/Basic.effect")
                )
            );
        root->addChild(banner);

        auto models = std::vector<scene::Node::Ptr>();

        for (int i = 0; i < modelPaths.size(); i++)
        {
            auto model = assets->symbol(modelPaths[i]);

            // change position
            if (i == currentIndex)
                model->component<Transform>()->matrix()->translation(0.f, -1.f, 0.f);
            else
            {
                // model->component<Surface>()->visible(false); // No surface!
                model->component<Transform>()->matrix()->translation(0.f, -5.f, 0.f);
            }

            // add to the scene
            root->addChild(model);

            models.push_back(model);
        }
        
        Signal<input::Mouse::Ptr, int, int>::Slot mouseMove;
        float cameraRotationSpeed = 0.f;

        auto mouseDown = canvas->mouse()->leftButtonDown()->connect([&](input::Mouse::Ptr mouse)
        {
            mouseMove = canvas->mouse()->move()->connect([&](input::Mouse::Ptr mouse, int dx, int dy)
            {
                cameraRotationSpeed = (float) -dx * .01f;
            });
        });

        auto mouseUp = canvas->mouse()->leftButtonUp()->connect([&](input::Mouse::Ptr mouse)
        {
            mouseMove = nullptr;
        });

        auto keyDown = canvas->keyboard()->keyDown()->connect([&](input::Keyboard::Ptr k)
        {
            if (k->keyIsDown(input::Keyboard::KeyCode::SPACE))
            {
                if (currentIndex == previousIndex)
                    currentIndex = (currentIndex + 1) % models.size();
            }
        });

        auto enterFrame = canvas->enterFrame()->connect([&](Canvas::Ptr canvas, float t, float dt)
        {
            camera->component<Transform>()->matrix()->appendRotationY(cameraRotationSpeed);
            banner->component<Transform>()->matrix()->appendRotationY(cameraRotationSpeed);
            cameraRotationSpeed *= .99f;

            if (currentIndex == previousIndex)
                models[currentIndex]->component<Transform>()->matrix()->appendRotationY(0.001f * dt);

            if (currentIndex != previousIndex)
            {
                if (models[currentIndex]->component<Transform>()->y() < -1)
                {
                    models[currentIndex]->component<Transform>()->matrix()->appendTranslation(0, 0.01f * dt, 0);
                    models[previousIndex]->component<Transform>()->matrix()->appendTranslation(0, 0.01f * dt, 0);
                }
                else
                {
                    models[currentIndex]->component<Transform>()->matrix()->translation(
                        models[currentIndex]->component<Transform>()->x(),
                        -1,
                        models[currentIndex]->component<Transform>()->z());

                    models[previousIndex]->component<Transform>()->matrix()->translation(
                        models[previousIndex]->component<Transform>()->x(),
                        -5,
                        models[previousIndex]->component<Transform>()->z());

                    previousIndex = currentIndex;
                }
            }

            sceneManager->nextFrame(t, dt);
        });

        canvas->run();
    });

	sceneManager->assets()->loader()->load();

    return 0;
}