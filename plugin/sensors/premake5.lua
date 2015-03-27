PROJECT_NAME = path.getname(os.getcwd())

minko.project.library("minko-plugin-" .. PROJECT_NAME)

	files {
		"src/**.hpp",
		"src/**.cpp",
		"include/**.hpp",
        "asset/**"
	}

	includedirs {
		"include"
	}