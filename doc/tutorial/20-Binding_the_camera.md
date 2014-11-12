In the previous tutorial, we've seen [how to bind the model to world matrix](../tutorial/19-Binding_the_model_to_world_transform.md) in our effects. Using the same binding mechanism, we can bind the camera properties. Especially the "view matrix", which is used to transform "global" world-space 3D vertices into the camera relative view-space.

To do this, we will use data binding to :

-   update our effects' `uniformBindinds` to bind the `uViewMatrix` uniform to a property provided by the `PerspectiveCamera` component;
-   remove `uViewMatrix` related code, since this uniform will then be handled automatically by data binding.

Step 1: Binding the view matrix
-------------------------------

If you read the code for the `PerspectiveCamera::initialize()` method, you'll see that the `PerspectiveCamera` component declares the following properties:

-   `position`
-   `viewMatrix`
-   `projectionMatrix`
-   `worldToScreenMatrix`

Those properties are declared in the `PerspectiveCamera::data()` provider, which is a `[data::StructureProvider`](data::StructureProvider`) with "camera" as a structure name. Therefore, all those properties are actually available with the `camera.` prefix. For example, the `viewMatrix` property can be bound using the `camera.viewMatrix` token.

Here is how we can bind the camera transform and projection in our effect using `uniformBindings`:

```javascript
"uniformBindings" : {

 "uViewMatrix" : { "property" : "camera.viewMatrix", "source" : "renderer" },
 "uProjectionMatrix" : { "property" : "camera.projectionMatrix", "source" : "renderer" }

} 
```


If you read the [Binding the model to world transform](../tutorial/19-Binding_the_model_to_world_transform.md) tutorial, you'll notice that the `uniformBindings` we declare here are a bit different. This is because we have to declare where our bindings properties should be read from. Our `uniformBindings` declare the following fields:

-   `property`: the name of the data property that should be set in our application;
-   `source`: where the data should be read from.

The `source` declaration can have three values:

-   `target`: the bound values should be read from the node that will be hosting the `Surface` component;
-   `renderer`: the bound values should be read from the node that hosts the `Renderer` component;
-   `root`: the bound values should be read from the scene root node.

By default, the `source` field is set to `target`. But in the case of our camera, the `PerspectiveCamera` is located on the same node than the `Renderer`. Thus, we set the `source` field to `renderer`. To learn more about data binding and binding sources, please read the [Understanding data binding](../article/Understanding_data_binding.md) article.

The `PerspectiveCamera` also provides the `camera.worldToScreenMatrix`, which is the result of the view matrix mutiplied with the projection. Using this property will save us some computation in our vertex shader:

```javascript
{

 // ...

 "uniformBindings" : {
   "uColor" : "material[${materialId}].color",
   "uModelToWorldMatrix" : "transform.modelToWorldMatrix",
   "uWorldToScreenMatrix" : { "property" : "camera.worldToScreenMatrix", "source" : "renderer" }
 },

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

   // ...

} 
```


You can learn more about the `*.effect` files format in the [Effect files format reference](../article/Effect_file_format_reference.md) article.

Step 2: Updating the application code
-------------------------------------

Now that our view matrix is bound, we don't have to set it manually. But we have to make sure the property it's bound to is actually available! As this property is declared by the `PerspectiveCamera` component, we have to make sure it is available somewhere in our scene.

```cpp
auto camera = scene::Node::create()

 ->addComponent(Transform::create(Matrix4x4::create()->lookAt(Vector3::create(), Vector3::create(-5.0f, 5.0f, 5.0f))))
 ->addComponent(Renderer::create())
 ->addComponent(PerspectiveCamera::create((float)WINDOW_WIDTH / (float)WINDOW_HEIGHT));

root->addChild(camera); 
```


Note that because we will likely need to move and orient our camera, we also add a `Transform` component to our `camera` scene node. To learn more about this component, you can read the [Moving objects](../tutorial/04-Moving_objects.md) tutorial.

**Attention!** Always make sure the `PerspectiveCamera` is added on the same node as the `Renderer` component that is supposed to perform the rendering operations. As every scene might have multiple cameras, their respective `[data::Provider`](data::Provider`) has to be added to the `[data::Container`](data::Container`) of the same node than the `Renderer`. In short, the properties of a `PerspectiveCamera` will be available only to the `Renderer` that is on the same node; which makes it possible to have multiple camera/renderer in the same scene.

Because all of our camera-related properties are now handled by data binding, we also have to update our code to comment (or simply remove) any corresponding `Effect::setUniform()`:

```cpp
//myCustomEffect->setUniform("uViewMatrix", Matrix4x4::create()); //myCustomEffect->setUniform("uProjectionMatrix", Matrix4x4::create()->perspective((float)WINDOW_WIDTH / (float)WINDOW_HEIGHT)); 
```


Final code
----------

asset/effect/MyCustomEffect.effect 
```javascript
{

 "name" : "MyCustomEffect",
 "attributeBindings" : {
   "aPosition" : "geometry[${geometryId}].position"
 },
 "uniformBindings" : {
   "uColor" : "material[${materialId}].color",
   "uModelToWorldMatrix" : "transform.modelToWorldMatrix",
   "uWorldToScreenMatrix" : { "property" : "camera.worldToScreenMatrix", "source" : "renderer" }
 },
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

} 
```


src/main.cpp 
```cpp
#include "minko/Minko.hpp" 
#include "minko/MinkoSDL.hpp"

#include "MyCustomMaterial.hpp"

using namespace minko; 
using namespace minko::math; 
using namespace minko::component;

const uint WINDOW_WIDTH = 800; 
const uint WINDOW_HEIGHT = 600;

int main(int argc, char** argv) {

 auto canvas = Canvas::create("Minko Tutorial - Binding the camera", WINDOW_WIDTH, WINDOW_HEIGHT);
 auto sceneManager = component::SceneManager::create(canvas->context());
 sceneManager->assets()->queue("effect/MyCustomEffect.effect");
 auto complete = sceneManager->assets()->complete()->connect([&](file::AssetLibrary::Ptr assets)
 {
   auto root = scene::Node::create("root")
     ->addComponent(sceneManager);
   auto camera = scene::Node::create()
     ->addComponent(Renderer::create(0x7f7f7fff))
     ->addComponent(Transform::create(Matrix4x4::create()->lookAt(Vector3::create(), Vector3::create(-5.0f, 5.0f, 5.0f))))
     ->addComponent(PerspectiveCamera::create((float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, (float)PI * 0.25f, .1f, 1000.f));
   root->addChild(camera);
   auto myCustomEffect     = assets->effect("effect/MyCustomEffect.effect");
   auto myCustomMaterial   = material::MyCustomMaterial::create();
   auto cube = scene::Node::create("cube")
     ->addComponent(Transform::create(
       Matrix4x4::create()->translation(0.f, 0.f, -5.f)
     ))
     ->addComponent(Surface::create(
       geometry::CubeGeometry::create(assets->context()),
       myCustomMaterial,
       myCustomEffect
     ));
   root->addChild(cube);
   myCustomMaterial->color(Vector4::create(1.f, 0.f, 0.f, 1.f));

   auto enterFrame = canvas->enterFrame()->connect([&](Canvas::Ptr canvas, float t, float dt)
   {
     cube->component<Transform>()->matrix()->prependRotationY(0.01f);
     sceneManager->nextFrame(t, dt);
   });
   canvas->run();
 });
 sceneManager->assets()->load();
 return 0;

} 
```


