PROJECT_NAME = path.getname(os.getcwd())

minko.project.library("minko-plugin-" .. PROJECT_NAME)

	kind "StaticLib"
	language "C++"

	minko.plugin.enable("lua")
	
	files {
		"lib/**.hpp",
		"lib/**.h",
		"lib/**.cpp",
		"lib/**.c",
		"include/**.hpp",
		"src/**.cpp",
		"src/**.hpp"
	}

	includedirs { "include" }

	-- windows
	configuration { "windows32 or windows64" }
		defines { "CHROMIUM" }
		includedirs { "lib/cef3" }

--[[	-- emscripten
	configuration { "html5" }
		includedirs { "" }]]--
