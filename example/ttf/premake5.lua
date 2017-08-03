PROJECT_NAME = path.getname(os.getcwd())

minko.project.application("minko-example-" .. PROJECT_NAME)

	files {
		"src/**.cpp",
		"asset/**"
	}

	includedirs {
        "src"
    }

    links {
        "freetype"
    }

	-- plugins
    minko.plugin.enable("sdl")
	minko.plugin.enable("ttf")

	configuration { "html5" }
		minko.package.assets {
			['**.effect'] = { 'embed' },
            ['**.glsl'] = { 'embed' },
			['**.ttf'] = { 'embed' }
		}
	configuration { "not html5" }
		minko.package.assets {
			['**.effect'] = { 'copy' },
            ['**.glsl'] = { 'copy' },
			['**.ttf'] = { 'copy' }
		}
