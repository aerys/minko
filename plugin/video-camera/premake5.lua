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
			minko.plugin.path("video-camera") .. "/src/minko/video/AndroidVideoCamera.cpp"
		}

	configuration { "not html5" }
		excludes {
			minko.plugin.path("video-camera") .. "/include/minko/video/HTML5VideoCamera.hpp",
			minko.plugin.path("video-camera") .. "/src/minko/video/HTML5VideoCamera.cpp"
		}
