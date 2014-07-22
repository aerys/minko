-- Note: Premake5 crashes if there is no valid platform for a project.
-- Since this plugin is only supported on Linux, we must not define it
-- if the host platform is not Linux.

if not minko.platform.supports { "linux32", "linux64" } then
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
		"lib/osmesa/include"
	}
