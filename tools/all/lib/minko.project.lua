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
	
	configuration { "win" }
		includedirs { minko.sdk.path("/deps/win/include") }
		
	configuration { "osx" }
	
	configuration { "linux" }
		
	configuration { "html5" }
		includedirs { EMSCRIPTEN_HOME .. "/system/include" }
		buildoptions { "-Wno-warn-absolute-paths" }
		-- FIXME: Only enable in release when emscripten is finally able to successfully compile without -O2
		optimize "On"

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

	configuration { "win" }
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
	
	configuration { "osx" }
		libdirs { "/deps/mac/lib" }
		links {
			"m",
			"SDL2.framework",
			"Cocoa.framework",
			"OpenGL.framework",
			"IOKit.framework"
		}
		postbuildcommands {
			'cp -r ' .. minko.sdk.path('/framework/effect') .. ' ${TARGETDIR} || :',
			'cp -r asset/* ${TARGETDIR} || :'
		}

	configuration { "html5" }
		-- local inspect = require 'inspect'
		-- print(inspect.inspect(configuration().configset._current))
		minko.plugin.enable("webgl")

		prelinkcommands {
			'cp -r ' .. minko.sdk.path('/framework/effect') .. ' ${TARGETDIR} || :',
			'cp -r asset/* ${TARGETDIR} || :'
		}

		postbuildcommands {
			'cd ${TARGETDIR} && cp ' .. name .. ' ' .. name .. '.bc || ' .. minko.fail()	 
			-- 'cd ${TARGETDIR}'
			-- .. ' && ' .. emcc .. ' ' .. name .. '.bc -o ' .. name .. '.html -s DISABLE_EXCEPTION_CATCHING=0 -s CLOSURE_ANNOTATIONS=0 -s ASM_JS=0 -s TOTAL_MEMORY=268435456 -s ALLOW_MEMORY_GROWTH=1 --preload-file effect --preload-file texture  --compression ${EMSCRIPTEN_HOME}/third_party/lzma.js/lzma-native,${EMSCRIPTEN_HOME}/third_party/lzma.js/lzma-decoder.js,LZMA.decompress'
			-- -- .. ' && ' .. emcc .. ' ' .. name .. '.bc -o ' .. name .. '.js -O2 -s CLOSURE_ANNOTATIONS=0 -s ASM_JS=0 -s TOTAL_MEMORY=268435456 -s ALLOW_MEMORY_GROWTH=1 --preload-file effect --preload-file texture'
			-- .. ' || ' .. minko.fail()
		}

	configuration { "html5", "release" }
		local emcc = premake.tools.gcc.tools.emscripten.cc

		postbuildcommands {
			'cd ${TARGETDIR}'
			.. ' && ' .. emcc .. ' ' .. name .. '.bc -o ' .. name .. '.html -O2 -s CLOSURE_ANNOTATIONS=1 -s DISABLE_EXCEPTION_CATCHING=0 -s ASM_JS=0 -s TOTAL_MEMORY=268435456 -s ALLOW_MEMORY_GROWTH=1 --preload-file effect --preload-file texture --preload-file model'
			.. ' || ' .. minko.fail()
		}

	configuration { "html5", "debug" }
		local emcc = premake.tools.gcc.tools.emscripten.cc

		postbuildcommands {
			'cd ${TARGETDIR}'
			.. ' && ' .. emcc .. ' ' .. name .. '.bc -o ' .. name .. '.html -O2 -s DISABLE_EXCEPTION_CATCHING=0 -s ASM_JS=0 -s TOTAL_MEMORY=268435456 -s ALLOW_MEMORY_GROWTH=1 --preload-file effect --preload-file texture --preload-file model'
			.. ' || ' .. minko.fail()
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
