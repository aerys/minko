project "minko-framework"
	kind "StaticLib"
	language "C++"
	files {
		"src/**.hpp",
		"src/**.h",
		"src/**.cpp",
		"src/**.c"
	}
	includedirs {
		"src"
	}
	-- handle --no-glsl-optimizer option
	if not _OPTIONS["no-glsl-optimizer"] then
		defines { "MINKO_GLSL_OPTIMIZER" }
	end
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
	-- glsl optimizer
	files {
		"lib/glsl-optimizer/src/glsl/**.c",
		"lib/glsl-optimizer/src/glsl/**.h",
		"lib/glsl-optimizer/src/glsl/**.cpp",
		"lib/glsl-optimizer/src/glsl/**.hpp",
		"lib/glsl-optimizer/src/mesa/**.c",
		"lib/glsl-optimizer/src/mesa/**.h",
		"lib/glsl-optimizer/src/mesa/**.cpp",
		"lib/glsl-optimizer/src/mesa/**.hpp",
	}
	includedirs {
		"lib/glsl-optimizer/include",
		"lib/glsl-optimizer/src/mesa",
		"lib/glsl-optimizer/src/glsl"
	}
	excludes {
		"lib/glsl-optimizer/src/glsl/main.cpp",
		"lib/glsl-optimizer/src/glsl/builtin_compiler/builtin_stubs.cpp"
	}

	configuration { "debug"}
		defines { "DEBUG" }
		flags { "Symbols" }
		targetdir "bin/debug"

	configuration { "release" }
		defines { "NDEBUG" }
		flags { "OptimizeSpeed" }
		targetdir "bin/release"

	-- linux
	configuration { "linux" }
		links { "GL", "GLU" }
		buildoptions { "-std=c++11" }

	-- windows
	configuration { "windows", "x32" }
		links { "glew32" }
		includedirs {
			"../deps/win/include",
			-- c99 fix for windows only
			"lib/glsl-optimizer/include/c99"
		}
		libdirs { "../deps/win/lib" }

	-- macos
	configuration { "macosx" }
		buildoptions { "-std=c++11", "-stdlib=libc++" }
		includedirs { "../deps/mac/include" }
		libdirs { "../deps/mac/lib" }

	-- emscripten
	configuration { "emscripten" }
		flags { "Optimize" }

	newoption {
	   trigger     = "no-glsl-optimizer",
	   description = "Use this option to disable the GLSL optimizer."
	}
