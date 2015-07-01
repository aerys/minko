PROJECT_NAME = path.getname(os.getcwd())

minko.project.library("minko-plugin-" .. PROJECT_NAME)

	minko.plugin.enable("sdl")

	files {
		"lib/**.hpp",
		"lib/**.h",
		"lib/**.cpp",
		"lib/**.c",
		"src/**.cpp",
		"include/**.hpp",
		"include/**.h",
		"src/**.mm"
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

	configuration { "not ios" }
		excludes {
			minko.plugin.path("video-camera") .. "/include/minko/video/ios/**",
			minko.plugin.path("video-camera") .. "/src/minko/video/ios/**"
		}

	configuration { "ios" }
		buildoptions { "-x objective-c++" } 