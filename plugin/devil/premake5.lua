PROJECT_NAME = path.getname(os.getcwd())

if not minko.platform.supports { "windows32", "windows64", "linux32", "linux64", "osx64", "html5", "ios", "android" } then
	return
end

minko.project.library("minko-plugin-" .. PROJECT_NAME)

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

	configuration { "cc=clang or osx64" }
		defines {
			"NOINLINE"
		}
		buildoptions {
			"-Wno-implicit-function-declaration",
			"-Wno-format",
			"-Wno-int-to-void-pointer-cast",
			"-Wno-incompatible-pointer-types",
			"-Wno-unused-value",
			"-Wno-sizeof-pointer-memaccess",
			"-Wno-tautological-constant-out-of-range-compare",
			"-Wno-deprecated-declarations"
		}

	configuration { "html5" }
		defines {
			"NOINLINE"
		}
		buildoptions {
			"-Wno-implicit-function-declaration",
			"-Wno-format",
			"-Wno-int-to-pointer-cast",
			"-Wno-incompatible-pointer-types",
			"-Wno-unused-value",
			"-Wno-sizeof-pointer-memaccess",
			"-Wno-tautological-constant-out-of-range-compare",
			"-Wno-deprecated-declarations"
		}
