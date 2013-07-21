-- A project defines one build target
project "minko-example-mk-physic"
	kind "ConsoleApp"
	language "C++"
	links {
		"minko-jpeg",
		"minko-png",
		"minko-mk",
		"minko-bullet",
		"minko-particles",
		"minko-framework"
	}
	files { "**.hpp", "**.h", "**.cpp" }
	includedirs {
		"src",
		"../../lib/glfw/include",
		"../../framework/src",
		"../../plugins/mk/src",
		"../../plugins/bullet/src",
		"../../plugins/jpeg/src",
		"../../plugins/webgl/src",
		"../../plugins/png/src",
		"../../plugins/particles/src"
	}

	configuration { "debug"}
		defines { "DEBUG" }
		flags { "Symbols" }
		targetdir "bin/debug"

	configuration { "release" }
		defines { "NDEBUG" }
		flags { "OptimizeSpeed" }
		targetdir "bin/release"

	-- linux
	configuration { "linux" }
		links { "GL", "GLU", "glfw3", "m", "Xrandr", "Xxf86vm", "Xi", "rt" }
		libdirs {
			"../../deps/lin/lib"
		}
		includedirs {
			"../../deps/lin/include"
		}
		buildoptions "-std=c++11"
		postbuildcommands {
			'cp -r ../../framework/effect .'
		}

	-- windows
	configuration { "windows", "x32" }
		links { "OpenGL32", "glfw3dll", "glew32" }
		libdirs {
			"../../deps/win/lib"
		}
		includedirs {
			"../../deps/win/include"
		}
		postbuildcommands {
			'xcopy ..\\..\\framework\\effect . /E'
		}

	-- macos
	configuration { "macosx" }
		buildoptions { "-std=c++11", "-stdlib=libc++" }
		linkoptions { "-std=c++11", "-stdlib=libc++" }
		links {
			"m",
			"glfw3",
			"Cocoa.framework",
			"OpenGL.framework",
			"IOKit.framework"
		}
		libdirs {
			"../../deps/mac/lib"
		}
		includedirs {
			"../../deps/mac/include"
		}

	-- emscripten
	configuration { "emscripten" }
		flags { "Optimize" }
		links {
			"minko-webgl"
		}
