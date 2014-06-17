PROJECT_NAME = path.getname(os.getcwd())

minko.project.library("minko-plugin-" .. PROJECT_NAME)

	kind "StaticLib"
	language "C++"

	minko.plugin.enable("serializer")
	minko.plugin.enable("lua")
	
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
