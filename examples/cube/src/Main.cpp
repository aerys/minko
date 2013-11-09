/*
Copyright (c) 2013 Aerys

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
#include "minko/MinkoPNG.hpp"
#include "minko/MinkoSDL.hpp"

using namespace minko;
using namespace minko::component;
using namespace minko::math;

const std::string	PROP_NAME	= "alpha";
const float			PERIOD		= 2.0; // in seconds

int main(int argc, char** argv)
{
	auto canvas = Canvas::create("Minko Example - Cube", 800, 600);

	auto sceneManager = SceneManager::create(canvas->context());
	
	// setup assets
	sceneManager->assets()->defaultOptions()->generateMipmaps(true);
	sceneManager->assets()
		->registerParser<file::PNGParser>("png")
		->geometry("cube", geometry::CubeGeometry::create(sceneManager->assets()->context()))
		->queue("texture/box.png")
		//->queue("effect/Basic.effect")
		->queue("effect/AlphaBasic.effect");

	const clock_t	start	= clock();
	const float		period	= 2.0f;

	std::cout << "Press [Space] to inject a temporally-varying property to the root's data container" << std::endl;

	auto _ = sceneManager->assets()->complete()->connect([=](file::AssetLibrary::Ptr assets)
	{
		auto root = scene::Node::create("root")
			->addComponent(sceneManager);

		auto camera = scene::Node::create("camera")
			->addComponent(Renderer::create(0x7f7f7fff))
			->addComponent(Transform::create(
				Matrix4x4::create()->lookAt(Vector3::zero(), Vector3::create(0.f, 0.f, 3.f))
			))
			->addComponent(PerspectiveCamera::create(800.f / 600.f, (float)PI * 0.25f, .1f, 1000.f));
		
		auto mesh = scene::Node::create("mesh")
			->addComponent(Transform::create())
			->addComponent(Surface::create(
				assets->geometry("cube"),
				material::Material::create()
					->set("blendMode",		render::Blending::Mode::ALPHA)
					->set("diffuseColor",	Vector4::create(1.f, 1.f, 1.f, 1.f))
					->set("diffuseMap",		assets->texture("texture/box.png")),
				assets->effect("alphaBasic")
			));

		// handle keyboard signals
		auto keyDown = canvas->keyDown()->connect([&](Canvas::Ptr canvas, const Uint8* keyboard)
		{
			if (!root->data()->hasProperty(PROP_NAME))
			{
				std::cout << "add property '" << PROP_NAME << "' to root's container" << std::endl;
	
				auto provider = data::Provider::create();
				provider->set(PROP_NAME, 1.0f);
				root->data()->addProvider(provider);
			}
		});

		auto resized = canvas->resized()->connect([&](Canvas::Ptr canvas, uint w, uint h)
		{
			root->children()[0]->component<PerspectiveCamera>()->aspectRatio((float)w / (float)h);
		});

		auto enterFrame = canvas->enterFrame()->connect([&](Canvas::Ptr canvas)
		{
			mesh->component<Transform>()->transform()->appendRotationY(.01f);

			if (root->data()->hasProperty(PROP_NAME))
			{
				float value	= (float)(clock() - start) / (float)CLOCKS_PER_SEC;
				value		/= PERIOD;
				value		-= floorf(value);

				root->data()->set<float>(PROP_NAME, value);
			}

			sceneManager->nextFrame();
		});

		root->addChild(mesh);
		root->addChild(camera);

		canvas->run();
	});

	sceneManager->assets()->load();

	return 0;
}


