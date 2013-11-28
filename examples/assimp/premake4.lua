minko.project.application "example-assimp"
	kind "ConsoleApp"
	language "C++"
	files { "src/**.hpp", "src/**.cpp" }
	includedirs { "src"	}
	
	minko.plugin.enable("sdl")
	minko.plugin.enable("assimp")
	
	minko.plugin.import("angle")
	
	-- emscripten
	configuration { "emscripten" }
		local bin = "bin/release/" .. project().name
		postbuildcommands {
			'cp ' .. bin .. ' ' .. bin .. '.bc',
			'emcc ' .. bin .. '.bc -o ' .. bin .. '.js -O1 -s ASM_JS=1 -s TOTAL_MEMORY=1073741824 --preload-dir effect --preload-dir texture',
			'emcc ' .. bin .. '.bc -o index.html -O1 -s ASM_JS=1 -s TOTAL_MEMORY=1073741824 --preload-dir effect --preload-dir texture'
		}
