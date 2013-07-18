-- A project defines one build target
project "minko-examples"
	kind "ConsoleApp"
	language "C++"
	links {
		"minko-jpeg",
		"minko-png",
		"minko-webgl",
		"minko-framework"
	}
	files { "**.hpp", "**.h", "**.cpp" }
	includedirs {
		"src",
		"lib/glfw/include",
		"../framework/src",
		"../plugins/jpeg/src",
		"../plugins/png/src",
		"../plugins/webgl/src"
	}

	configuration { "debug"}
		defines { "DEBUG" }
		flags { "Symbols" }
		targetdir "bin/debug"

	configuration { "release" }
		defines { "NDEBUG" }
		-- flags { "OptimizeSpeed" }
		targetdir "bin/release"

	-- linux
	configuration { "linux" }
		links { "GL", "glfw3", "m", "Xrandr", "Xxf86vm", "Xi", "rt" }
		buildoptions "-std=c++11"
		linkoptions "-std=c++11"

	-- windows
	configuration { "windows", "x32" }
		links { "OpenGL32", "glfw3dll", "glew32" }
		libdirs {
			"lib/glfw/bin/win32",
			"../framework/lib/glew/bin/win32"
		}

	-- macos
	configuration { "macosx" }
		buildoptions { "-std=c++11", "-stdlib=libc++" }
		linkoptions { "-std=c++11", "-stdlib=libc++" }
		links { "glfw3", "m", "Cocoa.framework", "OpenGL.framework", "IOKit.framework" }
		libdirs { "/opt/local/lib/" }
		includedirs { "/opt/local/include/" }

	configuration { "emscripten", "release" }
		buildoptions { "-std=c++11" }
		-- prebuildcommands { "grep '\\.bc' Makefile || sed -i Makefile -r -e 's/^\s*TARGET\s*=.*$/&.bc/'" }
		postbuildcommands { 'BIN=bin/release/minko-examples; cp ${BIN} ${BIN}.bc; emcc ${BIN} -o ${BIN}.html -O1 -s ASM_JS=1 --preload-dir ../framework/bin/release/' }
