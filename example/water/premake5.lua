PROJECT_NAME = path.getname(os.getcwd())

minko.project.application("minko-example-" .. PROJECT_NAME)

	files {
		"src/**.cpp",
		"src/**.hpp",
		"asset/**"
	}

	includedirs { "src" }

	-- plugins
	minko.plugin.enable("sdl")
	minko.plugin.enable("fx")
	minko.plugin.enable("serializer")
	minko.plugin.enable("jpeg")
	minko.plugin.enable("png")
	minko.plugin.enable("assimp")