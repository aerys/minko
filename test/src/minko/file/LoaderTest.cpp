#include "LoaderTest.hpp"

#include "minko/MinkoTests.hpp"

using namespace minko;
using namespace minko::file;

TEST_F(LoaderTest, Create)
{
	try
	{
		auto loader = Loader::create();
	}
	catch (...)
	{
		ASSERT_TRUE(false);
	}
}

TEST_F(LoaderTest, FileDestroyed)
{
    static const auto fileName = "scene/box.fbx.scene";

    auto sceneManager = component::SceneManager::create(MinkoTests::canvas());

    auto file = File::Ptr();
    auto fileWeakPtr = std::weak_ptr<File>();

    {
        auto loader = Loader::create(sceneManager->assets()->loader());

        auto completeSlot = loader->complete()->connect([&file, &fileWeakPtr](Loader::Ptr loader)
            {
                file = loader->files().at(fileName);
                fileWeakPtr = file;
            }
        );

        loader
            ->queue(fileName)
            ->load();
    }

    ASSERT_EQ(file.use_count(), 1);

    file = nullptr;

    ASSERT_TRUE(fileWeakPtr.expired());

    fileWeakPtr.reset();
}
