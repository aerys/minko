project "minko-framework"
	kind "StaticLib"
	language "C++"
	files {
		"src/**.hpp",
		"src/**.cpp"
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
		includedirs { "../deps/win/include" }
		libdirs { "../deps/win/lib" }

	-- macos
	configuration { "macosx" }
		buildoptions { "-std=c++11", "-stdlib=libc++" }
		includedirs { "../deps/mac/include" }
		libdirs { "../deps/mac/lib" }

	-- emscripten
	configuration { "emscripten" }
		flags { "Optimize" }
