newoption {
	trigger		= "with-mk",
	description	= "Enable the Minko MK plugin."
}

PROJECT_NAME = path.getname(os.getcwd())

minko.project.library("minko-plugin-" .. PROJECT_NAME)

	kind "StaticLib"
	language "C++"

	files {
		"src/**.cpp",
		"src/**.hpp",
		"include/**.hpp"
	}

	includedirs {
		"include",
		"src"
	}
