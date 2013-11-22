newoption {
	trigger			= "with-webgl",
	description		= "Enable the Minko WebGL plugin."
}

minko.project.library "plugin-webgl"
	kind "StaticLib"
	language "C++"
	files { "**.hpp", "**.h", "**.cpp", "**.c" }
	includedirs { "src" }
