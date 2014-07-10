-- Note: Premake5 crashes if there is no valid platform for a project.
-- Since this plugin is only supported on Linux, we must not define it
-- if the host platform is not Linux.

if minko.platform.supports("html5") then

PROJECT_NAME = path.getname(os.getcwd())

minko.project.library("minko-plugin-" .. PROJECT_NAME)

	removeplatforms { "windows32", "windows64", "linux32", "linux64", "osx64", "ios", "android" }

	files {
		"lib/**.hpp",
		"lib/**.h",
		"lib/**.cpp",
		"lib/**.c",
		"include/**.hpp",
		"src/**.cpp"
	}

	includedirs { "include" }

end