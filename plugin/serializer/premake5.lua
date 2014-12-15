PROJECT_NAME = path.getname(os.getcwd())

minko.project.library("minko-plugin-" .. PROJECT_NAME)

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
		configuration { "android or ios or html5" }
			defines {
				"MINKO_NO_PVRTEXTOOL",
				"MINKO_NO_QCOMPRESS"
			}

		configuration { "linux64" }
			defines {
				"MINKO_NO_QCOMPRESS"
			}

		configuration { "windows or linux or osx" }
			includedirs {
				"lib/PVRTexTool/Include"
			}

		configuration { "windows or linux32 or osx" }
			includedirs {
				"lib/QCompress/Inc"
			}
	else
		configuration { }
			defines {
				"MINKO_NO_PVRTEXTOOL",
				"MINKO_NO_QCOMPRESS"
			}
	end
