In this tutorial, we will see how to create custom materials. The code for this tutorial will be based on the code done in the [Create your first custom effect](../tutorial/17-Creating_a_custom_effect.md) tutorial.

Before we begin it is important to understand the difference between a `Material` - such as the `BasicMaterial` - and an `Effect` - such as the `Basic.effect`:

-   an `Effect` is a (set of) rendering function(s)/program(s) - also known as shaders - that will compute the final color of a pixel on the screen,
-   a `Material` is a set of key/value pairs that provide the input data for the `Effect` to work on.

If an `Effect` was a function, then the values provided by a `Material` would be its arguments. It also means that materials and effects are loosely coupled: despite the fact that a `BasicMaterial` is expected to work with a `Basic.effect`, you can use one without the other. For example, you could:

-   use the `BasicMaterial` with the `Phong.effect`, but you won't have code-hinting on Phong related material properties in your IDE;
-   or use a `PhongMaterial` with a `Basic.effect`, but then a lot of material properties would actually be available in the code but unused during rendering.

We've seen [how to create a (simple) custom effect](../tutorial/17-Creating_a_custom_effect.md) in the previous tutorial. One of the conclusions to this tutorial is that working with shader `uniform`s is not really scalable since it will set per-effect values instead of per-object. Thus, it is quite difficult to create large scenes sharing the same rendering code.

This tutorial explains how to create materials and bind them in our effects in order to get per-object rendering properties.

Step 1: Updating our effect
---------------------------

As is, our effect only declares per-effect `uniform`s that we have to set/update manually. To fix this, we will use `uniformBindings` in order to map each `uniform` name to a data binding property (in this case, available in the material):

```json
"uniforms" : {
	  "uModelToWorldMatrix"   : "modelToWorldMatrix",
	  "uWorldToScreenMatrix"  : { "binding" : { "property" : "worldToScreenMatrix", "source" : "renderer" } },
	  "uColor"				: "material[${materialUuid}].color"
},
```


This `uniformBinding` field can be declared in the pass object or directly at the root of our effect. Here is the complete code for our updated `MyCustomEffect.effect` file:

```json
{
  "name" : "MyCustomEffect",
    "attributes" : {
        "aPosition" : "geometry[${geometryUuid}].position"
		},
  "uniforms" : {
        "uModelToWorldMatrix"   : "modelToWorldMatrix",
        "uWorldToScreenMatrix"  : { "binding" : { "property" : "worldToScreenMatrix", "source" : "renderer" } },
		"uColor"				: "material[${materialUuid}].color"
		},
  "techniques" : [{
	  "passes" : [{
		"name" : "my-custom-pass",
		"vertexShader" : "
		  #ifdef GL_ES
		  precision mediump float;
		  #endif

		  attribute vec3 aPosition;

		  uniform mat4 uModelToWorldMatrix;
		  uniform mat4 uWorldToScreenMatrix;

		  void main(void)
		  {
			gl_Position = uWorldToScreenMatrix * uModelToWorldMatrix * vec4(aPosition, 1.0);
		  }
		",
		"fragmentShader" : "
		  #ifdef GL_ES
		  precision mediump float;
		  #endif

		  uniform vec4 uColor;

		  void main(void)
		  {
			gl_FragColor = uColor;
		  }
		"
	  }]
  }]
}
```


The `uniformBindings` will affect:

-   the all the passes if declared the root of the effect;
-   a single pass when declared in this very pass object.

In this case, our effect as a single pass so declaring the `uniformBindings` at the effect root on in the pass object will make no difference. But if you write an effect with multiple passes, you might want to take this into consideration to have per-pass `uniformBindings`.

You can learn more about the `*.effect` files format in the [Effect files format reference](../article/Effect_file_format_reference.md) article.

Step 2: Setting up our custom material
--------------------------------------

Now that we've updated our `Effect` bindings, it will expect to find a "material.color" property in the `data::Container` provided by the `scene::Node::data()` property where we added our `Surface`.

In the very case of our `material.color` uniform binding, the rendering pipeline will expect:

-   a call to `material->data()->hasProperty("color")` to return `true`;
-   and a call to `material->data()->get("color")` to return the Vector4 object to set for the `uColor` uniform.

To make sure the rendering engine works as expected, we just have to make sure our `Material` object will indeed provide a `color` property:

```cpp
auto myCustomMaterial = material::Material::create();

if (myCustomMaterial->hasProperty("color"))
{
	// set "color" to red
	myCustomMaterial->set("color", vec4(1.f, 0.f, 0.f, 1.f));
}
```


