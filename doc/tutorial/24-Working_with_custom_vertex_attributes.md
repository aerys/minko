As you should have learned in the tutorials dedicated to the binding of your objects' [model-to-world transforms](../tutorial/19-Binding_the_model_to_world_transform.md) and your [camera](../tutorial/20-Binding_the_camera.md), you can easily pass uniform properties to the GPU by editing effect files. In this tutorial, we will dwell on a slightly different, albeit crucial sort of data you can hand over to your graphics card.

Principle
---------

The aforementioned tutorials illustrate the binding of `uniforms`, which are values that remain constant during the rendering of a drawcall (properties relative to your viewing or lighting conditions typically). Here, we will show you that Minko's convenient *binding* paradigm is also applicable to vertex `attributes` which, as their name implies, correspond to properties assigned to the vertices of the 3D geometry you render during your drawcall.

The granularity between uniform and attribute inputs is thus remarkably different: while the former remain constant over the entirety of your geometry, the latter vary along with it (for each of its vertices).

Typical attributes include (very surprisingly) 3D positions, but also vertex normals and texture coordinates. Those are already bound in most effects provided alongside the Minko SDK, so you will not have to worry about them too much in most cases. But shaders with such limited vertex-based inputs would be no fun, so we will show you how to hand over your own vertex attributes to your GLSL shaders here.

Where to start
--------------

This tutorial expects you already read how to bind uniforms and will start form the code you must have obtained at the end of the [binding of the camera properties](../tutorial/20-Binding_the_camera.md). You are strongly invited to at least grab the code you will find there and then proceed to the following sections.

![What you should have so far.](../../doc/image/StartingScreen.jpeg "What you should have so far.")

Step 1: Add your attributes to your geometry in Minko
-----------------------------------------------------

Let us assume you want to add a 3D displacement vector to each vertex of your mesh's geometry. Clearly, you cannot resort to uniforms and shall use a vertex attribute instead. The first step to follow is thus to encapsulate this additional data into your `geometry::Geometry` and hand it over to the GPU as before. You can perfectly entirely redefine your own custom geometry, but since we will stick with the cube (and hence the `geometry::CubeGeometry`) the easiest way to go is to add another *vertex buffer* to it via the `geometry::Geometry::addVertexBuffer(render::VertexBuffer::Ptr)` method.

A `render::VertexBuffer` instance can simply be considered as a convenient wrapper around your vertex-based data (represented by a `std::vector<float>`) that only stores the information necessary for the rendering context to process your data. Besides the data itself, a vertex buffer also specifies:

-   the *property name* bound to the attribute (here, `positionOffset`),
-   the *size* of each attribute in terms of floats (here, 3 as each corresponds to a `vec3` GLSL type),
-   the *offset* inside the `std::vector<float>` of the attribute in terms of floats (here, 0 as the attribute is alone within the vertex buffer).

In the following code, we exploit the simplicity of the cube geometry (six separate vertices for the six faces of the cube) in order to create a vertex buffer, the data of which is used to slightly move the vertex positions along the face normals (the actual vertex displacement is done in the vertex shader program as shown below).

```cpp
geometry::Geometry::Ptr createGeometryWithAttribute(render::AbstractContext::Ptr context) {

 auto cubeGeometry = geometry::CubeGeometry::create(context); // original cube geometry

 auto numVertices  = cubeGeometry->numVertices(); // 36 vertices (6 vertices per face)
 auto offsetData   = std::vector<float>(3 * numVertices, 0.0f); // vec3 per vertex

 uint i = 0;
 for (uint vertexId = 0; vertexId < numVertices; ++vertexId)
 {
   float dx = 0.0f; // components of the normal displacement
   float dy = 0.0f;
   float dz = 0.0f;

   switch (vertexId / 6) // faceId
   {
   case 0:
     dy += 0.1f;
     break;
   case 1:
     dy -= 0.1f;
     break;
   case 2:
     dz -= 0.1f;
     break;
   case 3:
     dz += 0.1f;
     break;
   case 4:
     dx -= 0.1f;
     break;
   case 5:
     dx += 0.1f;
     break;
   }

   offsetData[i++] = dx;
   offsetData[i++] = dy;
   offsetData[i++] = dz;
 }

 auto offsetBuffer = render::VertexBuffer::create(context, offsetData);
 offsetBuffer->addAttribute("positionOffset", 3, 0);

 cubeGeometry->addVertexBuffer(offsetBuffer);

 return cubeGeometry;

}
```


This vertex buffer is added to the geometry and finally, in the `main` procedure, we simply hand over this newly augmented geometry to the existing `component::Surface` in order to visualize the changes caused by our new vertex shader and its custom attribute inputs.

