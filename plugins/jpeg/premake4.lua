project "minko-jpeg"
	kind "StaticLib"
	language "C++"
	files { "**.hpp", "**.h", "**.cpp", "**.c" }
	includedirs {
	"src",
	"lib/jpeg-compressor/src",
	"../../framework/src"
	}
	links { "minko-framework" }
	
	configuration { "debug"}
		defines { "DEBUG" }
		flags { "Symbols" }
		targetdir "bin/debug"

	configuration { "release" }
		defines { "NDEBUG" }
		-- flags { "OptimizeSpeed" }
		targetdir "bin/release"

	-- linux
	configuration { "linux" }
		buildoptions "-std=c++11"

	-- windows
	configuration { "windows", "x32" }

	-- macos
	configuration { "debug", "macosx" }
		buildoptions { "-std=c++11", "-stdlib=libc++" }
		libdirs { "/opt/local/lib/" }
		includedirs { "/opt/local/include/" }

	configuration { "emscripten", "release" }
		buildoptions { "-std=c++11" }
		-- prebuildcommands { "sed -i Makefile -r -e 's/-rcs/rk/g'" }
