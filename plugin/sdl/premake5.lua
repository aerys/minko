PROJECT_NAME = path.getname(os.getcwd())

minko.project.library("minko-plugin-" .. PROJECT_NAME)

	files {
		"include/**.hpp",
		"src/**.cpp",
		"src/**.hpp"
	}

	includedirs {
		"include"
	}

	configuration { "android" }
		defines {
			"ANDROID",
			"GL_GLEXT_PROTOTYPES"
		}
		files {
			"lib/sdl/src/*.c",
			"lib/sdl/src/audio/*.c",
			"lib/sdl/src/audio/dummy/*.c",
			"lib/sdl/src/atomic/*.c",
			"lib/sdl/src/cpuinfo/*.c",
			"lib/sdl/src/dynapi/*.c",
			"lib/sdl/src/events/*.c",
			"lib/sdl/src/file/*.c",
			"lib/sdl/src/haptic/*.c",
			"lib/sdl/src/haptic/dummy/*.c",
			"lib/sdl/src/joystick/*.c",
			"lib/sdl/src/loadso/dlopen/*.c",
			"lib/sdl/src/power/*.c",
			"lib/sdl/src/filesystem/dummy/*.c",
			"lib/sdl/src/render/*.c",
			"lib/sdl/src/stdlib/*.c",
			"lib/sdl/src/thread/*.c",
			"lib/sdl/src/thread/pthread/*.c",
			"lib/sdl/src/timer/*.c",
			"lib/sdl/src/timer/unix/*.c",
			"lib/sdl/src/video/*.c",
			"lib/sdl/src/test/*.c"
		}

		files {
			"lib/sdl/src/audio/android/*.c",
			"lib/sdl/src/core/android/*.c",
			"lib/sdl/src/joystick/android/*.c",
			"lib/sdl/src/power/android/*.c",
			"lib/sdl/src/video/android/*.c",
			"lib/sdl/src/main/android/*.c",
			"lib/sdl/src/render/opengles2/*.c",
			"lib/sdl/src/render/opengles/*.c",
			"lib/sdl/src/render/software/*.c",
		}

		includedirs { minko.plugin.path("android") .. "/include" }

	configuration { "not html5" }
		includedirs { "lib/sdl/include" }

	configuration { "html5" }
		minko.plugin.enable { "webgl" }
