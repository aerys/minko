-- A project defines one build target
project "minko-example-sponza"
	kind "ConsoleApp"
	language "C++"
	files {
		"**.hpp",
		"**.h",
		"**.cpp"
	}
	includedirs {
		"src",
		"../../deps/all/include"
	}

	-- ugly, but couldn't find a better solution to maintain linking order.
	if _OPTIONS["platform"] == "emscripten" then
		links { "minko-webgl" }
	end
	
	-- minko-mk
	includedirs { "../../plugins/mk/src" }
	links { "minko-mk" }
	-- minko-bullet
	includedirs { "../../plugins/bullet/src" }
	links { "minko-bullet" }
	-- minko-png
	includedirs { "../../plugins/png/src" }
	links { "minko-png" }
	-- minko-jpeg
	includedirs { "../../plugins/jpeg/src" }
	links { "minko-jpeg" }
	-- minko-particles
	includedirs { "../../plugins/particles/src" }
	links { "minko-particles" }
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
		buildoptions { "-std=c++11" }
		links {
			"GL",
			"GLU",
			"SDL2",
			"m",
			"Xrandr",
			"Xxf86vm",
			"Xi",
			"rt",
			"X11",
			"pthread"
		}
		libdirs {
			"../../deps/lin/lib"
		}
		includedirs {
			"../../deps/lin/include"
		}
		postbuildcommands {
			'cp -r ../../framework/effect ${TARGETDIR}',
			'cp -r asset/* ${TARGETDIR}'
		}

	-- windows
	configuration { "windows", "x32" }
		buildoptions { "-std=c++11" }
		links {
			"OpenGL32",
			"SDL2",
			"SDL2main",
			"glew32"
		}
		libdirs {
			"../../deps/win/lib"
		}
		includedirs {
			"../../deps/win/include"
		}
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
			"SDL2",
			"OpenGL.framework",
			"GLUT.framework",
			"Cocoa.framework",
			"IOKit.framework"
		}
		libdirs {
			"../../deps/mac/lib"
		}
		includedirs {
			"../../deps/mac/include"
		}
		postbuildcommands {
			'cp -r ../../framework/effect/* effect/',
			'cp -r asset/* .'
		}

	-- emscripten
	configuration { "emscripten" }
		flags { "Optimize" }
		buildoptions { "-std=c++11" }
		-- webgl plugin
		includedirs { "../../plugins/webgl/src" }

		local bin = "bin/release/" .. project().name
		postbuildcommands {
			'cp ' .. bin .. ' ' .. bin .. '.bc',
			'cp -r effect bin/release; cp -r texture bin/release; cp -r model bin/release',
			'rm bin/release/model/Sponza_lite.mks',
			'cd bin/release && emcc ' .. project().name .. '.bc -o ' .. project().name .. '.html -O2 -s ASM_JS=1 -s TOTAL_MEMORY=268435456 --preload-file effect --preload-file texture --preload-file model --compression /home/vagrant/src/emscripten/third_party/lzma.js/lzma-native,/home/vagrant/src/emscripten/third_party/lzma.js/lzma-decoder.js,LZMA.decompress',
--			'emcc ' .. bin .. '.bc -o ' .. bin .. '.js -O2 -s ASM_JS=1 -s TOTAL_MEMORY=268435456 --preload-file effect --preload-file texture --preload-file model'
		}
