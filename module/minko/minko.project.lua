minko.project = {}

minko.project.library = function(name)
	project(name)

	language "C++"
	kind "StaticLib"

	location "."
	includedirs { minko.sdk.path("/framework/include") }

	includedirs { minko.sdk.path("/framework/lib/glm") }

	includedirs { minko.sdk.path("/framework/lib/sparsehash/src") }

	includedirs { minko.sdk.path("/framework/lib/jsoncpp/src") }
	defines { "JSON_IS_AMALGAMATION" }

	configuration { "windows" }
		includedirs { minko.sdk.path("/framework/lib/sparsehash/include/windows") }
		buildoptions { "/wd4996" }
	configuration { "not windows*" }
		includedirs { minko.sdk.path("/framework/lib/sparsehash/include") }

	configuration { "debug" }
		defines { "DEBUG" }
		flags { "Symbols" }

	configuration { "release" }
		defines { "NDEBUG" }
		optimize "On"

	configuration { "windows32 or windows64" }
		includedirs { minko.sdk.path("/framework/lib/glew/include") }

	configuration { "ios" }
		xcodebuildsettings {
			IPHONEOS_DEPLOYMENT_TARGET = "7.0"
		}

	configuration { "vs*" }
		defines {
			"NOMINMAX",										-- do not define min/max as macro in windows.h
			"_VARIADIC_MAX=10",								-- fix for faux variadic templates limited to 5 arguments by default
			"_USE_MATH_DEFINES",							-- enable M_PI
			"_SILENCE_STDEXT_HASH_DEPRECATION_WARNINGS"		-- disable "<hash_map> is deprecated and will be REMOVED" error
		}
		flags {
			--"NoMinimalRebuild"
		}
		buildoptions {
			"/wd4503",				-- remove warnings about too long type names
			--"/MP"					-- Multi Processor build (NoMinimalRebuild flag is needed)
			"/MT"					-- Used to statically link runtime libraries
		}

	configuration { "html5", "debug" }
		buildoptions {
			"-O2 --llvm-opts 0 --js-opts 0 -g4" -- for source maps
		}
	configuration { "html5", "release" }
		buildoptions {
			"-O3 --llvm-lto 1"
		}

	configuration { }
end

