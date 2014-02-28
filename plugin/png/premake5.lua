PROJECT_NAME = path.getname(os.getcwd())

minko.project.library("minko-plugin-" .. PROJECT_NAME)

	kind "StaticLib"
	language "C++"

	files {
		"lib/**.hpp",
		"lib/**.h",
		"lib/**.cpp",
		"lib/**.c",
		"src/**.cpp",
		"include/**.hpp"
	}

	includedirs {
		"include",
		"src",
		"lib/lodepng/src"
	}

	configuration { "windows" }
		defines {
			"_CRT_SECURE_NO_WARNINGS"
		}


	defines
	{
		--"LODEPNG_NO_COMPILE_ENCODER"
		--"LODEPNG_NO_COMPILE_DISK"
	}
