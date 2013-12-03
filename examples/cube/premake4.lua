dofile(os.getenv("MINKO_HOME") .. "/sdk.lua")

PROJECT_NAME = "minko-example-" .. path.getname(os.getcwd())

minko.project.solution(PROJECT_NAME)

	minko.project.application(PROJECT_NAME)

		language "c++"
		kind "ConsoleApp"

		files { "src/**.cpp", "src/**.hpp", "asset/**" }
		includedirs { "src" }

		-- plugins
		minko.plugin.enable("sdl")
		--minko.plugin.enable("bullet")
		--minko.plugin.enable("jpeg")
		--minko.plugin.enable("mk")
		--minko.plugin.enable("particles")
		minko.plugin.enable("png")
		
		minko.plugin.import("angle")
