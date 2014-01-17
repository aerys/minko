newoption {
	trigger			= "with-leap",
	description		= "Enable the Minko Leap plugin."
}

PROJECT_NAME = path.getname(os.getcwd())

minko.project.library("minko-plugin-" .. PROJECT_NAME)
	kind "StaticLib"
	language "C++"
	files { "src/**.hpp", "src/**.h", "src/**.cpp", "src/**.c", "include/**.hpp" }
	includedirs { "include", "src" }

	removeplatforms { "html5", "ios", "android" }

	configuration { "windows", "x32" }
		includedirs { minko.sdk.path("plugins/leap/lib/leap/windows32/include") } 

	configuration { "windows", "x64" }
		includedirs { minko.sdk.path("plugins/leap/lib/leap/windows64/include") } 

	configuration { "macosx" }
		-- FIXME

	configuration { "linux", "x32" }
		includedirs { minko.sdk.path("plugins/leap/lib/leap/linux32/include") } 

	configuration { "linux", "x64" }
		includedirs { minko.sdk.path("plugins/leap/lib/leap/linux64/include") } 

	configuration {}
