#include "minko/Minko.hpp"
#include "minko/MinkoJPEG.hpp"
#include "minko/MinkoPNG.hpp"
#include "minko/MinkoSDL.hpp"
#include "minko/MinkoASSIMP.hpp"
// #include "minko/MinkoSerializer.hpp"
#include "minko/MinkoFX.hpp"

using namespace minko;
using namespace minko::component;
using namespace minko::math;

const float CAMERA_LIN_SPEED    = 0.05f;
const float CAMERA_ANG_SPEED    = float(M_PI) * 2.f / 180.0f;
const float flowMapCycle        = 0.25f;

Signal<input::Keyboard::Ptr>::Slot keyDown;

// #define FLOW_MAP // comment to deactivate flowmap
// #define ENABLE_REFLECTION // comment to deactivate reflections

int
main(int argc, char** argv)
{
    auto canvas = Canvas::create("Minko Example - Water");
    auto sceneManager = SceneManager::create(canvas);
    auto assets = sceneManager->assets();

    canvas->context()->errorsEnabled(true);

    assets->loader()->options()
        ->resizeSmoothly(true)
        ->generateMipmaps(true)
        ->registerParser<file::PNGParser>("png")
        ->registerParser<file::JPEGParser>("jpg")
        ->registerParser<file::PNGParser>("png")
        ->registerParser<file::ASSIMPParser>("dae");

    auto root = scene::Node::create("root")
        ->addComponent(sceneManager);

    auto camera = scene::Node::create("camera")
        ->addComponent(Renderer::create(0x7f7f7fff))
        ->addComponent(Transform::create(math::inverse(math::lookAt(
            math::vec3(), math::vec3(3.f, 3.f, 3.f), math::vec3(0.f, 1.f, 0.f)
        ))))
        ->addComponent(PerspectiveCamera::create(canvas->aspectRatio()));

    auto fxLoader = file::Loader::create(sceneManager->assets()->loader())
        // ->queue("effect/Basic.effect")
        ->queue("effect/Phong.effect");

    auto fxComplete = fxLoader->complete()->connect([&](file::Loader::Ptr l)
    {
        assets->loader()
            ->queue("texture/skybox_texture-diffuse.jpg")
            ->queue("effect/Skybox/Skybox.effect")
            ->queue("effect/Sky.effect")
            ->queue("texture/normalmap.png")
            // ->queue("texture/flowmap.png")
            // ->queue("texture/water_dudv.jpg")
            // ->queue("texture/noise.png")
            ->queue("effect/Water/Water.effect")
            // ->queue("effect/Reflection/PlanarReflection.effect");
            ->load();
    });


    auto _ = assets->loader()->complete()->connect([=](file::Loader::Ptr loader)
    {
#ifdef ENABLE_REFLECTION
        auto reflectionComponent = Reflection::create(sceneManager->assets(), 2048, 2048, 0x00000000);
        camera->addComponent(reflectionComponent);
#endif
        root->addChild(camera);

        auto fogColor = math::vec4(.9f, .9f, .9f, 1.f);

        // auto sky = scene::Node::create()
        //     ->addComponent(Surface::create(
        //         geometry::SphereGeometry::create(assets->context(), 16, 16),
        //         material::Material::create()->set({
        //             { "diffuseLatLongMap", assets->texture("texture/skybox_texture-diffuse.jpg")->sampler() },
        //             { "gammaCorrection", 2.2f },
        //             { "fogColor", fogColor }//,
        //             // { "sunDirection", math::vec3(1.f, 0.f, 0.f) },
        //             // { "reileighCoefficient", 1.f },
        //             // { "mieCoefficient", .053f },
        //             // { "mieDirectionalG", .75f },
        //             // { "turbidity", 1.f }
        //         }),
        //         assets->effect("effect/Skybox/Skybox.effect")
        //         // assets->effect("effect/Skybox.effect")
        //     ));
        // root->addChild(sky);

        auto sky = scene::Node::create()
            ->addComponent(Surface::create(
                geometry::QuadGeometry::create(assets->context()),
                material::Material::create()->set({
                    { "gammaCorrection", 2.2f },
                    { "fogColor", fogColor },
                    { "sunDirection", math::vec3(1.f, 0.f, 0.f) },
                    { "reileighCoefficient", 1.f },
                    { "mieCoefficient", .053f },
                    { "mieDirectionalG", .75f },
                    { "turbidity", 1.f }
                }),
                assets->effect("effect/Sky.effect")
                // assets->effect("effect/Skybox.effect")
            ));
        root->addChild(sky);

        auto waterMaterial = material::WaterMaterial::create(5);
        auto waves = scene::Node::create("waves")
            ->addComponent(Transform::create(math::rotate(-float(M_PI) / 2.f, math::vec3(1.f, 0.f, 0.f))))
            ->addComponent(Surface::create(
                geometry::QuadGeometry::create(assets->context(), 200, 200, 1000, 1000),
                waterMaterial->set({
                    { "fogTechnique", (int)material::FogTechnique::LIN },
                    { "fogBounds", math::vec2(300.f, 500.f) },
                    { "fogColor", fogColor },
                    { "uvScale", math::vec2(2.f) },
                    { "environmentMap2d", assets->texture("texture/skybox_texture-diffuse.jpg")->sampler() },
                    // { "normalMap", assets->texture("texture/normalmap.png")->sampler() },
                    { "gammaCorrection", 2.2f }
                }),
                assets->effect("effect/Water/Water.effect")
                // assets->effect("effect/Phong.effect")
            ));
        // root->addChild(waves);

// #ifdef FLOW_MAP
//         waterMaterial->noiseMap(assets->texture("texture/noise.png"));
//         waterMaterial->flowMap(assets->texture("texture/flowmap.png"));
//         waterMaterial->flowMapCycle(flowMapCycle);
//         waterMaterial->flowMapOffset1(0.f);
//         waterMaterial->flowMapOffset2(flowMapCycle / 2.f);
// #endif
//
// #ifdef ENABLE_REFLECTION
//         waterMaterial->dudvMap(sceneManager->assets()->texture("texture/water_dudv.jpg"));
//         waterMaterial->reflectionMap(camera->components<Reflection>()[0]->getRenderTarget());
//         waterMaterial->reflectivity(0.4f);
//         waterMaterial->dudvFactor(0.02f);
//         waterMaterial->dudvSpeed(0.00015f);
//         waterMaterial->diffuseColor(0x052540FF);
// #else
//         waterMaterial->diffuseColor(0x306090D0);
// #endif

        waterMaterial->normalMap(assets->texture("texture/normalmap.png"));
        waterMaterial->diffuseColor(0x001033FF);

        // waterMaterial->flowMapScale(1.f);
        waterMaterial->shininess(64.f);
        waterMaterial->specularColor(0xFFFFFF33);

// #ifdef FLOW_MAP
//         waterMaterial->setAmplitude(0, 0.06f);
//         waterMaterial->setAmplitude(1, 0.0173f);
//         waterMaterial->setAmplitude(2, 0.0312f);
//         waterMaterial->setAmplitude(3, 0.0287f);
//         waterMaterial->setAmplitude(4, 0.0457f);
// #else
        waterMaterial->setAmplitude(0, 1.43f);
        waterMaterial->setAmplitude(1, .373f);
        waterMaterial->setAmplitude(2, .112f);
        waterMaterial->setAmplitude(3, .187f);
        waterMaterial->setAmplitude(4, 1.f);
// #endif
        waterMaterial->setWaveLength(0, 50.0f);
        waterMaterial->setWaveLength(1, 17.7f);
        waterMaterial->setWaveLength(2, 13.13f);
        waterMaterial->setWaveLength(3, 40.17f);
        waterMaterial->setWaveLength(4, 100.f);

        waterMaterial->setSpeed(0, 7.4f);
        waterMaterial->setSpeed(1, 8.8f);
        waterMaterial->setSpeed(2, 3.2f);
        waterMaterial->setSpeed(3, 4.6f);
        waterMaterial->setSpeed(4, 6.f);

        waterMaterial->setDirection(0, math::vec2(1.f, 1.f));
        waterMaterial->setDirection(1, math::vec2(0.1f, 1.f));
        waterMaterial->setCenter(2, math::vec2(1000.f, -1000.f));
        waterMaterial->setCenter(3, math::vec2(1000.f, 1000.f));
        waterMaterial->setDirection(4, math::vec2(1.f, 0.f));

        waterMaterial->setSharpness(0, .5f);
        waterMaterial->setSharpness(1, .5f);
        waterMaterial->setSharpness(2, .3f);
        waterMaterial->setSharpness(3, .5f);
        waterMaterial->setSharpness(4, .5f);

        root->addChild(scene::Node::create()
            ->addComponent(DirectionalLight::create(0.8f, .8f))
            ->addComponent(Transform::create(math::inverse(math::lookAt(
                math::vec3(-.8f, 1.f, 0.f), math::vec3(0.f), math::vec3(0.f, 1.f, 0.f)
            )))));
        root->addChild(scene::Node::create()->addComponent(AmbientLight::create(0.1f)));
    });

    auto resized = canvas->resized()->connect([&](AbstractCanvas::Ptr canvas, uint w, uint h)
    {
        camera->component<PerspectiveCamera>()->aspectRatio(float(w) / float(h));
    });

    float yaw = 0.3f;
    // float pitch = 1.3f;//float(M_PI) * .5f;
    float pitch = float(M_PI) * .5f;
    auto minPitch = 0.f + 0.1f;
    // auto maxPitch = float(M_PI) * .5f - .1f;
    auto maxPitch = float(M_PI) - .1f;
    auto lookAt = math::vec3(0.f, 2.f, 0.f);
    float distance = 3.f;
    float minDistance = 1.f;
    float zoomSpeed = 0.f;

    // handle mouse signals
    auto mouseWheel = canvas->mouse()->wheel()->connect([&](input::Mouse::Ptr m, int h, int v)
    {
        zoomSpeed -= float(v) * .1f;
    });

    Signal<input::Mouse::Ptr, int, int>::Slot mouseMove;
    auto cameraRotationXSpeed = 0.f;
    auto cameraRotationYSpeed = 0.f;

    auto mouseDown = canvas->mouse()->leftButtonDown()->connect([&](input::Mouse::Ptr m)
    {
        mouseMove = canvas->mouse()->move()->connect([&](input::Mouse::Ptr, int dx, int dy)
        {
            cameraRotationYSpeed = (float)dx * .01f;
            cameraRotationXSpeed = (float)dy * -.01f;
        });
    });

    auto mouseUp = canvas->mouse()->leftButtonUp()->connect([&](input::Mouse::Ptr m)
    {
        mouseMove = nullptr;
    });

    auto enterFrame = canvas->enterFrame()->connect([&](Canvas::Ptr canvas, float time, float deltaTime)
    {
        distance += zoomSpeed;
        zoomSpeed *= 0.9f;
        if (distance < minDistance)
            distance = minDistance;

        yaw += cameraRotationYSpeed;
        cameraRotationYSpeed *= 0.9f;

        pitch += cameraRotationXSpeed;
        cameraRotationXSpeed *= 0.9f;

        if (pitch > maxPitch)
            pitch = maxPitch;
        else if (pitch < minPitch)
            pitch = minPitch;

        camera->component<Transform>()->matrix(math::inverse(math::lookAt(
            math::vec3(
                lookAt.x + distance * std::cos(yaw) * std::sin(pitch),
                lookAt.y + distance * std::cos(pitch),
                lookAt.z + distance * std::sin(yaw) * std::sin(pitch)
            ),
            lookAt,
            math::vec3(0.f, 1.f, 0.f)
        )));

        sceneManager->nextFrame(time, deltaTime);
    });

    fxLoader->load();
    canvas->run();
}