```cpp
int main(int argc, char** argv) {

 ...
 auto complete = sceneManager->assets()->complete()->connect([&](file::AssetLibrary::Ptr assets)
 {
   ...
   auto cube = scene::Node::create("cube")
     ->addComponent(Transform::create(translate(vec3(0.f, 0.f, -5.f))))
     ->addComponent(Surface::create(
       createGeometryWithAttribute(assets->context()), // geometry with add. vertex attribute
       myCustomMaterial,
       myCustomEffect
     ));
   ...
 });
 sceneManager->assets()->load();
 return 0;

}
```


What we did here is simply enhance the former cube geometry with an additional `positionOffset` vertex attribute. However, the vertex attribute values are yet to be sent over to the GPU and your GLSL code. We will see how to proceed in an instant.

Step 2: Add your binding in your effect file
--------------------------------------------

Pretty similarly to what we did for the uniform inputs for our effect (in order to specify the [model's transform](../tutorial/19-Binding_the_model_to_world_transform.md) and [viewing settings of our scene](../tutorial/20-Binding_the_camera.md)), we just have to *bind* the data stored in our Minko geometry to some actual `attribute` on the GLSL side. The per-vertex values taken by our `positionOffset` attribute are currently stored in the data provider of our geometry, and we will actually use them via a GLSL attribute input we will call `aPositionOffset`.

The vertex attribute binding between the geometry's data provider and our GLSL attribute is straightforwardly done by simply adding a single line in the effect file's `attributes` dictionary, where the key corresponds to the name of the GLSL attribute and the value to the matching property name manipulated by the Minko engine's data providers.

```json
{

 "name" : "MyCustomEffect",
 "attributes" : {
   ...
   "aPositionOffset" : "geometry[${geometryId}].positionOffset"
 },
 ...

}
```


From now on, we can actually use the values we stored in the geometry on the CPU side, within the code of our shaders executed on the GPU.

Step 3: Use your vertex attributes in GLSL
------------------------------------------

Because a meaningful vertex attribute is only one that is actually used in computations of the shader program, we now update the source code of our custom effect's vertex shader in order to use our additional `aOffsetPosition` attribute and move the position of our vertices in local space according to its values.

```json
{
	"name" : "MyCustomEffect",
	...
	"techniques" : [{
		"passes" : [{
			"vertexShader" : "
				#ifdef GL_ES
				precision mediump float;
				#endif
				attribute vec3 aPosition;
				attribute vec3 aPositionOffset;

				uniform mat4 uModelToWorldMatrix;

				uniform mat4 uWorldToScreenMatrix;
				void main(void)
				{
				gl_Position = uWorldToScreenMatrix * uModelToWorldMatrix * vec4(aPosition + aPositionOffset, 1.0);
				}
			",
			"fragmentShader" : "
				...
			"
		}]
	}]
}
```


If everything went smooth, you should obtain something like this (the vantage point has been slightly altered for better readability):

![Cube with displaced vertex positions.](../../doc/image/CubePositionOffsets.jpeg "Cube with displaced vertex positions.")

Final code
----------

asset/effect/MyCustomEffect.effect

```json
{
  "name" : "MyCustomEffect",
  "attributes" : {
    "aPosition" : "geometry[${geometryId}].position",
    "aPositionOffset" : "geometry[${geometryId}].positionOffset",
	"aVertexColor" : "geometry[${geometryId}].color"
  },
  "uniforms" : {
    "uModelToWorldMatrix" : "modelToWorldMatrix",
    "uWorldToScreenMatrix" :  { "binding" : { "property" : "worldToScreenMatrix", "source" : "renderer" } }
  },
  "techniques" : [{
	  "passes" : [{
		"vertexShader" : "
		  #ifdef GL_ES
		  precision mediump float;
		  #endif

		  attribute vec3 aPosition;
		  attribute vec3 aPositionOffset;
		  attribute vec4 aVertexColor;

		  varying vec4 vVertexColor;

		  uniform mat4 uModelToWorldMatrix;
		  uniform mat4 uWorldToScreenMatrix;

		  void main(void)
		  {
			vVertexColor = aVertexColor;

			gl_Position = uWorldToScreenMatrix * uModelToWorldMatrix * vec4(aPosition + aPositionOffset, 1.0);
		  }
		",
		"fragmentShader" : "
		  #ifdef GL_ES
		  precision mediump float;
		  #endif

		  varying vec4 vVertexColor; // interpolated across triangular face

		  void main(void)
		  {
			gl_FragColor = vVertexColor;
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

geometry::Geometry::Ptr
createGeometryWithAttribute(render::AbstractContext::Ptr);

void
addCustomAttribute(render::AbstractContext::Ptr context, geometry::Geometry::Ptr geometry);

int
main(int argc, char** argv)
{
	auto canvas = Canvas::create("Minko Tutorial - Working with custom vertex attributes", WINDOW_WIDTH, WINDOW_HEIGHT);
	auto sceneManager = component::SceneManager::create(canvas);

	sceneManager->assets()->loader()->queue("effect/MyCustomEffect.effect");

	auto root = scene::Node::create("root")
		->addComponent(sceneManager);

	auto camera = scene::Node::create()
		->addComponent(Renderer::create(0x7f7f7fff))
		->addComponent(Transform::create(inverse(lookAt(vec3(0.f, 1.f, -5.f), vec3(), vec3(0.f, 1.f, 0.f)))))
		->addComponent(PerspectiveCamera::create((float)WINDOW_WIDTH / (float)WINDOW_HEIGHT));
	root->addChild(camera);

	auto cube = scene::Node::create("cube");

	auto complete = sceneManager->assets()->loader()->complete()->connect([&](file::Loader::Ptr loader)
	{
		auto myCustomMaterial = material::MyCustomMaterial::create();
		cube
			->addComponent(Transform::create())
			->addComponent(Surface::create(
				createGeometryWithAttribute(canvas->context()), // geometry with add. vertex attribute
				myCustomMaterial,
				sceneManager->assets()->effect("effect/MyCustomEffect.effect")
			));
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

geometry::Geometry::Ptr
createGeometryWithAttribute(render::AbstractContext::Ptr context)
{
	auto cubeGeometry = geometry::CubeGeometry::create(context); // original cube geometry

	auto numVertices = cubeGeometry->numVertices(); // 36 vertices (6 vertices per face)
	auto offsetData = std::vector<float>(3 * numVertices, 0.0f); // vec3 per vertex

	math::uint i = 0;
	for (math::uint vertexId = 0; vertexId < numVertices; ++vertexId)
	{
		float dx = 0.0f; // components of the normal displacement
		float dy = 0.0f;
		float dz = 0.0f;

		switch (vertexId / 6) // faceId
		{
		case 0:
			dy += 0.1f;
			break;
		case 1:
			dy -= 0.1f;
			break;
		case 2:
			dz -= 0.1f;
			break;
		case 3:
			dz += 0.1f;
			break;
		case 4:
			dx -= 0.1f;
			break;
		case 5:
			dx += 0.1f;
			break;
		}

		offsetData[i++] = dx;
		offsetData[i++] = dy;
		offsetData[i++] = dz;
	}

	auto offsetBuffer = render::VertexBuffer::create(context, offsetData);
	offsetBuffer->addAttribute("positionOffset", 3, 0);

	cubeGeometry->addVertexBuffer(offsetBuffer);

	addCustomAttribute(context, cubeGeometry);

	return cubeGeometry;
}

void
addCustomAttribute(render::AbstractContext::Ptr context, geometry::Geometry::Ptr geometry)
{
	const math::uint	numVertices = 36;
	auto		colorData = std::vector<float>(4 * numVertices, 0.0f); // vec4 per vertex

	math::uint i = 0;
	for (math::uint vId = 0; vId < numVertices; ++vId)
	{
		float r = 0.0f;
		float g = 0.0f;
		float b = 0.0f;
		float a = 1.0f;

		if (vId == 7 || vId == 10 || vId == 12 || vId == 16 || vId == 30 || vId == 35)
		{
			r = 1.0f;
		}
		else if (vId == 4 || vId == 13 || vId == 15 || vId == 25 || vId == 27)
		{
			g = 1.0f;
		}
		else if (vId == 6 || vId == 11 || vId == 19 || vId == 21 || vId == 24 || vId == 28)
		{
			b = 1.0f;
		}
		else if (vId == 0 || vId == 3 || vId == 14 || vId == 31)
		{
			r = 1.0f; g = 1.0f;
		}
		else if (vId == 8 || vId == 22 || vId == 34)
		{
			r = 1.0f; b = 1.0f;
		}
		else if (vId == 1 || vId == 5 || vId == 18 || vId == 29)
		{
			g = 1.0f; b = 1.0f;
		}
		else if (vId == 2 || vId == 20 || vId == 23 || vId == 32 || vId == 33)
		{
			r = 1.0f; g = 1.0f; b = 1.0f;
		}
		colorData[i++] = r;
		colorData[i++] = g;
		colorData[i++] = b;
		colorData[i++] = a;
	}

	auto colorBuffer = render::VertexBuffer::create(context, colorData);
	colorBuffer->addAttribute("color", 4, 0);

	geometry->addVertexBuffer(colorBuffer);
}
```
