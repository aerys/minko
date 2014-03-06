include "lib/googletest"

minko.project.application "minko-tests"
	removeplatforms { "html5" }

	kind "ConsoleApp"
	language "C++"
	files {
		"src/**.hpp",
		"src/**.cpp"
	}
	includedirs { "src" }
	defines { "MINKO_TEST" }

	-- plugin
	minko.plugin.enable("sdl")
	minko.plugin.enable("serializer")

	-- googletest framework
	links { "googletest" }
	includedirs { "lib/googletest/include" }

	configuration { "linux" }
		links { "pthread" }
