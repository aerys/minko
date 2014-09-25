PROJECT_NAME = path.getname(os.getcwd())

minko.project.library("minko-plugin-" .. PROJECT_NAME)

	minko.plugin.enable("android")
	minko.plugin.enable("sdl")

	files {
		"lib/**.hpp",
		"lib/**.h",
		"lib/**.cpp",
		"lib/**.c",
		"src/**.cpp",
		"include/**.hpp"
	}

	includedirs {
		"include",
		"src"
	}
