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
#include "minko/MinkoJPEG.hpp"
#include "minko/MinkoSDL.hpp"
#include "minko/MinkoOculus.hpp"

using namespace minko;
using namespace minko::scene;
using namespace minko::component;
using namespace minko::math;

const std::string   CUBE_TEXTURE        = "texture/entrance_hall_hor.jpg";

const uint          NUM_SPHERES         = 24;
const float         SPHERES_DIST        = 5.0f;
const float         SPHERES_MOVE_AMPL   = 2.0f;
const float         SPHERES_MOVE_SPEED  = 2.5f;
const float         SPHERES_PRIORITY    = render::Priority::TRANSPARENT;

const uint          NUM_QUADS           = 16;
const float         QUADS_DIST          = 7.5f;
const float         QUADS_MOVE_AMPL     = 2.0f;
const float         QUADS_MOVE_SPEED    = 2.5f;
const float         QUADS_PRIORITY      = render::Priority::TRANSPARENT + 1.0f;

typedef std::pair<Transform::Ptr, Vector3::Ptr> AnimData;

Node::Ptr
createObjectGroup(unsigned int                  numObjects,
                  bool                          doSpheres,
                  float                         distanceToEye,
                  float                         priority,
                  file::AssetLibrary::Ptr       assets,
                  std::vector<AnimData>&        nodeAnimData);

void
animateObjects(float                            moveAmplitude,
               float                            moveSpeed,
               const float                      currTime,
               const std::vector<AnimData>&     nodeAnimData);

int
main(int argc, char** argv)
{
    auto canvas = Canvas::create("Minko Example - Oculus");

    auto sceneManager = SceneManager::create(canvas);

    auto loader = sceneManager->assets()->loader();

    canvas->desiredFramerate(120);

    // setup assets
    loader->options()
        ->resizeSmoothly(true)
        ->generateMipmaps(true)
        ->registerParser<file::JPEGParser>("jpg");

    sceneManager->assets()
        ->geometry("cube",        geometry::CubeGeometry::create(sceneManager->assets()->context()))
        ->geometry("quad",        geometry::QuadGeometry::create(sceneManager->assets()->context()))
        ->geometry("sphere",    geometry::SphereGeometry::create(sceneManager->assets()->context(), 16, 16));

    loader
        ->queue(CUBE_TEXTURE, loader->options()->clone()->isCubeTexture(true))
        ->queue("effect/Basic.effect")
        ->queue("effect/OculusVR/OculusVR.effect");

    std::vector<AnimData> spheresAnimData;
    std::vector<AnimData> quadsAnimData;

    Node::Ptr spheres;
    Node::Ptr quads;
    Node::Ptr camera;

    auto _ = sceneManager->assets()->loader()->complete()->connect([&](file::Loader::Ptr loader)
    {
        auto root = scene::Node::create("root")
            ->addComponent(sceneManager);

        camera = scene::Node::create("camera")
            ->addComponent(Transform::create())
            ->addComponent(VRCamera::create(canvas->width(), canvas->height(), 0.1f, 100.0f));

        spheres = createObjectGroup(NUM_SPHERES, false, SPHERES_DIST, SPHERES_PRIORITY, sceneManager->assets(), spheresAnimData);
        quads = createObjectGroup(NUM_QUADS, true, QUADS_DIST, QUADS_PRIORITY, sceneManager->assets(), quadsAnimData);

        auto cube = scene::Node::create("cube")
            ->addComponent(Transform::create(
                Matrix4x4::create()->appendScale(50.0f)
            ))
            ->addComponent(Surface::create(
                sceneManager->assets()->geometry("cube"),
                material::BasicMaterial::create()
                    ->diffuseCubeMap(sceneManager->assets()->cubeTexture(CUBE_TEXTURE))
                    ->triangleCulling(render::TriangleCulling::FRONT),
                    //->diffuseColor(0xffffffff),
                sceneManager->assets()->effect("basic")
            ));

        root->addChild(camera);
        root->addChild(spheres);
        root->addChild(quads);
        root->addChild(cube);
    });

    auto resized = canvas->resized()->connect([&](AbstractCanvas::Ptr canvas, uint width, uint height)
    {
        camera->component<VRCamera>()->updateViewport(width, height);
    });

    auto enterFrame = canvas->enterFrame()->connect([&](AbstractCanvas::Ptr canvas, float time, float deltaTime)
    {
        //animateObjects(SPHERES_MOVE_AMPL, SPHERES_MOVE_SPEED, time, spheresAnimData);
        spheres->component<Transform>()->matrix()->appendRotationY(.001f);

        //animateObjects(QUADS_MOVE_AMPL, QUADS_MOVE_SPEED, time, quadsAnimData);
        quads->component<Transform>()->matrix()->appendRotationY(-.0005f);

        sceneManager->nextFrame(time, deltaTime);
    });

    loader->load();
    canvas->run();
}

