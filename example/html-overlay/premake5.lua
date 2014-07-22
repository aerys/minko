PROJECT_NAME = path.getname(os.getcwd())

if not minko.platform.supports { "windows32", "windows64", "linux32", "linux64", "osx64", "html5", "ios" } then
	return
end

minko.project.application("minko-example-" .. PROJECT_NAME)

	removeplatforms { "android" }

	files {
		"src/**.cpp",
		"src/**.hpp",
		"asset/**"
	}

	includedirs { "src" }

	-- plugins
	minko.plugin.enable("sdl")
	--minko.plugin.enable("bullet")
	--minko.plugin.enable("jpeg")
	--minko.plugin.enable("mk")
	--minko.plugin.enable("particles")
	minko.plugin.enable("html-overlay")

	configuration { "cc=clang" }
		buildoptions {
			"-Wno-extern-c-compat",
			"-Wno-objc-missing-super-calls"
		}
