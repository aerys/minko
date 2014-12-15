In this tutorial we will see how to author post-processing effects and how to setup our application/scene to use them. Before you read this tutorial, you should read:

-   [Create your first custom effect](../tutorial/17-Creating_a_custom_effect.md) to learn about `*.effect` files authoring basics;
-   [Binding the camera](../tutorial/20-Binding_the_camera.md) and/or [Binding the model to world transform](../tutorial/19-Binding_the_model_to_world_transform.md) to learn about `uniformBindings`.

As a practical example, this tutorial will guide you through the creation of a "black and white" or "desaturate" post-processing effect. Its purpose is quite obvious: it will post-process your rendering to get a black and white final picture.

Post-processing is done in two steps:

-   The scene is rendered as usual but in a texture.
-   This texture is used to render a fullscreen quad and each pixel can be modified using the fragment shader.

enterFrame The second step implies writing at least one shader. But you might also have to write multiple shaders for multi-pass post-processing effects.

Step 1: The vertex shader
-------------------------

For post-processing to work, we have to render a fullscreen quad and then apply a function on each pixel of the backbuffer that will be sampled as a texture. In the vertex shader, we will do the very first step: make sure your quad fills the entire screen.

The `geometry::QuadGeometry` class provides the geometry for a unit sized quad lying in the (x, y) plane. Thus, its top left corner is in (-0.5, 0.5, 0.0) and its bottom right corner is in (0.5, -0.5, 0.0). Our vertex shader is supposed to output normalized device coordinates in the [-1 .. 1] bounds. So we just have to scale our quad to make it fill the entire screen:

```c
gl_Position = vec4(aPosition, 1) * vec4(1., 1., 1., .5); 
```


Note how we actually divide `w` by 2 instead of multiplying `x` and `y` by 2. It's exactly the same result in the end since the value will be normalized by the hardware.

Because OpenGL performs render to texture using an inverted y axis, we also have modify the original UVs to make sure our backbuffer will be sampled properly. Here is the final code for our post-processing vertex shader:

```c
#ifdef GL_ES precision mediump float; #endif

attribute vec3 aPosition; attribute vec2 aUv;

varying vec4 vVertexUv;

void main(void) {

 vVertexUv = vec2(aUv.x, 1 - uv.y);

 gl_Position = vec4(aPosition, 1) * vec4(1., 1., 1., .5);

} 
```


Step 2: The fragment shader
---------------------------

The vertex shader is pretty standard and should be pretty much the same for most of your post-processing effects. But the fragment shader is where you can customize your pixel function to get the effect you want.

Here, we will simply sample the backbuffer and use an average of its `RGB` value to get a greyscale result color:

```c
#ifdef GL_ES precision mediump float; #endif

uniform sampler2D uBackbuffer;

varying vec2 vVertexUv;

void main() {

 vec4 pixel = texture2D(uBackbuffer, vVertexUv);
 float average = (pixel.r + pixel.g + pixel.b) / 3;

 gl_FragColor = vec4(average, average, average, 1);

} 
```


Step 3: Setting up the scene
----------------------------

The first thing to do is to create the `Texture` that will be used as a replacement for our backbuffer:

```cpp
auto ppTarget = render::Texture::create(assets->context(), 1024, 1024, false, true);

ppTarget->upload(); 
```


**Attention!** Don't forget to call `Texture::upload()`: even if the texture has no actual data, it needs to be allocated on the GPU.

Just like any `Effect`, our post-processing file should be loaded using the `AssetLibrary::load()` method. You can read more on this subject in the [Loading effects](../tutorial/16-Loading_effects.md) tutorial.

When our `Effect` has been successfully loaded, we can fetch it from the library using the `AssetLibrary::effect()` method. The following code makes sure the effect has been properly loaded and throws an exception otherwise:

```cpp
auto ppFx = sceneManager->assets()->effect("effect/Desaturate.effect");

if (!ppFx)

 throw std::logic_error("The post-processing effect has not been loaded.");

ppFx->setUniform("uBackbuffer", ppTarget); 
```


As you can see, we also set the `uBackbuffer` uniform to be our `ppTarget` which will - indeed - be used as the backbuffer replacement for the first stage of post-processing.

The final initialization step is to create a second scene - completely different from your actual 3D scene - that will only hold a single surface made from the quad geometry that it supposed to represent our screen, a dummy `Material` and our post-processing effect:

```cpp
auto ppRenderer = Renderer::create(); auto ppScene = scene::Node::create()

 ->addComponent(ppRenderer)
 ->addComponent(Surface::create(
   geometry::QuadGeometry::create(sceneManager->assets()->context()),
   material::Material::create(),
   ppFx
 ));

```


Now that all we need for post-processing is intialized, we just have to make sure that:

-   every frame is rendered inside our `ppTarget` render target;
-   our `ppRenderer` renders a frame and, because of our setup, this frame will render a single quad using our post-processin effect.

We just have to update what we do in our `Canvas::enterFrame()` callback:

