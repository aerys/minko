newoption {
	trigger			= "rebuild-sdl",
	description		= "Rebuilds SDL for the specified platform."
}

if _OPTIONS["rebuild-sdl"] then
	include "lib/sdl"
end

PROJECT_NAME = path.getname(os.getcwd())

minko.project.library("minko-plugin-" .. PROJECT_NAME)

	files {
		"include/**.hpp",
		"src/**.cpp",
		"src/**.hpp"
	}
	excludes {
		"src/minko/SDLWebGLBackend.cpp"
	}

	includedirs {
		"include",
		"lib/sdl/include",
		"src"
	}

	configuration { }

	if minko.plugin.requested("offscreen") then
		minko.plugin.enable("offscreen")
	end

	configuration { "html5" }
		removeincludedirs { "lib/sdl/include" }
		includedirs { "SDL" }
		files { "src/minko/SDLWebGLBackend.cpp" }

	configuration { "ios" }
		buildoptions { "-x objective-c++" }

	configuration { "not android" }
		excludes {
			"include/minko/file/APKProtocol.hpp",
			"src/minko/file/APKProtocol.cpp",
			"include/minko/file/APKProtocolWorker.hpp",
			"src/minko/file/APKProtocolWorker.cpp"
		}

	configuration { }

	if minko.plugin.requested("offscreen") then
		defines {
			"\"SDL_AUDIO_ENABLED=0\""
		}
	end
