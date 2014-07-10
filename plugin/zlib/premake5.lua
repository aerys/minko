PROJECT_NAME = path.getname(os.getcwd())

minko.project.library("minko-plugin-" .. PROJECT_NAME)

	files {
		"lib/zlib/**.c",
		"lib/zlib/**.h"
	}

	includedirs {
		"lib/zlib"
	}

	configuration { "vs*" }
		buildoptions { "/wd4996" }
		defines { "_CRT_SECURE_NO_WARNINGS" }
