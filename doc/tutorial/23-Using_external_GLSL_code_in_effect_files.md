In this tutorial, we will see how to reference external GLSL files in `*.effect` files. This mechanism is useful for two reasons:

-   it eases the integration of 3rd party GLSL code;
-   it makes your `*.effect` files cleaner by externalizing the shader code.

To reference external GLSL code, we will use the `#pragma include` directive within shader fields.

As an example, we will break apart the custom effect that we previously set up in the tutorial [Creating a custom effect](../tutorial/17-Creating_a_custom_effect.md).

Step 1: Referencing the external GLSL files
-------------------------------------------

To reference our external `MyCustomEffect.vertex.glsl` and `MyCustomEffect.fragment.glsl` files, we will use the `#pragma include` directive within the respective shader fields:

```json
{
	"name" : "MyCustomEffect",
	"techniques" : [{
		"passes" : [{
			"vertexShader" : "#pragma include \"MyCustomEffect.vertex.glsl\"",
			"fragmentShader" : "#pragma include \"MyCustomEffect.fragment.glsl\""
		}]
	}]
}
```


In the code above, `MyCustomShader.vertex.glsl` and `MyCustomShader.fragment.glsl` are expected to be located in the same directory as the `MyCustomEffect.effect` file.

The effect of the `#pragma include` directive is pretty much the same as the `#include` C/C++ pre-processor macro: the code from the included file(s) are copy/pasted directly.

Step 2 (optional): Binding the uniforms
---------------------------------------

As you can imagine, every GLSL code is different... so you'll want to adapt this part of the tutorial to the actual uniforms declared by the GLSL code/shader you're including in your effect.

You also have to remember that you can always declaring some `uniformBindings` in your `*.effect` files.

To learn how to setup `uniformBindings`, you can read the following tutorials:

-   [Creating custom materials](../tutorial/18-Creating_custom_materials.md)
-   [Binding the model to world transform](../tutorial/19-Binding_the_model_to_world_transform.md)
-   [Binding the camera](../tutorial/20-Binding_the_camera.md)

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
		"uColor"				: "material[${materialUuid}].myColor"
		},
    "techniques" : [{
	    "passes" : [{
        	"vertexShader" : "#pragma include \"MyCustomEffect.vertex.glsl\"",
        	"fragmentShader" : "#pragma include \"MyCustomEffect.fragment.glsl\""
	    }]
    }]
}
```


asset/effect/MyCustomEffect.vertex.glsl
```c
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
```


asset/effect/MyCustomEffect.fragment.glsl
```c
#ifdef GL_ES
	precision mediump float;
#endif

uniform vec4 uColor;

void main(void)
{
	gl_FragColor = uColor;
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

using namespace minko;
using namespace minko::math;
using namespace minko::component;

const math::uint WINDOW_WIDTH = 800;
const math::uint WINDOW_HEIGHT = 600;

int
main(int argc, char** argv)
{
	auto canvas = Canvas::create("Minko Tutorial - Using external GLSL code in effect files", WINDOW_WIDTH, WINDOW_HEIGHT);

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
		auto material = material::BasicMaterial::create();

		auto myCustomEffect = sceneManager->assets()->effect("effect/MyCustomEffect.effect");
		material->data()->set("myColor", vec4(1.f, 0.f, 0.f, 1.f));

		cube->addComponent(Surface::create(
			geometry::CubeGeometry::create(canvas->context()),
			material,
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
