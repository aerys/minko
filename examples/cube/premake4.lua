minko.project.application "example-cube"
	kind "ConsoleApp"
	language "C++"
	files { "src/**.hpp", "src/**.cpp" }
	includedirs { "src"	}
	
	minko.plugin.enable("sdl")
	minko.plugin.enable("png")
	
	minko.plugin.import("angle")
	
	-- emscripten
	configuration { "emscripten" }
		postbuildcommands {
			'cd ${TARGETDIR} ; cp ' .. project().name .. ' ' .. project().name .. '.bc',
			'cp -r asset/* ${TARGETDIR} || :',
			'cd ${TARGETDIR} ; emcc ' .. project().name .. '.bc -o ' .. project().name .. '.html -O2 -s ASM_JS=0 -s TOTAL_MEMORY=1073741824 --preload-file effect --preload-file texture'
		}
