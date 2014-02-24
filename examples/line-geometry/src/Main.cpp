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
#include "minko/MinkoSDL.hpp"

using namespace minko;
using namespace minko::scene;
using namespace minko::geometry;
using namespace minko::component;
using namespace minko::data;
using namespace minko::math;
using namespace minko::render;

struct Star
{
	Node::Ptr	node;
	float		angRate;
};

void
addStar(Node::Ptr, file::AssetLibrary::Ptr, std::vector<Star>&);

LineGeometry::Ptr
createStarLineGeometry(unsigned int,
						float inRadius,
						float outRadius,
						AbstractContext::Ptr);

int main(int argc, char** argv)
{
	std::cout << "Press [A] to add stars." << std::endl;

	auto canvas = Canvas::create("Minko Example - Line Geometry", 800, 600);

	auto sceneManager = SceneManager::create(canvas->context());
	sceneManager->assets()->queue("effect/Line.effect");

	auto root = scene::Node::create("root")
		->addComponent(sceneManager);

	auto camera = scene::Node::create("camera")
		->addComponent(Renderer::create(0x7f7f7fff))
		->addComponent(Transform::create(
			Matrix4x4::create()->lookAt(Vector3::zero(), Vector3::create(0.f, 0.f, 3.f))
		))
		->addComponent(PerspectiveCamera::create(800.f / 600.f, (float)PI * 0.25f, .1f, 1000.f));
	
	root->addChild(camera);
	root->data()->addProvider(canvas->data()); // FIXME

	std::vector<Star> stars;
	auto _ = sceneManager->assets()->complete()->connect([&](file::AssetLibrary::Ptr assets)
	{
		addStar(root, assets, stars);
	});

	auto keyDown = canvas->keyboard()->keyDown()->connect([&](input::Keyboard::Ptr k)
	{
		if (k->keyIsDown(input::Keyboard::ScanCode::A))
			addStar(root, sceneManager->assets(), stars);
	});

	auto resized = canvas->resized()->connect([&](AbstractCanvas::Ptr canvas, uint w, uint h)
	{
		camera->component<PerspectiveCamera>()->aspectRatio((float)w / (float)h);
	});

	auto enterFrame = canvas->enterFrame()->connect([&](Canvas::Ptr canvas, uint t, float dt)
	{
		for (auto& star : stars)
			star.node->component<Transform>()->matrix()->appendRotationY(star.angRate);

		sceneManager->nextFrame();
	});

	sceneManager->assets()->load();

	canvas->run();

	return 0;
}

void
addStar(Node::Ptr root, file::AssetLibrary::Ptr assets, std::vector<Star>& stars)
{
	const unsigned int MAX_NUM_STARS = 20;
	if (stars.size() == MAX_NUM_STARS)
	{
		std::cerr << "cannot add more stars." << std::endl;
		return;
	}
	
	unsigned int	numBranches = 4 + rand() % 4;
	float			outRadius   = 0.9f + 0.2f * (rand() / (float)RAND_MAX - 0.5f);
	float			inRadius	= outRadius * (0.5f + 0.1f * (rand() / (float)RAND_MAX - 0.5f));
	
	auto starNode = Node::create("star_" + std::to_string(stars.size()))
		->addComponent(Surface::create(
			createStarLineGeometry(numBranches, inRadius, outRadius, assets->context()),
			ArrayProvider::create("material")
				->set("diffuseColor",	Color::hslaToRgba(rand() / (float)RAND_MAX, 0.75f, 0.6f, 1.0f))
				->set("lineThickness",	1.0f + 3.0f * (rand() / (float)RAND_MAX)),
			assets->effect("line")
		))
		->addComponent(Transform::create(Matrix4x4::create()->appendRotationZ(2.0f * (float)PI * rand() / (float)RAND_MAX)));
	
	stars.push_back(Star());
	
	stars.back().node		= starNode;
	stars.back().angRate	= 0.01f +  0.05f * (rand() / (float)RAND_MAX);
	
	root->addChild(stars.back().node);
}

LineGeometry::Ptr
createStarLineGeometry(unsigned int numStarBranches,
						float inRadius,
						float outRadius,
						AbstractContext::Ptr context)
{
	if (context == nullptr)
		throw std::invalid_argument("context");
	
	const unsigned int	numBranches = std::max((unsigned int)3, numStarBranches);
	const float			innerRadius	= std::min(inRadius, outRadius);
	const float			outerRadius	= std::max(inRadius, outRadius);
	const float			cStep		= cosf((float)PI / (float)numBranches);
	const float			sStep		= sinf((float)PI / (float)numBranches);

	LineGeometry::Ptr	lines		= LineGeometry::create(context);

	float				cAng		= 1.0f;
	float				sAng		= 0.0f;
	for (unsigned int i = 0; i < numBranches; ++i)
	{
		const float	outX	= outerRadius * cAng;
		const float	outY	= outerRadius * sAng;

		if (lines->numLines() > 0)
			lines->lineTo(outX, outY, 0.0f, 2);
		else
			lines->moveTo(outX, outY, 0.0f);

		float	c	= cAng*cStep - sAng*sStep;
		float	s	= sAng*cStep + cAng*sStep;
		cAng		= c;
		sAng		= s;
		
		const float	inX		= innerRadius * cAng;
		const float	inY		= innerRadius * sAng;

		lines->lineTo(inX, inY, 0.0f, 2);

		c		= cAng*cStep - sAng*sStep;
		s		= sAng*cStep + cAng*sStep;
		cAng	= c;
		sAng	= s;
	}
	lines->lineTo(outerRadius, 0.0f, 0.0f, 2);

	return lines;
}