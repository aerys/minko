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
using namespace minko::scene;
using namespace minko::material;
using namespace minko::geometry;
using namespace minko::component;
using namespace minko::render;

math::vec4
generateColor()
{
	auto r = math::sphericalRand(1.f);
	return math::vec4((r + 1.f) * .5f, 1.f);
}

math::uint
generateHexColor()
{
    math::vec4 color = generateColor();

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
		auto starMaterial = Material::create();
		starMaterial->data()
            ->set("diffuseColor", generateColor())
		    ->set("colorMask", true)
		    ->set("depthMask", false)
		    ->set("depthFunction", CompareMode::ALWAYS)
		    ->set("stencilFunction", CompareMode::EQUAL)
		    ->set("stencilReference", 1)
		    ->set("stencilMask", uint(0xff))
		    ->set("stencilFailOperation", StencilOperation::KEEP);

        starNodes[i] = Node::create("star_" + std::to_string(i))
            ->addComponent(Transform::create())
            ->addComponent(Surface::create(
                assets->geometry("smallStar"),
                starMaterial,
                assets->effect("effect/Basic.effect")
            ));

        const float minX = -1.0f;
        const float rangeX = 1.0f - minX;
        const float minY = -1.0f;
        const float rangeY = 1.0f - minY;

		auto starTransform = starNodes[i]->component<Transform>();
		starTransform->matrix(math::scale(math::vec3(0.25f)) * starTransform->matrix());
		starTransform->matrix(math::rotate(2.0f * float(M_PI) * (rand() / float(RAND_MAX)), math::vec3(0.f, 0.f, 1.0f)) * starTransform->matrix());
		starTransform->matrix(math::translate(math::vec3(
			minX + (rand() / float(RAND_MAX)) * rangeX,
			minY + (rand() / float(RAND_MAX)) * rangeY,
			0.0f)) * starTransform->matrix());
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
        ->geometry("bigStar", StarGeometry::create(sceneManager->assets()->context(), 5, 0.5f, 0.325f))
        ->geometry("smallStar", StarGeometry::create(sceneManager->assets()->context(), 5, 0.5f, 0.25f))
        ->geometry("quad", QuadGeometry::create(sceneManager->assets()->context()));

    sceneManager->assets()->loader()
        ->queue("effect/Basic.effect");

    unsigned int numSmallStars = 30;
    std::vector<Node::Ptr> smallStars;

    auto root = Node::create("root")
        ->addComponent(sceneManager);

    auto camera = Node::create("camera")
        ->addComponent(Renderer::create(generateHexColor()))
        ->addComponent(Camera::create(math::perspective(.785f, canvas->aspectRatio(), 0.1f, 1000.f)))
        ->addComponent(
			Transform::create(
				math::inverse(
					math::lookAt(
						math::vec3(0.f, 0.f, 3.f),
						math::zero<math::vec3>(),
						math::vec3(0.f, 1.f, 0.f)
					)
				)
			)
		);

    auto bigStarNode = Node::create("bigStarNode")
        ->addComponent(Transform::create());

    auto quadNode = Node::create("quadNode")
        ->addComponent(Transform::create());

    root->addChild(camera);

    auto _ = sceneManager->assets()->loader()->complete()->connect([&](file::Loader::Ptr loader)
    {
		auto bigStarMaterial = Material::create();
		bigStarMaterial->data()
            ->set("diffuseColor", math::vec4(1.0f, 1.0f, 1.0f, 1.0f))
		    ->set("colorMask", false)
		    ->set("depthMask", false)
		    ->set("depthFunction", CompareMode::ALWAYS)
		    ->set("stencilFunction", CompareMode::NEVER)
		    ->set("stencilReference", 1)
		    ->set("stencilMask", uint(0xff))
		    ->set("stencilFailOperation", StencilOperation::REPLACE)
		    ->set("triangleCulling", TriangleCulling::BACK);

        bigStarNode
            ->addComponent(Surface::create(
                assets->geometry("bigStar"),
                bigStarMaterial,
                assets->effect("effect/Basic.effect")
            ));

		auto bigStarTransform = bigStarNode->component<Transform>();
		bigStarTransform->matrix(math::scale(math::vec3(2.5f)) * bigStarTransform->matrix());

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

		auto quadTransform = quadNode->component<Transform>();
		quadTransform->matrix(math::scale(math::vec3(4.0f)) * quadTransform->matrix());

        generateStars(numSmallStars, sceneManager->assets(), smallStars);

        // stencil writing pass
        root->addChild(bigStarNode);
        // stencil fetching pass
        root->addChild(quadNode);

        for (auto& star : smallStars)
            root->addChild(star);
    });

	auto keyDown = canvas->keyboard()->keyDown()->connect([&](input::Keyboard::Ptr k)
	{
		if (k->keyIsDown(input::Keyboard::UP))
		{
			bigStarNode->component<Transform>()->matrix(math::scale(math::vec3(1.1f)) * bigStarNode->component<Transform>()->matrix());
		}
		else if (k->keyIsDown(input::Keyboard::DOWN))
		{
			bigStarNode->component<Transform>()->matrix(math::scale(math::vec3(0.9f)) * bigStarNode->component<Transform>()->matrix());
		}
	});

    auto enterFrame = canvas->enterFrame()->connect([&](AbstractCanvas::Ptr canvas, float time, float deltaTime, bool shouldRender)
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

        sceneManager->nextFrame(time, deltaTime, shouldRender);
    });

    sceneManager->assets()->loader()->load();
    canvas->run();
}
