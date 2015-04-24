PROJECT_NAME = path.getname(os.getcwd())

minko.project.application("minko-example-" .. PROJECT_NAME)

	files {
		"src/**.cpp",
		"src/**.hpp"
	}

	minko.package.assets {
		['**.effect'] = { 'embed' },
		['**.glsl'] = { 'embed' }
	}

	includedirs { "src" }

	-- plugins
	minko.plugin.enable("sdl")
