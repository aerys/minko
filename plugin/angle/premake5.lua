-- Note: Premake5 crashes if there is no valid platform for a project.
-- Since this plugin is only supported on Windows, we must not define it
-- if the host platform is not Windows.

if not minko.platform.supports { "windows32", "windows64" } then
	return
end

PROJECT_NAME = path.getname(os.getcwd())

minko.project.library("minko-plugin-" .. PROJECT_NAME)

	files {
		"lib/**.hpp",
		"lib/**.h",
		"lib/**.cpp",
		"lib/**.c",
		"include/**.hpp",
		"src/**.cpp"
	}

	includedirs {
		"include",
		minko.plugin.path("sdl") .. "/include",
		minko.plugin.path("sdl") .. "/lib/sdl/include"
	}
