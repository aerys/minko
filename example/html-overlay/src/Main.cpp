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
#include "minko/MinkoSDL.hpp"
#include "minko/MinkoHtmlOverlay.hpp"

using namespace minko;
using namespace minko::component;

Signal<minko::dom::AbstractDOM::Ptr, std::string>::Slot onloadSlot;
Signal<minko::dom::AbstractDOMMouseEvent::Ptr>::Slot onclickSlot;

dom::AbstractDOM::Ptr gameInterfaceDom;
dom::AbstractDOMElement::Ptr redScoreElement;
dom::AbstractDOMElement::Ptr blueScoreElement;

bool enableRenderToTexture = true;
bool renderToTexture = enableRenderToTexture;
minko::render::AbstractTexture::Ptr sharedTexture = nullptr;

int redScore;
int blueScore;

void
updateRedScore();

void
updateBlueScore();

int
main(int argc, char** argv)
{
    redScore = 0;
    blueScore = 0;

    auto canvas = Canvas::create("Minko Example - Overlay");

    auto overlay = HtmlOverlay::create(argc, argv);

    auto sceneManager = SceneManager::create(canvas);

    // setup assets
    sceneManager->assets()->loader()->options()
        ->resizeSmoothly(true)
        ->generateMipmaps(true);

    sceneManager->assets()->loader()
        ->queue("effect/Basic.effect");

#if MINKO_PLATFORM == MINKO_PLATFORM_ANDROID
    if (renderToTexture)
        sceneManager->assets()->loader()
            ->queue("effect/Custom.effect");
#endif
    
    sceneManager->assets()->context()->errorsEnabled(true);

    auto cubeGeometry = geometry::CubeGeometry::create(sceneManager->assets()->context());
    sceneManager->assets()->geometry("cubeGeometry", cubeGeometry);

    auto root = scene::Node::create("root")
        ->addComponent(sceneManager)
        ->addComponent(overlay);

    enableRenderToTexture = 0; // enableRenderToTexture is not implemented

    if (enableRenderToTexture)
    {
        // Create the shared texture to render the WebView into it
#if MINKO_PLATFORM == MINKO_PLATFORM_ANDROID
        sharedTexture = minko::render::SharedTexture::create(
            canvas->context(), 
            512, 
            512,
            minko::render::TextureFormat::RGBA, 
            "WebViewTexture"
        );
#elif MINKO_PLATFORM == MINKO_PLATFORM_IOS
        sharedTexture = minko::render::Texture::create(canvas->context(), 512, 512);
#endif
        
        sharedTexture->upload();
    }

    auto mesh = scene::Node::create("mesh")
        ->addComponent(Transform::create());
    root->addChild(mesh);

    auto camera = scene::Node::create("camera")
        ->addComponent(Renderer::create(0x7f7f7fff))
        ->addComponent(Transform::create(
            math::inverse(math::lookAt(math::vec3(0.f, 0.f, 3.f), math::vec3(), math::vec3(0, 1, 0)))
        ))
        ->addComponent(Camera::create(math::perspective(.785f, canvas->aspectRatio(), 0.1f, 1000.f)));

    root->addChild(camera);

    auto resized = canvas->resized()->connect([&](AbstractCanvas::Ptr canvas, uint w, uint h)
    {
        camera->component<Camera>()->projectionMatrix(math::perspective(.785f, canvas->aspectRatio(), 0.1f, 1000.f));
    });

    auto material = material::BasicMaterial::create();
    material->diffuseColor(0xCCCCCCFF);

    auto _ = sceneManager->assets()->loader()->complete()->connect([=](file::Loader::Ptr loader)
    {
        minko::render::Effect::Ptr meshEffect = nullptr;

        mesh->addComponent(Surface::create(
            sceneManager->assets()->geometry("cubeGeometry"),
            material,
            sceneManager->assets()->effect("effect/Basic.effect")
        ));
    });

    onloadSlot = overlay->onload()->connect([=](minko::dom::AbstractDOM::Ptr dom, std::string page)
    {
        if (!dom->isMain())
            return;

        if (dom->fileName() == "interface.html")
        {
            onclickSlot = dom->document()->onclick()->connect([=](dom::AbstractDOMMouseEvent::Ptr event)
            {
                dom->sendMessage("hello");
            });

            gameInterfaceDom = dom;
            redScoreElement = gameInterfaceDom->getElementById("teamScoreRed");
            blueScoreElement = gameInterfaceDom->getElementById("teamScoreBlue");
        }

        if (enableRenderToTexture)
        {
            overlay->enableRenderToTexture(sharedTexture);
            
#if MINKO_PLATFORM == MINKO_PLATFORM_ANDROID
            mesh->component<Surface>()->effect(sceneManager->assets()->effect("effect/Custom.effect"));
#endif
            
            material->data()->set("diffuseMap", sharedTexture->sampler());
        }
    });

    overlay->load("html/interface.html");

    auto rightButtonDown = canvas->mouse()->rightButtonDown()->connect([&](input::Mouse::Ptr m)
    {
        updateRedScore();
    });

    auto leftButtonDown = canvas->mouse()->leftButtonDown()->connect([&](input::Mouse::Ptr m)
    {
        updateBlueScore();
        
        if (enableRenderToTexture)
        {
            renderToTexture = !renderToTexture;
            
            if (renderToTexture)
            {
                overlay->enableRenderToTexture(sharedTexture);
                
#if MINKO_PLATFORM == MINKO_PLATFORM_ANDROID
                mesh->component<Surface>()->effect(sceneManager->assets()->effect("effect/Custom.effect"));
#endif
                
                material->data()->set("diffuseMap", sharedTexture->sampler());
            }
            else
            {
                overlay->disableRenderToTexture();
                
                mesh->component<Surface>()->effect(sceneManager->assets()->effect("effect/Basic.effect"));
                material->data()->unset("diffuseMap");
            }
        }
    });

    auto enterFrame = canvas->enterFrame()->connect([&](AbstractCanvas::Ptr canvas, float time, float deltaTime, bool shouldRender)
    {
        mesh->component<Transform>()->matrix(mesh->component<Transform>()->matrix() * math::rotate(0.01f, math::vec3(0, 1, 0)));

        sceneManager->nextFrame(time, deltaTime, shouldRender);
    });

    sceneManager->assets()->loader()->load();
    canvas->run();

    return 0;
}

void
updateRedScore()
{
    if (gameInterfaceDom != nullptr)
        redScoreElement->textContent(std::to_string(redScore++));
}

void
updateBlueScore()
{
    if (gameInterfaceDom != nullptr)
        blueScoreElement->textContent(std::to_string(blueScore++));
}
