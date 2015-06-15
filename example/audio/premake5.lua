PROJECT_NAME = path.getname(os.getcwd())

if not minko.platform.supports { "windows32", "windows64", "android", "html5" } then
	return
end

minko.project.application("minko-example-" .. PROJECT_NAME)

	removeplatforms { "ios", "linux32", "linux64", "osx64" }

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