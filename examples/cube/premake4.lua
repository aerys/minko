-- A project defines one build target
project "minko-example-cube"
	kind "ConsoleApp"
	language "C++"
	links {
		"minko-png",
		"minko-framework"
	}
	files { "**.hpp", "**.h", "**.cpp" }
	includedirs {
		"src",
		"../../framework/src",
		"../../plugins/png/src",
		"../../deps/all/include"
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
<<<<<<< HEAD
		buildoptions { "-std=c++11" }
		linkoptions { "-std=c++11" }
=======
		libdirs {
			"../../deps/lin/lib"
		}
		includedirs {
			"../../deps/lin/include"
		}
		buildoptions "-std=c++11"
>>>>>>> ea431194a11202d0c169511b171b0923b49f9170
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
			'xcopy /y /e /i ..\\..\\framework\\effect\\* $(TargetDir)effect'
		}

	-- macos
	configuration { "macosx" }
		buildoptions { "-std=c++11", "-stdlib=libc++" }
		linkoptions { "-std=c++11", "-stdlib=libc++" }
<<<<<<< HEAD
		links { "glfw3", "m", "Cocoa.framework", "OpenGL.framework", "IOKit.framework" }
		includedirs { "../../deps/mac/include" }
		libdirs { "../../deps/mac/lib" }
		postbuildcommands {
			'cp -r ../../framework/effect .'
=======
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
>>>>>>> ea431194a11202d0c169511b171b0923b49f9170
		}

	-- emscripten
	configuration { "emscripten" }
		flags { "Optimize" }
		links { "minko-webgl" }
		includedirs { "../../plugins/webgl/src" }
		buildoptions { "-std=c++11" }
		local bin = "bin/release/" .. project().name
		postbuildcommands {
			'cp ' .. bin .. ' ' .. bin .. '.bc',
			'emcc ' .. bin .. '.bc -o ' .. bin .. '.html -O1 -s ASM_JS=1 -s TOTAL_MEMORY=1073741824 --preload-dir effect --preload-dir texture'
		}
