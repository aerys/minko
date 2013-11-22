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
	
	-- plugins
	minko.plugin.import("angle")

	-- linux
	configuration { "linux" }
		links { "GL", "GLU" }
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
		libdirs { "../deps/mac/lib" }

	-- emscripten
	configuration { "emscripten" }
		flags { "Optimize" }
		defines { "EMSCRIPTEN" }

	newoption {
		trigger     = "with-glsl-optimizer",
		description = "Enable the GLSL optimizer."
	}
