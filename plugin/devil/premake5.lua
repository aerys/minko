PROJECT_NAME = path.getname(os.getcwd())

minko.project.library("minko-plugin-" .. PROJECT_NAME)
	removeplatforms { "html5", "ios", "android" }
	kind "StaticLib"
	language "C++"
	
	minko.plugin.enable("zlib")
	
	files {
		"**.hpp",
		"**.h",
		"**.cpp",
		"**.c"
	}
	
	excludes {
		"lib/devil/src/src-ILU/ilur/ilur.c",
		"lib/devil/src/src-IL/src/il_main.c",
		--"lib/devil/src/src-ILU/src/ilu_main.c",
		"lib/devil/lib/libjpeg/wrjpgcom.c",
		"lib/devil/lib/libjpeg/jpegtran.c",
		"lib/devil/lib/libjpeg/djpeg.c",
		"lib/devil/lib/libjpeg/cjpeg.c",
		"lib/devil/lib/libjpeg/ckconfig.c"
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
		"lib/devil/lib/libjasper/include",
		"lib/devil/lib/lcms/include"
	}
	
	defines {
		"_CRT_SECURE_NO_WARNINGS",
		"IL_STATIC_LIB"
	}

	configuration { "osx64", "cc=gcc" }
		buildoptions {
			"-std=gnu89"
		}

	configuration { "linux" }
		defines {}

	configuration { "html5" }
		buildoptions {
			"-std=gnu89"
		}
