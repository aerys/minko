#include "gtest/gtest.h"

#include "minko/Minko.hpp"
#include "minko/MinkoSDL.hpp"
#include "minko/MinkoTests.hpp"

using namespace minko;

int main(int argc, char **argv)
{
	auto canvas = Canvas::create("Minko Tests", 640, 480);

	::testing::InitGoogleTest(&argc, argv);

	MinkoTests::context(canvas->context());

	return RUN_ALL_TESTS();
}
