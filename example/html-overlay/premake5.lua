PROJECT_NAME = path.getname(os.getcwd())

if not minko.platform.supports { "windows32", "windows64", "linux32", "linux64", "osx64", "html5", "ios", "android" } then
	return
end

minko.project.application("minko-example-" .. PROJECT_NAME)

	files {
		"src/**.cpp",
		"src/**.hpp",
		"asset/**"
	}

	includedirs { "src" }

	-- plugins
	minko.plugin.enable("sdl")
	minko.plugin.enable("html-overlay")

	prelinkcommands {
		minko.action.copy("asset")
	}

	configuration { "html5" }
		minko.package.assets {
			['**.effect'] = { 'embed' }, -- fixme: restrict to the actual effect/glsl needed
			['**.glsl'] = { 'embed' }
		}

	configuration { "cc=clang" }
		buildoptions {
			"-Wno-extern-c-compat",
			"-Wno-objc-missing-super-calls"
		}
