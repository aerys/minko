**We recommand you to read the following tutorials before starting this one:**

-   [Create your first custom effect](../tutorial/17-Creating_a_custom_effect.md)
-   [Binding the model to world transform](../tutorial/19-Binding_the_model_to_world_transform.md)
-   [Binding the camera](../tutorial/20-Binding_the_camera.md)

In this tutorial, we will see how to create "über shaders". Über shaders are rendering programs that can handle many different rendering scenarios. For example, a lighting über shader will be able to handle many different counts and types of lights. An even simpler scenario is whether we want to render using the vertex color, a solid color or a texture.

To learn how to create an über-shader, we will update the effect created in the [Create your first custom effect](../tutorial/17-Creating_a_custom_effect.md) tutorial to be able to render two scenarios: # rendering with a solid color; # rendering with a texture.

Step 0: Introduction to über shaders
------------------------------------

What's important to understand is how critic it is to be able to write über shaders: all 3D scenes are different and - as each rendering program is fixed - each rendering scenario would require to write a specific shader to be rendered properly. As your scene gets more and more complex, the number of scenarios augments exponentially. For example, a scene with up to 10 lights with 4 different types (ambient, directional, spot or point) and normal mapping (on or off) can lead to up to (4 * 10) ^ 2 = 1 600 different shaders! Writing each of them is not a scalable solution...

That's where über shaders kick in: an über shader is a single shader program that will adapt its operations according to whether some options are enabled or not. To do this, Minko leverages the GLSL pre-processor:

```c
#ifdef SOME_OPTION
// do something...
#else
// do something else...
#endif
```


In the case above, the actual behavior of the shader will depend on whether the `SOME_OPTION` macro is defined or not. Furthermore, this mechanism has no cost when rendering since the pre-processor is executed only once when the shader is compiled. Thus, properly using über-shaders can lead to massive performance optimizations.

Step 1: Updating the fragment shader
------------------------------------

