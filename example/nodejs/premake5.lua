PROJECT_NAME = path.getname(os.getcwd())

if not minko.platform.supports { "android" } then
	return
end

minko.project.application("minko-example-" .. PROJECT_NAME)

	removeplatforms { "html5", "windows32", "windows64", "linux32", "linux64", "osx64", "ios" }

	files {
		"src/**.cpp",
		"src/**.hpp",
		"asset/**"
	}

	includedirs {
		"src"
	}

	-- plugin
	minko.plugin.enable("sdl")
	minko.plugin.enable("nodejs-worker")
	minko.plugin.enable("http-loader")
	minko.plugin.enable("html-overlay")

	minko.package.assets {
		['**'] = { 'copy', 'embed' }
	}

	removelinks {
		"minko-plugin-zlib"
	}
