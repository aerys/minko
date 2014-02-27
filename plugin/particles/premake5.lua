newoption {
	trigger		= "with-particles",
	description	= "Enable the Minko particles plugin."
}

PROJECT_NAME = path.getname(os.getcwd())

minko.project.library("minko-plugin-" .. PROJECT_NAME)

	kind "StaticLib"
	language "C++"

	files {
		"include/**.hpp",
		"src/**.cpp",
		"src/**.hpp",
		"asset/**"
	}

	includedirs {
		"include",
		"src"
	}
