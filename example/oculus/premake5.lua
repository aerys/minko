PROJECT_NAME = path.getname(os.getcwd())

if not minko.platform.supports { "windows32", "windows64", "linux32", "linux64", "html5" } then
	return
end

minko.project.application("minko-example-" .. PROJECT_NAME)

	removeplatforms { "ios", "android", "osx64" }

	files {
		"src/**.cpp",
		"src/**.hpp",
		"asset/**"
	}

	includedirs { "src" }

	-- plugins
	minko.plugin.enable("oculus")
	minko.plugin.enable("jpeg")
	minko.plugin.enable("sdl")
