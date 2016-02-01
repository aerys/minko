Unlike [point lights](../tutorial/27-Working_with_point_lights.md), which occupy a specific location in the 3D scene, a directional light doesn't have a position, ONLY a direction. Rays of from directional lights run parallel in a single direction from every point in the sky, and are typically used to simulate direct light.

Because a directional light represents a distant light source, its x,y,z coordinate means nothing—only its rotational attribute has any bearing on how the scene will be illuminated.

![](../../doc/image/Directionallight.jpg "../../doc/image/Directionallight.jpg")

Directional light emitting from the left to the right

![](../../doc/image/DirectionallightExample.jpg "../../doc/image/DirectionallightExample.jpg")

As you can see in this more complex example, the directional light comes from a direction, not a specific location. it's particularly visible on the column shadow.

Step 1: Instantiate a directional light
---------------------------------------

The first very first thing you need to do is to include the following header to your .cpp file.

```cpp
#include <DirectionalLight.hpp>
```


To create a new [DirectionalLight](http://doc.v3.minko.io/reference/classminko_1_1component_1_1_directional_light.html) object, we simply use the `DirectionalLight::create` static method.

Step 2: Adding a directional light to the scene
-----------------------------------------------

To add a point light to a scene, you have to understand that a `Scene` is a graph composed of one or more `Node`. Those nodes can contain elements that are called `Component`.

As `DirectionalLight` is a `Component` you'll have to follow this logic to create and add a light to a scene.

```cpp


 // Creating the Node
 auto directionalLightNode = scene::Node::create("directionalLight");

 // Adding the Component DirectionalLight
 directionalLightNode->addComponent(DirectionalLight::create());

  // Adding the Node to the root of the scene graph
 root->addChild(directionalLightNode);

```


As you can see in the code above, we first create a node named "directionalLight" to which we add an instance of a directional light. Of course you can chain those methods to perform all in one line of code

```cpp


 // Creating the Node & adding the Component DirectionalLight
 auto directionalLightNode = scene::Node::create("directionalLight")->addComponent(DirectionalLight::create());

```


Step 3: Change directional light properties
-------------------------------------------

you can directly change way the light appears by setting parameters, for example :
```cpp


 auto directionalLight = DirectionalLight::create();

 directionalLight->component<DirectionalLight>()
 		->diffuse(.4f)
 		->color(vec3(0.f, 1.f, 0.f));

```


Step 4: Adjust a directional light
----------------------------------

Once the light is created you might want to give a direction to your directional light. To do this you need to add a transformation matrix to the Node holding your light.

```cpp
auto directionalLightNode = scene::Node::create("directionalLight")
       ->addComponent(DirectionalLight::create())
       ->addComponent(Transform::create(inverse(lookAt(vec3(15.f, 20.f, 0.f), vec3(), vec3(0.f, 1.f, 0.f)))));

```


Step 5: Remove a directional light from the scene
-------------------------------------------------

You may sometime need to remove a light from a scene, to do this you simply need to remove the directional light from the `Node` it has been added to before.

```cpp


 // Adding an directional light
 auto directionalLightNode = scene::Node::create("directionalLight")->addComponent(DirectionalLight::create());

 // retrieving the component
 auto directionalLight = directionalLightNode->component<DirectionalLight>(0);

 // Remove the component from the node
 directionalLightNode->removeComponent(directionalLight);

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

using namespace minko;
using namespace minko::math;
using namespace minko::component;

const math::uint WINDOW_WIDTH = 800;
const math::uint WINDOW_HEIGHT = 600;

int	main(int argc, char** argv)
{
	auto canvas = Canvas::create("Tutorial - Working with directional light", WINDOW_WIDTH, WINDOW_HEIGHT);
	auto sceneManager = component::SceneManager::create(canvas);

	sceneManager->assets()->loader()
		->queue("effect/Phong.effect");

	auto root = scene::Node::create("root")
		->addComponent(sceneManager);

	auto camera = scene::Node::create("camera")
		->addComponent(Renderer::create(0x00000000))
		->addComponent(Transform::create(inverse(lookAt(vec3(0.f, 1.f, 1.3f), vec3(), vec3(0.f, 1.f, 0.f)))))
		->addComponent(PerspectiveCamera::create((float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, (float)M_PI * 0.25f, .1f, 1000.f));

	auto directionalLight = scene::Node::create("directionalLight")
		->addComponent(DirectionalLight::create())
		->addComponent(Transform::create(inverse(lookAt(vec3(15.f, 20.f, 0.f), vec3(), vec3(0.f, 1.f, 0.f)))));
	directionalLight->component<DirectionalLight>()
		->diffuse(.4f)
		->color(vec3(0.f, 1.f, 0.f));

	auto ambientLight = scene::Node::create("ambientLight")
		->addComponent(AmbientLight::create(.2f));

	ambientLight->component<AmbientLight>()->color(vec3(1.f, 0.f, 0.f));

	root->addChild(directionalLight);
	root->addChild(ambientLight);
	root->addChild(camera);

	auto sphere = scene::Node::create("sphere");

	auto complete = sceneManager->assets()->loader()->complete()->connect([&](file::Loader::Ptr loader)
	{
		auto phongMaterial = material::PhongMaterial::create();

		phongMaterial->diffuseColor(0xff0000ff);
		phongMaterial->specularColor(0xffffffff);
		phongMaterial->shininess(16.0f);

		sphere->addComponent(Transform::create());
		sphere->addComponent(Surface::create(
			geometry::SphereGeometry::create(sceneManager->assets()->context()),
			phongMaterial,
			sceneManager->assets()->effect("effect/Phong.effect")
			));
		root->addChild(sphere);

	});

	sceneManager->assets()->loader()->load();

	auto enterFrame = canvas->enterFrame()->connect([&](Canvas::Ptr canvas, float t, float dt)
	{
		sceneManager->nextFrame(t, dt);
	});

	canvas->run();

	return 0;
}
```
