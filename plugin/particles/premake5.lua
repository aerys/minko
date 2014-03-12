PROJECT_NAME = path.getname(os.getcwd())

minko.project.library("minko-plugin-" .. PROJECT_NAME)

	kind "StaticLib"
	language "C++"

	minko.plugin.enable("serializer")
	
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
