-- A project defines one build target
project "minko-framework"
	kind "StaticLib"
	language "C++"
	files { "**.hpp", "**.h", "**.cpp", "**.c" }
	includedirs {
		"src",
		"lib/jsoncpp/src"
	}
	defines {
		"JSON_IS_AMALGAMATION"
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
		links { "GL", "GLU" }
		buildoptions "-std=c++11"
		linkoptions "-std=c++11"

	-- windows
	configuration { "windows", "x32" }
	links { "glew32" }
	includedirs { "lib/glew/include" }
	libdirs { "lib/glew/bin/win32" }

	-- macos
	configuration { "macosx" }
		buildoptions { "-std=c++11", "-stdlib=libc++" }
		-- libdirs { "/opt/local/lib/" }
		-- includedirs { "/opt/local/include/" }
		links { "OpenGL.framework", "GLUT.framework" }

