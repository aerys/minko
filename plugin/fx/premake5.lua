newoption {
	trigger		= "with-fx",
	description	= "Enable the Minko FX plugin."
}

PROJECT_NAME = path.getname(os.getcwd())

minko.project.library("minko-plugin-" .. PROJECT_NAME)

	files {
		"src/**.cpp",
		"include/**.hpp",
		"asset/**"
	}

	includedirs {
		"src",
		"include"
	}

	-- linux
	configuration { "linux32 or linux64" }
		buildoptions {
			"-Wno-narrowing -Wno-int-to-pointer-cast"
		}

	-- windows
	configuration { "vs*" }
		defines {
			"_CRT_SECURE_NO_WARNINGS",
			"_CRT_SECURE_NO_DEPRECATE"
		}

	-- macosx
	configuration { "osx64" }
		buildoptions {
			"-Wno-narrowing -Wno-int-to-pointer-cast"
		}

	-- emscripten
	configuration { "html5" }
		buildoptions {
			"-Wno-narrowing -Wno-int-to-pointer-cast"
		}