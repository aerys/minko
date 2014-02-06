minko.project.library "minko-framework"
	kind "StaticLib"

	language "C++"

	files {
		"src/**.hpp",
		"src/**.cpp",
		"include/**.hpp",
		"assets/**"
	}

	includedirs {
		"src"
	}
	
	-- json cpp
	files {
		"lib/jsoncpp/src/**.cpp",
		"lib/jsoncpp/src/**.hpp",
		"lib/jsoncpp/src/**.c",
		"lib/jsoncpp/src/**.h"
	}
	includedirs {
		"lib/jsoncpp/src"
	}
	defines {
		"JSON_IS_AMALGAMATION"
	}
	
	-- plugins
	minko.plugin.import("angle")

	-- windows
	configuration { "windows32" }
		libdirs { "lib/glew/lib/windows32" }
		
	configuration { "windows64" }
		libdirs { "lib/glew/lib/windows64" }