We will take the fragment shader explained in the [Step 3 of the Create your first custom effect tutorial](../tutorial/17-Creating_a_custom_effect.md#step-3-the-fragment-shader) and update it to use a texture if the `DIFFUSE_MAP` macro is defined:

```c
#ifdef GL_ES
precision mediump float;
#endif

uniform vec4 uDiffuseColor;
uniform sampler2D uDiffuseMap;

varying vec2 vVertexUv;

void main(void) {

 #ifdef DIFFUSE_MAP
   gl_FragColor = texture2D(uDiffuseMap, vVertexUv);
 #else
   gl_FragColor = uDiffuseColor;
 #endif

}
```


To sample the `uDiffuseMap` texture, our fragment shader will also need the texture coordinates interpolated from the vertex data. Thus, we have to make sure the `vVertexUv` varying is properly filled by our vertex shader:

```c
#ifdef GL_ES
precision mediump float;
#endif

attribute vec3 aPosition;
attribute vec2 aUv;

uniform mat4 uModelToWorldMatrix;
uniform mat4 uWorldToScreenMatrix;

varying vec2 vVertexUv;

void main(void) {

 #ifdef DIFFUSE_MAP
   vVertexUv = aUv;
 #endif

 gl_Position = uWorldToScreenMatrix * uModelToWorldMatrix * vec4(aPosition, 1.0);

}
```


We've made some modifications in the fragment shader that required a minor update of our vertex shader. We will have to update our effect's `attributeBindings` to make sure the `aUv` vertex attribute is properly set:

```json
"attributes" : {
	"aPosition" : "geometry[${geometryUuid}].position",
	"aUv" : "geometry[${geometryUuid}].uv"
}
```


and the `uniforms` to make sure our `uDiffuseMap` property is properly bound too:

```json
"uniforms" : {
	"uDiffuseColor" : "material[${materialUuid}].diffuseColor",
	"uDiffuseMap" : "material[${materialUuid}].diffuseMap",
	"uModelToWorldMatrix" : "transform.modelToWorldMatrix",
	"uWorldToScreenMatrix"  : { "binding" : { "property" : "worldToScreenMatrix", "source" : "renderer" } },

}
```


This last step is optional, but you'll likely want to use the `uDiffuseMap` as a material property since it's already the case for the `uDiffuseColor` one.

Step 2: Über shaders automation with macro bindings
---------------------------------------------------

We now have a fragment shader that can use a solid color or a texture depending on whether the `DIFFUSE_MAP` macro is set:

```c
#ifdef DIFFUSE_MAP
	gl_FragColor = texture2D(uDiffuseMap, vVertexUv);
#else
	gl_FragColor = uDiffuseColor;
#endif
```


It works but it's still not really scalable: we would have to manually define the `DIFFUSE_MAP` macro by adding:

```c
#define DIFFUSE_MAP
```


at the begining of our vertex/fragment shader if we want to use a texture. Manually changing the source code of our shader at runtime in our application code is not really an option: we would have to fork each program and decide which macro should be defined manually. Instead, we will - once again - use data binding to automate this macro definition process.

Using `macros`, we can bind a macro definition to a data property provided by the engine/our application:

```json
"macros" : {
	"DIFFUSE_MAP" : "material[${materialUuid}].diffuseMap"
}
```


The behavior of a macro binding is described in the following pseudo-code:

```lua
defineString = "" if propertyExists(propertyName) then

 if isInteger(data[propertyName]) then
   defineString = "#define " + propertyName + " " + data[propertyName] // #define MACRO_NAME propertyValue
 else
   defineString = "#define " + propertyName // #define MACRO_NAME

// else no #define
```


In our case, `propertyName` would be "material.diffuseMap".

In this case, the `material.diffuseMap` should be a texture so the second case applies: if the `diffuseMap` property exists on our material, then the `DIFFUSE_MAP` macro will be defined; otherwise it will be undefined (and the `diffuseColor` will be used).

Final code
----------

asset/effect/MyCustomUberEffect.effect
```json
{
	"name" : "MyCustomUberEffect",
	"attributes" : {
		"aPosition" : "geometry[${geometryId}].position",
		"aUv" : "geometry[${geometryId}].uv"
		},
	"uniforms" : {
		"uDiffuseColor" : "material[${materialId}].diffuseColor",
		"uDiffuseMap" : "material[${materialId}].diffuseMap",
		"uModelToWorldMatrix" : "modelToWorldMatrix",
		"uWorldToScreenMatrix"  : { "binding" : { "property" : "worldToScreenMatrix", "source" : "renderer" } }
		},
	"macros" : {
		"DIFFUSE_MAP" : "material[${materialId}].diffuseMap"
		},
	"techniques" : [{
		"passes" : [{
		"vertexShader" : "
			#ifdef GL_ES
			precision mediump float;
			#endif

			attribute vec3 aPosition;
			attribute vec2 aUv;

			uniform mat4 uModelToWorldMatrix;
			uniform mat4 uWorldToScreenMatrix;

			varying vec2 vVertexUv;

			void main(void)
			{
			#ifdef DIFFUSE_MAP
				vVertexUv = aUv;
			#endif

			gl_Position = uWorldToScreenMatrix * uModelToWorldMatrix * vec4(aPosition, 1.0);
			}
		",
		"fragmentShader" : "
			#ifdef GL_ES
			precision mediump float;
			#endif

			uniform vec4 uDiffuseColor;
			uniform sampler2D uDiffuseMap;

			varying vec2 vVertexUv;

			void main(void)
			{
			#ifdef DIFFUSE_MAP
				gl_FragColor = texture2D(uDiffuseMap, vVertexUv);
			#else
				gl_FragColor = uDiffuseColor;
			#endif
			}
		"
		}]
	}]
}
```


src/main.cpp
```cpp
/*
Copyright (c) 2014 Aerys

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
using namespace minko::math;
using namespace minko::component;

const math::uint WINDOW_WIDTH = 800;
const math::uint WINDOW_HEIGHT = 600;

int
main(int argc, char** argv)
{
    auto canvas = Canvas::create("Minko Tutorial - Authoring uber-shaders", WINDOW_WIDTH, WINDOW_HEIGHT);

	auto sceneManager = component::SceneManager::create(canvas);

	sceneManager->assets()->loader()
        ->queue("effect/MyCustomUberEffect.effect")
		->queue("texture/box.png")
		->options()->registerParser<file::PNGParser>("png");

    auto root = scene::Node::create("root")
        ->addComponent(sceneManager);

    auto camera = scene::Node::create("camera")
        ->addComponent(Renderer::create(0x7f7f7fff))
        ->addComponent(PerspectiveCamera::create(canvas->aspectRatio()))
        ->addComponent(Transform::create(inverse(lookAt(vec3(0.f, 0.f, 3.f), vec3(), vec3(0.f, 1.f, 0.f)))));

	root->addChild(camera);

	auto texturedCube = scene::Node::create("texturedCube");
	auto coloredCube = scene::Node::create("coloredCube");

    auto complete = sceneManager->assets()->loader()->complete()->connect([&](file::Loader::Ptr loader)
    {

		auto texturedMaterial = material::Material::create();
		texturedMaterial->data()->set("diffuseMap", sceneManager->assets()->texture("texture/box.png")->sampler());

		texturedCube
			->addComponent(Transform::create(translate(vec3(-1.f, 0.f, -1.f))))
            ->addComponent(Surface::create(
				geometry::CubeGeometry::create(canvas->context()),
				texturedMaterial,
				sceneManager->assets()->effect("effect/MyCustomUberEffect.effect")));

		root->addChild(texturedCube);

		auto coloredMaterial = material::Material::create();
		coloredMaterial->data()->set("diffuseColor", vec4(0.f, 0.f, 1.f, 1.f));

        coloredCube
			->addComponent(Transform::create(translate(vec3(1.f, 0.f, -1.f))))
            ->addComponent(Surface::create(
				geometry::CubeGeometry::create(canvas->context()),
				coloredMaterial,
				sceneManager->assets()->effect("effect/MyCustomUberEffect.effect")
            ));

        root->addChild(coloredCube);

    });

    sceneManager->assets()->loader()->load();

	auto enterFrame = canvas->enterFrame()->connect([&](Canvas::Ptr canvas, float t, float dt)
	{
		auto texturedTransform = texturedCube->component<Transform>();
		texturedTransform->matrix(texturedTransform->matrix() * rotate(.01f, vec3(0.f, 1.f, 0.f)));

		auto coloredTransform = coloredCube->component<Transform>();
		coloredTransform->matrix(coloredTransform->matrix() * rotate(-.01f, vec3(0.f, 1.f, 0.f)));

		sceneManager->nextFrame(t, dt);
	});

	canvas->run();

    return 0;
}
```
