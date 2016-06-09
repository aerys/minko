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

#include "minko/serialize/SceneParserTest.hpp"
#include "minko/serialize/TypeSerializer.hpp"
#include "minko/deserialize/TypeDeserializer.hpp"

using namespace minko;
using namespace minko::serialize;
using namespace minko::deserialize;

TEST_F(SceneParserTest, SceneDependencyLoadedWithRelativePath)
{
    auto sceneManager = component::SceneManager::create(MinkoTests::canvas());
    auto loader = file::Loader::create(sceneManager->assets()->loader());
    loader->options(loader->options()->clone());

    loader->options()
        ->registerParser<file::SceneParser>("scene")
        ->loadAsynchronously(false);

    auto loaderErrorSlot = loader->error()->connect(
        [](file::Loader::Ptr loader,
           const file::Error& error)
        {
            ASSERT_TRUE(false);
        }
    );

    auto loaderCompleteSlot = loader->complete()->connect(
        [sceneManager](file::Loader::Ptr loader)
        {
            ASSERT_NE(sceneManager->assets()->texture(file::File::getBinaryDirectory() + "/asset/scene/box.png.texture"), nullptr);
        }
    );

    loader
        ->queue("scene/box.fbx.scene")
        ->load();
}

TEST_F(SceneParserTest, SceneDependencyLoadedWithAbsolutePath)
{
    auto sceneManager = component::SceneManager::create(MinkoTests::canvas());
    auto loader = file::Loader::create(sceneManager->assets()->loader());
    loader->options(loader->options()->clone());

    loader->options()
        ->registerParser<file::SceneParser>("scene")
        ->loadAsynchronously(false);

    auto loaderErrorSlot = loader->error()->connect(
        [](file::Loader::Ptr loader,
           const file::Error& error)
        {
            ASSERT_TRUE(false);
        }
    );

    auto loaderCompleteSlot = loader->complete()->connect(
        [sceneManager](file::Loader::Ptr loader)
        {
            ASSERT_NE(sceneManager->assets()->texture(file::File::getBinaryDirectory() + "/asset/scene/box.png.texture"), nullptr);
        }
    );

    loader
        ->queue(file::File::getBinaryDirectory() + "/asset/scene/box.fbx.scene")
        ->load();
}
