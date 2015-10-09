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
		buildoptions { "/wd4996" }
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

	-- handle no-glsl-optimizer option
	if not _OPTIONS["no-glsl-optimizer"] then
		defines { "MINKO_GLSL_OPTIMIZER_ENABLED" }

		-- glsl-optimizer
		files {
			"lib/glsl-optimizer/src/getopt/**.c",
			"lib/glsl-optimizer/src/getopt/**.h",
			"lib/glsl-optimizer/src/glsl/**.cpp",
			"lib/glsl-optimizer/src/glsl/**.h",
			"lib/glsl-optimizer/src/glsl/**.c",
			"lib/glsl-optimizer/src/mesa/main/**.c",
			"lib/glsl-optimizer/src/mesa/main/**.h",
			"lib/glsl-optimizer/src/mesa/program/**.c",
			"lib/glsl-optimizer/src/mesa/program/**.h",
			"lib/glsl-optimizer/src/util/**.h",
			"lib/glsl-optimizer/src/util/**.c"
		}
		includedirs {
			"lib/glsl-optimizer/include",
			"lib/glsl-optimizer/src",
			"lib/glsl-optimizer/src/getopt",
			"lib/glsl-optimizer/src/glsl",
			"lib/glsl-optimizer/src/glsl/glcpp",
			"lib/glsl-optimizer/src/mesa",
			"lib/glsl-optimizer/src/mesa/main",
			"lib/glsl-optimizer/src/mesa/program",
			"lib/glsl-optimizer/src/util",
		}
		excludes {
			"lib/glsl-optimizer/src/glsl/main.cpp"
		}

		configuration { "android or ios or linux*" }
			buildoptions {
				"-fno-strict-aliasing"
			}

		configuration { "html5" }
			defines { "HAVE___BUILTIN_FFS" }
			excludes {
				"lib/glsl-optimizer/src/getopt/getopt.h",
				"lib/glsl-optimizer/src/getopt/getopt_long.c",
			}
	end

	configuration { "android" }
		defines {
			"ANDROID"
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

	configuration { "not android" }
		excludes {
			"include/minko/log/AndroidLogSink.hpp"
		}

	newoption {
		trigger     = "no-glsl-optimizer",
		description = "Use this option to disable the GLSL optimizer"
	}
