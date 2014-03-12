PROJECT_NAME = path.getname(os.getcwd())

minko.project.application("minko-example-" .. PROJECT_NAME)
	removeplatforms { "html5", "ios", "android" }

	files {
		"src/**.cpp",
		"src/**.hpp",
		"asset/**"
	}
	
	includedirs { "src" }

	-- plugins
	minko.plugin.enable("sdl")
	minko.plugin.enable("devil")
	--minko.plugin.enable("bullet")
	--minko.plugin.enable("jpeg")
	--minko.plugin.enable("mk")
	--minko.plugin.enable("particles")
