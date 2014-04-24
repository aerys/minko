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
#include "minko/MinkoDevIL.hpp"
#include "minko/MinkoSDL.hpp"

using namespace minko;
using namespace minko::component;
using namespace minko::math;

const std::string TEXTURE_JPG = "texture/bricks.jpg";
const std::string TEXTURE_PNG = "texture/bricks.png";
const std::string TEXTURE_TGA = "texture/bricks.tga";
const std::string TEXTURE_TIF = "texture/bricks.tif";
const std::string TEXTURE_BMP = "texture/bricks.bmp";
const std::string TEXTURE_PSD = "texture/bricks.psd";
const std::string TEXTURE_DDS = "texture/bricks.dds";

std::vector<minko::render::AbstractTexture::Ptr> textures;

int main(int argc, char** argv)
{
	auto canvas = Canvas::create("Minko Example - DevIL", 800, 600);

	auto sceneManager = SceneManager::create(canvas->context());

	// setup assets
	sceneManager->assets()->loader()->options()->generateMipmaps(true);
	sceneManager->assets()->loader()
		->queue(TEXTURE_JPG)
		->queue(TEXTURE_PNG)
		->queue(TEXTURE_TGA)
		->queue(TEXTURE_TIF)
		->queue(TEXTURE_BMP)
		->queue(TEXTURE_PSD)
		->queue(TEXTURE_DDS)
		->queue("effect/Basic.effect");

	std::vector<std::string> extensions = file::DevILParser::getSupportedFileExensions();

	for (uint i = 0; i < extensions.size(); i++)
	{
		std::string extension = extensions[i];
		sceneManager->assets()->loader()->options()->registerParser<file::DevILParser>(extension);
	}

	sceneManager->assets()->geometry("cube", geometry::CubeGeometry::create(sceneManager->assets()->context()));
    
	auto root = scene::Node::create("root")
		->addComponent(sceneManager);

	auto mesh = scene::Node::create("mesh")
		->addComponent(Transform::create());
	root->addChild(mesh);

	auto camera = scene::Node::create("camera")
		->addComponent(Renderer::create(0x7f7f7fff))
		->addComponent(Transform::create(
		Matrix4x4::create()->lookAt(Vector3::zero(), Vector3::create(0.f, 0.f, 3.f))
		))
		->addComponent(PerspectiveCamera::create(800.f / 600.f, (float)PI * 0.25f, .1f, 1000.f));
	root->addChild(camera);

	int frames = 0;

	auto _ = sceneManager->assets()->loader()->complete()->connect([=](file::Loader::Ptr loader)
	{
		textures.push_back(sceneManager->assets()->texture(TEXTURE_JPG));
		textures.push_back(sceneManager->assets()->texture(TEXTURE_PNG));
		textures.push_back(sceneManager->assets()->texture(TEXTURE_TGA));
		textures.push_back(sceneManager->assets()->texture(TEXTURE_TIF));
		textures.push_back(sceneManager->assets()->texture(TEXTURE_BMP));
		textures.push_back(sceneManager->assets()->texture(TEXTURE_PSD));
		textures.push_back(sceneManager->assets()->texture(TEXTURE_DDS));

		mesh->addComponent(Surface::create(
				geometry::CubeGeometry::create(sceneManager->assets()->context()),
				material::BasicMaterial::create()->diffuseMap(textures[0]),
				sceneManager->assets()->effect("effect/Basic.effect")
			));
	});

	auto resized = canvas->resized()->connect([&](AbstractCanvas::Ptr canvas, uint w, uint h)
	{
		camera->component<PerspectiveCamera>()->aspectRatio((float)w / (float)h);
	});

	auto enterFrame = canvas->enterFrame()->connect([&](Canvas::Ptr canvas, float time, float deltaTime)
	{
		mesh->component<Transform>()->matrix()->appendRotationY(.01f);

		sceneManager->nextFrame(time, deltaTime);

		if (textures.size() > 0)
		{
			frames++;

			int i = frames % textures.size();
			mesh->component<Surface>()->material()->set("diffuseMap", textures[i]);
		}
	});

	sceneManager->assets()->loader()->load();
	canvas->run();

	return 0;
}