Note that we set the `color` property using the `Material::set()` method: this method will take care of adding the `material.` prefix all by itself.

Step 3 (optional) : Creating a custom material class
----------------------------------------------------

Setting all the properties of a material can be quite difficult since developers have to open the corresponding `*.effect` file to read find all the relevant `uniformBindings` declarations. It would be much simpler to have an actual `Material`-derived class that eventually declare static setter methods for all those properties.

In our very case, we will create a `MyCustomMaterial` class that extends `Material` and declares a `color` setter in a `MyCustomMaterial.hpp` file:

```cpp
#include "minko/Common.hpp"
#include "minko/material/Material.hpp"

namespace minko
{
	namespace material
	{
		class MyCustomMaterial :
			public Material
		{
		public:
			typedef std::shared_ptr<MyCustomMaterial>	Ptr;

		public:
			inline static
			Ptr
			create(const std::string& name = "MyCustomMaterial")
			{
				return Ptr(new MyCustomMaterial(name));
			}

			inline
			void
			color(math::vec4 rgba)
			{
				set({ {"color", rgba} });
			}

		private:
			MyCustomMaterial(const std::string& name):
				Material(name)
			{
			}
		};

	}
}
```


Developers can now use your material as follow:

```cpp
auto myCustomMaterial = material::MyCustomMaterial::create();

myCustomMaterial->color(vec4(0.f, 1.f, 0.f, 1.f));

auto cube = scene::Node::create()->addComponent(Surface::create(
	geometry::CubeGeometry::create(),
	myCustomMaterial,
	myCustomEffect
	));
```


Final code
----------

asset/effect/MyCustomEffect.effect
```json
{
  "name" : "MyCustomEffect",
    "attributes" : {
        "aPosition" : "geometry[${geometryUuid}].position"
		},
  "uniforms" : {
        "uModelToWorldMatrix"   : "modelToWorldMatrix",
        "uWorldToScreenMatrix"  : { "binding" : { "property" : "worldToScreenMatrix", "source" : "renderer" } },
		"uColor"				: "material[${materialUuid}].color"
		},
  "techniques" : [{
	  "passes" : [{
		"name" : "my-custom-pass",
		"vertexShader" : "
		  #ifdef GL_ES
		  precision mediump float;
		  #endif

		  attribute vec3 aPosition;

		  uniform mat4 uModelToWorldMatrix;
		  uniform mat4 uWorldToScreenMatrix;

		  void main(void)
		  {
			gl_Position = uWorldToScreenMatrix * uModelToWorldMatrix * vec4(aPosition, 1.0);
		  }
		",
		"fragmentShader" : "
		  #ifdef GL_ES
		  precision mediump float;
		  #endif

		  uniform vec4 uColor;

		  void main(void)
		  {
			gl_FragColor = uColor;
		  }
		"
	  }]
  }]
}
```


src/main.cpp
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

#include "MyCustomMaterial.hpp"

using namespace minko;
using namespace minko::math;
using namespace minko::component;

const math::uint WINDOW_WIDTH = 800;
const math::uint WINDOW_HEIGHT = 600;

int
main(int argc, char** argv)
{
	auto canvas = Canvas::create("Minko Tutorial - Create custom material", WINDOW_WIDTH, WINDOW_HEIGHT);

	auto sceneManager = component::SceneManager::create(canvas);

	sceneManager->assets()->loader()
		->queue("effect/MyCustomEffect.effect");

	auto root = scene::Node::create("root")
		->addComponent(sceneManager);

	auto camera = scene::Node::create("camera")
		->addComponent(Renderer::create(0x00000000))
		->addComponent(Transform::create(inverse(lookAt(vec3(0.f, 1.f, 2.f), vec3(), vec3(0.f, 1.f, 0.f)))))
		->addComponent(PerspectiveCamera::create(canvas->aspectRatio()));

	auto cube = scene::Node::create("cube")
		->addComponent(Transform::create());

	root->addChild(cube);
	root->addChild(camera);

	auto complete = sceneManager->assets()->loader()->complete()->connect([&](file::Loader::Ptr loader)
	{
		auto customMaterial = material::MyCustomMaterial::create();
		customMaterial->color(vec4(0.f, 1.f, 0.f, 1.f));

		auto myCustomEffect = sceneManager->assets()->effect("effect/MyCustomEffect.effect");

		cube->addComponent(Surface::create(
			geometry::CubeGeometry::create(canvas->context()),
			customMaterial,
			myCustomEffect
			));
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
