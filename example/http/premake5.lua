PROJECT_NAME = path.getname(os.getcwd())

minko.project.application("minko-example-" .. PROJECT_NAME)

	removeplatforms { 'android' }

	files {
		"src/**.cpp",
		"src/**.hpp",
		"asset/**"
	}

	includedirs { "src" }

	-- plugins
	minko.plugin.enable("sdl")
	minko.plugin.enable("png")
	minko.plugin.enable("http-loader")

	configuration { "not html5" }
		-- fixme: should not be visible (but plugin loading priority is broken)
		minko.plugin.enable("http-worker")
