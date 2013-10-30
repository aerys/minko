minko.project = {}

minko.project.library = function(name)
	project(name)
	
	links { "minko-framework" }
	includedirs { minko.sdk.path("/framework/src") }
	
	configuration { "windows" }
		links { "OpenGL32", "glew32" }
		libdirs { minko.sdk.path("/deps/win/lib") }
		includedirs { minko.sdk.path("/deps/win/include") }
		postbuildcommands {
			minko.vs.getdllscopycommand(minko.sdk.path("deps/win/lib"))
		}
		
	configuration { "macosx" }
		libdirs { "/deps/mac/lib" }
		includedirs { minko.sdk.path("/deps/mac/include") }
		buildoptions { "-std=c++11", "-stdlib=libc++" }
		linkoptions { "-std=c++11", "-stdlib=libc++" }
		links {
			"m",
			"SDL2.framework",
			"Cocoa.framework",
			"OpenGL.framework",
			"IOKit.framework"
		}
	
	configuration { "linux" }
		links { "GL", "SDL2", "m", "Xrandr", "Xxf86vm", "Xi", "rt", "X11", "pthread" }
		libdirs { minko.sdk.path("/deps/lin/lib") }
		includedirs { minko.sdk.path("/deps/lin/include") }
		buildoptions { "-std=c++11" }		
		
	if _OPTIONS["platform"] == "emscripten" then
		configuration { "emscripten" }
			minko.plugin.enable("webgl")
			flags { "Optimize" }
			buildoptions { "-std=c++11" }
	end	
	
	configuration { }
end

minko.project.application = function(name)
	minko.project.library(name)
	
	configuration { "windows" }
		postbuildcommands {
			'xcopy /y /i ' .. minko.sdk.path('/framework/effect') .. ' "$(TargetDir)\\effect"',
			'xcopy /y /s asset\\* "$(TargetDir)"'
		}
		
	configuration { "linux" }
		postbuildcommands {
			'cp -r ' .. minko.sdk.path('/framework/effect') .. ' ${TARGETDIR}',
			'cp -r asset/* ${TARGETDIR}'
		}
	
	configuration { "macosx" }
		postbuildcommands {
			'cp -r ' .. minko.sdk.path('/framework/effect') .. ' .',
			'cp -r asset/* .'
		}
		
	configuration { }
end