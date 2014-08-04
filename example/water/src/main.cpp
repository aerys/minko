#include "minko/Minko.hpp"
#include "minko/MinkoJPEG.hpp"
#include "minko/MinkoPNG.hpp"
#include "minko/MinkoSDL.hpp"
#include "minko/MinkoASSIMP.hpp"
#include "minko/MinkoSerializer.hpp"
#include "minko/MinkoFX.hpp"

using namespace minko;
using namespace minko::component;
using namespace minko::math;

const float CAMERA_LIN_SPEED    = 0.05f;
const float CAMERA_ANG_SPEED    = float(M_PI) * 2.f / 180.0f;
const float flowMapCycle        = 0.25f;

Signal<input::Keyboard::Ptr>::Slot keyDown;

#define FLOW_MAP // comment to deactivate flowmap
#define ENABLE_REFLECTION // comment to deactivate reflections

int
main(int argc, char** argv)
{
    auto canvas = Canvas::create("Minko Example - Water");

    auto sceneManager = SceneManager::create(canvas->context());

    sceneManager->assets()->loader()->options()->resizeSmoothly(true);
    sceneManager->assets()->loader()->options()->generateMipmaps(true);
    sceneManager->assets()->loader()->options()
        ->registerParser<file::PNGParser>("png")
        ->registerParser<file::JPEGParser>("jpg")
        ->registerParser<file::PNGParser>("png")
        ->registerParser<file::ASSIMPParser>("dae")
        ->registerParser<file::SceneParser>("scene");

    sceneManager->assets()->loader()
        ->queue("texture/water_normalmap4.jpg")
        ->queue("texture/flowmap.png")
        ->queue("texture/water_dudv.jpg")
        ->queue("texture/noise.png")
        ->queue("effect/Water/Water.effect")
        ->queue("effect/Reflection/PlanarReflection.effect");

    auto root = scene::Node::create("root")
        ->addComponent(sceneManager);

    auto camera = scene::Node::create("camera")
        ->addComponent(Renderer::create(0x7f7f7fff))
        ->addComponent(Transform::create(
            Matrix4x4::create()->lookAt(Vector3::zero(), Vector3::create(3.f, 3.f, 3.f))
        ))
        ->addComponent(PerspectiveCamera::create(canvas->aspectRatio()));

    auto fxLoader = file::Loader::create(sceneManager->assets()->loader())
        ->queue("effect/Basic.effect")
        ->queue("effect/Phong.effect");

    sceneManager->assets()
        ->geometry("cube", geometry::CubeGeometry::create(sceneManager->assets()->context()))
        ->geometry("sphere", geometry::SphereGeometry::create(sceneManager->assets()->context(), 20, 20));

    auto fxComplete = fxLoader->complete()->connect([&](file::Loader::Ptr l)
    {
        sceneManager->assets()->loader()->queue("model/skybox.scene");
        sceneManager->assets()->loader()->load();
    });

    auto waterMaterial = material::WaterMaterial::create(5);

    auto _ = sceneManager->assets()->loader()->complete()->connect([=](file::Loader::Ptr loader)
    {
#ifdef ENABLE_REFLECTION
        auto reflectionComponent = Reflection::create(sceneManager->assets(), 2048, 2048, 0x00000000);
        camera->addComponent(reflectionComponent);
#endif
        root->addChild(camera);

        auto nodeSet = scene::NodeSet::create(sceneManager->assets()->symbol("model/skybox.scene"))->descendants(false, true);

        for (auto n : nodeSet->nodes())
        {
            n->layouts(n->layouts() | scene::Layout::Group::REFLECTION);

            if (n->hasComponent<Surface>())
                n->component<Surface>()->effect(sceneManager->assets()->effect("effect/Basic.effect"));
        }

        root->addChild(sceneManager->assets()->symbol("model/skybox.scene"));

        auto waves = scene::Node::create("waves")
            ->addComponent(Transform::create(math::Matrix4x4::create()->appendRotationX(-float(M_PI) / 2.f)))
            ->addComponent(Surface::create(
                geometry::QuadGeometry::create(sceneManager->assets()->context(), 200, 200, 400, 400),
                waterMaterial,
                sceneManager->assets()->effect("effect/Water/Water.effect")
            ));

        waves->addComponent(component::Water::create(flowMapCycle, waterMaterial));

#ifdef FLOW_MAP
        waterMaterial->noiseMap(sceneManager->assets()->texture("texture/noise.png"));
        waterMaterial->flowMap(sceneManager->assets()->texture("texture/flowmap.png"));
        waterMaterial->flowMapCycle(flowMapCycle);
        waterMaterial->flowMapOffset1(0.f);
        waterMaterial->flowMapOffset2(flowMapCycle / 2.f);
#endif
        waterMaterial->normalMap(sceneManager->assets()->texture("texture/water_normalmap4.jpg"));
#ifdef ENABLE_REFLECTION
        waterMaterial->dudvMap(sceneManager->assets()->texture("texture/water_dudv.jpg"));
        waterMaterial->reflectionMap(camera->components<Reflection>()[0]->getRenderTarget());
        waterMaterial->reflectivity(0.4f);
        waterMaterial->dudvFactor(0.02f);
        waterMaterial->dudvSpeed(0.00015f);
        waterMaterial->diffuseColor(0x052540FF);
#else
        waterMaterial->diffuseColor(0x306090D0);
#endif

        waterMaterial->normalMapSpeed(0.00002f);
        waterMaterial->normalMapScale(8.f);
        waterMaterial->flowMapScale(1.f);
        waterMaterial->shininess(11.5f);
        waterMaterial->specularColor(0xF09030FF);
        waterMaterial->fresnelMultiplier(2.0f);
        waterMaterial->fogColor(0xFFFFFFFF);
        waterMaterial->fresnelPow(8.f);
        waterMaterial->fogDensity(0.0075f);
        waterMaterial->fogType(render::FogType::Exponential2);

#ifdef FLOW_MAP
        waterMaterial->setAmplitude(0, 0.06f);
        waterMaterial->setAmplitude(1, 0.0173f);
        waterMaterial->setAmplitude(2, 0.0312f);
        waterMaterial->setAmplitude(3, 0.0287f);
        waterMaterial->setAmplitude(4, 0.0457f);
#else
        waterMaterial->setAmplitude(0, 0.6f);
        waterMaterial->setAmplitude(1, 0.173f);
        waterMaterial->setAmplitude(2, 0.312f);
        waterMaterial->setAmplitude(3, 0.287f);
        waterMaterial->setAmplitude(4, 0.457f);
#endif
        waterMaterial->setWaveLenght(0, 20.0f);
        waterMaterial->setWaveLenght(1, 17.7f);
        waterMaterial->setWaveLenght(2, 13.13f);
        waterMaterial->setWaveLenght(3, 13.17f);
        waterMaterial->setWaveLenght(4, 13.f);

        waterMaterial->setSpeed(0, 0.05f);
        waterMaterial->setSpeed(1, 0.05f);
        waterMaterial->setSpeed(2, 0.07f);
        waterMaterial->setSpeed(3, 0.11f);
        waterMaterial->setSpeed(4, 0.08f);

        waterMaterial->setDirection(0,    math::Vector2::create(1.f, 0.f));
        waterMaterial->setDirection(1,    math::Vector2::create(0.1f, 0.8f));
        waterMaterial->setDirection(2,    math::Vector2::create(0.4f, -0.2f));
        waterMaterial->setCenter(3,        math::Vector2::create(1000.f, 1000.f));
        waterMaterial->setDirection(4,    math::Vector2::create(1.f, -0.1f));

        waterMaterial->setSharpness(0, 2.0f);
        waterMaterial->setSharpness(1, 1.0f);
        waterMaterial->setSharpness(2, 1.f);
        waterMaterial->setSharpness(3, 2.0f);
        waterMaterial->setSharpness(4, 1.f);

        auto directional = scene::Node::create()
            ->addComponent(DirectionalLight::create(0.8f, 0.6f))
            ->addComponent(Transform::create(math::Matrix4x4::create()->lookAt(math::Vector3::create(), math::Vector3::create(-2.f, 2.0f, 0.f))));
        auto directional2 = scene::Node::create()
            ->addComponent(DirectionalLight::create(0.4f, 0.4f))
            ->addComponent(Transform::create(math::Matrix4x4::create()->lookAt(math::Vector3::create(), math::Vector3::create(2.f, 0.5f, -2.f))));
        auto ambient = scene::Node::create()->addComponent(AmbientLight::create(0.1f))->addComponent(Transform::create());

        root->addChild(ambient);
        root->addChild(directional);
        root->addChild(directional2);
        root->addChild(waves);
    });

    auto resized = canvas->resized()->connect([&](AbstractCanvas::Ptr canvas, uint w, uint h)
    {
        camera->component<PerspectiveCamera>()->aspectRatio(float(w) / float(h));
    });

    float    yaw = 0.3f;
    float    pitch = float(M_PI) * .5f;
    float    minPitch = 0.f + float(1e-5);
    float    maxPitch = float(M_PI) - float(1e-5);
    auto     lookAt = Vector3::create(0.f, 2.f, 0.f);
    float    distance = 80.f;

    // handle mouse signals
    auto mouseWheel = canvas->mouse()->wheel()->connect([&](input::Mouse::Ptr m, int h, int v)
    {
        distance += (float)v / 2.f;
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
        yaw += cameraRotationYSpeed;
        cameraRotationYSpeed *= 0.9f;

        pitch += cameraRotationXSpeed;
        cameraRotationXSpeed *= 0.9f;

        if (pitch > maxPitch)
            pitch = maxPitch;
        else if (pitch < minPitch)
            pitch = minPitch;

        camera->component<Transform>()->matrix()->lookAt(
            lookAt,
            Vector3::create(
                lookAt->x() + distance * cosf(yaw) * sinf(pitch),
                lookAt->y() + distance * cosf(pitch),
                lookAt->z() + distance * sinf(yaw) * sinf(pitch)
            )
        );

        sceneManager->nextFrame(time, deltaTime);
    });

    fxLoader->load();
    canvas->run();
}
