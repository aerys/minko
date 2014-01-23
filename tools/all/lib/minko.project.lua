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

	configuration { "vs*" }
		defines { "NOMINMAX" }
		
	configuration { "osx" }
	
	configuration { "linux" }
		
	configuration { "html5" }
		if EMSCRIPTEN_HOME then
			includedirs { EMSCRIPTEN_HOME .. "/system/include" }
		end
		buildoptions {
			"--closure 1",
			"-Wno-warn-absolute-paths"
		}
		optimize "On"

	configuration { }
end

minko.project.application = function(name)

	minko.project.library(name)

	if MINKO_SDK_DIST then
		minko.sdk.links("minko-framework", "framework")
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
			minko.vs.getdllscopycommand(minko.sdk.path('/deps/win/lib'))
		}
	configuration { "win", "release" }
		postbuildcommands {
			'xcopy /y /s asset\\* "$(TargetDir)"'
		}
		
	configuration { "linux" }
		libdirs { minko.sdk.path("/deps/lin/lib") }
		linkoptions { "-Wl,--no-as-needed" }
		links {
			"GL",
			"m"
		}
		postbuildcommands {
			'cp -r ' .. minko.sdk.path('/framework/effect') .. ' ${TARGETDIR} || :'
		}
	configuration { "linux", "release" }
		postbuildcommands {
			'cp -r asset/* ${TARGETDIR} || :'
		}
	
	configuration { "osx" }
		links {
			"m",
			"SDL2.framework",
			"Cocoa.framework",
			"OpenGL.framework",
			"IOKit.framework"
		}
		postbuildcommands {
			'cp -r ' .. minko.sdk.path('/framework/effect') .. ' ${TARGETDIR} || :'
		}
	configuration { "osx", "release" }
		postbuildcommands {
			'cp -r asset/* ${TARGETDIR} || :'
		}

	configuration { "html5" }
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
			.. ' && ' .. emcc .. ' ' .. name .. '.bc -o ' .. name .. '.html -O2 -s CLOSURE_ANNOTATIONS=1 -s DISABLE_EXCEPTION_CATCHING=0 -s TOTAL_MEMORY=268435456 --closure 1 --preload-file effect --preload-file texture --preload-file model --preload-file script --preload-file symbol'
			.. ' || ' .. minko.fail()
		}

	configuration { "html5", "debug" }
		local emcc = premake.tools.gcc.tools.emscripten.cc

		postbuildcommands {
			'cd ${TARGETDIR}'
			.. ' && ' .. emcc .. ' ' .. name .. '.bc -o ' .. name .. '.html -O2 -s DISABLE_EXCEPTION_CATCHING=0 -s TOTAL_MEMORY=268435456 --preload-file effect --preload-file texture --preload-file model --preload-file script --preload-file symbol'
			.. ' || ' .. minko.fail()
		}

	configuration { }
	
end

minko.project.solution = function(name)
	solution(name)
	configurations { "debug", "release" }
	platforms { "linux", "win", "osx", "html5", "ios", "android" }

	print(name)

	if not MINKO_SDK_DIST then
		include(minko.sdk.path("framework"))
	end
end
