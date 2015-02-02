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
#include "minko/MinkoBullet.hpp"

#define DISPLAY_COLLIDERS

using namespace minko;
using namespace minko::scene;
using namespace minko::component;

const std::string   TEXTURE_FILENAME    = "texture/box.png";
const float         GROUND_WIDTH        = 5.0f;
const float         GROUND_HEIGHT       = 0.25f;
const float         GROUND_DEPTH        = 5.0f;
const float         GROUND_THICK        = 0.05f;

const float         MIN_MASS            = 1.0f;
const float         MAX_MASS            = 3.0f;
const float         MIN_SCALE           = 0.2f;
const float         MAX_SCALE           = 1.0f;
const float         IMPULSE_STRENGTH    = 3.0f;
const auto          MIN_DROP_POS        = math::vec3(-GROUND_WIDTH * 0.5f + 0.5f, 5.0f, -GROUND_DEPTH * 0.5f + 0.5f);
const auto          MAX_DROP_POS        = math::vec3( GROUND_WIDTH * 0.5f - 0.5f, 5.0f,  GROUND_DEPTH * 0.5f - 0.5f);

const unsigned int  MAX_NUM_OBJECTS     = 32;

Signal<input::Keyboard::Ptr>::Slot keyDown;
Signal<input::Touch::Ptr, int, float, float>::Slot touchDown;

uint numObjects = 0;

Node::Ptr
createPhysicsObject(unsigned int id, file::AssetLibrary::Ptr, bool isCube);

void
addPhysicObject(Node::Ptr root, file::AssetLibrary::Ptr assets);

void
bouncePhysicObjects(Node::Ptr root);

