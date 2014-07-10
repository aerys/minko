PROJECT_NAME = path.getname(os.getcwd())

minko.project.application("minko-example-" .. PROJECT_NAME)

	files {
		"src/**.cpp",
		"src/**.hpp",
		"asset/**"
	}
	
	includedirs { "src" }

	-- plugins
	minko.plugin.enable("sdl")
	--minko.plugin.enable("bullet")
	minko.plugin.enable("jpeg")
	--minko.plugin.enable("mk")
	--minko.plugin.enable("particles")
	--minko.plugin.enable("png")

	configuration { "debug" }
		-- note: this is usually unnecessary, but we have a case where
		-- PlatformTexture.effect depends on effect/Basic.vertex.glsl.
		prelinkcommands {
			minko.action.copy("asset"),
		}
