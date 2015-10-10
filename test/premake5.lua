include "lib/googletest"

minko.project.application "minko-test"

	removeplatforms { "html5" }

	files {
		"src/**.hpp",
		"src/**.cpp",
		"asset/**"
	}
	includedirs { "src" }
	defines { "MINKO_TEST" }

	-- plugin
	minko.plugin.enable("sdl")
	minko.plugin.enable("serializer")

	-- googletest framework
	links { "googletest" }

	includedirs { "lib/googletest/include" }

	if _OPTIONS['with-offscreen'] then
		minko.plugin.enable("offscreen")
	end

	configuration { "not windows" }
		links { "pthread" }

	configuration { "not html5" }
		minko.package.assets {
			['**.effect'] = { 'copy' },
			['**.glsl'] = { 'copy' },
			['**.png'] = { 'copy' }
		}