int
main(int argc, char** argv)
{
    auto canvas = Canvas::create("Minko Example - Physics");
    auto sceneManager = SceneManager::create(canvas);

    // setup assets
    sceneManager->assets()->loader()->options()
        ->resizeSmoothly(true)
        ->generateMipmaps(true)
        ->disposeIndexBufferAfterLoading(true)
        ->disposeTextureAfterLoading(true)
        ->disposeVertexBufferAfterLoading(true)
        ->registerParser<file::PNGParser>("png");

    sceneManager->assets()
        ->geometry("sphere", geometry::SphereGeometry::create(sceneManager->assets()->context(), 16, 16))
        ->geometry("cube", geometry::CubeGeometry::create(sceneManager->assets()->context()));

    sceneManager->assets()->loader()
#ifdef DISPLAY_COLLIDERS
        ->queue("effect/Line.effect")
#endif // DISPLAY_COLLIDERS
        ->queue(TEXTURE_FILENAME)
        ->queue("effect/Phong.effect");

    std::cout << "[space]\tdrop an object onto the scene (up to " << MAX_NUM_OBJECTS << ")" << std::endl;
    std::cout << "[I]\tapply vertical impulse to a ramdomly-picked object of your scene" << std::endl;

    auto root = scene::Node::create("root")
        ->addComponent(sceneManager)
        ->addComponent(bullet::PhysicsWorld::create());

    auto camera = scene::Node::create("camera")
        ->addComponent(Renderer::create(0x7f7f7fff))
        ->addComponent(Transform::create(
        math::inverse(math::lookAt(math::vec3(5.0f, 1.5f, 5.0f), math::vec3(), math::vec3(0.f, 1.f, 0.f))
        )))
        ->addComponent(PerspectiveCamera::create(canvas->aspectRatio()));

    root->addChild(camera);

    auto groundNode = scene::Node::create("groundNode")->addComponent(Transform::create(
        math::rotate(-float(M_PI) * 0.1f, math::vec3(0.f, 0.f, 1.f)) * math::mat4()
    ));

    // set-up lighting environment
    auto ambientLightNode = scene::Node::create("ambientLight")
        ->addComponent(AmbientLight::create());

    auto dirLightNode = scene::Node::create("dirLight")
        ->addComponent(DirectionalLight::create())
        ->addComponent(
            Transform::create(
                math::inverse(
                    math::lookAt(
                        math::vec3(0.5f, 5.0f, 3.0f), 
                        math::vec3(), 
                        math::vec3(0.f, 1.f, 0.f)
                    )
                )
            )
        );

    dirLightNode->component<DirectionalLight>()->specular(0.5f);

    root
        ->addChild(ambientLightNode)
        ->addChild(dirLightNode);

#ifdef DISPLAY_COLLIDERS
    root->data().addProvider(canvas->data());
#endif // DISPLAY_COLLIDERS

    auto _ = sceneManager->assets()->loader()->complete()->connect([=](file::Loader::Ptr loader)
    {
        auto material = material::BasicMaterial::create();
        material->diffuseMap(sceneManager->assets()->texture(TEXTURE_FILENAME));

        auto groundNodeA = scene::Node::create("groundNodeA")
            ->addComponent(Transform::create(
                math::scale(math::vec3(GROUND_WIDTH, GROUND_THICK, GROUND_DEPTH)) * math::mat4()
            ))
            ->addComponent(Surface::create(
                sceneManager->assets()->geometry("cube"),
                material,
                sceneManager->assets()->effect("phong")
            ))
            ->addComponent(bullet::Collider::create(
                    bullet::ColliderData::create(
                        0.0f, // static object (no mass)
                        bullet::BoxShape::create(GROUND_WIDTH * 0.5f, GROUND_THICK * 0.5f, GROUND_DEPTH * 0.5f)
                    )
            ));

        auto materialGroundNodeB = material::BasicMaterial::create();
        materialGroundNodeB->diffuseColor(0x241f1cff);

        auto groundNodeB = scene::Node::create("groundNodeB")
            ->addComponent(Transform::create(
                math::translate(math::vec3(0.5f * (GROUND_WIDTH + GROUND_THICK), 0.5f * (GROUND_HEIGHT - GROUND_THICK), 0.0f)) * 
                (math::scale(math::vec3(GROUND_THICK, GROUND_HEIGHT, GROUND_DEPTH)) * math::mat4())
            ))
            ->addComponent(Surface::create(
                sceneManager->assets()->geometry("cube"),
                materialGroundNodeB,
                sceneManager->assets()->effect("phong")
            ))
            ->addComponent(bullet::Collider::create(
                bullet::ColliderData::create(
                    0.0f, // static object (no mass)
                    bullet::BoxShape::create(GROUND_THICK * 0.5f, GROUND_HEIGHT * 0.5f, GROUND_DEPTH * 0.5f))
            ));

        groundNode
            ->addChild(groundNodeA)
            ->addChild(groundNodeB);

        root->addChild(groundNode);

        keyDown = canvas->keyboard()->keyDown()->connect([&](input::Keyboard::Ptr k)
        {
            if (k->keyIsDown(input::Keyboard::SPACE))
                addPhysicObject(root, sceneManager->assets());
            else if (k->keyIsDown(input::Keyboard::I))
                bouncePhysicObjects(root);
        });

        touchDown = canvas->touch()->touchDown()->connect([=](input::Touch::Ptr t, int fingerId, float x, float y)
        {
            x = x / canvas->width();
            y = y / canvas->height();

            // top left corner
            if (x > 0 && x < 0.25 && y > 0 && y < 0.25)
                addPhysicObject(root, sceneManager->assets());
            // top right corner
            if (x > 0.75 && x < 1 && y > 0 && y < 0.25)
                bouncePhysicObjects(root);
        });
    });

    auto resized = canvas->resized()->connect([&](AbstractCanvas::Ptr canvas, uint w, uint h)
    {
        camera->component<PerspectiveCamera>()->aspectRatio(float(w) / float(h));
    });

    auto enterFrame = canvas->enterFrame()->connect([&](Canvas::Ptr canvas, float time, float deltaTime)
    {
        sceneManager->nextFrame(time, deltaTime);
    });

    sceneManager->assets()->loader()->load();
    canvas->run();
}


