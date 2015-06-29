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

typedef std::pair<Transform::Ptr, math::vec3> AnimData;

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
    auto canvas = Canvas::create("Minko Example - VR");

    auto sceneManager = SceneManager::create(canvas);

    auto loader = sceneManager->assets()->loader();

    canvas->desiredFramerate(120);

    // setup assets
    loader->options()
        ->resizeSmoothly(true)
        ->generateMipmaps(true)
        ->registerParser<file::JPEGParser>("jpg");

    sceneManager->assets()
        ->geometry("cube", geometry::CubeGeometry::create(sceneManager->assets()->context()))
        ->geometry("quad", geometry::QuadGeometry::create(sceneManager->assets()->context()))
        ->geometry("sphere", geometry::SphereGeometry::create(sceneManager->assets()->context(), 16, 16));

    loader
        ->queue(CUBE_TEXTURE, loader->options()->clone()->isCubeTexture(true))
        ->queue("effect/Basic.effect")
        ->queue("effect/OculusVR/OculusVR.effect")
    ;

    std::vector<AnimData> spheresAnimData;
    std::vector<AnimData> quadsAnimData;

    Node::Ptr spheres;
    Node::Ptr quads;
    Node::Ptr camera;

    Signal<>::Slot actionButtonPressed = nullptr;
    Signal<>::Slot actionButtonReleased = nullptr;

    auto _ = sceneManager->assets()->loader()->complete()->connect([&](file::Loader::Ptr loader)
    {
        auto root = scene::Node::create("root")
            ->addComponent(sceneManager);

		camera = scene::Node::create("camera")
			->addComponent(Transform::create());

		auto HMDDetected = VRCamera::detected();

		if (HMDDetected)
			camera->addComponent(VRCamera::create(canvas->width(), canvas->height(), 0.1f, 100.0f));
		else
		{
			camera
				->addComponent(PerspectiveCamera::create(canvas->aspectRatio(), 1.0f))
				->addComponent(Renderer::create(0x050514ff));
		}
		
        spheres = createObjectGroup(NUM_SPHERES, false, SPHERES_DIST, SPHERES_PRIORITY, sceneManager->assets(), spheresAnimData);
        quads = createObjectGroup(NUM_QUADS, true, QUADS_DIST, QUADS_PRIORITY, sceneManager->assets(), quadsAnimData);

		auto cubeMaterial = material::BasicMaterial::create();
		cubeMaterial->diffuseCubeMap(sceneManager->assets()->cubeTexture(CUBE_TEXTURE));
		cubeMaterial->triangleCulling(render::TriangleCulling::FRONT);
		//cubeMaterial->diffuseColor(0xffffffff);

        auto cube = scene::Node::create("cube")
            ->addComponent(Transform::create(
                math::scale(math::vec3(50.0f)) * math::mat4()
            ))
            ->addComponent(Surface::create(
                sceneManager->assets()->geometry("cube"),
                cubeMaterial,
                sceneManager->assets()->effect("effect/Basic.effect")
            ));

        //root->addChild(cube);
        root->addChild(camera);
        root->addChild(spheres);
        root->addChild(quads);

        if (camera->hasComponent<VRCamera>())
        {
            actionButtonPressed = camera->component<VRCamera>()->actionButtonPressed()->connect([&]()
            {
                LOG_INFO("HMD Action Button Pressed!");
            });

            actionButtonReleased = camera->component<VRCamera>()->actionButtonReleased()->connect([&]()
            {
                LOG_INFO("HMD Action Button Released!");
            });
        }
    });

    auto resized = canvas->resized()->connect([&](AbstractCanvas::Ptr canvas, uint width, uint height)
    {
		if (camera->hasComponent<VRCamera>())
			camera->component<VRCamera>()->updateViewport(width, height);
		else if (camera->hasComponent<PerspectiveCamera>())
			camera->component<PerspectiveCamera>()->aspectRatio(float(width) / float(height));
    });

    auto enterFrame = canvas->enterFrame()->connect([&](Canvas::Ptr canvas, float time, float deltaTime)
    {
        //animateObjects(SPHERES_MOVE_AMPL, SPHERES_MOVE_SPEED, time, spheresAnimData);
        spheres->component<Transform>()->matrix(math::rotate(.001f, math::vec3(0, 1, 0)) * spheres->component<Transform>()->matrix());

        //animateObjects(QUADS_MOVE_AMPL, QUADS_MOVE_SPEED, time, quadsAnimData);
        quads->component<Transform>()->matrix(math::rotate(-.0005f, math::vec3(0, 1, 0)) * quads->component<Transform>()->matrix());

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

    const float invNumObjects = 1.0f / float(numObjects);
    const float maxSize = float(M_PI) * distanceToEye * invNumObjects - 1e-2f;
    const float minSize = maxSize * 0.75f;
    const float deltaAng = 2.0f * float(M_PI) * invNumObjects;
    const float cDelta = std::cos(deltaAng);
    const float sDelta = std::sin(deltaAng);

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
        auto color = math::vec4(math::rgbColor(math::vec3(((i + (doQuads ? numObjects >> 1 : 0)) % numObjects * invNumObjects) * 360, 1.0f, 0.5f)), 0.5f);
        auto size = minSize + (rand() / float(RAND_MAX)) * (maxSize - minSize);

        auto toEyeVector = math::vec3(cAng, 0.0f, sAng);
        auto matrix = math::mat4();

        if (doQuads)
        {
            matrix = math::scale(math::vec3(2.0f * size, 3.0f * size, 1.0f)) * matrix;
            auto lookAt = math::inverse(math::lookAt(math::vec3(toEyeVector) * distanceToEye, math::vec3(), math::vec3(0, 1, 0)));
            matrix = lookAt * matrix;
        }
        else
        {
            matrix = math::scale(math::vec3(2.0f * size)) * matrix;
            matrix = math::translate(math::vec3(toEyeVector) * distanceToEye) * matrix;
        }

        auto transform = Transform::create(matrix);

        auto objectMaterial = material::BasicMaterial::create();
        objectMaterial->data()
            ->set("diffuseColor", color)
            ->set("triangleCulling", doQuads ? render::TriangleCulling::FRONT : render::TriangleCulling::BACK)
            ->set("priority", priority)
            ->set("blendMode", render::Blending::Mode::ALPHA)
            ->set("zsorted", false);

        auto objectNode = scene::Node::create((doQuads ? "quad_" : "sphere_") + std::to_string(i))
            ->addComponent(transform)
            ->addComponent(Surface::create(
                assets->geometry(doQuads? "quad" : "sphere"),
                objectMaterial,
                assets->effect("effect/Basic.effect")
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

    auto translation = math::vec3();

    for (auto& animData : nodeAnimData)
    {
        translation = animData.second * (moveAmplitude * (fPrev - fCurr));

        animData.first->matrix(math::translate(translation) * animData.first->matrix());
    }

    prevTime = currTime;
}