Node::Ptr
createObjectGroup(unsigned int              numObjects,
                  bool                      doQuads,
                  float                     distanceToEye,
                  float                     priority,
                  file::AssetLibrary::Ptr   assets,
                  std::vector<AnimData>&    nodeAnimData)
{
    nodeAnimData.clear();
    nodeAnimData.resize(numObjects);

    const float invNumObjects   = 1.0f / float(numObjects);
    const float maxSize         = float(M_PI) * distanceToEye * invNumObjects - 1e-2f;
    const float minSize         = maxSize * 0.75f;
    const float deltaAng        = 2.0f * float(M_PI) * invNumObjects;
    const float cDelta          = std::cos(deltaAng);
    const float sDelta          = std::sin(deltaAng);

    auto objectGroup = scene::Node::create(doQuads ? "quads" : "spheres")
        ->addComponent(Transform::create());

    if (assets == nullptr)
        return objectGroup;

    float cAng = 1.0f;
    float sAng = 0.0f;
    float cPrev = 1.0f;
    float sPrev = 0.0f;

    for (uint i = 0; i < numObjects; ++i)
    {
        auto color          = Color::hslaToRgba(((i + (doQuads ? numObjects >> 1 : 0)) % numObjects) * invNumObjects, 1.0f, 0.5f, 0.5f);
        auto size           = minSize + (rand() / float(RAND_MAX)) * (maxSize - minSize);

        auto toEyeVector    = Vector3::create(cAng, 0.0f, sAng);

        auto matrix         = Matrix4x4::create();

        if (doQuads)
            matrix
                ->appendScale(2.0f * size, 3.0f * size, 1.0f)
                ->append(Matrix4x4::create()->lookAt(Vector3::zero(), Vector3::create(toEyeVector)->scaleBy(distanceToEye)));
        else
            matrix
                ->appendScale(2.0f * size)
                ->appendTranslation(Vector3::create(toEyeVector)->scaleBy(distanceToEye));

        auto transform      = Transform::create(matrix);

        auto objectNode     = scene::Node::create((doQuads ? "quad_" : "sphere_") + std::to_string(i))
            ->addComponent(transform)
            ->addComponent(Surface::create(
                assets->geometry(doQuads? "quad" : "sphere"),
                material::BasicMaterial::create()
                    ->diffuseColor(color)
                    ->triangleCulling(doQuads ? render::TriangleCulling::FRONT : render::TriangleCulling::BACK)
                    ->set("priority", priority)
                    ->set("blendMode", render::Blending::Mode::ALPHA)
                    ->set("zsorted", false),
                assets->effect("basic")
            ));

        cPrev = cAng;
        sPrev = sAng;

        cAng = cDelta * cPrev - sDelta * sPrev;
        sAng = cDelta * sPrev + sDelta * cPrev;

        objectGroup->addChild(objectNode);
        nodeAnimData[i] = std::make_pair(transform, toEyeVector);
    }

    return objectGroup;
}

void
animateObjects(float                            moveAmplitude,
               float                            moveSpeed,
               const float                      currTime,
               const std::vector<AnimData>&     nodeAnimData)
{
    static float    prevTime    = 0;
    const  float    fPrev       = 0.5f + 0.5f * cosf(prevTime * moveSpeed); // [0, 1]
    const  float    fCurr       = 0.5f + 0.5f * cosf(currTime * moveSpeed); // [0, 1]

    auto        translation  = Vector3::create();

    for (auto& animData : nodeAnimData)
    {
        translation
            ->copyFrom(animData.second)
            ->scaleBy(moveAmplitude * (fPrev - fCurr));

        animData.first->matrix()->appendTranslation(translation);
    }

    prevTime = currTime;
}
