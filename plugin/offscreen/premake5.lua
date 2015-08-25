PROJECT_NAME = path.getname(os.getcwd())

minko.project.library("minko-plugin-" .. PROJECT_NAME)

	removeplatforms { "android", "ios", "html5" }

	files {
		"lib/**.hpp",
		"lib/**.h",
		"lib/**.cpp",
		"lib/**.c",
		"include/**.hpp",
		"src/**.cpp"
	}

	configuration {"linux32 or linux64 or osx64"}
		includedirs {
			"include",
			minko.plugin.path("sdl") .. "/include",
			"lib/osmesa/linux/include"
		}

	configuration {"windows32 or windows64"}
		includedirs {
			"include",
			minko.plugin.path("sdl") .. "/include",
			"lib/osmesa/windows/include",
			"lib/opengl/include/GL"
		}

