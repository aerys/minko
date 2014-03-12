PROJECT_NAME = path.getname(os.getcwd())

minko.project.library("minko-plugin-" .. PROJECT_NAME)

	kind "StaticLib"
	language "C++"

	files {
		"lib/**.hpp",
		"lib/**.h",
		"lib/**.cpp",
		"lib/**.c",
		"include/**.hpp",
		"src/**.cpp",
		"src/**.hpp"
	}

	includedirs {
		"include",
	}
	
	configuration { "not html5" }
		includedirs { "lib/sdl/include" }

	configuration { "html5" }
		minko.plugin.enable { "webgl" }
