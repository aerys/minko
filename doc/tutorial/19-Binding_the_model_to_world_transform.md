In this tutorial, we will see how to use the 3D transforms available in the engine in our custom effects. In the [Create your first custom effect](../tutorial/17-Creating_a_custom_effect.md) tutorial, we've seen how to declare `uniform`s and set them from our application code. But having to manually set each uniform is not really scalable, especially regarding 3D transforms and the camera because this kind of data is likely to change at every frame. Yet, most of our objects will need them in order to be renderer properly.

Step 1: Binding the model to world matrix
-----------------------------------------

In the [Moving objects](../tutorial/04-Moving_objects.md) tutorial, we've seen that we can add custom 3D transforms to our scene nodes using the `Transform` component. If you take a look at the code for the `Transform::Transform()` constructor, you'll notice that this very component declares the `modelToWorldMatrix` in it's `data::Provider`:

```cpp
//Transform.cpp
void Transform::Transform()
{
	// some other code...

	_data->set<math::mat4>("modelToWorldMatrix", math::mat4(1.f));
	_modelToWorld = _data->getUnsafePointer<math::mat4>("modelToWorldMatrix");
}
```


When the `Transform` component is added to some target `Node`, it will add its `data::Provider` to this `Node`'s `data::Container`. The immediate result is that the `modelToWorldMatrix` is then available and can be bound in the `uniforms` of our effect:

```json
"uniforms" : {

	"uModelToWorldMatrix" : "modelToWorldMatrix"

}
```

If we add this to the code from the [Creating custom materials](../tutorial/18-Creating_custom_materials.md) tutorial, we end up with the following code for our effect:

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


You can learn more about the `*.effect` files format in the [Effect files format reference](../article/Effect_file_format_reference.md) article. To learn more about data binding, please read the [Understanding data binding](../article/Understanding_data_binding.md) article.

Step 2: Udpating the application code
-------------------------------------

Because our `uModelToWorldMatrix` property is now bound to `transform.modelToWorldMatrix`, we have to update our application code to:

-   avoid setting `uModelToWorldMatrix` directly: it will be set automatically by data binding;
-   make sure our scene `Node` actually has a `Transform` component, otherwise the `uModelToWorldMatrix` will not be bound and rendering might be broken.

```cpp
auto cube = scene::Node::create()
	->addComponent(Transform::create(
			translate(math::vec3(0.f, 0.f, -10.f))
		))
	->addComponent(Surface::create(
			geometry::CubeGeometry::create(),
			myCustomMaterial,
			myCustomEffect
		));
```

Note how we initialize our `Transform` component with the translation `math::mat4` we used to use as the value to set `uModelToWorldMatrix`. But thanks to data binding, this `uniform` will now be automatically be updated everytime we change the `Transform::transform()` property.

You can read more about the `Transform` component in the [Moving objects](../tutorial/04-Moving_objects.md) tutorial.

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
	auto canvas = Canvas::create("Minko Tutorial - Binding the model to world transform", WINDOW_WIDTH, WINDOW_HEIGHT);

	auto sceneManager = component::SceneManager::create(canvas);

	sceneManager->assets()->loader()->queue("effect/MyCustomEffect.effect");

	auto root = scene::Node::create("root")
		->addComponent(sceneManager);

	auto camera = scene::Node::create("camera")
		->addComponent(Renderer::create(0x00000000))
		->addComponent(Transform::create(inverse(lookAt(vec3(0.f, 1.f, 2.f), vec3(), vec3(0.f, 1.f, 0.f)))))
		->addComponent(PerspectiveCamera::create(canvas->aspectRatio()));

	auto cube = scene::Node::create("cube")
		->addComponent(Transform::create(
				translate(vec3(0.f, 0.f, 0.f))
			));

	root->addChild(camera);

	auto complete = sceneManager->assets()->loader()->complete()->connect([&](file::Loader::Ptr loader)
	{
		auto myCustomMaterial = material::MyCustomMaterial::create();

		cube->addComponent(Surface::create(
			geometry::CubeGeometry::create(canvas->context()),
			myCustomMaterial,
			sceneManager->assets()->effect("effect/MyCustomEffect.effect")
			));

		myCustomMaterial->color(vec4(1.f, 0.f, 0.f, 1.f));

		root->addChild(cube);
	});

	auto enterFrame = canvas->enterFrame()->connect([&](Canvas::Ptr canvas, float t, float dt)
	{
		auto transform = cube->component<Transform>();
		transform->matrix(transform->matrix() * rotate(.01f, vec3(0.f, 1.f, 0.f)));

		sceneManager->nextFrame(t, dt);
	});

	sceneManager->assets()->loader()->load();

	canvas->run();

	return 0;
}
```
