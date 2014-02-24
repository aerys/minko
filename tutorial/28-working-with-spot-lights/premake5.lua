PROJECT_NAME = path.getname(os.getcwd())

minko.project.application("minko-tutorial-" .. PROJECT_NAME)

	language "c++"
	kind "ConsoleApp"

	files {
		"src/**.cpp",
		"src/**.hpp"
	}
	
	includedirs { "src" }

	-- plugins
	minko.plugin.enable("sdl")