newoption {
	trigger			= "with-offscreen",
	description		= "Enable the Minko Offscreen plugin."
}

PROJECT_NAME = path.getname(os.getcwd())

minko.project.library("minko-plugin-" .. PROJECT_NAME)
	platforms { "win", "osx", "html5", "ios", "android" }
	
	kind "StaticLib"
	language "C++"
	files { "**.hpp", "**.h", "**.cpp", "**.c", "include/**.hpp" }
	includedirs { "include", "src", "lib/osmesa/include" }
