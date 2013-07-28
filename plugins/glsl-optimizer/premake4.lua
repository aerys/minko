include "lib/glsl-optimizer"

project "minko-glsl-optimizer"
	kind "StaticLib"
	language "C++"
	links { "minko-framework" }
	files {
		"src/**.hpp",
		"src/**.cpp"
	}
	includedirs {
		"src",
		"../../framework/src"
	}
	
	-- glsl-optimizer
	links { "glsl-optimizer" }
	includedirs {
		"lib/glsl-optimizer/include",
		"lib/glsl-optimizer/src/mesa",
		"lib/glsl-optimizer/src/glsl"
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
		buildoptions { "-std=c++11" }

	-- windows
	configuration { "windows", "x32" }
		buildoptions { "-std=c++11" }
		libdirs { "../../deps/win/lib" }
		includedirs { "../../deps/win/include" }
	
	-- macos
	configuration { "macosx" }
		buildoptions { "-std=c++11", "-stdlib=libc++" }

	-- emscripten
	configuration { "emscripten" }
		flags { "Optimize" }
