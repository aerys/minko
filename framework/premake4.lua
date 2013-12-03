if _OPTIONS["with-glsl-optimizer"] then
	include "lib/glsl-optimizer"
end

project "framework"
	kind "StaticLib"
	language "C++"
	files {
		"src/**.hpp",
		"src/**.cpp",
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

	configuration { "debug"}
		defines { "DEBUG" }
		defines { "DEBUG_FALLBACK" }
		flags { "Symbols" }
		targetdir "bin/debug"

	configuration { "release" }
		defines { "NDEBUG" }
		flags { "Optimize" } -- { "OptimizeSpeed" }
		targetdir "bin/release"
	
	-- plugins
	minko.plugin.import("angle")

	-- linux
	configuration { "linux" }
		buildoptions { "-std=c++11" }

	-- windows
	configuration { "windows" }
		includedirs { "../deps/win/include" }
		libdirs { "../deps/win/lib" }

	-- visual studio
	configuration { "vs*" }
		-- fix for faux variadic templates limited to 5 arguments by default
		defines { "_VARIADIC_MAX=10" }

	-- macos
	configuration { "macosx" }
		buildoptions { "-std=c++11 -stdlib=libc++" }
		includedirs { "../deps/mac/include" }

	-- emscripten
	configuration { "emscripten" }
		flags { "Optimize" }
		defines { "EMSCRIPTEN" }

	newoption {
		trigger     = "with-glsl-optimizer",
		description = "Enable the GLSL optimizer."
	}
