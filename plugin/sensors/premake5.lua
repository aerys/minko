PROJECT_NAME = path.getname(os.getcwd())

minko.project.library("minko-plugin-" .. PROJECT_NAME)

	minko.plugin.enable("sdl")

	files {
		"src/**.hpp",
		"src/**.cpp",
		"include/**.hpp"
	}

	excludes {
		"include/android/**",
		"include/apple/**",
		"src/android/**",
		"src/apple/**"
	}

	includedirs { "include" }

	configuration { "html5" }
		files {
			"include/emscripten/**",
			"src/emscripten/**"
		}

	configuration { "android" }
		files {
			"include/android/**",
			"src/android/**"
		}

	-- Mac platforms
	configuration { "ios or osx64" }
		buildoptions { "-x objective-c++" }

	configuration { "ios" }
		files {
			"include/apple/**",
			"src/apple/**"
		}

		links {
			"CoreMotion.framework" -- To receive data from mobile device sensors
		}
