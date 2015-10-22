PROJECT_NAME = path.getname(os.getcwd())

minko.project.application("minko-example-" .. PROJECT_NAME)

	files {
		"src/**.hpp",
		"src/**.cpp"
	}

	includedirs { "src"	}

	minko.plugin.enable("sdl")
	minko.plugin.enable("png")

	configuration { "html5" }
		minko.package.assets {
			['**.effect'] = { 'embed' },
			['**.glsl'] = { 'embed' }
		}

	configuration { "not html5" }
		minko.package.assets {
			['**.effect'] = { 'copy' },
			['**.glsl'] = { 'copy' }
		}
