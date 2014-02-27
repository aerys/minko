PROJECT_NAME = path.getname(os.getcwd())

minko.project.application("minko-example-" .. PROJECT_NAME)

	removeplatforms { "html5" }

	language "c++"
	kind "ConsoleApp"

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
