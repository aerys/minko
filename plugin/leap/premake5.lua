PROJECT_NAME = path.getname(os.getcwd())

minko.project.library("minko-plugin-" .. PROJECT_NAME)

	removeplatforms { "html5", "ios", "android" }

	files {
		"src/**.hpp",
		"src/**.h",
		"src/**.cpp",
		"src/**.c",
		"include/**.hpp"
	}

	includedirs {
		"include",
		"src",
		"lib/leap/include"
	}
