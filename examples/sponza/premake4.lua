-- A project defines one build target
project "minko-example-sponza"
	kind "ConsoleApp"
	language "C++"
	links {
		"minko-png",
		"minko-jpeg",
		"minko-mk",
		"minko-bullet",
		"minko-particles",
		"minko-webgl",
		"minko-framework"
	}
	files {
		"**.hpp",
		"**.h",
		"**.cpp"
	}
	includedirs {
		"src",
		"../../deps/all/include",
		"../../framework/src",
		"../../plugins/mk/src",
		"../../plugins/bullet/src",
		"../../plugins/webgl/src",
		"../../plugins/png/src",
		"../../plugins/jpeg/src",
		"../../plugins/webgl/src",
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
			'cp -r ../../framework/effect .',
			'cp -r asset/* .'
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
			-- copy framework effects
			'xcopy /y /e /i ..\\..\\framework\\effect\\* $(TargetDir)effect',
			-- copy assets
			'xcopy /y /e /i asset\\* $(TargetDir)',
			-- copy dlls
			'for /r %%x in (..\\..\\deps\\win\\lib\\*.dll) do xcopy /y /e /i "%%x" $(TargetDir)'
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
		postbuildcommands {
			'cp -r ../../framework/effect .',
			'cp -r asset/* .'
		}

	-- emscripten
	configuration { "emscripten" }
		flags { "Optimize" }
--		links { "minko-webgl" }
--		includedirs { "../../plugins/webgl/src" }
		buildoptions { "-std=c++11" }
		local bin = "bin/release/" .. project().name
		postbuildcommands {
			'cp ' .. bin .. ' ' .. bin .. '.bc',
			'emcc ' .. bin .. '.bc -o ' .. bin .. '.html -O1 -s ASM_JS=1 -s TOTAL_MEMORY=1073741824 --preload-file effect --preload-file texture --preload-file model'
		}
