minko.project.application "minko-example-light"
	kind "ConsoleApp"
	language "C++"
	files { "src/**.hpp", "src/**.cpp" }
	includedirs { "src" }
	
	-- plugins
	minko.plugin.enable("sdl")
	minko.plugin.enable("png")
	
	minko.plugin.import("angle")
	
	-- debug configuration
	configuration { "debug"}
		defines { "DEBUG" }
		flags { "Symbols" }
		targetdir "bin/debug"

	-- release configuration
	configuration { "release" }
		defines { "NDEBUG" }
		flags { "OptimizeSpeed" }
		targetdir "bin/release"
	
	-- emscripten configuration
	configuration { "emscripten" }
		local bin = "bin/release/" .. project().name
		postbuildcommands {
			'cp ' .. bin .. ' ' .. bin .. '.bc',
			'cp -r asset/* bin/release/ || :',
			'cd bin/release && emcc ' .. project().name .. '.bc -o index.html -O2 -s ASM_JS=1 -s TOTAL_MEMORY=268435456 --preload-file effect --preload-file texture --compression /home/vagrant/src/emscripten/third_party/lzma.js/lzma-native,/home/vagrant/src/emscripten/third_party/lzma.js/lzma-decoder.js,LZMA.decompress',
		}
