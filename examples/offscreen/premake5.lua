PROJECT_NAME = path.getname(os.getcwd())

minko.project.application("minko-example-" .. PROJECT_NAME)

	removeplatforms { "android", "ios", "win", "osx", "html5" }

	kind "ConsoleApp"
	language "C++"

	files {
		"src/**.hpp",
		"src/**.cpp",
		"asset/**"
	}

	includedirs { "src" }

	minko.plugin.enable("png");

	-- linux
	configuration { "linux" }
		links { "OSMesa" }
