-- sdl plugin
if minko.plugin.enabled("sdl") then

	links { "plugin-sdl" }
	defines { "MINKO_PLUGIN_SDL" }

	configuration { "windows" }
		links { "SDL2", "SDL2main" }
		libdirs { minko.sdk.path("plugins/sdl/lib/win/SDL/lib") }
		includedirs {
			minko.sdk.path("plugins/sdl/lib/SDL2/include"),
			minko.sdk.path("plugins/sdl/include")
		}
		postbuildcommands {
			minko.vs.getdllscopycommand(minko.sdk.path("plugins/sdl/lib/win/SDL/lib"))
		}
		
	configuration { "linux" }
		links { "SDL2" }
		includedirs {
			minko.sdk.path("plugins/sdl/lib/SDL2/include"),
			minko.sdk.path("plugins/sdl/include")
		}

	configuration { "macosx" }
		links { "SDL2.framework" }
		includedirs {
			minko.sdk.path("plugins/sdl/lib/SDL2/include"),
			minko.sdk.path("plugins/sdl/include")
		}

	configuration { "emscripten" }
		defines { "HAVE_M_PI" }
		includedirs {
			minko.sdk.path("plugins/sdl/lib/emscripten/SDL/include"),
			minko.sdk.path("plugins/sdl/include")
		}
end
