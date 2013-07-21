project "minko-png"
	kind "StaticLib"
	language "C++"
	files { "**.hpp", "**.h", "**.cpp", "**.c" }
	includedirs {
		"src",
		"lib/lodepng/src",
		"../../framework/src"
	}
	links { "minko-framework" }
	defines
	{
		"LODEPNG_NO_COMPILE_ENCODER",
		"LODEPNG_NO_COMPILE_DISK"
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

	-- macos
	configuration { "macosx" }
		buildoptions { "-std=c++11", "-stdlib=libc++" }

	-- emscripten
	configuration { "emscripten" }
		flags { "Optimize" }
