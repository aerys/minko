minko.project = {}

minko.project.library = function(name)
	project(name)

	includedirs { minko.sdk.path("/framework/src") }
	
	configuration { "debug"}
		defines { "DEBUG" }
		flags { "Symbols" }
		targetdir "bin/debug"

	configuration { "release" }
		defines { "NDEBUG" }
		flags { "Optimize" } -- { "OptimizeSpeed" }
		targetdir "bin/release"
	
	configuration { "windows" }
		includedirs { minko.sdk.path("/deps/win/include") }
		
	configuration { "macosx" }
		includedirs { minko.sdk.path("/deps/mac/include") }
		buildoptions { "-std=c++11 -stdlib=libc++" }
	
	configuration { "linux" }
		includedirs { minko.sdk.path("/deps/lin/include") }
		buildoptions { "-std=c++11" }		
		
	if _OPTIONS["platform"] == "emscripten" then
		configuration { "emscripten" }
			if name ~= 'plugin-webgl' then
				minko.plugin.enable("webgl")
			end
			buildoptions { "-std=c++11" }
	end	
	
	configuration { }
end

minko.project.application = function(name)

	minko.project.library(name)

	links { "framework" }

	configuration { "windows" }
		links { "OpenGL32", "glew32" }
		libdirs { minko.sdk.path("/deps/win/lib") }
		postbuildcommands {
			'xcopy /y /i "' .. minko.sdk.path('/framework/effect') .. '" "$(TargetDir)\\effect"',
			'xcopy /y /s asset\\* "$(TargetDir)"',
			minko.vs.getdllscopycommand(minko.sdk.path('/deps/win/lib'))
		}
		
	configuration { "linux" }
		links { "GL", "m", "Xrandr", "Xxf86vm", "Xi", "rt", "X11", "pthread" }
		libdirs { minko.sdk.path("/deps/lin/lib") }
		postbuildcommands {
			'cp -r ' .. minko.sdk.path('/framework/effect') .. ' ${TARGETDIR} || :',
			'cp -r asset/* ${TARGETDIR} || :'
		}
	
	configuration { "macosx" }
		linkoptions { "-stdlib=libc++" }
		libdirs { "/deps/mac/lib" }
		links {
			"m",
			"Cocoa.framework",
			"OpenGL.framework",
			"IOKit.framework"
		}

		postbuildcommands {
			'cp -r ' .. minko.sdk.path('/framework/effect') .. ' ${TARGETDIR} || :',
			'cp -r asset/* ${TARGETDIR} || :'
		}
		
	configuration { }
	
end

minko.project.solution = function(name)
	solution(name)
	configurations { "debug", "release" }

	include(minko.sdk.path("framework"))
end
