PROJECT_NAME = path.getname(os.getcwd())

if not minko.platform.supports { "windows32", "windows64", "osx64", "android", "ios", "html5" } then
	return
end

minko.project.application("minko-example-" .. PROJECT_NAME)

	removeplatforms { "linux32", "linux64" }

	files {
		"src/**.cpp",
		"src/**.hpp",
		"asset/**"
	}

	minko.package.assets {
		['**.effect'] = { 'embed' },
		['**.glsl'] = { 'embed' },
		['*.ogg'] = { 'embed' }
	}

	includedirs { "src" }

	-- plugins
	minko.plugin.enable("sdl")
	minko.plugin.enable("png")
