include "lib/googletest"

minko.project.application "minko-tests"
	kind "ConsoleApp"
	language "C++"
	files {
		"src/**.hpp",
		"src/**.cpp"
	}
	includedirs { "src" }

	-- plugins
	minko.plugin.enable("sdl");

	-- googletest framework
	links { "googletest" }
	includedirs { "lib/googletest/include" }

	links { "framework" }

	-- configurations
	configuration { "debug"}
		defines { "DEBUG" }
		flags { "Symbols" }
		targetdir "bin/debug"

	configuration { "release" }
		defines { "NDEBUG" }
		flags { "OptimizeSpeed" }
		targetdir "bin/release"
