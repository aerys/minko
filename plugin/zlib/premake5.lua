PROJECT_NAME = path.getname(os.getcwd())

minko.project.library("minko-plugin-" .. PROJECT_NAME)
	kind "StaticLib"
	language "C++"
	
	files {
		"lib/zlib/**.c",
		"lib/zlib/**.h"
	}

	includedirs {
		"lib/zlib"
	}
