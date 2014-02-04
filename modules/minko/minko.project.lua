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
		includedirs { minko.sdk.path("/framework/lib/glew/include") }

	configuration { "vs*" }
		defines { "NOMINMAX" }
		
	configuration { "macosx" }
	
	configuration { "linux" }
		
	configuration { "html5" }
		if EMSCRIPTEN then
			includedirs { EMSCRIPTEN .. "/system/include" }
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

	configuration { "windows32" }
		libdirs {
			minko.sdk.path("/framework/lib/glew/lib/windows32")
		}
		links {
			"minko-framework",
			"OpenGL32",
			"glew32"
		}
		postbuildcommands {
			'xcopy /y /i "' .. minko.sdk.path("/framework/effect") .. '" "$(TargetDir)\\effect"',
			minko.vs.getdllscopycommand(minko.sdk.path("/framework/lib/glew/lib/windows32"))
		}

	configuration { "windows32", "debug" }
		libdirs {
			minko.sdk.path("/framework/bin/windows32/debug")
		}

	configuration { "windows32", "release" }
		libdirs {
			minko.sdk.path("/framework/bin/windows32/release")
		}
		postbuildcommands {
			'xcopy /y /s asset\\* "$(TargetDir)"',
		}

	configuration { "windows64" }
		libdirs { minko.sdk.path("/framework/lib/glew/lib/windows64") }
		links {
			"OpenGL32",
			"glew32"
		}
		postbuildcommands {
			'xcopy /y /i "' .. minko.sdk.path("/framework/effect") .. '" "$(TargetDir)\\effect"',
			'xcopy /y /s asset\\* "$(TargetDir)"',
			minko.vs.getdllscopycommand(minko.sdk.path("/framework/lib/glew/lib/windows64"))
		}

	configuration { "windows64", "debug" }
		libdirs {
			minko.sdk.path("/framework/bin/windows64/debug")
		}

	configuration { "windows64", "release" }
		libdirs {
			minko.sdk.path("/framework/bin/windows64/release")
		}
		postbuildcommands {
			'xcopy /y /s asset\\* "$(TargetDir)"',
		}

	configuration { "linux64" }
		linkoptions { "-Wl,--no-as-needed" }
		links {
			"GL",
			"m"
		}
		postbuildcommands {
			'cp -r ' .. minko.sdk.path("/framework/effect") .. ' ${TARGETDIR} || :'
		}

	configuration { "linux64", "debug" }
		libdirs {
			minko.sdk.path("/framework/bin/linux64/debug")
		}

	configuration { "linux64", "release" }
		libdirs {
			minko.sdk.path("/framework/bin/linux64/release")
		}
		postbuildcommands {
			'cp -r asset/* ${TARGETDIR} || :'
		}
	
	configuration { "linux32" }
		linkoptions { "-Wl,--no-as-needed" }
		links {
			"GL",
			"m"
		}
		postbuildcommands {
			'cp -r ' .. minko.sdk.path("/framework/effect") .. ' ${TARGETDIR} || :'
		}

	configuration { "linux32", "debug" }
		libdirs {
			minko.sdk.path("/framework/bin/linux32/debug")
		}

	configuration { "linux32", "release" }
		libdirs {
			minko.sdk.path("/framework/bin/linux32/release")
		}
		postbuildcommands {
			'cp -r asset/* ${TARGETDIR} || :'
		}
	
	configuration { "osx64" }
		links {
			"m",
			"Cocoa.framework",
			"OpenGL.framework",
			"IOKit.framework"
		}
		postbuildcommands {
			'cp -r ' .. minko.sdk.path('/framework/effect') .. ' ${TARGETDIR} || :'
		}

	configuration { "osx64", "debug" }
		libdirs {
			minko.sdk.path("/framework/bin/osx64/debug")
		}

	configuration { "osx64", "release" }
		libdirs {
			minko.sdk.path("/framework/bin/osx64/release")
		}
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
			'cd ${TARGETDIR} && cp ' .. name .. ' ' .. name .. '.bc || ' .. minko.action.fail()	 
			-- 'cd ${TARGETDIR}'
			-- .. ' && ' .. emcc .. ' ' .. name .. '.bc -o ' .. name .. '.html -s DISABLE_EXCEPTION_CATCHING=0 -s CLOSURE_ANNOTATIONS=0 -s ASM_JS=0 -s TOTAL_MEMORY=268435456 -s ALLOW_MEMORY_GROWTH=1 --preload-file effect --preload-file texture  --compression ${EMSCRIPTEN}/third_party/lzma.js/lzma-native,${EMSCRIPTEN}/third_party/lzma.js/lzma-decoder.js,LZMA.decompress'
			-- -- .. ' && ' .. emcc .. ' ' .. name .. '.bc -o ' .. name .. '.js -O2 -s CLOSURE_ANNOTATIONS=0 -s ASM_JS=0 -s TOTAL_MEMORY=268435456 -s ALLOW_MEMORY_GROWTH=1 --preload-file effect --preload-file texture'
			-- .. ' || ' .. minko.action.fail()
		}

	configuration { "html5", "release" }
		local emcc = premake.tools.gcc.tools.emscripten.cc

		postbuildcommands {
			'cd ${TARGETDIR}'
			.. ' && ' .. emcc .. ' ' .. name .. '.bc -o ' .. name .. '.html -O2 -s CLOSURE_ANNOTATIONS=1 -s DISABLE_EXCEPTION_CATCHING=0 -s TOTAL_MEMORY=268435456 --closure 1 --preload-file effect --preload-file texture --preload-file model --preload-file script --preload-file symbol'
			.. ' || ' .. minko.action.fail()
		}

	configuration { "html5", "debug" }
		local emcc = premake.tools.gcc.tools.emscripten.cc

		postbuildcommands {
			'cd ${TARGETDIR}'
			.. ' && ' .. emcc .. ' ' .. name .. '.bc -o ' .. name .. '.html -O2 -s DISABLE_EXCEPTION_CATCHING=0 -s TOTAL_MEMORY=268435456 --preload-file effect --preload-file texture --preload-file model --preload-file script --preload-file symbol'
			.. ' || ' .. minko.action.fail()
		}

	configuration { }
	
end

minko.project.solution = function(name)
	solution(name)

	configurations {
		"debug",
		"release"
	}

	minko.platform.platforms {
		"linux32",
		"linux64",
		"windows32",
		"windows64",
		"osx64",
		"html5",
		"ios",
		"android",
	}

	if not MINKO_SDK_DIST then
		include(minko.sdk.path("framework"))
	end
end
