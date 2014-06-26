if minko.platform.supports("linux") then

PROJECT_NAME = path.getname(os.getcwd())

minko.project.application("minko-example-" .. PROJECT_NAME)

	removeplatforms { "android", "ios", "windows32", "windows64", "osx64", "html5" }

	files {
		"src/**.hpp",
		"src/**.cpp",
		"asset/**"
	}

	includedirs { "src" }

	minko.plugin.enable("png");
	minko.plugin.enable("offscreen");

end
