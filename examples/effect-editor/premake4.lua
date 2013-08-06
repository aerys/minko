project "minko-effect-editor"
	kind "ConsoleApp"
	language "C++"
	files {
		"**.hpp",
		"**.h",
		"**.cpp"
	}
	includedirs {
		"src",
		"lib",
		"../../deps/all/include"
	}

	-- QT5
	if not _OPTIONS["os"] == "windows" then
		error("ERROR\tcurrently, premake4.lua files only handle windows for QT5 support, sorry.");
	end
	 
	QT_DIR = os.getenv("QT_DIR")
	if not QT_DIR then
		error "ERROR\tenvironment variable QT_DIR is not specified."
	end
	
	includedirs { QT_DIR .. "/include" }
	libdirs { QT_DIR .. "/include" }
	links { "Qt5Core", "Qt5OpenGL", "Qt5Gui" }
	
	local qtMoc = QT_DIR .. "/bin/moc.exe"
	local qtRcc = QT_DIR .. "/bin/rcc.exe"
	if not os.isfile(qtMoc) then
		error("ERROR\tmoc is not found at '" .. qtMoc .. "'")
	end
	if not os.isfile(qtRcc) then
		error("ERROR\trcc is not found at '" .. qtRcc .. "'")
	end
	
	function generateMOC(files)
		local mocFiles = os.matchfiles(files)
		for _, file in pairs(mocFiles) do
			local extension		= path.getextension(file)
			local outputFile	= "moc/moc_" .. path.getbasename(file) .. ".cpp"
			if extension == ".cpp" then
				outputFile = "moc/" .. path.getbasename(file) .. ".moc"
			end
			
			print("building " .. outputFile)
			os.execute(qtMoc .. " " .. file .. " -o " .. outputFile)
		end
	end
	
	generateMOC("src/openglwindow.h")
	
	files { "moc/*.cpp" }
	-- end of QT5
	
	
	-- ugly, but couldn't find a better solution to maintain linking order.
	if _OPTIONS["platform"] == "emscripten" then
		links { "minko-webgl" }
	end
	
	-- minko-png
	includedirs { "../../plugins/png/src" }
	links { "minko-png" }
	-- minko-jpeg
	includedirs { "../../plugins/jpeg/src" }
	links { "minko-jpeg" }
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
			"glfw3",
			"m",
			"Xrandr",
			"Xxf86vm",
			"Xi",
			"rt"
		}
		libdirs {
			"../../deps/lin/lib"
		}
		includedirs {
			"../../deps/lin/include"
		}
		--postbuildcommands {
			--'cp -r asset/* .',
			--'cp -r ../../framework/effect/* effect/'
		--}

	-- windows
	configuration { "windows", "x32" }
		buildoptions { "-std=c++11" }
		links {
			"OpenGL32",
			"glfw3dll",
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
			--'xcopy /y /e /i ..\\..\\framework\\effect\\* "$(TargetDir)effect"',
			-- copy assets
			--'xcopy /y /e /i asset\\* "$(TargetDir)"',
			-- copy dlls
			'for /r %%x in (..\\..\\deps\\win\\lib\\*.dll) do xcopy /y /e /i "%%x" "$(TargetDir)"'
		}

	-- macos
	configuration { "macosx" }
		buildoptions { "-std=c++11", "-stdlib=libc++" }
		linkoptions { "-std=c++11", "-stdlib=libc++" }
		links {
			"m",
			"glfw3",
			"OpenGL.framework",
			"GLUT.framework",
			"Cocoa.framework",
			"IOKit.framework"
		}
		libdirs {
			"../../../../deps/mac/lib"
		}
		includedirs {
			"../../deps/mac/include"
		}
		--postbuildcommands {
		--	'cp -r asset/* .',
		--	'cp -r ../../framework/effect/* effect/'
		--}

	-- emscripten
	configuration { "emscripten" }
		flags { "Optimize" }
		buildoptions { "-std=c++11" }
		-- webgl plugin
		includedirs { "../../plugins/webgl/src" }

		local bin = "bin/release/" .. project().name
		postbuildcommands {
			'cp ' .. bin .. ' ' .. bin .. '.bc',
			'emcc ' .. bin .. '.bc -o ' .. bin .. '.html -O1 -s ASM_JS=1 -s TOTAL_MEMORY=1073741824 --preload-file assets/effect --preload-file assets/texture'
		}
