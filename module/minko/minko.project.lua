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
		defines {
			"NOMINMAX",				-- do not define min/max as macro in windows.h
			"_VARIADIC_MAX=10"		-- fix for faux variadic templates limited to 5 arguments by default
		}
		buildoptions {
			"/wd4503"				-- remove warnings about too long type names
		}
		
	configuration { "html5" }
		if EMSCRIPTEN then
			includedirs { EMSCRIPTEN .. "/system/include" }
		end

	configuration { "html5", "debug" }
		buildoptions {
			"-g4"
		}

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
		prelinkcommands {
			minko.action.copy(minko.sdk.path("/framework/asset")),
			minko.action.copy(minko.sdk.path("/framework/lib/glew/lib/windows32/*.dll"))
		}

	configuration { "windows32", "debug" }
		libdirs {
			minko.sdk.path("/framework/bin/windows32/debug")
		}

	configuration { "windows32", "release" }
		libdirs {
			minko.sdk.path("/framework/bin/windows32/release")
		}
		prelinkcommands {
			minko.action.copy("asset")
		}

	configuration { "windows64" }
		libdirs { minko.sdk.path("/framework/lib/glew/lib/windows64") }
		links {
			"minko-framework",
			"OpenGL32",
			"glew32"
		}
		prelinkcommands {
			minko.action.copy(minko.sdk.path("/framework/asset")),
			minko.action.copy(minko.sdk.path("/framework/lib/glew/lib/windows64/*.dll"))
		}

	configuration { "windows64", "debug" }
		libdirs {
			minko.sdk.path("/framework/bin/windows64/debug")
		}

	configuration { "windows64", "release" }
		libdirs {
			minko.sdk.path("/framework/bin/windows64/release")
		}
		prelinkcommands {
			minko.action.copy("asset"),
		}

	configuration { "linux64" }
		linkoptions { "-Wl,--no-as-needed" }
		links {
			"minko-framework",
			"GL",
			"m"
		}
		prelinkcommands {
			minko.action.copy(minko.sdk.path("/framework/asset")),
		}

	configuration { "linux64", "debug" }
		libdirs {
			minko.sdk.path("/framework/bin/linux64/debug")
		}

	configuration { "linux64", "release" }
		libdirs {
			minko.sdk.path("/framework/bin/linux64/release")
		}
		prelinkcommands {
			minko.action.copy("asset"),
		}

	configuration { "linux32" }
		linkoptions { "-Wl,--no-as-needed" }
		links {
			"minko-framework",
			"GL",
			"m"
		}
		prelinkcommands {
			minko.action.copy(minko.sdk.path("/framework/asset")),
		}

	configuration { "linux32", "debug" }
		libdirs {
			minko.sdk.path("/framework/bin/linux32/debug")
		}

	configuration { "linux32", "release" }
		libdirs {
			minko.sdk.path("/framework/bin/linux32/release")
		}
		prelinkcommands {
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
		linkoptions {
			"-Wl,-rpath,."
		}
		prelinkcommands {
			minko.action.copy(minko.sdk.path("/framework/asset")),
		}

	configuration { "osx64", "debug" }
		libdirs {
			minko.sdk.path("/framework/bin/osx64/debug")
		}

	configuration { "osx64", "release" }
		libdirs {
			minko.sdk.path("/framework/bin/osx64/release")
		}
		prelinkcommands {
			minko.action.copy("asset"),
		}

	configuration { "html5" }
		minko.plugin.enable("webgl")

		links {
			"minko-framework",
		}

		targetsuffix ".bc"
		
		prelinkcommands {
			minko.action.copy(minko.sdk.path("/framework/asset")),
			minko.action.copy("asset"),
		}

	configuration { "html5", "release" }
		local emcc = premake.tools.gcc.tools.emscripten.cc

		postbuildcommands {
			emcc .. ' ${TARGET} -o ${TARGETDIR}/' .. name .. '.html -O2 --closure 1 -s CLOSURE_ANNOTATIONS=1 -s ERROR_ON_UNDEFINED_SYMBOLS=1 -s DISABLE_EXCEPTION_CATCHING=0 -s TOTAL_MEMORY=268435456 --preload-file ${TARGETDIR}/asset || ' .. minko.action.fail()
		}

	configuration { "html5", "debug" }
		local emcc = premake.tools.gcc.tools.emscripten.cc

		buildoptions {
			"-g4" -- allow source maps in final .js
		}

		postbuildcommands {
			emcc .. ' ${TARGET} -o ${TARGETDIR}/' .. name .. '.html -O2 --js-opts 0 -g4 -s ASM_JS=0 -s DISABLE_EXCEPTION_CATCHING=0 -s ERROR_ON_UNDEFINED_SYMBOLS=1 -s TOTAL_MEMORY=268435456 --preload-file ${TARGETDIR}/asset || ' .. minko.action.fail()
		}

	configuration { "ios" }
		links {
			"minko-framework",
			"m",
			"OpenGLES.framework",
			"Foundation.framework",
			"UIKit.framework",
			"QuartzCore.framework",
			"CoreGraphics.framework"
		}

		prelinkcommands {
			minko.action.copy(minko.sdk.path("/framework/asset")),
			minko.action.copy("asset"),
		}

		files {
			"**.plist"
		}

	configuration { }
	
end

minko.project.worker = function(name)
	minko.project.library(name)

	kind "StaticLib"
	language "C++"

	removelinks { "minko-framework" }

	configuration { "html5" }
		local emcc = premake.tools.gcc.tools.emscripten.cc

		postbuildcommands {
			emcc .. ' ${TARGET} -o ${TARGETDIR}/' .. name .. '.js -O2 --closure 1 -s DISABLE_EXCEPTION_CATCHING=0 -s TOTAL_MEMORY=268435456 -s EXPORTED_FUNCTIONS="[\'minkoWorkerEntryPoint\']" || ' .. minko.action.fail()
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
