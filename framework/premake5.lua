minko.project.library "minko-framework"

	-- fixme remove dependency with offscreen plugin
	if _OPTIONS['with-offscreen'] then
		minko.plugin.enable("offscreen")
	end

	configuration { }

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
        "lib/glm/**.hpp",
		"lib/glm/**.inl"
    }
	includedirs {
	   "lib/glm"
	}

	-- sparsehash
	files {
		"lib/sparsehash/src/sparsehash/**.h",
		"lib/sparsehash/src/sparsehash/**.cc"
	}
	configuration { "windows" }
		defines {
			"_SCL_SECURE_NO_WARNINGS"
		}
		files {
			--"lib/sparsehash/src/windows/**.cc",
			"lib/sparsehash/include/windows/**.h"
		}
		includedirs {
			"lib/sparsehash/include/windows"
		}
	configuration { "not windows*" }
		files {
			"lib/sparsehash/include/**.h"
		}
		includedirs {
			"lib/sparsehash/include"
		}
	configuration {}

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
