PROJECT_NAME = path.getname(os.getcwd())

minko.project.application("minko-example-" .. PROJECT_NAME)

	kind "ConsoleApp"
	language "C++"

	files {
		"src/**.hpp",
		"src/**.cpp",
		"asset/**"
	}
	
	includedirs { "src" }

	-- plugins 
	minko.plugin.enable("mk")
	minko.plugin.enable("bullet")
	minko.plugin.enable("png")
	minko.plugin.enable("jpeg")
	minko.plugin.enable("particles")
	minko.plugin.enable("sdl")
	
	minko.plugin.import("angle")

	configuration { "windows or macosx"}
		minko.plugin.enable("oculus")

