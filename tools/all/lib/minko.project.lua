minko.project = {}

minko.project.library = function(name)
	project(name)

	location "."
	includedirs { minko.sdk.path("/framework/include") }
	
	configuration { "debug"}
		defines { "DEBUG" }
		flags { "Symbols" }

	configuration { "release" }
		defines { "NDEBUG" }
		optimize "On"
	
	configuration { "windows" }
		includedirs { minko.sdk.path("/deps/win/include") }
		
	configuration { "macosx" }
		includedirs { minko.sdk.path("/deps/mac/include") }
	
	configuration { "linux" }
		includedirs { minko.sdk.path("/deps/lin/include") }
		
	configuration { }
end

minko.project.application = function(name)

	minko.project.library(name)

	if MINKO_SDK_DIST then
		configuration { "debug"}
			links { minko.sdk.path("framework/bin/debug/" .. minko.sdk.gettargetplatform() .. "/minko-framework") }
		configuration { "release"}
			links { minko.sdk.path("framework/bin/release/" .. minko.sdk.gettargetplatform() .. "/minko-framework") }
	else
		links { "minko-framework" }
	end

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
		links {
			"m",
			"SDL2.framework",
			"Cocoa.framework",
			"OpenGL.framework",
			"IOKit.framework"
		}
		postbuildcommands {
			'cp -r ' .. minko.sdk.path('/framework/effect') .. ' . || ' .. minko.fail(),
			'cp -r asset/* . || ' .. minko.fail()
		}

	configuration { "emscripten" }
		postbuildcommands {
			'cp -r asset/* ${TARGETDIR} || ' .. minko.fail(),
			'cd ${TARGETDIR} && cp ' .. name .. ' ' .. name .. '.bc || ' .. minko.fail(),			 
			'cd ${TARGETDIR} && emcc ' .. name .. '.bc -o ' .. name .. '.html -O2 -s CLOSURE_ANNOTATIONS=0 -s ASM_JS=0 -s TOTAL_MEMORY=268435456 -s ALLOW_MEMORY_GROWTH=1 --preload-file effect --preload-file texture  --compression ${EMSCRIPTEN_HOME}/third_party/lzma.js/lzma-native,${EMSCRIPTEN_HOME}/third_party/lzma.js/lzma-decoder.js,LZMA.decompress || ' .. minko.fail(),
			'cd ${TARGETDIR} && emcc ' .. name .. '.bc -o ' .. name .. '.js -O2 -s CLOSURE_ANNOTATIONS=0 -s ASM_JS=0 -s TOTAL_MEMORY=268435456 -s ALLOW_MEMORY_GROWTH=1 --preload-file effect --preload-file texture  --compression ${EMSCRIPTEN_HOME}/third_party/lzma.js/lzma-native,${EMSCRIPTEN_HOME}/third_party/lzma.js/lzma-decoder.js,LZMA.decompress || ' .. minko.fail()
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
