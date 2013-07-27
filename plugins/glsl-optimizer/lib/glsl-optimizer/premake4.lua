project "glsl-optimizer"
	kind "StaticLib"
	language "C++"
	files {
		"src/glsl/**.h",
		"src/glsl/**.cpp",
		"src/mesa/**.h",
		"src/mesa/**.cpp",
	}
	includedirs {
		"include",
		"src/mesa",
		"src/glsl"
	}
	excludes {
		"src/glsl/glcpp",
		"src/glsl/main.cpp",
		"src/glsl/builtin_compiler/builtin_stubs.cpp"
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
		buildoptions { "-Wno-narrowing" }

	-- windows
	configuration { "windows", "x32" }
		includedirs {
			-- c99 fix for windows only
			"include/c99"
		}

	-- macos
	configuration { "macosx" }

	-- emscripten
	configuration { "emscripten" }
		flags { "Optimize" }
		buildoptions { "-Wno-narrowing" }