Node::Ptr
createPhysicsObject(unsigned int id, file::AssetLibrary::Ptr assets, bool isCube)
{
    const float mass = MIN_MASS  + (rand() / (float)RAND_MAX) * (MAX_MASS - MIN_MASS);
    const float size = MIN_SCALE + (rand() / (float)RAND_MAX) * (MAX_SCALE - MIN_SCALE);

    const float startX = MIN_DROP_POS.x + (rand() / (float)RAND_MAX) * (MAX_DROP_POS.x - MIN_DROP_POS.x);
    const float startY = MIN_DROP_POS.y + (rand() / (float)RAND_MAX) * (MAX_DROP_POS.y - MIN_DROP_POS.y);
    const float startZ = MIN_DROP_POS.z + (rand() / (float)RAND_MAX) * (MAX_DROP_POS.z - MIN_DROP_POS.z);

    const float halfSize = 0.5f * size;
    const float hue = (id % 10) * 0.1f;
    auto diffColor = math::rgbColor(math::vec3(hue, 1.0f, 0.5f));
    auto specColor = math::rgbColor(math::vec3(hue, 1.0f, 0.8f));
    const float shininess = 2.0f * (rand() / (float)RAND_MAX) * 6.0f;

    bullet::Collider::Ptr collider = nullptr;

    if (isCube)
    {
        auto boxColliderData = bullet::ColliderData::create(
            mass,
            bullet::BoxShape::create(halfSize, halfSize, halfSize)
        );

        collider = bullet::Collider::create(boxColliderData);
    }
    else
    {
        auto sphColliderData = bullet::ColliderData::create(
            mass,
            bullet::SphereShape::create(halfSize)
        );

        collider = bullet::Collider::create(sphColliderData);
    }

    auto matrix = math::mat4();
    matrix = math::scale(math::vec3(size)) * matrix;
    matrix = math::translate(math::vec3(startX, startY, startZ)) * matrix;

    auto material = material::PhongMaterial::create();
    material->data()->set("specularColor", specColor);
    material->data()->set("shininess", shininess);
    material->data()->set("diffuseColor", diffColor);

    return scene::Node::create("physicsObject_" + std::to_string(id))
        ->addComponent(Transform::create(matrix))
        ->addComponent(Surface::create(
            assets->geometry(isCube ? "cube" : "sphere"),
            material,
            assets->effect("phong")
        ))
        ->addComponent(collider)
#ifdef DISPLAY_COLLIDERS
        ->addComponent(bullet::ColliderDebug::create(assets))
#endif // DISPLAY_COLLIDERS
        ;
}

void
addPhysicObject(Node::Ptr root, file::AssetLibrary::Ptr assets)
{
    if (numObjects < MAX_NUM_OBJECTS)
    {
        auto physicsObject = createPhysicsObject(numObjects, assets, rand() / (float)RAND_MAX > 0.5f);
        root->addChild(physicsObject);
        ++numObjects;

        std::cout << "object #" << numObjects << " dropped" << std::endl;
    }
    else
        std::cout << "You threw away all your possible objects. Try again!" << std::endl;
}

void
bouncePhysicObjects(Node::Ptr root)
{
    auto physicsObjects = NodeSet::create(root)
        ->descendants(true)
        ->where([](Node::Ptr n)
    {
        return n->hasComponent<component::bullet::Collider>()
            && math::length(math::vec3(n->component<component::Transform>()->modelToWorldMatrix()[3])) < 10.0f // still close to the origin
            && n->name().find("physicsObject") != std::string::npos;
    });

    if (!physicsObjects->nodes().empty())
    {
        auto randomId = rand() % physicsObjects->nodes().size();
        auto randomCollider = physicsObjects->nodes()[randomId]->component<component::bullet::Collider>();

        randomCollider->applyImpulse(math::vec3(0.0f, IMPULSE_STRENGTH * randomCollider->colliderData()->mass(), 0.0), math::vec3());
    }
}