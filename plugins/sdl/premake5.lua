newoption {
	trigger			= "with-sdl",
	description		= "Enable the Minko SDL plugin."
}

PROJECT_NAME = path.getname(os.getcwd())

minko.project.library("minko-plugin-" .. PROJECT_NAME)

	kind "StaticLib"
	language "C++"

	files {
		"lib/**.hpp",
		"lib/**.h",
		"lib/**.cpp",
		"lib/**.c",
		"include/**.hpp",
		"src/**.cpp"
	}

	includedirs { "include" }
	
	minko.plugin.import("angle")
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
