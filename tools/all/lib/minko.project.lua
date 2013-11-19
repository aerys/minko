minko.project = {}

minko.project.library = function(name)
	project(name)

	includedirs { minko.sdk.path("/framework/src") }
	
	configuration { "windows" }
		links { "OpenGL32", "glew32" }
		libdirs { minko.sdk.path("/deps/win/lib") }
		includedirs { minko.sdk.path("/deps/win/include") }
		
	configuration { "macosx" }
		libdirs { "/deps/mac/lib" }
		includedirs { minko.sdk.path("/deps/mac/include") }
		buildoptions { "-std=c++11 -stdlib=libc++" }
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
			-- minko.plugin.enable("webgl")
			flags { "Optimize" }
			buildoptions { "-std=c++11" }
	end	
	
	configuration { }
end

minko.project.application = function(name)

	minko.project.library(name)

	links { "framework" }

	configuration { "debug"}
		defines { "DEBUG" }
		flags { "Symbols" }
		targetdir "bin/debug"

	configuration { "release" }
		defines { "NDEBUG" }
		flags { "OptimizeSpeed" }
		targetdir "bin/release"
	
	configuration { "windows" }
		postbuildcommands {
			'xcopy /y /i "' .. minko.sdk.path('/framework/effect') .. '" "$(TargetDir)\\effect"',
			'xcopy /y /s asset\\* "$(TargetDir)"',
			minko.vs.getdllscopycommand(minko.sdk.path('/deps/win/lib'))
		}
		
	configuration { "linux" }
		postbuildcommands {
			'cp -r ' .. minko.sdk.path('/framework/effect') .. ' ${TARGETDIR} || :',
			'cp -r asset/* ${TARGETDIR} || :'
		}
	
	configuration { "macosx" }
		linkoptions { "-stdlib=libc++" }
		postbuildcommands {
			'cp -r ' .. minko.sdk.path('/framework/effect') .. ' . || :',
			'cp -r asset/* . || :'
		}
		
	configuration { }
	
end

minko.project.solution = function(name)
	solution(name)
	configurations { "debug", "release" }

	include(minko.sdk.path("framework"))
end
