project "glsl-optimizer"
	kind "StaticLib"
	language "C++"
	files {
		"src/glsl/**.h",
		"src/glsl/**.c",
		"src/glsl/**.cpp",
		"src/mesa/**.h",
		"src/mesa/**.c",
		"src/mesa/**.cpp"
	}
	includedirs {
		"include",
		"src/mesa",
		"src/glsl",
		"src/glsl/glcpp"
	}
	excludes {
		"src/glsl/main.cpp",
		"src/glsl/builtin_compiler/builtin_stubs.cpp",
		"src/glsl/glcpp/glcpp-parse.y"
	}
	flags { "NoExceptions" }
	
	configuration { "debug"}
		defines { "_DEBUG" }

	-- linux
	configuration { "linux" }
		buildoptions { "-Wno-narrowing" }

	-- windows
	configuration { "windows", "x32" }
		defines {
			"_LIB",
			"NOMINMAX",
			"_CRT_SECURE_NO_WARNINGS",
			"_CRT_SECURE_NO_DEPRECATE",
			"__STDC_VERSION__=199901L",
			"__STDC__",
			"strdup=_strdup",
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
		optimize "On"
		buildoptions { "-Wno-narrowing" }
