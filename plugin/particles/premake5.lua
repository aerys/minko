PROJECT_NAME = path.getname(os.getcwd())

minko.project.library("minko-plugin-" .. PROJECT_NAME)

	minko.plugin.enable("serializer")

	if minko.plugin.requested("lua") then
		minko.plugin.enable { "lua" }
	end

	files {
		"include/**.hpp",
		"src/**.cpp",
		"src/**.hpp",
		"asset/**"
	}

	includedirs {
		"include",
		"src"
	}
