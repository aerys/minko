Minko contains a plugin specific for effect more advanced than tha basic or phong ones. Enabling this plugin allow you to use, for instance, the anti-aliasing but not only (lens flares, reflection, refraction, etc...).

Step 0: Invoke the 'fx' Minko plugin
------------------------------------

The first thing you have to do is anything else than [enable the 'fx' plugin](../tutorial/How_to_enable_a_plugin.md) adding this line at the end of your project's premake file:

```lua
minko.plugin.enable("fx") 
```


Doing this will automatically copy the advanced effect files in the project output directory before compiling.

Step 1: Load the anti-aliasing effect
-------------------------------------

The next step is to load the effect that we want to use. For us, it's the FXAA one:

```cpp
auto sceneManager = SceneManager::create(canvas->context());

sceneManager->assets()->queue("effect/FXAA/FXAA.effect"); 
```


We can check that our anti-aliasing effect is correctly loaded like that:

```cpp
auto complete = sceneManager->assets()->complete()->connect([&](file::AssetLibrary::Ptr assets) {

   auto effect = sceneManager->assets()->effect("effect/FXAA/FXAA.effect");

   if (!effect)
       throw std::logic_error("The FXAA effect has not been loaded.");

});

sceneManager->assets()->load();

```


Step 2: Create a render target
------------------------------

The anti-aliasing effect is a post processing effect, so we need to create a render target to render the scene into it. I would strongly advise you to read the [tutorial that explains how to create a simple post-processing effect](../tutorial/22-Creating_a_simple_post-processing_effect.md).

```cpp
auto renderTarget = render::Texture::create(assets->context(), clp2(WINDOW_WIDTH), clp2(WINDOW_HEIGHT), false, true); 
```


Don't forget to call the *upload()* method to really allocate GPU memory for the render target:

```cpp
renderTarget->upload(); 
```


Step 3 : Set anti-aliasing effect mandatory parameters
------------------------------------------------------

Most effects needs some properties to be set to work properly. For the anti-aliasing, we need to set:

-   **textureSampler**: that represent the texture into the render target
-   **texcoordOffset**: that is the inverse of the texture dimensions along X and Y of the render target.

```cpp
effect->setUniform("textureSampler", renderTarget); effect->setUniform("texcoordOffset", Vector2::create(1.0f / renderTarget->width(), 1.0f / renderTarget->height())); 
```


Step 4 : Draw the scene
-----------------------

The final initialization step is to create a second scene - completely different from your actual 3D scene - that will only hold a single surface made from the quad geometry that it supposed to represent our screen, a dummy Material and our post-processing effect:

```cpp
auto renderer = Renderer::create(); auto postProcessingScene = scene::Node::create() ->addComponent(renderer) ->addComponent(

   Surface::create(
       geometry::QuadGeometry::create(sceneManager->assets()->context()),
       material::Material::create(),
       effect
   )

); 
```


Now that all we need for anti-aliasing is intialized, we just have to make sure that:

-   every frame is rendered inside our `renderTarget`;
-   our `renderer` renders a frame and, because of our setup, this frame will render a single quad using our post-processin effect.

We just have to update what we do in our `Canvas::enterFrame()` callback:

```cpp
auto enterFrame = canvas->enterFrame()->connect([&](Canvas::Ptr canvas, float t, float dt) {

 sceneManager->nextFrame(t, dt, renderTarget);
 renderer->render(assets->context());

} 
```


Step 5 (optionnal): Managing the size of the backbuffer
-------------------------------------------------------

If we want our post-processing to have a good quality, we have to make sure the size of the render target we use always has the same size as the backbuffer. OpenGL ES 2.0 requires textures with a power of 2 width/height. Thus, we will use `math::clp2` to make sure our render target width/height is always upscaled to the closest upper power of 2.

We will do this in our `Canvas::resized()` callback:

```cpp
auto resized = canvas->resized()->connect([&](AbstractCanvas::Ptr canvas, uint width, uint height) {

   camera->component<PerspectiveCamera>()->aspectRatio((float) width / (float) height);

   renderTarget = render::Texture::create(assets->context(), clp2(width), clp2(height), false, true);
   renderTarget->upload();

   effect->setUniform("textureSampler", renderTarget);
   effect->setUniform("texcoordOffset",
       Vector2::create(1.0f / renderTarget->width(), 1.0f / renderTarget->height()));

}); 
```


