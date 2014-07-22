PROJECT_NAME = path.getname(os.getcwd())

minko.project.application("minko-example-" .. PROJECT_NAME)

	files {
		"src/**.hpp",
		"src/**.cpp"
	}

	includedirs { "src"	}

	minko.plugin.enable("sdl")