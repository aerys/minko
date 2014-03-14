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

int main(int argc, char** argv)
{
	auto canvas = Canvas::create("Minko Example - Ray Casting", 800, 600);

	auto sceneManager = SceneManager::create(canvas->context());

	// setup assets
	sceneManager->assets()->loader()->options()->generateMipmaps(true);
	sceneManager->assets()->loader()->options()
                ->registerParser<file::PNGParser>("png");
        sceneManager->assets()->loader()
                ->queue("texture/box.png")
		->queue("effect/Basic.effect");

	auto root = scene::Node::create("root")
		->addComponent(sceneManager);

	auto camera = scene::Node::create("camera")
		->addComponent(Renderer::create(0x7f7f7fff))
		->addComponent(Transform::create(
			Matrix4x4::create()->lookAt(Vector3::zero(), Vector3::create(0.f, 1.f, 3.f))
		))
		->addComponent(PerspectiveCamera::create(800.f / 600.f, (float)PI * 0.25f, .1f, 1000.f));
	root->addChild(camera);

	auto mesh = scene::Node::create("mesh")
		->addComponent(Transform::create());


	auto hit = scene::Node::create()
		->addComponent(Transform::create());

	root->addComponent(MousePicking::create());

	auto _ = sceneManager->assets()->loader()->complete()->connect([=](file::Loader::Ptr loader)
	{
		mesh->addComponent(BoundingBox::create())
			->addComponent(Surface::create(
				geometry::CubeGeometry::create(sceneManager->assets()->context()),
				material::BasicMaterial::create()->diffuseMap(sceneManager->assets()->texture("texture/box.png")),
				sceneManager->assets()->effect("effect/Basic.effect")
			));
		root->addChild(mesh);

		hit->addComponent(Surface::create(
				geometry::CubeGeometry::create(sceneManager->assets()->context()),
				material::BasicMaterial::create()->diffuseColor(0x00ff00ff),
				sceneManager->assets()->effect("effect/Basic.effect")
			));
	});

	auto mouseOver = root->component<MousePicking>()->move()->connect(
		[&](MousePicking::Ptr mp, MousePicking::HitList& hits, Ray::Ptr ray)
		{
			if (hit->parent() != root)
				root->addChild(hit);
			hit->component<Transform>()->matrix()
				->identity()
				->appendScale(.1f)
				->translation(
					ray->origin()->x() + ray->direction()->x() * hits.front().second,
					ray->origin()->y() + ray->direction()->y() * hits.front().second,
					ray->origin()->z() + ray->direction()->z() * hits.front().second
				);
		}
	);

	auto resized = canvas->resized()->connect([&](AbstractCanvas::Ptr canvas, uint w, uint h)
	{
		camera->component<PerspectiveCamera>()->aspectRatio((float)w / (float)h);
	});


	auto enterFrame = canvas->enterFrame()->connect([&](Canvas::Ptr canvas, float time, float deltaTime)
	{
		auto distance = 0.f;
		auto ray = camera->component<PerspectiveCamera>()->unproject(
			canvas->mouse()->normalizedX(), canvas->mouse()->normalizedY()
		);

		mesh->component<Transform>()->matrix()
			//->translation(sinf((float)time * .001f), 0.f, 0.f);
			->prependRotationY(.01f);

		root->component<MousePicking>()->pick(ray);

		/*
		if (mesh->component<BoundingBox>()->shape()->cast(ray, distance))
		{
			if (hit->parent() != root)
				root->addChild(hit);
			hit->component<Transform>()->transform()
				->identity()
				->appendScale(.1f)
				->translation(
					ray->origin()->x() + ray->direction()->x() * distance,
					ray->origin()->y() + ray->direction()->y() * distance,
					ray->origin()->z() + ray->direction()->z() * distance
				);
		}
		else if (hit->parent() == root)
			root->removeChild(hit);
		*/
		//camera->component<Transform>()->matrix()->appendRotationY(.01f);
		sceneManager->nextFrame(time, deltaTime);
	});

	sceneManager->assets()->loader()->load();

	canvas->run();

	return 0;
}


