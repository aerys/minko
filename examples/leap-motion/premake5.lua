PROJECT_NAME = path.getname(os.getcwd())

minko.project.application("minko-example-" .. PROJECT_NAME)

	removeplatforms { "android", "ios", "linux", "html5" }

	language "c++"
	kind "ConsoleApp"

	files { "src/**.cpp", "src/**.hpp", "asset/**" }
	includedirs { "src" }
	includedirs { minko.sdk.path("plugins/leap/lib/include") } -- FIXME shall disappear soon
	
	-- plugins
	minko.plugin.enable("mk")
	minko.plugin.enable("png")
	minko.plugin.enable("jpeg")
	minko.plugin.enable("sdl")
	minko.plugin.enable("leap")
	
	minko.plugin.import("angle")