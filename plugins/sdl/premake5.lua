newoption {
	trigger			= "with-sdl",
	description		= "Enable the Minko SDL plugin."
}

PROJECT_NAME = path.getname(os.getcwd())

minko.project.library("minko-plugin-" .. PROJECT_NAME)
	kind "StaticLib"
	language "C++"
	files { "**.hpp", "**.h", "**.cpp", "**.c", "include/**.hpp" }
	includedirs { "include", "src"	}
	
	minko.plugin.import("angle")
	minko.plugin.import("webgl")
	minko.plugin.enable("webgl")

	-- linux
	configuration { "linux" }
		includedirs { "lib/SDL2/include" }

	-- windows
	configuration { "win" }
		includedirs { "lib/SDL2/include" }
		
	-- macos
	configuration { "osx" }
		includedirs { "lib/SDL2/include" }

	-- emscripten
	configuration { "html5" }
		includedirs { "lib/emscripten/SDL/include" }
