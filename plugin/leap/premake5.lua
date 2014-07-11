PROJECT_NAME = path.getname(os.getcwd())

if not minko.platform.supports { "windows32", "windows64", "linux32", "linux64", "osx64" } then
	return
end

minko.project.library("minko-plugin-" .. PROJECT_NAME)

	removeplatforms { "html5", "ios", "android" }

	files {
		"src/**.hpp",
		"src/**.h",
		"src/**.cpp",
		"src/**.c",
		"include/**.hpp"
	}

	includedirs {
		"include",
		"src",
		"lib/leap/include"
	}
