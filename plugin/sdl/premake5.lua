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

	configuration { "android" }
		minko.plugin.enable { "android" }

	configuration { }
	if _OPTIONS['with-offscreen'] then
		minko.plugin.enable("offscreen")
	end

	configuration { "html5" }
		removeincludedirs { "lib/sdl/include" }
		includedirs { "SDL" }
		files { "src/minko/SDLWebGLBackend.cpp" }

	configuration { "ios" }
		buildoptions { "-x objective-c++" }

	-- Audio only works for HTML5, Windows and Android
	configuration { "linux32 or linux64 or osx64 or ios" }
		excludes {
			"include/minko/audio/**.hpp",
			"src/minko/audio/**.cpp"
		}

	configuration { }

	if _OPTIONS['with-offscreen'] then
		excludes {
			"include/minko/audio/**.hpp",
			"src/minko/audio/**.cpp"
		}
	end
