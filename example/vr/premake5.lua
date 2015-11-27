PROJECT_NAME = path.getname(os.getcwd())

minko.project.application("minko-example-" .. PROJECT_NAME)

	removeplatforms {"osx64" }

	files {
		"src/**.cpp",
		"src/**.hpp",
		"asset/**"
	}

	includedirs { "src" }

	-- plugins
	minko.plugin.enable("vr")
	minko.plugin.enable("jpeg")
	minko.plugin.enable("sdl")

	configuration { "html5" }
		minko.package.assets {
			['**.effect'] = { 'embed' },
			['**.glsl'] = { 'embed' },
			['**.jpg'] = { 'embed' }
		}
	configuration { "not html5" }
		minko.package.assets {
			['**.effect'] = { 'copy' },
			['**.glsl'] = { 'copy' },
			['**.jpg'] = { 'copy' }
		}