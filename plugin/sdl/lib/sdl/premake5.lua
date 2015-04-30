minko.project.library("SDL2")

	targetdir "lib/unknown"
	objdir "obj"

	files {
		"src/*.c",
		"src/audio/*.c",
		"src/audio/dummy/*.c",
		"src/atomic/*.c",
		"src/cpuinfo/*.c",
		"src/dynapi/*.c",
		"src/events/*.c",
		"src/file/*.c",
		"src/haptic/*.c",
		"src/haptic/dummy/*.c",
		"src/joystick/*.c",
		"src/loadso/dlopen/*.c",
		"src/power/*.c",
		"src/filesystem/dummy/*.c",
		"src/render/*.c",
		"src/stdlib/*.c",
		"src/thread/*.c",
		"src/thread/pthread/*.c",
		"src/timer/*.c",
		"src/timer/unix/*.c",
		"src/video/*.c",
		"src/test/*.c"
	}

	includedirs {
		"include"
	}

	configuration { "android" }

		targetdir "lib/android"

		defines {
			"ANDROID",
			"GL_GLEXT_PROTOTYPES"
		}

		files {
			"src/audio/android/*.c",
			"src/core/android/*.c",
			"src/joystick/android/*.c",
			"src/power/android/*.c",
			"src/video/android/*.c",
			"src/main/android/*.c",
			"src/render/opengles2/*.c",
			"src/render/opengles/*.c",
			"src/render/software/*.c",
		}