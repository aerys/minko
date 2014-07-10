PROJECT_NAME = path.getname(os.getcwd())

minko.project.application("minko-example-" .. PROJECT_NAME)

	removeplatforms { "android" }

	--fixme: only for osx
	--kind "WindowedApp"
	--system "macosx"

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
	minko.plugin.enable("png")
	minko.plugin.enable("html-overlay")

	configuration { "cc=clang" }
		buildoptions {
			"-Wno-extern-c-compat",
			"-Wno-objc-missing-super-calls"
		}
