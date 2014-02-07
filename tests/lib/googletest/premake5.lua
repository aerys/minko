project "googletest"
	removeplatforms { "html5" }
	
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
