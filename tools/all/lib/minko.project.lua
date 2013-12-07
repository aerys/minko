minko.project = {}

minko.project.library = function(name)
	project(name)

	includedirs { minko.sdk.path("/framework/include") }
	
	configuration { "debug"}
		defines { "DEBUG" }
		flags { "Symbols" }
		targetdir("bin/debug/" .. os.get())

	configuration { "release" }
		defines { "NDEBUG" }
		flags { "Optimize" } -- { "OptimizeSpeed" }
		targetdir("bin/release/" .. os.get())
	
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
			minko.plugin.enable("webgl")
			flags { "Optimize" }
			buildoptions { "-std=c++11" }
	end	
	
	configuration { }
end

minko.project.application = function(name)

	minko.project.library(name)

	if MINKO_SDK_DIST then
		configuration { "debug"}
			links { minko.sdk.path("framework/bin/debug/" .. minko.sdk.gettargetplatform() .. "/framework") }
		configuration { "release"}
			links { minko.sdk.path("framework/bin/release/" .. minko.sdk.gettargetplatform() .. "/framework") }
	else
		links { "minko-framework" }
	end

	configuration { "debug"}
		defines { "DEBUG" }
		flags { "Symbols" }
		targetdir "bin/debug"

	configuration { "release" }
		defines { "NDEBUG" }
		flags { "OptimizeSpeed" }
		targetdir "bin/release"
	
	configuration { "windows" }
		libdirs { minko.sdk.path("/deps/win/lib") }
		links {
			"OpenGL32",
			"glew32"
		}
		postbuildcommands {
			'xcopy /y /i "' .. minko.sdk.path('/framework/effect') .. '" "$(TargetDir)\\effect"',
			'xcopy /y /s asset\\* "$(TargetDir)"',
			minko.vs.getdllscopycommand(minko.sdk.path('/deps/win/lib'))
		}
		
	configuration { "linux" }
		libdirs { minko.sdk.path("/deps/lin/lib") }
		linkoptions { "-Wl,--no-as-needed" }
		links {
			"GL",
			"m"
		}
		postbuildcommands {
			'cp -r ' .. minko.sdk.path('/framework/effect') .. ' ${TARGETDIR} || :',
			'cp -r asset/* ${TARGETDIR} || :'
		}
	
	configuration { "macosx" }
		libdirs { "/deps/mac/lib" }
		linkoptions { "-stdlib=libc++" }
		links {
			"m",
			"SDL2.framework",
			"Cocoa.framework",
			"OpenGL.framework",
			"IOKit.framework"
		}
		postbuildcommands {
			'cp -r ' .. minko.sdk.path('/framework/effect') .. ' . || :',
			'cp -r asset/* . || :'
		}
		
	configuration { }
	
end

minko.project.solution = function(name)
	solution(name)
	configurations { "debug", "release" }

	if not MINKO_SDK_DIST then
		include(minko.sdk.path("framework"))
	end
end
