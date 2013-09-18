if not _OPTIONS["no-glsl-optimizer"] then
	include "lib/glsl-optimizer"
end

project "minko-framework"
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
	-- glsl-optimizer
	if not _OPTIONS["no-glsl-optimizer"] then
		links { "glsl-optimizer" }
		defines { "MINKO_GLSL_OPTIMIZER" }
		includedirs { "lib/glsl-optimizer/src/glsl" }
	end
	
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
		links { "libGLESv2", "libEGL" }
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

	newoption {
		trigger     = "no-glsl-optimizer",
		description = "Disable the GLSL optimizer."
	}