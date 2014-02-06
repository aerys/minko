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
	
	configuration { "windows32 or windows64" }
		includedirs { minko.sdk.path("/framework/lib/glew/include") }

	configuration { "vs*" }
		defines { "NOMINMAX" }
		
	configuration { "html5" }
		if EMSCRIPTEN then
			includedirs { EMSCRIPTEN .. "/system/include" }
		end

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
			minko.action.copy(minko.sdk.path("/framework/effect")),
			minko.action.copy(minko.sdk.path("/framework/lib/glew/lib/windows32/*.dll")),
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
			minko.action.copy("asset"),
		}

	configuration { "windows64" }
		libdirs { minko.sdk.path("/framework/lib/glew/lib/windows64") }
		links {
			"minko-framework",
			"OpenGL32",
			"glew32"
		}
		postbuildcommands {
			minko.action.copy(minko.sdk.path("/framework/effect")),
			minko.action.copy(minko.sdk.path("/framework/lib/glew/lib/windows64/*.dll")),
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
			minko.action.copy("asset"),
		}

	configuration { "linux64" }
		linkoptions { "-Wl,--no-as-needed" }
		links {
			"minko-framework",
			"GL",
			"m"
		}
		postbuildcommands {
			minko.action.copy(minko.sdk.path("/framework/effect")),
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
			minko.action.copy("asset"),
		}
	
	configuration { "linux32" }
		linkoptions { "-Wl,--no-as-needed" }
		links {
			"GL",
			"m"
		}
		postbuildcommands {
			minko.action.copy(minko.sdk.path("/framework/effect")),
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
			minko.action.copy("asset"),
		}
	
	configuration { "osx64" }
		links {
			"minko-framework",
			"m",
			"Cocoa.framework",
			"OpenGL.framework",
			"IOKit.framework"
		}
		postbuildcommands {
			minko.action.copy(minko.sdk.path("/framework/effect")),
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
			minko.action.copy("asset"),
		}

	configuration { "html5" }
		minko.plugin.enable("webgl")

		links {
			"minko-framework",
		}

		targetsuffix "bc"
		
		prelinkcommands {
			minko.action.copy(minko.sdk.path("/framework/effect")),
			minko.action.copy("asset"),
		}

	configuration { "html5", "release" }
		local emcc = premake.tools.gcc.tools.emscripten.cc

		postbuildcommands {
			'cd ${TARGETDIR}'
			.. ' && ' .. emcc .. ' ${TARGETNAME} -o ' .. name .. '.html -O2 --closure 1 -s CLOSURE_ANNOTATIONS=1 -s DISABLE_EXCEPTION_CATCHING=0 -s TOTAL_MEMORY=268435456 --preload-file asset'
			.. ' || ' .. minko.action.fail()
		}

	configuration { "html5", "debug" }
		local emcc = premake.tools.gcc.tools.emscripten.cc

		buildoptions {
			"-g4"
		}

		postbuildcommands {
			'cd ${TARGETDIR}'
			.. ' && ' .. emcc .. ' ${TARGETNAME} -o ' .. name .. '.html -O2 --js-opts 0 -g4 -s ASM_JS=0 -s DISABLE_EXCEPTION_CATCHING=0 -s TOTAL_MEMORY=268435456 --preload-file asset'
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
