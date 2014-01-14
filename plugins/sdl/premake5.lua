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
		"src/**.cpp",
		"src/**.hpp"
	}

	includedirs { "include" }
	
	minko.plugin.import("angle")
	minko.plugin.enable("webgl")

	configuration { "windows", "x32" }
		includedirs { minko.sdk.path("plugins/sdl/lib/sdl/windows32/include") }

	configuration { "windows", "x64" }
		includedirs { minko.sdk.path("plugins/sdl/lib/sdl/windows64/include") }
		
	configuration { "linux", "x32" }
		includedirs { minko.sdk.path("plugins/sdl/lib/sdl/linux32/include") }

	configuration { "linux", "x64" }
		includedirs { minko.sdk.path("plugins/sdl/lib/sdl/linux64/include") }

	configuration { "macosx" }
		includedirs { minko.sdk.path("plugins/sdl/lib/sdl/linux64/include") } -- FIXME

	configuration { "html5" }
		if EMSCRIPTEN_HOME then
			includedirs { EMSCRIPTEN_HOME .. "/system/include" }
		end
