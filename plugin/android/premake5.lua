PROJECT_NAME = path.getname(os.getcwd())

if not minko.platform.supports { "android" } then
	return
end

minko.project.library("minko-plugin-" .. PROJECT_NAME)

	removeplatforms { "windows32", "windows64", "linux32", "linux64", "osx64", "html5", "ios" }

	files {
		"src/**.hpp",
		"src/**.h",
		"src/**.cpp",
		"src/**.c"
	}

	includedirs {
		"include",
		minko.plugin.path("sdl") .. "/lib/sdl/include" -- hard coded but minko-plugin-sdl is always enabled here.
	}
