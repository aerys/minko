/*
Copyright (c) 2013 Aerys

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

#include "CanvasTest.hpp"

using namespace minko;
using namespace minko::scene;
using namespace minko::component;

TEST_F(CanvasTest, CanvasAlpha)
{
    MinkoTests::canvas()->context()->errorsEnabled(true);

    auto sceneManager = SceneManager::create(MinkoTests::canvas());
    auto root = Node::create()->addComponent(sceneManager);

    auto aspectRatio = static_cast<float>(MinkoTests::canvas()->width()) / static_cast<float>(MinkoTests::canvas()->height());

    auto camera = Node::create()
        ->addComponent(Renderer::create(0x10203050))
        ->addComponent(PerspectiveCamera::create(aspectRatio));

    root->addChild(camera);

    sceneManager->nextFrame(0.0f, 0.0f);

    unsigned char pixel[4];
    MinkoTests::canvas()->context()->readPixels(0, 0, 1, 1, &pixel[0]);

    // Each cells of the pixel array contains the RGBA components from 0 to 255
    ASSERT_EQ(static_cast<int>(pixel[3]), 80);
}