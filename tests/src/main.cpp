#include "gtest/gtest.h"

#include "minko/Minko.hpp"
#include "minko/MinkoSDL.hpp"
#include "minko/MinkoTests.hpp"

using namespace minko;

int main(int argc, char **argv)
{
	MinkoSDL::initialize("Minko Tests", 640, 480);
	::testing::InitGoogleTest(&argc, argv);

	MinkoTests::context(MinkoSDL::context());

	return RUN_ALL_TESTS();
}
