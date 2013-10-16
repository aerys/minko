project "googletest"
	kind "StaticLib"
	language "C++"
	files {
		"src/gtest-all.cc"
	}
	includedirs {
		"."
	}

	buildoptions { "-std=c++11", "-isystem include", "-pthread" }

	configuration { "debug"}
		defines { "DEBUG", "_DEBUG" }
		flags { "Symbols" }
		targetdir "bin/debug"

	configuration { "release" }
		defines { "NDEBUG" }
		flags { "OptimizeSpeed" }
		targetdir "bin/release"

	-- linux
	--configuration { "linux" }

	-- windows
	--configuration { "windows", "x32" }

	-- macos
	--configuration { "macosx" }

	-- emscripten
	--	configuration { "emscripten" }