minko.project.application = function(name)

	minko.project.library(name)

	kind "ConsoleApp"

	minko.package.assetdirs {
		"asset", -- current directory
		minko.sdk.path("/framework/asset")
	}

	prelinkcommands {
		minko.action.copy(minko.sdk.path("/framework/asset"))
	}

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

	configuration { "windows64" }
		libdirs { minko.sdk.path("/framework/lib/glew/lib/windows64") }
		links {
			"minko-framework",
			"OpenGL32",
			"glew32"
		}
		prelinkcommands {
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

	configuration { "linux64" }
		linkoptions { "-Wl,--no-as-needed" }
		links {
			"minko-framework",
			"GL",
			"m",
			"pthread"
		}

	configuration { "linux64", "debug" }
		libdirs {
			minko.sdk.path("/framework/bin/linux64/debug")
		}

	configuration { "linux64", "release" }
		libdirs {
			minko.sdk.path("/framework/bin/linux64/release")
		}

	configuration { "linux32" }
		linkoptions { "-Wl,--no-as-needed" }
		links {
			"minko-framework",
			"GL",
			"m",
			"pthread"
		}

	configuration { "linux32", "debug" }
		libdirs {
			minko.sdk.path("/framework/bin/linux32/debug")
		}

	configuration { "linux32", "release" }
		libdirs {
			minko.sdk.path("/framework/bin/linux32/release")
		}

	configuration { "osx64" }
		links {
			"minko-framework",
			"m",
			"Cocoa.framework",
			"OpenGL.framework",
			"IOKit.framework"
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

	configuration { "html5" }
		links {
			"minko-framework",
		}
		linkoptions { "-Wl,--no-as-needed" }

		targetsuffix ".bc"

	local emcc = premake.tools.gcc.tools.emscripten.cc
	local empkg = premake.tools.gcc.tools.emscripten.pkg

	configuration { "html5", "release" }
		linkoptions {
			"--llvm-lto 1"
		}

		local cmd = emcc .. ' ${TARGET} -o ${TARGETDIR}/' .. name .. '.html '

		-- optimization
		cmd = cmd .. buildoptions()[1]
		cmd = cmd .. ' -O3'
		-- enable the closure compiler
		cmd = cmd .. ' --closure 1'

		-- add minko-specific emscripten library extension
		cmd = cmd .. ' --js-library "' .. minko.sdk.path("/module/emscripten/library.js") .. '"'

		-- treat undefined symbol warnings as errors
		cmd = cmd .. ' -s ERROR_ON_UNDEFINED_SYMBOLS=1'
		-- disable exception catching
		cmd = cmd .. ' -s DISABLE_EXCEPTION_CATCHING=1'
		cmd = cmd .. ' -s ALLOW_MEMORY_GROWTH=1'
		cmd = cmd .. ' -s NO_EXIT_RUNTIME=1'

		--[[
			optimize (very) long functions by breaking them into smaller ones

			from emscripten's settings.js:
			"OUTLINING_LIMIT: break up functions into smaller ones, to avoid the downsides of very
			large functions (JS engines often compile them very slowly, compile them with lower optimizations,
			or do not optimize them at all)"
		]]--
		cmd = cmd .. ' -s OUTLINING_LIMIT=20000'
		-- use a separate *.mem file to initialize the app memory
		cmd = cmd .. ' --memory-init-file 1'
		-- set the app (or the sdk) template.html
		if os.isfile('template.html') then
			cmd = cmd .. ' --shell-file "${CURDIR}/template.html"'
		else
			cmd = cmd .. ' --shell-file "' .. minko.sdk.path('/skeleton/template.html') .. '"'
		end

		postbuildcommands {
			cmd .. ' || ' .. minko.action.fail()
		}

		libdirs {
			minko.sdk.path("/framework/bin/html5/release")
		}

	configuration { "html5", "debug" }
		linkoptions {
			"--llvm-lto 0"
		}

		local cmd = emcc .. ' ${TARGET} -o ${TARGETDIR}/' .. name .. '.html '

		-- disable optimization
		cmd = cmd .. buildoptions()[1]

		-- add minko-specific emscripten library extension
		cmd = cmd .. ' --js-library "' .. minko.sdk.path("/module/emscripten/library.js") .. '"'

		-- treat undefined symbol warnings as errors
		-- cmd = cmd .. ' -s ERROR_ON_UNDEFINED_SYMBOLS=1'
		-- disable exception catching
		cmd = cmd .. ' -s DISABLE_EXCEPTION_CATCHING=0'
		-- allow memory pool to be dynamic
		cmd = cmd .. ' -s ALLOW_MEMORY_GROWTH=1'
		-- demangling C++ symbols
		cmd = cmd .. ' -s DEMANGLE_SUPPORT=1'
		-- use a separate *.mem file to initialize the app memory
		cmd = cmd .. ' --memory-init-file 1'
		-- set the app (or the sdk) template.html
		if os.isfile('template.html') then
			cmd = cmd .. ' --shell-file "${CURDIR}/template.html"'
		else
			cmd = cmd .. ' --shell-file "' .. minko.sdk.path('/skeleton/template.html') .. '"'
		end

		postbuildcommands {
			-- minko.action.unless('${TARGETDIR}/' .. name .. '.html') ..
			cmd .. ' || ' .. minko.action.fail()
		}

		libdirs {
			minko.sdk.path("/framework/bin/html5/debug")
		}

	configuration { "html5" }
		-- include the preloaded assets into the file system
		local cmd = empkg .. ' ${TARGETDIR}/' .. name .. '.data'

		postbuildcommands {
			-- minko.action.unless('${TARGETDIR}/' .. name .. '.data') ..
			cmd .. ' || ' .. minko.action.fail()
		}

	configuration { "ios" }

		kind "WindowedApp"

		links {
			"minko-framework",
			"m",
			"OpenGLES.framework",
			"Foundation.framework",
			"UIKit.framework",
			"QuartzCore.framework",
			"CoreGraphics.framework"
		}

		files {
			"**.plist"
		}

	configuration { "ios", "debug" }
		libdirs {
			minko.sdk.path("/framework/bin/ios/debug")
		}

	configuration { "ios", "release" }
		libdirs {
			minko.sdk.path("/framework/bin/ios/release")
		}

	configuration { "android" }

		kind "SharedLib"

		prelinkcommands {
			minko.action.cpjf('${CURDIR}/src/', '${TARGETDIR}/src/com/minko/')
		}

		links {
			"minko-framework",
			"GLESv1_CM",
			"GLESv2",
			"EGL",
			"dl",
			"z",
			"log",
			"android",
			"stdc++",
		}

		targetprefix "lib"
		targetextension ".so"
		linkoptions {
			"-shared",
			"-pthread",
			"-Wl,--no-undefined",
			"-Wl,--undefined=Java_org_libsdl_app_SDLActivity_nativeInit"
		}

		prelinkcommands {
			minko.action.copy(minko.sdk.path("/framework/asset"))
		}

		postbuildcommands {
			minko.action.copy(MINKO_HOME .. "/template/android/*"),
			minko.action.buildandroid()
		}

	configuration { "android", "debug" }
		libdirs {
			minko.sdk.path("/framework/bin/android/debug")
		}

	configuration { "android", "release" }
		libdirs {
			minko.sdk.path("/framework/bin/android/release")
		}

	if _OPTIONS['with-offscreen'] then
			minko.plugin.enable { "offscreen" }
	end

	configuration { "android", "debug" }
		libdirs {
			minko.sdk.path("/framework/bin/android/debug")
		}

	configuration { "android", "release" }
		libdirs {
			minko.sdk.path("/framework/bin/android/release")
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
			emcc .. ' ${TARGET} -o ${TARGETDIR}/' .. name .. '.js -O3 --closure 1 -s DISABLE_EXCEPTION_CATCHING=0 -s TOTAL_MEMORY=268435456 -s EXPORTED_FUNCTIONS="[\'minkoWorkerEntryPoint\']" || ' .. minko.action.fail()
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
