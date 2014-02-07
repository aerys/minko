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
		"include",
		"lib/libcurl/include"
	}
	
	-- linux
	configuration { "linux32 or linux64" }
		links { "curl"}

	-- windows
	configuration { "windows32 or windows 64" }
		includedirs { "lib/libcurl/include" }
		
	-- macos
	configuration { "osx64" }
		includedirs { "lib/libcurl/include" }
		links { "curl"}
