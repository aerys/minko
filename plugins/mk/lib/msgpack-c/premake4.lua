project "msgpack-c"
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
		
	configuration { "debug"}
		defines { "DEBUG", "_DEBUG" }
		flags { "Symbols" }
		targetdir "bin/debug"

	configuration { "release" }
		defines { "NDEBUG" }
		flags { "OptimizeSpeed" }
		targetdir "bin/release"

	-- linux
	configuration { "linux" }

	-- windows
	configuration { "windows", "x32" }
		defines {
			"_LIB",
			"_CRT_SECURE_NO_WARNINGS",
			"_CRT_SECURE_NO_DEPRECATE",
			"__STDC_VERSION__=199901L",
			"__STDC__",
			"WIN32"
		}
		buildoptions {
			"/wd4028",
			"/wd4244",
			"/wd4267",
			"/wd4996",
			"/wd4273"
		}
		-- c99 fix for windows only
		includedirs { "include/c99" }

	-- macos
	configuration { "macosx" }

	-- emscripten
	configuration { "emscripten" }
		flags { "Optimize" }

