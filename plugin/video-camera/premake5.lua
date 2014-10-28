PROJECT_NAME = path.getname(os.getcwd())

minko.project.library("minko-plugin-" .. PROJECT_NAME)

	minko.plugin.enable("android")
	minko.plugin.enable("sdl")

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
		"src"
	}

	configuration { "not android" }
	    excludes {
	        minko.plugin.path("video-camera") .. "/include/minko/video/AndroidVideoCamera.hpp",
	        minko.plugin.path("video-camera") .. "/src/minko/video/AndroidVideoCamera.cpp",
	        -- minko.plugin.path("video-camera") .. "/include/minko/video/HtmlWebCamera.hpp",
	        -- minko.plugin.path("video-camera") .. "/src/minko/video/HtmlWebCamera.cpp"
	    }

