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
			['**.png'] = { 'copy' }
		}

	configuration { "html5" }
		minko.package.assets {
			['**.effect'] = { 'embed' },
			['**.glsl'] = { 'embed' },
			['**.png'] = { 'embed' }
		}

	if premake.tools.gcc.tools.emscripten then
		configuration { "html5" }
			includedirs { "${EMSCRIPTEN}/system/lib/libcxxabi/include" }
	else
		removeplatforms { "html5" }
	end

