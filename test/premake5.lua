include "lib/googletest"

minko.project.application "minko-tests"

	removeplatforms { "html5" }

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
	links { "googletest", "pthread" }
	includedirs { "lib/googletest/include" }

	if _OPTIONS['with-offscreen'] then
		minko.plugin.enable("offscreen")
	end
