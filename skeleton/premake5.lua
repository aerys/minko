dofile(os.getenv("MINKO_HOME") .. "/sdk.lua")

PROJECT_NAME = path.getname(os.getcwd())

minko.project.solution(PROJECT_NAME)

	minko.project.application(PROJECT_NAME)

		language "c++"
		kind "ConsoleApp"

		files { "src/**.cpp", "src/**.hpp" }
		includedirs { "src" }

		-- plugins
		minko.plugin.enable("sdl")
		--minko.plugin.enable("bullet")
		--minko.plugin.enable("jpeg")
		--minko.plugin.enable("mk")
		--minko.plugin.enable("particles")
		--minko.plugin.enable("png")

