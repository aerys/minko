newoption {
	trigger		= "with-http",
	description	= "Enable the Minko HTTP plugin."
}

PROJECT_NAME = path.getname(os.getcwd())

minko.project.library("minko-plugin-" .. PROJECT_NAME)

	kind "StaticLib"
	language "C++"

	files {
		"lib/**.hpp",
		"lib/**.h",
		"lib/**.cpp",
		"lib/**.c",
		"include/**.hpp",
		"src/**.cpp"
	}

	includedirs {
		"include"
	}
	
	-- linux
	configuration { "linux" }
		includedirs { "lib/libcurl/include" }

	-- windows
	configuration { "win" }
		includedirs { "lib/libcurl/include" }
		
	-- macos
	configuration { "osx" }
		includedirs { "lib/libcurl/include" }