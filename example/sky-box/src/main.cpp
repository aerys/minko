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
#include "minko/MinkoJPEG.hpp"
#include "minko/MinkoSDL.hpp"

using namespace minko;
using namespace minko::component;
using namespace minko::scene;

const std::string	CUBE_TEXTURE	= "texture/cubemap_sea.jpeg";
const unsigned int	NUM_OBJECTS		= 15;

Node::Ptr
createTransparentObject(float, float rotationY, file::AssetLibrary::Ptr);

int main(int argc, char** argv)
{
	auto canvas = Canvas::create("Minko Example - Sky Box", 800, 600);
	auto sceneManager = SceneManager::create(canvas->context());

	// setup assets
    auto loader = sceneManager->assets()->loader();
	loader->options()->resizeSmoothly(true);
	loader->options()->generateMipmaps(true);
	loader->options()
		->registerParser<file::PNGParser>("png")
		->registerParser<file::JPEGParser>("jpg")
        ->registerParser<file::JPEGParser>("jpeg");
	
	sceneManager->assets()->loader()
        ->queue(CUBE_TEXTURE, file::Options::create(loader->options())->isCubeTexture(true))
		->queue("effect/Basic.effect");

	sceneManager->assets()
		->geometry("cube", geometry::CubeGeometry::create(sceneManager->assets()->context()))
		->geometry("sphere", geometry::SphereGeometry::create(sceneManager->assets()->context(), 16, 16));

	auto root = scene::Node::create("root")
		->addComponent(sceneManager);

	auto camera = scene::Node::create("camera")
		->addComponent(Renderer::create(0x7f7f7fff))
		->addComponent(Transform::create(
			math::inverse(math::lookAt(math::vec3(0.f, 0.f, 3.f), math::vec3(0.f), math::vec3(0.f, 1.f, 0.f))
		)))
		->addComponent(PerspectiveCamera::create(800.f / 600.f, (float)PI * 0.25f, .1f, 1000.f));

	auto sky = scene::Node::create("sky")
		->addComponent(Transform::create(
			math::scale(math::mat4(1.f), math::vec3(100.f))
		));

	auto objects = scene::Node::create("objects")
		->addComponent(Transform::create(
			//math::rotate(math::mat4(1.f), .2f, math::vec3(1.f, 0.f, 0.f))
		));

	auto _ = sceneManager->assets()->loader()->complete()->connect([=](file::Loader::Ptr loader)
	{
		sky->addComponent(Surface::create(
				sceneManager->assets()->geometry("cube"),
				material::BasicMaterial::create()
					->diffuseCubeMap(sceneManager->assets()->texture(CUBE_TEXTURE))
					->triangleCulling(render::TriangleCulling::FRONT),
				sceneManager->assets()->effect("effect/Basic.effect")
			));

		assert(NUM_OBJECTS > 0);
		const float scale = 1.25f * (float) PI / (float)NUM_OBJECTS;
		const float	dAngle = 2.0f * (float) PI / (float)NUM_OBJECTS;

		for (unsigned int objId = 0; objId < NUM_OBJECTS; ++objId)
            objects->addChild(createTransparentObject(scale, objId * dAngle, sceneManager->assets()));

		root
			->addChild(camera)
			->addChild(sky)
			->addChild(objects);
	});

	auto resized = canvas->resized()->connect([&](AbstractCanvas::Ptr canvas, uint w, uint h)
	{
		camera->component<PerspectiveCamera>()->aspectRatio((float)w / (float)h);
	});

	auto enterFrame = canvas->enterFrame()->connect([&](Canvas::Ptr canvas, float time, float deltaTime)
	{
		auto skyTransform = sky->component<Transform>();
		auto objectsTransform = objects->component<Transform>();

		skyTransform->matrix(math::rotate(skyTransform->matrix(), .001f, math::vec3(0.f, 1.f, 0.f)));
		objectsTransform->matrix(math::rotate(objectsTransform->matrix(), -.02f, math::vec3(0.f, 1.f, 0.f)));

		sceneManager->nextFrame(time, deltaTime);
	});

	sceneManager->assets()->loader()->load();

	canvas->run();

	return 0;
}

Node::Ptr
createTransparentObject(float scale, float rotationY, file::AssetLibrary::Ptr assets)
{
	assert(assets);
	assert(NUM_OBJECTS > 0);

	auto		randomAxis	= math::normalize(math::vec3((float)rand(), (float)rand(), (float)rand()));
	const float randomAng	= 2.0f * (float)PI * rand() / (float)RAND_MAX;

	auto m = math::mat4(1.f);
	m = math::rotate(m, randomAng, randomAxis);
	m = math::translate(m, math::vec3(1.f, 0.f, 0.f));
	m = math::scale(m, math::vec3(scale));
	m = math::rotate(m, rotationY, math::vec3(0.f, 1.f, 0.f));

	return scene::Node::create()
		->addComponent(Transform::create(m))
		->addComponent(Surface::create(
            assets->geometry("cube"),
			material::BasicMaterial::create()
				->diffuseColor(math::vec4(math::rgbColor(math::vec3((rotationY / (2.f * (float)PI)) * 360, 1.0f, 0.5f)), 0.5f))
				->isTransparent(true, true)
				->triangleCulling(render::TriangleCulling::BACK),
			assets->effect("effect/Basic.effect")
		));
}
