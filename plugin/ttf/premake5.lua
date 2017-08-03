PROJECT_NAME = path.getname(os.getcwd())

minko.project.library("minko-plugin-" .. PROJECT_NAME)

	files {
		"src/**.cpp",
		"include/**.hpp"
	}

	includedirs {
		"include",
        "lib/freetype/include/freetype2",
        "lib/freetype/include"
	}
