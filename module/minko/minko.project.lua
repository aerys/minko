minko.project = {}

minko.project.library = function(name)
	project(name)

	language "C++"
	kind "StaticLib"

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
			"_VARIADIC_MAX=10",		-- fix for faux variadic templates limited to 5 arguments by default
			"_USE_MATH_DEFINES"		-- enable M_PI
		}
		buildoptions {
			"/wd4503"				-- remove warnings about too long type names
		}

	configuration { "html5", "debug" }
		buildoptions {
			"-g4"					-- for source maps
		}

	configuration { }
end

minko.project.application = function(name)

	minko.project.library(name)

	kind "ConsoleApp"

	-- defines {
	-- 	"MINKO_APPLICATION_NAME=" .. name
	-- }

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
		-- linkoptions {
		-- 	"-Wl,-rpath,."
		-- }
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

	if premake.tools.gcc.tools.emscripten then
	configuration { "html5", "release" }
		local emcc = premake.tools.gcc.tools.emscripten.cc
		local cmd = emcc .. ' ${TARGET} -o ${TARGETDIR}/' .. name .. '.html -O2'

		-- enable the closure compiler
		cmd = cmd .. ' --closure 1 -s CLOSURE_ANNOTATIONS=1'
		-- treat undefined symbol warnings as errors
		cmd = cmd .. ' -s ERROR_ON_UNDEFINED_SYMBOLS=1'
		-- disable exception catching
		cmd = cmd .. ' -s DISABLE_EXCEPTION_CATCHING=0'
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
		-- include the app's 'asset' directory into the file system
		cmd = cmd .. ' --preload-file ${TARGETDIR}/asset'

		postbuildcommands {
			cmd .. ' || ' .. minko.action.fail(),
			-- fix the "invalid increment operand" syntax error caused by ++0 in the output file
			'python "' .. minko.sdk.path('/module/emscripten/fix_invalid_increment_operand.py') .. '"  ${TARGETDIR}/' .. name .. '.js'
		}

		libdirs {
			minko.sdk.path("/framework/bin/html5/release")
		}

	configuration { "html5", "debug" }
		local emcc = premake.tools.gcc.tools.emscripten.cc
		local cmd = emcc .. ' ${TARGET} -o ${TARGETDIR}/' .. name .. '.html -O2 --js-opts 0 -g4 -s ASM_JS=0 -s DISABLE_EXCEPTION_CATCHING=0 -s ERROR_ON_UNDEFINED_SYMBOLS=1 --memory-init-file 1 --preload-file ${TARGETDIR}/asset'

		if os.isfile('template.html') then
			cmd = cmd .. ' --shell-file "${CURDIR}/template.html"'
		else
			cmd = cmd .. ' --shell-file "' .. minko.sdk.path('/skeleton/template.html') .. '"'
		end

		buildoptions {
			"-g4" -- allow source maps in final .js
		}

		postbuildcommands {
			cmd .. ' || ' .. minko.action.fail(),
			-- fix the "invalid increment operand" syntax error caused by ++0 in the output file
			'python "' .. minko.sdk.path('/module/emscripten/fix_invalid_increment_operand.py') .. '"  ${TARGETDIR}/' .. name .. '.js'
		}

		libdirs {
			minko.sdk.path("/framework/bin/html5/debug")
		}
	end

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

		prelinkcommands {
			minko.action.copy(minko.sdk.path("/framework/asset")),
			minko.action.copy("asset")
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
			-- "gnustl_static",
		}

		targetprefix "lib"
		targetextension ".so"
		linkoptions {
			-- "-s",
			"-shared",
			"-pthread",
			"-Wl,--no-undefined",
			"-Wl,--undefined=Java_org_libsdl_app_SDLActivity_nativeInit",
			"-Wl,--undefined=Java_minko_plugin_htmloverlay_InitWebViewTask_webViewInitialized",
			"-Wl,--undefined=Java_minko_plugin_htmloverlay_MinkoWebViewClient_webViewPageLoaded",
			"-Wl,--undefined=Java_minko_plugin_htmloverlay_WebViewJSInterface_minkoNativeOnMessage",
			"-Wl,--undefined=Java_minko_plugin_htmloverlay_WebViewJSInterface_minkoNativeOnEvent"
		}

		prelinkcommands {
			minko.action.copy(minko.sdk.path("/framework/asset")),
			minko.action.copy("asset")
		}

	configuration { "android", "debug" }
		libdirs {
			minko.sdk.path("/framework/bin/android/debug")
		}

	configuration { "android", "release" }
		libdirs {
			minko.sdk.path("/framework/bin/android/release")
		}

    configuration { "with-offscreen" }
            minko.plugin.enable { "offscreen" }

	configuration { }

end

minko.project.worker = function(name)
	minko.project.library(name)

	kind "StaticLib"
	language "C++"

	removelinks { "minko-framework" }

	if premake.tools.gcc.tools.emscripten then
	configuration { "html5" }
		local emcc = premake.tools.gcc.tools.emscripten.cc

		postbuildcommands {
			emcc .. ' ${TARGET} -o ${TARGETDIR}/' .. name .. '.js -O2 --closure 1 -s DISABLE_EXCEPTION_CATCHING=0 -s TOTAL_MEMORY=268435456 -s EXPORTED_FUNCTIONS="[\'minkoWorkerEntryPoint\']" || ' .. minko.action.fail()
		}
	end
	
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
