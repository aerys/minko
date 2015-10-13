PROJECT_NAME = path.getname(os.getcwd())

minko.project.application("minko-example-" .. PROJECT_NAME)

	files {
		"src/**.cpp",
		"src/**.hpp",
		"asset/**"
	}

	includedirs { "src" }

	-- plugins
	minko.plugin.enable("sdl")
	minko.plugin.enable("png")
	minko.plugin.enable("fx")
    minko.plugin.enable("assimp")
    minko.plugin.enable("jpeg")

	configuration { "html5" }
		minko.package.assets {
			['**.effect'] = { 'embed' },
			['**.glsl'] = { 'embed' },
			['**.png'] = { 'embed' },
            ['*.dae'] = { 'embed' },
            ['*.jpg'] = { 'embed' }
		}
	configuration { "not html5" }
		minko.package.assets {
			['**.effect'] = { 'copy' },
			['**.glsl'] = { 'copy' },
			['**.png'] = { 'copy' },
            ['*.dae'] = { 'copy' },
            ['*.jpg'] = { 'copy' }
		}
