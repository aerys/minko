-- sdl plugin
if minko.plugin.enabled("sdl") then

	defines { "MINKO_SDL" }

	configuration { "windows" }
		links { "minko-sdl", "SDL2", "SDL2main" }
		libdirs { minko.sdk.path("plugins/sdl/lib/win/SDL/lib") }
		includedirs {
			minko.sdk.path("plugins/sdl/lib/SDL2/include"),
			minko.sdk.path("plugins/sdl/src")
		}
		postbuildcommands {
			minko.vs.getdllscopycommand(minko.sdk.path("plugins/sdl/lib/win/SDL/lib"))
		}
		
	configuration { "linux" }
		links { "minko-sdl", "SDL2" }
		includedirs {
			minko.sdk.path("plugins/sdl/lib/SDL2/include"),
			minko.sdk.path("plugins/sdl/src")
		}

	configuration { "macosx" }
		links { "minko-sdl", "SDL2.framework" }
		includedirs {
			minko.sdk.path("plugins/sdl/lib/SDL2/include"),
			minko.sdk.path("plugins/sdl/src")
		}

	configuration { "emscripten" }
		links { "minko-sdl" }
		defines { "HAVE_M_PI" }
		includedirs {
			minko.sdk.path("plugins/sdl/lib/emscripten/SDL/include"),
			minko.sdk.path("plugins/sdl/src")
		}
end
