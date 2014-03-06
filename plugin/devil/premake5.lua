PROJECT_NAME = path.getname(os.getcwd())

minko.project.library("minko-plugin-" .. PROJECT_NAME)
	removeplatforms { "html5", "ios", "android" }
	kind "StaticLib"
	language "C++"
	
	files {
		"**.hpp",
		"**.h",
		"**.cpp",
		"**.c"
	}

	includedirs {
		"include",
		"lib/devil/src/include",
		"lib/devil/src/src-IL/include",
		"lib/devil/src/src-ILU/include",
		"lib/devil/src/src-ILUT/include",
		"lib/devil/lib/include",
		"lib/devil/lib/libjpeg",
		"lib/devil/lib/libtiff",
		"lib/devil/lib/libpng",
		"lib/devil/lib/zlib",
		"lib/devil/lib/libjasper/include",
		"lib/devil/lib/lcms/include"
	}

	configuration { "osx64" }
		buildoptions {
			"-std=gnu89"
		}

	configuration { "linux" }
		defines {}

	configuration { "html5" }
		buildoptions {
			"-std=gnu89"
		}
