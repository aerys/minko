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

Vector4::Ptr
generateColor()
{
	return Color::hslaToRgba(rand() / (float)RAND_MAX, 0.75f, 0.5f);
}

uint
generateHexColor()
{
	Vector4::Ptr color = generateColor();

	const unsigned char r = (unsigned char)floorf(255.0f*color->x());
	const unsigned char g = (unsigned char)floorf(255.0f*color->y());
	const unsigned char b = (unsigned char)floorf(255.0f*color->z());
	const unsigned char a = (unsigned char)floorf(255.0f*color->w());

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
				Material::create()
					->set("diffuseColor",	generateColor())
					->set("colorMask",		true)
					->set("depthMask",		false)
					->set("depthFunc",		CompareMode::ALWAYS)
					->set("stencilFunc",	CompareMode::EQUAL)
					->set("stencilRef",		1)
					->set("stencilMask",	(unsigned int)0xff)
					->set("stencilFailOp",	StencilOperation::KEEP),
				assets->effect("basic")
			));

		const float minX	= -1.0f;
		const float rangeX	= 1.0f - minX;
		const float minY	= -1.0f;
		const float rangeY	= 1.0f - minY;

		starNodes[i]->component<Transform>()->matrix()
			->appendScale(0.25f, 0.25f, 0.25f)
			->appendRotationZ(2.0f * (float)PI * (rand() / (float)RAND_MAX))
			->appendTranslation(
				minX + (rand() / (float)RAND_MAX)*rangeX, 
				minY + (rand() / (float)RAND_MAX)*rangeY,
				0.0f
			);
	}
}

int
main(int argc, char** argv)
{
	auto canvas = Canvas::create("Minko Example - Stencil", 800, 600, true);
	auto sceneManager = SceneManager::create(OpenGLES2Context::create());
	
	// setup assets
	sceneManager->assets()
		->geometry("bigStar",	StarGeometry::create(sceneManager->assets()->context(), 5, 0.5f, 0.325f))
		->geometry("smallStar",	StarGeometry::create(sceneManager->assets()->context(), 5, 0.5f, 0.25f))
		->geometry("quad",		QuadGeometry::create(sceneManager->assets()->context()))
		->queue("effect/Basic.effect");

	clock_t					startTime = clock();
	unsigned int			numSmallStars	= 30;
	std::vector<Node::Ptr>	smallStars;

	auto root = Node::create("root")
		->addComponent(sceneManager);

	auto camera	= Node::create("camera")
		->addComponent(Renderer::create(generateHexColor()))
		->addComponent(PerspectiveCamera::create(800.0f / 600.0f, (float)PI * 0.25f, 0.1f, 1000.0f))
		->addComponent(Transform::create());
	camera->component<Transform>()->matrix()
		->lookAt(Vector3::zero(), Vector3::create(0.f, 0.f, 3.f));

	auto bigStarNode = Node::create("bigStarNode")
			->addComponent(Transform::create());

	auto quadNode = Node::create("quadNode")
			->addComponent(Transform::create());

	root->addChild(camera);

	auto _ = sceneManager->assets()->complete()->connect([&](file::AssetLibrary::Ptr assets)
	{
		bigStarNode
			->addComponent(Surface::create(
				assets->geometry("bigStar"),
				Material::create()
					->set("diffuseColor",		Vector4::create(1.0f, 1.0f, 1.0f, 1.0f))
					->set("depthMask",			false)
					->set("depthFunc",			CompareMode::ALWAYS)
					->set("colorMask",			false)
					->set("stencilFunc",		CompareMode::NEVER)
					->set("stencilRef",			1)
					->set("stencilMask",		(unsigned int)0xff)
					->set("stencilFailOp",		StencilOperation::REPLACE)
					->set("triangleCulling",	TriangleCulling::BACK),
				assets->effect("basic")
			));
		bigStarNode->component<Transform>()->matrix()->appendScale(2.5f, 2.5f, 2.5f);

		quadNode
			->addComponent(Surface::create(
				assets->geometry("quad"),
				Material::create()
					->set("diffuseColor",		generateColor())
					->set("depthMask",			false)
					->set("depthFunc",			CompareMode::ALWAYS)
					->set("colorMask",			true)
					->set("stencilFunc",		CompareMode::EQUAL)
					->set("stencilRef",			1)
					->set("stencilMask",		(unsigned int)0xff)
					->set("stencilFailOp",		StencilOperation::KEEP)
					->set("triangleCulling",	TriangleCulling::BACK),
				assets->effect("basic")
			));
		quadNode->component<Transform>()->matrix()
			->appendScale(4.0f, 4.0f, 4.0f);

		generateStars(numSmallStars, assets, smallStars);

		// stencil writting pass
		root->addChild(bigStarNode);
		// stencil fetching pass
		root->addChild(quadNode);
		for (auto& star : smallStars)
			root->addChild(star);
	});

		
	auto enterFrame = canvas->enterFrame()->connect([&](Canvas::Ptr canvas, float time, float deltaTime)
	{
		bigStarNode->component<Transform>()->matrix()->appendRotationZ(.001f);
		for (auto& star : smallStars)
			star->component<Transform>()->matrix()
			->prependRotationZ(-0.025f);

		sceneManager->nextFrame(time, deltaTime);
	});

	sceneManager->assets()->load();
	
	canvas->run();
	
	return 0;
}
