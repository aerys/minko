In this tutorial, we will see how to reference external GLSL files in `*.effect` files. This mechanism is useful for two reasons:

-   it eases the integration of 3rd party GLSL code;
-   it makes your `*.effect` files cleaner by externalizing the shader code.

To reference external GLSL code, we will use the `#pragma include` directive within shader fields.

As an example, we will break apart the custom effect that we previously set up in the tutorial [Creating a custom effect](../tutorial/17-Creating_a_custom_effect.md).

Step 1: Referencing the external GLSL files
-------------------------------------------

To reference our external `MyCustomEffect.vertex.glsl` and `MyCustomEffect.fragment.glsl` files, we will use the `#pragma include` directive within the respective shader fields:

```javascript
{

 "name" : "MyCustomEffect",
 "passes" : [{
   "vertexShader" : "#pragma include('MyCustomEffect.vertex.glsl')",
   "fragmentShader" : "#pragma include('MyCustomEffect.fragment.glsl')"
 }]

} 
```


In the code above, `MyCustomShader.vertex.glsl` and `MyCustomShader.fragment.glsl` are expected to be located in the same directory as the `MyCustomEffect.effect` file.

The effect of the `#pragma include` directive is pretty much the same as the `#include` C/C++ pre-processor macro: the code from the included file(s) are copy/pasted directly.

Step 2 (optional): Binding the uniforms
---------------------------------------

As you can imagine, every GLSL code is different... so you'll want to adapt this part of the tutorial to the actual uniforms declared by the GLSL code/shader you're including in your effect.

You also have to remember that you can always choose between declaring some `uniformBindings` in your `*.effect` files or manually calling `Effect::setUniform()` in your application code.

To learn how to setup `uniformBindings`, you can read the following tutorials:

-   [Creating custom materials](../tutorial/18-Creating_custom_materials.md)
-   [Binding the model to world transform](../tutorial/19-Binding_the_model_to_world_transform.md)
-   [Binding the camera](../tutorial/20-Binding_the_camera.md)

Final code
----------

asset/effect/MyCustomEffect.effect 
```javascript
{

 "name" : "MyCustomEffect",
 "attributeBindings" : {
   "aPosition" : "geometry[${geometryId}].position"
 },
 "passes" : [{
   "vertexShader" : "#pragma include('MyCustomEffect.vertex.glsl')",
   "fragmentShader" : "#pragma include('MyCustomEffect.fragment.glsl')"
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
uniform mat4 uViewMatrix;
uniform mat4 uProjectionMatrix;

void main(void) {

 gl_Position = uProjectionMatrix * uViewMatrix * uModelToWorldMatrix * vec4(aPosition, 1.0);

} 
```


asset/effect/MyCustomEffect.fragment.glsl 
```c
#ifdef GL_ES

precision mediump float;

#endif

uniform vec4 uColor;

void main(void) {

 gl_FragColor = uColor;

} 
```


src/main.cpp 
```cpp
#include "minko/Minko.hpp" 
#include "minko/MinkoSDL.hpp"

using namespace minko; 
using namespace minko::math; 
using namespace minko::component;

const uint WINDOW_WIDTH = 800; 
const uint WINDOW_HEIGHT = 600;

int main(int argc, char** argv) {

   auto canvas = Canvas::create("Minko Tutorial - Using external GLSL code in effect files", WINDOW_WIDTH, WINDOW_HEIGHT);
   auto sceneManager = component::SceneManager::create(canvas);

   sceneManager->assets()
       ->queue("effect/MyCustomEffect.effect");
   auto complete = sceneManager->assets()->complete()->connect([&](file::AssetLibrary::Ptr assets)
   {
       auto myCustomEffect = assets->effect("effect/MyCustomEffect.effect");

       auto root = scene::Node::create("root")
           ->addComponent(sceneManager)
           ->addComponent(Renderer::create(0x7f7f7fff));

       auto cube = scene::Node::create("cube")
           ->addComponent(Surface::create(
           geometry::CubeGeometry::create(assets->context()),
           material::BasicMaterial::create()->diffuseColor(Vector4::create(0.f, 0.f, 1.f, 1.f)),
           myCustomEffect
           ));
       root->addChild(cube);

       auto modelToWorldMatrix = Matrix4x4::create()->translation(0.f, 0.f, -5.f);

       myCustomEffect->setUniform("uModelToWorldMatrix", modelToWorldMatrix);
       myCustomEffect->setUniform("uViewMatrix", Matrix4x4::create());
       myCustomEffect->setUniform("uProjectionMatrix", Matrix4x4::create()->perspective((float)PI * 0.25f, (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, .1f, 1000.f));

       myCustomEffect->setUniform("uColor", Vector4::create(0.f, 0.f, 1.f, 1.f));

       auto enterFrame = canvas->enterFrame()->connect([&](Canvas::Ptr canvas, float t, float dt)
       {
           modelToWorldMatrix->prependRotationY(0.01f);
           myCustomEffect->setUniform("uModelToWorldMatrix", modelToWorldMatrix);

           sceneManager->nextFrame(t, dt);
       });

       canvas->run();
   });

   sceneManager->assets()->load();

   return 0;

} 
```


