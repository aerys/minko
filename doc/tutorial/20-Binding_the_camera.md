In the previous tutorial, we've seen [how to bind the model to world matrix](../tutorial/19-Binding_the_model_to_world_transform.md) in our effects. Using the same binding mechanism, we can bind the camera properties. Especially the "view matrix", which is used to transform "global" world-space 3D vertices into the camera relative view-space.

To do this, we will use data binding to :

-   update our effects' `uniforms` to bind the `uViewMatrix` uniform to a property provided by the `PerspectiveCamera` component;
-   remove `uViewMatrix` related code, since this uniform will then be handled automatically by data binding.

Step 1: Binding the view matrix
-------------------------------

If you read the code for the `PerspectiveCamera::PerspectiveCamera()` constructor, you'll see that the `PerspectiveCamera` component declares the following properties:

-   `position`
-   `viewMatrix`
-   `projectionMatrix`
-   `worldToScreenMatrix`

Those properties are declared in the `PerspectiveCamera::data()` provider, which is a `data::StructureProvider` with "camera" as a structure name.

Here is how we can bind the camera transform and projection in our effect using `uniforms` bindings:

```json
"uniformBindings" : {

 "uViewMatrix" : { "binding" : { "property" : "viewMatrix", "source" : "renderer" } },
 "uProjectionMatrix" : { "binding" : { "property" : "projectionMatrix", "source" : "renderer" } }

}
```


If you read the [Binding the model to world transform](../tutorial/19-Binding_the_model_to_world_transform.md) tutorial, you'll notice that the `uniformBindings` we declare here are a bit different. This is because we have to declare where our bindings properties should be read from. Our `uniforms` declare the following fields:

-   `property`: the name of the data property that should be set in our application;
-   `source`: where the data should be read from.

The `source` declaration can have three values:

-   `target`: the bound values should be read from the node that will be hosting the `Surface` component;
-   `renderer`: the bound values should be read from the node that hosts the `Renderer` component;
-   `root`: the bound values should be read from the scene root node.

By default, the `source` field is set to `target`. But in the case of our camera, the `PerspectiveCamera` is located on the same node than the `Renderer`. Thus, we set the `source` field to `renderer`. To learn more about data binding and binding sources, please read the [Understanding data binding](../article/Understanding_data_binding.md) article.

The `PerspectiveCamera` also provides the `worldToScreenMatrix`, which is the result of the view matrix multiplied with the projection. Using this property will save us some computation in our vertex shader:

```json
{

	...

	"uniforms" : {
		"uModelToWorldMatrix" : "modelToWorldMatrix",
		"uWorldToScreenMatrix" : { "binding" : { "property" : "worldToScreenMatrix", "source" : "renderer" } },
		"uColor" : "material[${materialId}].color"
		},
	"techniques" : [{
		"passes" : [{
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
			...
		}]
	}]
}
```


You can learn more about the `*.effect` files format in the [Effect files format reference](../article/Effect_file_format_reference.md) article.

Step 2: Updating the application code
-------------------------------------

Now that our view matrix is bound, we don't have to set it manually. But we have to make sure the property it's bound to is actually available! As this property is declared by the `PerspectiveCamera` component, we have to make sure it is available somewhere in our scene.

```cpp
auto camera = scene::Node::create()
		->addComponent(Renderer::create(0x00000000))
		->addComponent(Transform::create(inverse(lookAt(vec3(0.f, 1.f, 2.f), vec3(), vec3(0.f, 1.f, 0.f)))))
		->addComponent(PerspectiveCamera::create(canvas->aspectRatio()));

root->addChild(camera);
```


Note that because we will likely need to move and orient our camera, we also add a `Transform` component to our `camera` scene node. To learn more about this component, you can read the [Moving objects](../tutorial/04-Moving_objects.md) tutorial.

**Attention!** Always make sure the `PerspectiveCamera` is added on the same node as the `Renderer` component that is supposed to perform the rendering operations. As every scene might have multiple cameras, their respective `data::Provider` has to be added to the `data::Container` of the same node than the `Renderer`. In short, the properties of a `PerspectiveCamera` will be available only to the `Renderer` that is on the same node; which makes it possible to have multiple camera/renderer in the same scene.

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
	auto canvas = Canvas::create("Minko Tutorial - Binding the camera", WINDOW_WIDTH, WINDOW_HEIGHT);

	auto sceneManager = component::SceneManager::create(canvas);

	auto root = scene::Node::create("root")
		->addComponent(sceneManager);

	auto camera = scene::Node::create()
		->addComponent(Renderer::create(0x00000000))
		->addComponent(Transform::create(inverse(lookAt(vec3(0.f, 1.5f, 2.f), vec3(), vec3(0.f, 1.f, 0.f)))))
		->addComponent(PerspectiveCamera::create(canvas->aspectRatio()));

	sceneManager->assets()->loader()->queue("effect/MyCustomEffect.effect");

	auto cube = scene::Node::create("cube")
		->addComponent(Transform::create(
				translate(vec3(0.f, 0.f, 0.f))
			));

	root->addChild(camera);

	auto complete = sceneManager->assets()->loader()->complete()->connect([&](file::Loader::Ptr loader)
	{
		auto myCustomEffect = sceneManager->assets()->effect("effect/MyCustomEffect.effect");
		auto myCustomMaterial = material::MyCustomMaterial::create();

		cube->addComponent(Surface::create(
				geometry::CubeGeometry::create(canvas->context()),
				myCustomMaterial,
				myCustomEffect
				));

		root->addChild(cube);

		myCustomMaterial->color(vec4(0.f, 1.f, 0.f, 1.f));
	});

	auto enterFrame = canvas->enterFrame()->connect([&](Canvas::Ptr canvas, float t, float dt)
	{
		auto transform = cube->component<Transform>();
		transform->matrix(transform->matrix() * rotate(-.01f, vec3(0.f, 1.f, 0.f)));

		sceneManager->nextFrame(t, dt);
	});

	sceneManager->assets()->loader()->load();

	canvas->run();

	return 0;
}
```
