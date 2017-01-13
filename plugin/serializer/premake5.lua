PROJECT_NAME = path.getname(os.getcwd())

minko.project.library("minko-plugin-" .. PROJECT_NAME)

    minko.plugin.enable("jpeg")
	minko.plugin.enable("png")

	files {
		"src/**.hpp",
		"src/**.h",
		"src/**.cpp",
		"src/**.c",
		"include/**.hpp",
		"lib/msgpack-c/src/**.cpp",
		"lib/msgpack-c/include/**.hpp"
	}

	includedirs {
		"include",
		"src",
		"lib/msgpack-c/include",
		"lib/msgpack-c/src"
	}

	configuration { "windows" }
		-- msgpack
		defines {
			"_LIB",
			"_CRT_SECURE_NO_WARNINGS",
			"_CRT_SECURE_NO_DEPRECATE",
			"__STDC_VERSION__=199901L",
			"__STDC__",
			"WIN32",
			"_WINDLL_IMPORT"
		}
		buildoptions {
			"/wd4028",
			"/wd4244",
			"/wd4267",
			"/wd4996",
			"/wd4273",
			"/wd4503"
		}

	configuration { "linux32 or linux64 or osx64" }
		buildoptions {
			"-Wno-deprecated-declarations"
		}
		defines {
			"__STDC_FORMAT_MACROS"
		}

	if _OPTIONS['with-texture-compressor'] then
		configuration {}
			files {
				"lib/crnlib/**.h",
				"lib/crnlib/**.hpp",
				"lib/crnlib/**.c",
				"lib/crnlib/**.cpp"
			}

			includedirs {
				"lib/crnlib/crnlib",
				"lib/crnlib/inc"
			}

			excludes {
				"lib/crnlib/crnlib/lzham_win32_threading.h",
				"lib/crnlib/crnlib/lzham_win32_threading.cpp",
				"lib/crnlib/crnlib/lzham_timer.h",
				"lib/crnlib/crnlib/lzham_timer.cpp",
				"lib/crnlib/crnlib/crn_jpgd.cpp",
				"lib/crnlib/crnlib/crn_winhdr.h"
			}

			defines {
				"MINIZ_NO_TIME",
				"CRNLIB_NO_ETC1"
			}

		configuration { "linux" }
			excludes {
				"lib/crnlib/crnlib/crn_threading_win32.h",
				"lib/crnlib/crnlib/crn_threading_win32.cpp",
				"lib/crnlib/crnlib/lzma_LzFindMt.h",
				"lib/crnlib/crnlib/lzma_LzFindMt.cpp",
				"lib/crnlib/crnlib/lzma_Threads.h",
				"lib/crnlib/crnlib/lzma_Threads.cpp"
			}

		configuration { "linux", "debug" }
			defines { "_DEBUG" }

		configuration { "android or ios or html5" }
			defines {
				"MINKO_NO_PVRTEXTOOL",
				"MINKO_NO_QCOMPRESS",
				"MINKO_NO_CRNLIB"
			}

			configuration { "osx64" }
				defines {
					"MINKO_NO_QCOMPRESS",
					"MINKO_NO_CRNLIB"
				}

		configuration { "windows64 or linux64" }
			defines {
				"MINKO_NO_QCOMPRESS"
			}

		configuration { "windows or linux or osx64" }
			includedirs {
				"lib/PVRTexTool/Include"
			}

		configuration { "windows32 or linux32" }
			includedirs {
				"lib/QCompress/Inc"
			}
	else
		configuration { }
			defines {
				"MINKO_NO_PVRTEXTOOL",
				"MINKO_NO_QCOMPRESS",
				"MINKO_NO_CRNLIB"
			}
	end
