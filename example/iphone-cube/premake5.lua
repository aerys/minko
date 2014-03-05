PROJECT_NAME = path.getname(os.getcwd())

minko.project.application(PROJECT_NAME)

	language "c++"
	kind "WindowedApp"

	files { "src/**.cpp", "src/**.hpp" }
	includedirs { "src" }

	-- plugin
	minko.plugin.enable("sdl")
	--minko.plugin.enable("bullet")
	--minko.plugin.enable("jpeg")
	--minko.plugin.enable("serializer")
	--minko.plugin.enable("particles")
	--minko.plugin.enable("png")
