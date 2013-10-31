project "example-cube-offscreen"
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
	if _OPTIONS["platform"] == "emscripten" then
		links { "minko-webgl" }
	end

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
		links {
                   "GL",
                   "GLU",
                   "m",
                   --"Xrandr",
                   --"Xxf86vm",
                   --"Xi",
                   --"rt",
                   "OSMesa"
                }
		libdirs { "../../deps/lin/lib" }
		includedirs { "../../deps/lin/include" }
		buildoptions { "-std=c++11" }
		postbuildcommands {
			'cp -r ../../framework/effect ${TARGETDIR} || :',
			'cp -r asset/* ${TARGETDIR} || :'
		}

	-- windows
	configuration { "windows", "x32" }
		links { "OpenGL32", "glew32" }
		libdirs { "../../deps/win/lib" }
		includedirs { "../../deps/win/include" }
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
			'cp -r ../../framework/effect . || :',
			'cp -r asset/* . || :'
		}

	-- emscripten
	configuration { "emscripten" }
		flags { "Optimize" }
		links { "minko-webgl" }
		includedirs { "../../plugins/webgl/src" }
		buildoptions { "-std=c++11" }
		local bin = "bin/release/" .. project().name
		postbuildcommands {
			'cp ' .. bin .. ' ' .. bin .. '.bc',
			'emcc ' .. bin .. '.bc -o ' .. bin .. '.js -O1 -s ASM_JS=1 -s TOTAL_MEMORY=1073741824 --preload-dir effect --preload-dir texture',
			'emcc ' .. bin .. '.bc -o ' .. bin .. '.html -O1 -s ASM_JS=1 -s TOTAL_MEMORY=1073741824 --preload-dir effect --preload-dir texture'
		}
