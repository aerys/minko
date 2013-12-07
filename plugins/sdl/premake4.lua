newoption {
	trigger			= "with-sdl",
	description		= "Enable the Minko SDL plugin."
}

PROJECT_NAME = path.getname(os.getcwd())

minko.project.library("minko-plugin-" .. PROJECT_NAME)
	kind "StaticLib"
	language "C++"
	files { "**.hpp", "**.h", "**.cpp", "**.c" }
	includedirs { "include", "src"	}
	
	minko.plugin.import("angle")

	-- linux
	configuration { "linux" }
		includedirs { "lib/SDL2/include" }

	-- windows
	configuration { "windows" }
		includedirs { "lib/SDL2/include" }
		
	-- macos
	configuration { "macosx" }
		includedirs { "lib/SDL2/include" }

	-- emscripten
	configuration { "emscripten" }
		defines { "EMSCRIPTEN" }
		flags { "Optimize" }
		includedirs { "lib/emscripten/SDL/include" }
