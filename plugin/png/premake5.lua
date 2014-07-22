PROJECT_NAME = path.getname(os.getcwd())

minko.project.library("minko-plugin-" .. PROJECT_NAME)

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
		"src",
		"lib/lodepng/src"
	}

	configuration { "windows" }
		defines {
			"_CRT_SECURE_NO_WARNINGS"
		}
