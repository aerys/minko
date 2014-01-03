if _OPTIONS["with-glsl-optimizer"] then
	include "lib/glsl-optimizer"
end

project "minko-framework"
	kind "StaticLib"
	location "."
	language "C++"
	files {
		"src/**.hpp",
		"src/**.cpp",
		"include/**.hpp",
		"assets/**"
	}
	includedirs {
		"include",
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

	configuration { "release" }
		defines { "NDEBUG" }
		optimize "On"
	
	-- plugins
	minko.plugin.import("angle")

	-- linux
	configuration { "linux" }
		buildoptions { "-std=c++11" }

	-- windows
	configuration { "win" }
		includedirs { "../deps/win/include" }
		libdirs { "../deps/win/lib" }

	-- visual studio
	configuration { "vs*" }
		-- fix for faux variadic templates limited to 5 arguments by default
		defines { "_VARIADIC_MAX=10" }

	-- macos
	configuration { "osx" }
		includedirs { "../deps/mac/include" }

	-- html5
	configuration { "html5" }
		optimize "On"

	newoption {
		trigger     = "with-glsl-optimizer",
		description = "Enable the GLSL optimizer."
	}
