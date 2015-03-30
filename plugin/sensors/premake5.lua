PROJECT_NAME = path.getname(os.getcwd())

minko.project.library("minko-plugin-" .. PROJECT_NAME)

	minko.plugin.enable("sdl")

	files {
		"src/**.hpp",
		"src/**.cpp",
		"include/**.hpp",
        "asset/**"
	}

	includedirs {
		"include"
	}