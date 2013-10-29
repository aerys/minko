minko.project.application "minko-example-sponza"
	kind "ConsoleApp"
	language "C++"
	files {
		"**.hpp",
		"**.cpp"
	}
	includedirs { "src" }

	-- plugins 
	minko.plugin.enable("mk")
	minko.plugin.enable("bullet")
	minko.plugin.enable("png")
	minko.plugin.enable("jpeg")
	minko.plugin.enable("particles")
	minko.plugin.enable("sdl")
	minko.plugin.enable("oculus")
	
	minko.plugin.import("angle")

	-- configurations
	configuration { "debug"}
		defines { "DEBUG" }
		flags { "Symbols" }
		targetdir "bin/debug"

	configuration { "release" }
		defines { "NDEBUG" }
		flags { "OptimizeSpeed" }
		targetdir "bin/release"

	-- emscripten
	configuration { "emscripten" }
		local bin = "bin/release/" .. project().name
		postbuildcommands {
			'cp ' .. bin .. ' ' .. bin .. '.bc',
			'cp -r asset/* bin/release/ || :',
			'rm bin/release/model/Sponza_lite.mks',
			'cd bin/release && emcc ' .. project().name .. '.bc -o ' .. project().name .. '.html -O2 -s ASM_JS=1 -s TOTAL_MEMORY=268435456 --preload-file effect --preload-file texture --preload-file model --compression /home/vagrant/src/emscripten/third_party/lzma.js/lzma-native,/home/vagrant/src/emscripten/third_party/lzma.js/lzma-decoder.js,LZMA.decompress',
--			'emcc ' .. bin .. '.bc -o ' .. bin .. '.js -O2 -s ASM_JS=1 -s TOTAL_MEMORY=268435456 --preload-file effect --preload-file texture --preload-file model'
		}
