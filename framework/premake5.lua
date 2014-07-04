minko.project.library "minko-framework"
	kind "StaticLib"

	language "C++"

	files {
		"src/**.hpp",
		"src/**.cpp",
		"include/**.hpp",
		"asset/**"
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

	-- glm
    files {
        "lib/glm/**.hpp"
    }
	includedirs {
	   "lib/glm"
	}
	    
	-- windows
	configuration { "windows32" }
        includedirs { minko.sdk.path("/framework/lib/glew/include") }
        files { "lib/glew/**.h" }
        libdirs { "lib/glew/lib/windows32" }
		buildoptions {
			"/wd4503"
		}
		
	configuration { "windows64" }
        includedirs { minko.sdk.path("/framework/lib/glew/include") }
        files { "lib/glew/**.h" }
		libdirs { "lib/glew/lib/windows64" }
