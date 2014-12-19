A point light casts illumination in every direction from a single point in 3D space. Point lights are useful for simulating any omnidirectional light source: Light-bulbs, candles, Christmas tree lights, gun-fire etc.

![](../../doc/image/Pointlight.jpg "../../doc/image/Pointlight.jpg")

As the point light is emitting in every directions, his reflection can be multiple places in the environment.

![](../../doc/image/Pointlightexample.jpg "../../doc/image/Pointlightexample.jpg")

In this more complex setup, you can see how multiple point lights render on a complex environment.

Step 1: Instantiate a point light
---------------------------------

The first very first thing you need to do is to include the following header to your .cpp file.

```cpp
#include <PointLight.hpp> 
```


To create a new [PointLight](http://doc.v3.minko.io/reference/classminko_1_1component_1_1_point_light.html) object, we simply use the `PointLight::create(float attenuationDistance=-1.0f)` static method. The argument passed to the method is optional and defines the attenuation of the light emitted proportionally to the distance, the default value is -1.

Step 2: Adding a point light to the scene
-----------------------------------------

To add a point light to a scene, you have to understand that a `Scene` is a graph composed of one or more `Node`. Those nodes can contain elements that are called `Component`.

As a `PointLight` is a `Component` you'll have to follow this logic to create and add a light to a scene.

```cpp


 // Creating the Node
 auto pointLightNode = scene::Node::create("pointLight");
       
 // Adding the Component PointLight
 pointLightNode->addComponent(PointLight::create(0.1f));

// Adding the Node to the root of the scene graph
 root->addChild(pointLightNode);

```


As you can see in the code above, we first create a node named "pointLight" to which we add an instance of a point light. Of course you can chain those methods to perform actions in one line of code

```cpp


 // Creating the Node & adding the Component PointLight
 auto pointLightNode = scene::Node::create("pointLight")->addComponent(PointLight::create(0.1f));

```


Step 3: Change point light properties
-------------------------------------

you can directly change way the light appears by modifying it's public parameters, for example :

```cpp


auto pointLight = PointLight::create();

pointLight->diffuse(.4f); 
```


Step 4: Remove a point light from the scene
-------------------------------------------

You may sometime need to remove a light from a scene, to do this you simply need to remove the point light from the `Node` it has been added to before.

```cpp


 // Adding an point light
 auto pointLightNode = scene::Node::create("pointLight")->addComponent(PointLight::create(0.1f));

 // retrieving the component 
 auto pointLight = pointLightNode->component<PointLight>(0);

 // Remove the component from the node
 pointLightNode->removeComponent(pointLight);

```


As you can see above, the first step is to retrieve the light component with previously created and then remove it from the `Node`

Final code
----------

```cpp
#include "minko/Minko.hpp" 
#include "minko/MinkoPNG.hpp" 
#include "minko/MinkoSDL.hpp"

using namespace minko; 
using namespace minko::math; 
using namespace minko::component;

const uint WINDOW_WIDTH = 800; 
const uint WINDOW_HEIGHT = 600;

int main(int argc, char** argv) {

   auto canvas = Canvas::create("Minko Tutorial - Working with point lights", WINDOW_WIDTH, WINDOW_HEIGHT);
   auto sceneManager = component::SceneManager::create(canvas->context());

   // setup assets
   sceneManager->assets()->defaultOptions()->generateMipmaps(true);
   sceneManager->assets()->registerParser<[file::PNGParser>](file::PNGParser>)("png");
   sceneManager->assets()
       ->queue("effect/Sprite.effect")
       ->queue("effect/Phong.effect")
       ->queue("texture/sprite-pointlight.jpg");

   auto complete = sceneManager->assets()->complete()->connect([&](file::AssetLibrary::Ptr assets)
   {
       auto root = scene::Node::create("root")->addComponent(sceneManager);

       auto camera = scene::Node::create("camera")
           ->addComponent(Renderer::create(0x7f7f7fff))
           ->addComponent(Transform::create(
           Matrix4x4::create()->lookAt(Vector3::create(0.f, 1.f, 0.f), Vector3::create(0.f, 1.f, -3.f))
           ))
           ->addComponent(PerspectiveCamera::create(
           (float) WINDOW_WIDTH / (float) WINDOW_HEIGHT, (float) PI * 0.25f, .1f, 1000.f)
           );
       root->addChild(camera);

       // create a ground
       auto ground = scene::Node::create("ground")
           ->addComponent(Surface::create(
           geometry::QuadGeometry::create(assets->context()),
           material::BasicMaterial::create()->diffuseColor(Vector4::create(0.5f, 0.5f, 0.5f, 1.f)),
           assets->effect("effect/Phong.effect")
           ))
           ->addComponent(Transform::create(Matrix4x4::create()->appendScale(4.f)->appendRotationX(-(PI /2))));
       root->addChild(ground);

       // create a left wall
       auto leftWall = scene::Node::create("leftWall")
           ->addComponent(Surface::create(
           geometry::QuadGeometry::create(assets->context()),
           material::BasicMaterial::create()->diffuseColor(Vector4::create(0.5f, 0.5f, 0.5f, 1.f)),
           assets->effect("effect/Phong.effect")
           ))
           ->addComponent(Transform::create(Matrix4x4::create()->appendScale(4.f)->appendRotationY(-(PI / 2))->appendTranslation(1.f, 1.f, 0.f)));
       root->addChild(leftWall);

       // create a right wall
       auto rightWall = scene::Node::create("rightWall")
           ->addComponent(Surface::create(
           geometry::QuadGeometry::create(assets->context()),
           material::BasicMaterial::create()->diffuseColor(Vector4::create(0.5f, 0.5f, 0.5f, 1.f)),
           assets->effect("effect/Phong.effect")
           ))
           ->addComponent(Transform::create(Matrix4x4::create()->appendScale(4.f)->appendRotationY((PI / 2))->appendTranslation(-1.f, 1.f, 0.f)));
       root->addChild(rightWall);

       // create a back wall
       auto backWall = scene::Node::create("backWall")
           ->addComponent(Surface::create(
           geometry::QuadGeometry::create(assets->context()),
           material::BasicMaterial::create()->diffuseColor(Vector4::create(0.5f, 0.5f, 0.5f, 1.f)),
           assets->effect("effect/Phong.effect")
           ))
           ->addComponent(Transform::create(Matrix4x4::create()->appendScale(4.f)->appendRotationX(PI)->appendTranslation(0.f, 1.f, 1.f)));
       root->addChild(backWall);

       // create the point light node
       auto pointLightNode = scene::Node::create("pointLight")
           ->addComponent(Transform::create(Matrix4x4::create()->translation(0, 1.f, 0)));

       // add a sprite to have a light representation
       pointLightNode->addComponent(Surface::create(
           geometry::QuadGeometry::create(assets->context()),
           material::Material::create()
           ->set("diffuseMap", assets->texture("texture/sprite-pointlight.jpg"))
           ->set("diffuseTint", Vector4::create(1.f, 1.f, 1.f, 1.f)),
           assets->effect("effect/Sprite.effect")
           ));

       // create the point light component
       auto pointLight = PointLight::create();

       // update the point light component attributes
       pointLight->diffuse(0.5f);

       // add the component to the point light node
       pointLightNode->addComponent(pointLight);

       // add the Node to the root of the scene graph
       root->addChild(pointLightNode);

       auto enterFrame = canvas->enterFrame()->connect([&](Canvas::Ptr canvas, float t, float dt)
       {
           sceneManager->nextFrame(t, dt);
       });

       canvas->run();
   });

   sceneManager->assets()->load();

   return 0;

} 
```


