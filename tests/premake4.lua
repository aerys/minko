include "lib/googletest"

minko.project.application "minko-tests"
	kind "ConsoleApp"
	language "C++"
	files {
		"src/**.hpp",
		"src/**.cpp"
	}
	includedirs { "src" }

	defines { "MINKO_TESTS" }

	-- plugins
	minko.plugin.enable("sdl");

	-- googletest framework
	links { "googletest" }
	includedirs { "lib/googletest/include" }

	links { "pthread" }
