PROJECT_NAME = path.getname(os.getcwd())

minko.project.application("minko-example-" .. PROJECT_NAME)

	kind "ConsoleApp"

	language "C++"
	
	files {
		"src/**.hpp",
		"src/**.cpp"
	}

	includedirs { "src"	}
	
	minko.plugin.enable("sdl")
	minko.plugin.enable("png")
	
	minko.plugin.import("angle")
