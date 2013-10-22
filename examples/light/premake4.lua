project "minko-example-light"
	kind "ConsoleApp"
	language "C++"
	files {
		"src/**.hpp",
		"src/**.cpp"
	}
	includedirs {
		"src",
		"../../deps/all/include"
	}
	
	-- minko-webgl
	-- ugly, but couldn't find a better solution to maintain linking order.
	--if _OPTIONS["platform"] == "emscripten" then
	--	links { "minko-webgl" }
	--end

	-- minko-png
	links { "minko-png" }
	includedirs { "../../plugins/png/src" }
	-- minko-framework
	links { "minko-framework" }
	includedirs { "../../framework/src" }
	if not _OPTIONS["no-glsl-optimizer"] then
		links { "glsl-optimizer" }
	end

	configuration { "debug"}
		defines { "DEBUG" }
		flags { "Symbols" }
		targetdir "bin/debug"

	configuration { "release" }
		defines { "NDEBUG" }
		flags { "OptimizeSpeed" }
		targetdir "bin/release"

	-- linux
	configuration { "linux" }
		links { "GL", "GLU", "SDL2", "m", "Xrandr", "Xxf86vm", "Xi", "rt", "X11", "pthread" }
		libdirs { "../../deps/lin/lib" }
		includedirs { "../../deps/lin/include" }
		buildoptions { "-std=c++11" }
		postbuildcommands {
			'cp -r ../../framework/effect ${TARGETDIR}',
			'cp -r asset/* ${TARGETDIR}'
		}

	-- windows
	configuration { "windows", "x32" }
		links { "SDL2", "SDL2main" }
		libdirs { "../../deps/win/lib" }
		includedirs { "../../deps/win/include" }
		if _OPTIONS[ "directX" ] then
			defines { "MINKO_ANGLE" }
			links { "libGLESv2", "libEGL" }
		else
			links { "OpenGL32", "glew32" }
		end
		postbuildcommands {
			-- copy framework effects
			'xcopy /y /e /i ..\\..\\framework\\effect\\* "$(TargetDir)effect"',
			-- copy assets
			'xcopy /y /e /i asset\\* "$(TargetDir)"',
			-- copy dlls
			'for /r %%x in (..\\..\\deps\\win\\lib\\*.dll) do xcopy /y /e /i "%%x" "$(TargetDir)"'
		}

	-- macos
	configuration { "macosx" }
		buildoptions { "-std=c++11", "-stdlib=libc++" }
		linkoptions { "-std=c++11", "-stdlib=libc++" }
		links {
			"m",
			"SDL2.framework",
			"Cocoa.framework",
			"OpenGL.framework",
			"IOKit.framework"
		}
		libdirs {
			"../../deps/mac/lib"
		}
		includedirs {
			"../../deps/mac/include"
		}
		postbuildcommands {
			'cp -r ../../framework/effect .',
			'cp -r asset/* .'
		}

	-- emscripten
	configuration { "emscripten" }
		flags { "Optimize" }

		-- minko-webgl
		links { "minko-webgl" }
		includedirs { "../../plugins/webgl/src" }

		includedirs { "../../deps/emscripten/include" }
		defines { "HAVE_M_PI" }
		buildoptions { "-std=c++11" }
		
		local bin = "bin/release/" .. project().name
		postbuildcommands {
			'cp ' .. bin .. ' ' .. bin .. '.bc',
			'cp -r asset/* bin/release/',
			'cd bin/release && emcc ' .. project().name .. '.bc -o ' .. project().name .. '.html -O2 -s ASM_JS=1 -s TOTAL_MEMORY=268435456 --preload-file effect --preload-file texture --compression /home/vagrant/src/emscripten/third_party/lzma.js/lzma-native,/home/vagrant/src/emscripten/third_party/lzma.js/lzma-decoder.js,LZMA.decompress',
		}
