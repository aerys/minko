#include "gtest/gtest.h"

#include "minko/Minko.hpp"

using namespace minko;

int main(int argc, char **argv)
{
	::testing::InitGoogleTest(&argc, argv);

	return RUN_ALL_TESTS();
}