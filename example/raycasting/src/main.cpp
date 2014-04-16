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
using namespace minko::scene;
using namespace minko::component;
using namespace minko::math;

int main(int argc, char** argv)
{
	auto canvas = Canvas::create("Minko Example - Ray Casting", 800, 600);

	auto sceneManager = SceneManager::create(canvas->context());
	
	std::cout << "-----------------------------" << std::endl;
	std::cout << "[R]\ttoogle the raycasting layout mask of the RED box" << std::endl;
	std::cout << "[G]\trender only the GREEN box" << std::endl;
	std::cout << "[B]\trestrict ray intersection to the BLUE box" << std::endl;
	std::cout << "[Q]\tchange the green box's layouts" << std::endl;
	std::cout << "-----------------------------" << std::endl;

	bool	toogleGreenBoxLayouts	= false;
	int		greenBoxLayoutsId		= 0;
	Layouts greenBoxLayouts[2]		= { Layout::Group::DEFAULT | (1 << 3),  Layout::Group::DEFAULT };

	Layouts	blueBoxLayouts			= 1 << 2;

	bool	toogleRedBoxLayouts	= false;
	int		redBoxLayoutsId		= 0;
	Layouts	redBoxLayouts[2]	= { Layout::Group::DEFAULT, Layout::Group::IGNORE_RAYCASTING };

	bool	tooglePickingMask	= false;
	int		pickingMaskId		= 0;
	Layouts	pickingMasks[2]		= { Layout::Mask::RAYCASTING_DEFAULT, blueBoxLayouts };

	bool	toogleRendererMask	= false;
	int		rendererMaskId		= 0;
	Layouts rendererMasks[2]	= { Layout::Mask::EVERYTHING, 1 << 3 };

	// setup assets
	sceneManager->assets()->loader()->options()
		->generateMipmaps(true)
		->registerParser<file::PNGParser>("png");
	sceneManager->assets()->loader()
		->queue("texture/box.png")
		->queue("effect/Basic.effect")
		->queue("effect/Line.effect");

	auto root = Node::create("root")
		->addComponent(sceneManager);

	auto camera = Node::create("camera")
		->layouts(Layout::Mask::EVERYTHING)
		->addComponent(Renderer::create(0x7f7f7fff))
		->addComponent(Transform::create(
			Matrix4x4::create()->lookAt(Vector3::zero(), Vector3::create(0.f, 0.8f, 3.0f))
		))
		->addComponent(PerspectiveCamera::create(800.f / 600.f, (float)PI * 0.25f, .1f, 1000.f));
	root->addChild(camera);

	auto redBox = Node::create("redBox")
		->layouts(redBoxLayouts[0])
		->addComponent(Transform::create(
			math::Matrix4x4::create()->appendTranslation(0.0f, 0.0f, 1.5f)
		))
		->addComponent(BoundingBox::create());

	auto greenBox = Node::create("greenBox")
		->layouts(greenBoxLayouts[0])
		->addComponent(Transform::create(
			math::Matrix4x4::create()->appendTranslation(1.0f, 0.0f, -0.5f)
		))
		->addComponent(BoundingBox::create());

	auto blueBox = Node::create("blueBox")
		->layouts(blueBoxLayouts)
		->addComponent(Transform::create(
			math::Matrix4x4::create()->appendTranslation(-1.0f, 0.0f, -0.5f)
		))
		->addComponent(BoundingBox::create());

	auto hitProvider	= data::ArrayProvider::create("material")
		->set("depthFunc",	render::CompareMode::ALWAYS)
		->set("priority",	render::Priority::LAST);

	auto hit			= Node::create("hit")
		->addComponent(Transform::create());

	root->addComponent(MousePicking::create());

	auto _ = sceneManager->assets()->loader()->complete()->connect([=](file::Loader::Ptr loader)
	{
		auto assets = sceneManager->assets();

		redBox->addComponent(Surface::create(
			geometry::CubeGeometry::create(assets->context()),
			material::BasicMaterial::create()->diffuseMap(assets->texture("texture/box.png"))->diffuseColor(0xff6600ff),
			assets->effect("basic")
		));
		
		greenBox->addComponent(Surface::create(
			geometry::CubeGeometry::create(assets->context()),
			material::BasicMaterial::create()->diffuseMap(assets->texture("texture/box.png"))->diffuseColor(0x00ff00ff),
			assets->effect("basic")
		));

		blueBox->addComponent(Surface::create(
			geometry::CubeGeometry::create(assets->context()),
			material::BasicMaterial::create()->diffuseMap(assets->texture("texture/box.png"))->diffuseColor(0x0066ffff),
			assets->effect("basic")
		));

		auto hitGeometry = geometry::LineGeometry::create(canvas->context())
			->moveTo(-1.0f, 0.0f, 0.0f)
			->lineTo(1.0f, 0.0f, 0.0f)
			->moveTo(0.0f, -1.0f, 0.0f)
			->lineTo(0.0f, 1.0f, 0.0f);

		hit->addComponent(Surface::create(
				hitGeometry,
				hitProvider->set("diffuseColor", Vector4::create(1.0f, 1.0f, 1.0f, 1.0f)),
				assets->effect("line")));

		root
			->addChild(redBox)
			->addChild(greenBox)
			->addChild(blueBox);
	});

	auto mouseOver = root->component<MousePicking>()->move()->connect(
		[&](MousePicking::Ptr mp, MousePicking::HitList& hits, Ray::Ptr ray)
		{
			if (hit->parent() != root)
				root->addChild(hit);

			if (hits.empty())
			{
				root->removeChild(hit);
				return;
			}

			hit->component<Transform>()->matrix()
				->identity()
				->appendScale(.1f)
				->translation(
					ray->origin()->x() + ray->direction()->x() * hits.front().second,
					ray->origin()->y() + ray->direction()->y() * hits.front().second,
					ray->origin()->z() + ray->direction()->z() * hits.front().second
				);

			auto hitNode = hits.front().first;

			if (hitNode->name() == "redBox")
				hitProvider->set("diffuseColor", Vector4::create(1.0f, 0.5f, 0.0f, 1.0f));
			else if (hitNode->name() == "greenBox")
				hitProvider->set("diffuseColor", Vector4::create(0.0f, 1.0f, 0.0f, 1.0f));
			else if (hitNode->name() == "blueBox")
				hitProvider->set("diffuseColor", Vector4::create(0.0f, 0.75f, 1.0f, 1.0f));
			else
				hitProvider->set("diffuseColor", Vector4::create(1.0f, 1.0f, 1.0f, 1.0f));
		}
	);

	auto resized = canvas->resized()->connect([&](AbstractCanvas::Ptr canvas, uint w, uint h)
	{
		camera->component<PerspectiveCamera>()->aspectRatio((float)w / (float)h);
	});

	auto keyDown = canvas->keyboard()->keyDown()->connect([&](input::Keyboard::Ptr k)
	{
		if (k->keyIsDown(input::Keyboard::ScanCode::R))
			toogleRedBoxLayouts		= true;
		else if (k->keyIsDown(input::Keyboard::ScanCode::B))
			tooglePickingMask		= true;
		else if (k->keyIsDown(input::Keyboard::ScanCode::G))
			toogleRendererMask		= true;
		else if (k->keyIsDown(input::Keyboard::ScanCode::Q)) 
			toogleGreenBoxLayouts	= true;
	});

	auto enterFrame = canvas->enterFrame()->connect([&](Canvas::Ptr canvas, float time, float deltaTime)
	{
		auto distance = 0.f;
		auto ray = camera->component<PerspectiveCamera>()->unproject(
			canvas->mouse()->normalizedX(), canvas->mouse()->normalizedY()
		);

		if (tooglePickingMask || toogleRedBoxLayouts)
			root->removeChild(hit);

		if (toogleRedBoxLayouts)
		{
			redBoxLayoutsId		= (redBoxLayoutsId + 1) % 2;
			toogleRedBoxLayouts	= false;
			redBox->layouts(redBoxLayouts[redBoxLayoutsId]);
			
			if (redBox->layouts() == Layout::Group::IGNORE_RAYCASTING)
				std::cout << "The RED box ignores ray casting." << std::endl;
			else
				std::cout << "The RED box can be ray cast." << std::endl;
		}

		if (toogleGreenBoxLayouts)
		{
			greenBoxLayoutsId	= (greenBoxLayoutsId + 1) % 2;
			toogleGreenBoxLayouts	= false;
			greenBox->layouts(greenBoxLayouts[greenBoxLayoutsId]);

			std::cout << "green box layouts = " << std::bitset<32>(greenBox->layouts()) << std::endl;
		}

		if (tooglePickingMask)
		{
			pickingMaskId		= (pickingMaskId + 1) % 2;
			tooglePickingMask	= false;
			root->component<MousePicking>()->layoutMask(pickingMasks[pickingMaskId]);

			if (root->component<MousePicking>()->layoutMask() == blueBoxLayouts)
				std::cout << "The ray can only intersect the BLUE box." << std::endl;
			else
				std::cout << "The ray can intersect all boxes." << std::endl;
		}

		if (toogleRendererMask)
		{
			rendererMaskId		= (rendererMaskId + 1) % 2;
			toogleRendererMask	= false;
			camera->component<Renderer>()->layoutMask(rendererMasks[rendererMaskId]);

			if (camera->component<Renderer>()->layoutMask() == Layout::Mask::EVERYTHING)
				std::cout << "All boxes are rendered." << std::endl;
			else
				std::cout << "Only the GREEN box is rendered." << std::endl;
		}

		redBox->component<Transform>()->matrix()->prependRotationY(0.01f);
		greenBox->component<Transform>()->matrix()->prependRotationY(-0.01f);
		blueBox->component<Transform>()->matrix()->prependRotationY(-0.01f);

		root->component<MousePicking>()->pick(ray);

		sceneManager->nextFrame(time, deltaTime);
	});

	sceneManager->assets()->loader()->load();

	canvas->run();

	return 0;
}


