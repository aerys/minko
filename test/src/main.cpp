#include "gtest/gtest.h"

#include "minko/Minko.hpp"
#include "minko/MinkoTests.hpp"

#if defined(MINKO_PLUGIN_OFFSCREEN)
# include "minko/MinkoOffscreen.hpp"
#else
# include "minko/MinkoSDL.hpp"
#endif

using namespace minko;

void wait()
{
	std::cout << "Press ENTER to continue...";
	std::cin.ignore(std::numeric_limits <std::streamsize> ::max(), '\n');
}

int main(int argc, char **argv)
{
#if defined(MINKO_PLUGIN_OFFSCREEN)
	MinkoOffscreen::initialize("Minko Tests", 640, 480);
#else
	auto canvas = Canvas::create("Minko Tests", 640, 480);
#endif

	::testing::InitGoogleTest(&argc, argv);

#if defined(MINKO_PLUGIN_OFFSCREEN)
	MinkoTests::context(MinkoOffscreen::context());
#else
	MinkoTests::context(canvas->context());
#endif

	auto output = RUN_ALL_TESTS();

	wait();

	return output;
}
