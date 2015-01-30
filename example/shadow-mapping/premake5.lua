PROJECT_NAME = path.getname(os.getcwd())

minko.project.application("minko-example-" .. PROJECT_NAME)

	files { "src/**.cpp", "src/**.hpp", "asset/**" }
	includedirs { "src" }

	-- plugin
	minko.plugin.enable("sdl")
	minko.plugin.enable("debug")