```cpp
auto enterFrame = canvas->enterFrame()->connect([&](Canvas::Ptr canvas, float t, float dt) {

 sceneManager->nextFrame(t, dt, ppTarget);
 ppRenderer->render(assets->context());

} 
```


Step 4 (optional): Managing the size of the backbuffer
------------------------------------------------------

If we want our post-processing to have a good quality, we have to make sure the size of the render target we use always has the same size as the backbuffer. OpenGL ES 2.0 requires textures with a power of 2 width/height. Thus, we will use `math::clp2` to make sure our render target width/height is always upscaled to the closest upper power of 2.

We will do this in our `Canvas::resized()` callback:

```cpp
auto resized = canvas->resized()->connect([&](Canvas::Ptr canvas, unsigned int width, unsigned int height) {

 camera->component<PerspectiveCamera>()->aspectRatio((float)width / (float)height);

 ppTarget = render::Texture::create(assets->context(), clp2(width), clp2(height), false, true);
 ppTarget->upload();
 ppFx->setUniform("uBackbuffer", ppTarget);

}); 
```


By assigning a new value to `ppTarget`, we remove the only reference to the original render target `render::Texture` object. Thus, Minko will automatically dispose the corresponding GPU memory texture for you.

Final code
----------

asset/effect/Desaturate.effect 
```javascript
{

 "name" : "desaturate",
 "attributeBindings" : {
   "aPosition" : "geometry[${geometryId}].position",
   "aUv" : "geometry[${geometryId}].uv"
 },
 "passes" : [{
   "vertexShader" : "
     #ifdef GL_ES
     precision mediump float;
     #endif
     attribute vec3 aPosition;
     attribute vec2 aUv;
     varying vec4 vVertexUv;
     void main(void)
     {
       vVertexUv = vec2(aUv.x, 1 - uv.y);
       gl_Position = vec4(aPosition, 1) * vec4(1., 1., 1., .5);
     }
   ",
   "fragmentShader" : "
     #ifdef GL_ES
     precision mediump float;
     #endif
     uniform sampler2D uBackbuffer;
     varying vec2 vVertexUv;
     void main()
     {
       vec4 pixel = texture2D(uBackbuffer, vVertexUv);
       float average = (pixel.r + pixel.g + pixel.b) / 3;
       gl_FragColor = vec4(average, average, average, 1);
     }
   "
 }]

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

 auto canvas = Canvas::create("Minko Tutorial - Creating a simple post-processing effect", WINDOW_WIDTH, WINDOW_HEIGHT);
 auto sceneManager = component::SceneManager::create(canvas);
 sceneManager->assets()
   ->queue("effect/Basic.effect")
   ->queue("effect/Desaturate.effect");

 auto complete = sceneManager->assets()->complete()->connect([&](file::AssetLibrary::Ptr assets)
 {
   auto root = scene::Node::create("root")
     ->addComponent(sceneManager);
   auto camera = scene::Node::create("camera")
     ->addComponent(Renderer::create(0x7f7f7fff))
     ->addComponent(PerspectiveCamera::create(
       (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, (float)PI * 0.25f, .1f, 1000.f)
     ));
   root->addChild(camera);
   auto cube = scene::Node::create("cube")
     ->addComponent(Transform::create(Matrix4x4::create()->translation(0.f, 0.f, -5.f)))
     ->addComponent(Surface::create(
       geometry::CubeGeometry::create(assets->context()),
       material::BasicMaterial::create()->diffuseColor(Vector4::create(0.f, 0.f, 1.f, 1.f)),
       assets->effect("effect/Basic.effect")
     ));
   root->addChild(cube);

   auto ppTarget = render::Texture::create(assets->context(), 1024, 1024, false, true);

   ppTarget->upload();

   auto ppFx = sceneManager->assets()->effect("effect/Desaturate.effect");

   if (!ppFx)
     throw std::logic_error("The post-processing effect has not been loaded.");

   ppFx->setUniform("uBackbuffer", ppTarget);

   auto ppRenderer = Renderer::create();
   auto ppScene = scene::Node::create()
     ->addComponent(ppRenderer)
     ->addComponent(Surface::create(
       geometry::QuadGeometry::create(sceneManager->assets()->context()),
       material::Material::create(),
       ppFx
     ));
   auto resized = canvas->resized()->connect([&](Canvas::Ptr canvas, uint width, uint height)
   {
     camera->component<PerspectiveCamera>()->aspectRatio((float)width / (float)height);

     ppTarget = render::Texture::create(assets->context(), clp2(width), clp2(height), false, true);
     ppTarget->upload();
     ppFx->setUniform("uBackbuffer", ppTarget);
   });
   auto enterFrame = canvas->enterFrame()->connect([&](Canvas::Ptr canvas, float t, float dt)
   {
     cube->component<Transform>()->transform()->prependRotationY(.01f);

     sceneManager->nextFrame(t, dt, ppTarget);
     ppRenderer->render(assets->context());
   });
   canvas->run();
 });
 sceneManager->assets()->load();
 return 0;

} 
```


