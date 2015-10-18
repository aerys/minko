project "googletest"

	kind "StaticLib"
	language "C++"
	location "."

	files {
		"src/gtest-all.cc"
	}

	includedirs {
		"include",
		"."
	}

	configuration { "debug"}
		defines { "_DEBUG" }

	configuration { "cc=gcc or cc=clang" }
		buildoptions { "-std=c++11", "-isystem include", "-pthread" }

	configuration { "html5" }
		includedirs { "${EMSCRIPTEN}/system/lib/libcxxabi/include" }
