-- include "lib/googletest"

minko.project.application "minko-test"

	files {
		"src/**.hpp",
		"src/**.cpp",
		"asset/**"
	}
	includedirs { "src" }
	defines { "MINKO_TEST" }

	-- plugins
	minko.plugin.enable("sdl")
	minko.plugin.enable("serializer")
	minko.plugin.enable("websocket")
	minko.plugin.enable("http-loader")
    minko.plugin.enable("ssl")
    if _OPTIONS['with-offscreen'] then
		minko.plugin.enable("offscreen")
	end

	-- googletest
    files {
		"lib/googletest/src/gtest-all.cc",
	}
    includedirs {
        "lib/googletest/include",
        "lib/googletest",
    }
    excludes {
        "lib/googletest/gtest_main.cc",
    }

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