By assigning a new value to `renderTarget`, we remove the only reference to the original render target `render::Texture` object. Thus, Minko will automatically dispose the corresponding GPU memory texture for you.

Final code
----------

```cpp
#include "minko/Minko.hpp" 
#include "minko/MinkoSDL.hpp"

using namespace minko; 
using namespace minko::math; 
using namespace minko::component;

const uint WINDOW_WIDTH = 800; 
const uint WINDOW_HEIGHT = 600;

int main(int argc, char** argv) {

   auto canvas = Canvas::create("Minko Tutorial - Applying anti-aliasing effect", WINDOW_WIDTH, WINDOW_HEIGHT);
   auto sceneManager = SceneManager::create(canvas->context());

   sceneManager->assets()
       ->queue("effect/Basic.effect")
       ->queue("effect/FXAA/FXAA.effect")
       ;

   auto complete = sceneManager->assets()->complete()->connect([&](file::AssetLibrary::Ptr assets)
   {
       auto root = scene::Node::create("root")
           ->addComponent(sceneManager);

       auto camera = scene::Node::create("camera")
           ->addComponent(Renderer::create(0x00000000))
           ->addComponent(PerspectiveCamera::create(
               (float) WINDOW_WIDTH / (float) WINDOW_HEIGHT, (float) PI * 0.25f, .1f, 1000.f))
           ->addComponent(Transform::create(Matrix4x4::create()
               ->lookAt(Vector3::create(), Vector3::create(0.f, 0.f, -5.f)))
           );
       root->addChild(camera);

       auto cube = scene::Node::create("cube")
           ->addComponent(Transform::create())
           ->addComponent(Surface::create(
           geometry::CubeGeometry::create(assets->context()),
           material::BasicMaterial::create()->diffuseColor(Vector4::create(0.f, 0.f, 1.f, 1.f)),
           assets->effect("effect/Basic.effect")
           ));
       root->addChild(cube);

       auto effect = sceneManager->assets()->effect("effect/FXAA/FXAA.effect");

       // Check that the FXAA effect has been properly loaded
       if (!effect)
           throw std::logic_error("The FXAA effect has not been loaded.");

       auto renderTarget = render::Texture::create(
           assets->context(), clp2(WINDOW_WIDTH), clp2(WINDOW_HEIGHT), false, true);
       renderTarget->upload();

       effect->setUniform("textureSampler", renderTarget);
       effect->setUniform("texcoordOffset",
           Vector2::create(1.0f / renderTarget->width(), 1.0f / renderTarget->height()));

       auto renderer = Renderer::create();
       // Create a scene just to display the post processing render target texture
       auto postProcessingScene = scene::Node::create()
           ->addComponent(renderer)
           ->addComponent(
               Surface::create(
                   geometry::QuadGeometry::create(sceneManager->assets()->context()),
                   material::Material::create(),
                   effect
               )
           );

       auto resized = canvas->resized()->connect([&](AbstractCanvas::Ptr canvas, uint width, uint height)
       {
           camera->component<PerspectiveCamera>()->aspectRatio((float) width / (float) height);

           renderTarget = render::Texture::create(assets->context(), clp2(width), clp2(height), false, true);
           renderTarget->upload();

           effect->setUniform("textureSampler", renderTarget);
           effect->setUniform("texcoordOffset",
               Vector2::create(1.0f / renderTarget->width(), 1.0f / renderTarget->height()));
       });

       // Enable/Disable FXAA pressing space key
       auto enableFXAA = true;
       auto keyDown = canvas->keyboard()->keyDown()->connect([&](input::Keyboard::Ptr k)
       {
           if (k->keyIsDown(input::Keyboard::ScanCode::SPACE))
           {
               enableFXAA = !enableFXAA;

               if (enableFXAA)
                   std::cout << "Enable FXAA" << std::endl;
               else
                   std::cout << "Disable FXAA" << std::endl;
           }

       });

       auto enterFrame = canvas->enterFrame()->connect([&](Canvas::Ptr canvas, float t, float dt)
       {
           cube->component<Transform>()->matrix()->prependRotationY(.01f);

           if (enableFXAA)
           {
               sceneManager->nextFrame(t, dt, renderTarget);
               renderer->render(assets->context());
           }
           else
           {
               sceneManager->nextFrame(t, dt);
           }
       });

       canvas->run();
   });

   sceneManager->assets()->load();

   return 0;

} 
```


