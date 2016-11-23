include "lib/googletest"

minko.project.application "minko-test"

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
	minko.plugin.enable("websocket")
	minko.plugin.enable("http-loader")

	-- googletest framework
	links { "googletest" }

	includedirs { "lib/googletest/include" }

	if _OPTIONS['with-offscreen'] then
		minko.plugin.enable("offscreen")
	end

	configuration { "osx64 or linux32 or linux64" }
		links { "pthread" }

	configuration { "not html5" }
		minko.package.assets {
			['**.effect'] = { 'copy' },
			['**.glsl'] = { 'copy' },
			['**.png'] = { 'copy' },
			['**.scene'] = { 'copy' },
			['**.texture'] = { 'copy' }
		}

	configuration { "html5" }
		minko.package.assets {
			['**.effect'] = { 'embed' },
			['**.glsl'] = { 'embed' },
			['**.png'] = { 'embed' },
			['**.scene'] = { 'embed' },
			['**.texture'] = { 'embed' }
		}

		includedirs { '"${EMSCRIPTEN}/system/lib/libcxxabi/include"' }
