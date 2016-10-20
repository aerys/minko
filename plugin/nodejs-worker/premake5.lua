PROJECT_NAME = path.getname(os.getcwd())

if not minko.platform.supports { "android" } then
	return
end

minko.project.worker("minko-plugin-" .. PROJECT_NAME)

	removeplatforms { "html5", "windows32", "windows64", "linux32", "linux64", "osx64", "ios" }

	minko.plugin.enable("sdl")

	files {
		"include/**.hpp",
		"src/**.cpp"
	}

	includedirs {
		"include",
		"lib/node/deps/uv/include",
		"lib/node/deps/v8/include",
		"lib/node/src"
	}

	prebuildcommands {
		"bash script/build_nodejs.sh"
	}

	postbuildcommands {
		"bash script/merge_static_libraries.sh"
	}
