PROJECT_NAME = path.getname(os.getcwd())

minko.project.application("minko-example-" .. PROJECT_NAME)

	language "c++"
	kind "ConsoleApp"

	files {
		"src/**.cpp",
		"src/**.hpp",
		"asset/**"
	}
	
	includedirs { "src" }

	-- plugins
	minko.plugin.enable("sdl")
	minko.plugin.enable("png")
	minko.plugin.enable("http")

	-- fixme: remove
	minko.worker.enable("http")