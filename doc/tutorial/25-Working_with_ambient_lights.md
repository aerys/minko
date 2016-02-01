An ambient light emits soft light rays in every direction, it can be used to elevate the overall level of diffuse illumination in a scene. It's important to understand that it has no specific directionality, and therefore casts no ground shadow.

![](../../doc/image/Ambientlight.jpg "../../doc/image/Ambientlight.jpg")

Ambient light emitting in all direction. no ground shadows are displayed

The most important setting about ambient light is its diffuse level; in the following picture you can see the same scene with three different diffuse settings.

![](../../doc/image/AmbientlightExample.jpg "../../doc/image/AmbientlightExample.jpg")

Step 1: Instantiate an light
----------------------------

The first very first thing you need to do is to include the following header to your .cpp file.

```cpp
#include <AmbientLight.hpp>
```


To create a new [AmbientLight](http://doc.v3.minko.io/reference/classminko_1_1component_1_1_ambient_light.html) object, we simply use the `AmbientLight::create()` static method (alternatively you can specify the value of the ambient light intensity by passing a float to the create method, for example `AmbientLight::create(0.2f)`).

Step 2: Adding the light to the scene
-------------------------------------

To add a light to the scene, you have to understand that a `Scene` is a graph composed of one or more `Node`. Those nodes can contain elements that are called `Component`.

As an `AmbientLight` is a `Component` you'll have to follow this logic to create and add a light to a scene.

```cpp


 // Creating the Node
 auto ambientLightNode = scene::Node::create("ambientLight")

 // Adding the Component AmbientLight
 ambientLightNode->addComponent(AmbientLight::create(0.5f));

// Adding the Node containing the light component to the scene graph
 root->addChild(ambientLightNode);

```


As you can see in the code above, we first create a node named "ambientLight" to which we add an instance of an ambient light, then we can add this node to the scene graph. Of course you can chain methods to perform all in one line of code

```cpp


 // Creating the Node & adding the Component AmbientLight
auto ambientLight = scene::Node::create("ambientLight")
	->addComponent(AmbientLight::create(0.5f));

```


Step 3: Change ambient light properties
---------------------------------------

You can directly change way the light appears by setting parameters, for example :
```cpp
auto ambientLight = scene::Node::create("ambientLight")
   ->addComponent(AmbientLight::create(0.5f));

 // Set the light diffuse color
 ambientLight->component<AmbientLight>()->color(vec3(1.f, 0.f, 0.f));

```


Step 4: Removing a light from the scene
---------------------------------------

You may sometime need to remove a light from a scene, to do this you simply need to remove the ambient light from the `Node` it has been added to before.

```cpp


 // Adding an ambient light
 auto ambientLightNode = scene::Node::create("ambientLight")->addComponent(AmbientLight::create(0.5f));

 // retrieving the component
 auto ambientLight = ambientLightNode->component<AmbientLight>();

 // Remove the component from the node
 ambientLightNode->removeComponent(ambientLight);

```


As you can see above, the first step is to retrieve the light component with previously created and then remove it from the `Node`

Final Code
----------

```cpp
/*
Copyright (c) 2016 Aerys

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
#include "minko/MinkoPNG.hpp"

using namespace minko;
using namespace minko::math;
using namespace minko::component;

const math::uint WINDOW_WIDTH = 800;
const math::uint WINDOW_HEIGHT = 600;

const std::string MYTEXTURE = "texture/box.png";

int	main(int argc, char** argv)
{
	auto canvas = Canvas::create("Tutorial - Working with ambient light", WINDOW_WIDTH, WINDOW_HEIGHT);
	auto sceneManager = component::SceneManager::create(canvas);

	sceneManager->assets()->loader()->options()
		->registerParser<file::PNGParser>("png");

	sceneManager->assets()->loader()
		->queue(MYTEXTURE)
		->queue("effect/Phong.effect");

	auto root = scene::Node::create("root")
		->addComponent(sceneManager);

	auto camera = scene::Node::create("camera")
		->addComponent(Renderer::create(0x00000000))
		->addComponent(Transform::create(inverse(lookAt(vec3(0.f, 1.f, 1.3f), vec3(), vec3(0.f, 1.f, 0.f)))))
		->addComponent(PerspectiveCamera::create((float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, (float)M_PI * 0.25f, .1f, 1000.f));

	auto ambientLight = scene::Node::create("ambientLight")
		->addComponent(AmbientLight::create(.2f));

	ambientLight->component<AmbientLight>()->color(vec3(1.f, 0.f, 0.f));

	root->addChild(ambientLight);
	root->addChild(camera);

	auto cube = scene::Node::create("cube");

	auto complete = sceneManager->assets()->loader()->complete()->connect([&](file::Loader::Ptr loader)
	{
		auto cubeMaterial = material::PhongMaterial::create();

		cubeMaterial->diffuseMap(sceneManager->assets()->texture(MYTEXTURE));

		cube->addComponent(Transform::create());
		cube->addComponent(Surface::create(
			geometry::CubeGeometry::create(sceneManager->assets()->context()),
			cubeMaterial,
			sceneManager->assets()->effect("effect/Phong.effect")
			));
		root->addChild(cube);

	});

	sceneManager->assets()->loader()->load();

	auto enterFrame = canvas->enterFrame()->connect([&](Canvas::Ptr canvas, float t, float dt)
	{
		auto transform = cube->component<Transform>();
		transform->matrix(transform->matrix() * rotate(.01f, vec3(0.f, 1.f, 0.f)));

		sceneManager->nextFrame(t, dt);
	});

	canvas->run();

	return 0;
}
```
